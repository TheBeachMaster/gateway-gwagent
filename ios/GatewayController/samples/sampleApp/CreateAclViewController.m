/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#import "CreateAclViewController.h"
#import "AppDelegate.h"
#import "AnnouncementManager.h"
#import "ACLTableViewController.h"

@interface CreateAclViewController () <UITextFieldDelegate>

@property (nonatomic) AJGWCAclRules* aclRules;
@property (strong,nonatomic) AJGWCAcl *acl;

@end

@implementation CreateAclViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self startVC];
}

- (void)startVC
{
    self.appNameLbl.text = [self.connectorApp friendlyName];
    self.createAclBtn.enabled = NO;
    self.aclNameTextField.delegate = self; // Set TextField.delegate to enable dissmiss keyboard

    [self retrieveApplicableConnectorCapabilities];
}


- (void)retrieveApplicableConnectorCapabilities
{
    QStatus status;
    NSArray* ann =[[AnnouncementManager sharedInstance] getAnnouncements];

    AJGWCAclRules* tmpAclRules;

    status = [self.connectorApp retrieveApplicableConnectorCapabilitiesUsingSessionId:self.sessionId rules:&tmpAclRules announcements:ann];

    self.aclRules = tmpAclRules;

    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve configurable rules" status:status];
    } else {
        NSLog(@"Successfully retrieved configurable rules");
    }
}

- (IBAction)didTouchCreateAclBtn:(id)sender {
    QStatus status;
    AJGWCAclWriteResponse* aclWResp;
    status = [self.connectorApp createAclUsingSessionId:self.sessionId name:self.aclNameTextField.text aclRules:self.aclRules aclStatus:&aclWResp];
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to create acl" status:status];
    } else {
        NSLog(@"Successfully created a new acl");
        //Disable UI
        self.createAclBtn.enabled = NO;
        self.aclNameTextField.text = @"";
        self.aclNameTextField.enabled = NO;
        for (UIView *aSubview in[self.view subviews]) {
            if ([aSubview isKindOfClass:[UILabel class]]) {
                [(UILabel *)aSubview setTextColor:[UIColor grayColor]];
            }
        }

        NSMutableArray *acls = [[NSMutableArray alloc] init];
        status = [self.connectorApp retrieveAclsUsingSessionId:self.sessionId acls:acls];

        if (ER_OK != status) {
            [AppDelegate AlertAndLog:@"Failed to retrieve Acls" status:status];
        } else {
            for (AJGWCAcl* acl in acls)
            {
                if ([[acl aclId] isEqualToString:[aclWResp aclId]])
                    self.acl = acl;
            }

            if (!self.acl) {
                status = ER_FAIL;
                [AppDelegate AlertAndLog:@"New acl not found" status:status];
            } else {
                [self performSegueWithIdentifier:@"PostAclCreateSegue" sender:self];
            }
        }
    }
}


- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[ACLTableViewController class]]) {
        ACLTableViewController *aclTVC = segue.destinationViewController;
        aclTVC.sessionId = self.sessionId;
        aclTVC.acl = self.acl;
        aclTVC.connectorApp = self.connectorApp;
    }
}

#pragma mark - TextField methods
- (IBAction)aclNameTextFieldEditingDidEnd:(id)sender {
    self.createAclBtn.enabled = YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

// Set dismiss keyboard for each UITextField
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UIView *aSubview in[self.view subviews]) {
        if ([aSubview isKindOfClass:[UITextField class]]) {
            [(UITextField *)aSubview resignFirstResponder];
        }
    }
}

@end
