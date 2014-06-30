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

#import "TPAppInfoViewController.h"
#import "AJNStatus.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlAccessControlList.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlApplicationStatusSignalHandler.h"
#import "TPAppInfoAclsTableViewCell.h"
#import "ACLTableViewController.h"
#import "ManifestTabBarController.h"
#import "CreateAclViewController.h"
#import "AppDelegate.h"

@interface TPAppInfoViewController () <UIActionSheetDelegate, AJGWCGatewayCtrlApplicationStatusSignalHandler>

@property (strong, nonatomic) NSMutableArray *acls; // array of AJGWCGatewayCtrlAccessControlList
@property (strong, nonatomic) NSString *manifestFileText;

@property (nonatomic) NSIndexPath* indexPathToDelete;
@property (strong, nonatomic) UIAlertView* deleteAclAlertView;
@end

@implementation TPAppInfoViewController
- (void)viewDidLoad
{
    [super viewDidLoad];
//    [self startTPAppInfoViewController];
	// Do any additional setup after loading the view.
}

- (void)startTPAppInfoViewController
{
    self.title = [self.tpApplication friendlyName];
    
    UIBarButtonItem *optionsBtn = [[UIBarButtonItem alloc] initWithTitle:@"+" style:UIBarButtonItemStylePlain target:self action:@selector(didTouUpInsideOptionsBtn:)];
    self.navigationItem.rightBarButtonItem = optionsBtn;
    
    self.friendlyNameLbl.text = [self.tpApplication friendlyName];
    self.appVersionLbl.text = [self.tpApplication appVersion];
    
    [self retrieveStatus];
    
    [self retrieveAcls];
    
    QStatus handlerStatus = [self.tpApplication setStatusChangedHandler:self];
    if (ER_OK != handlerStatus) {
        [AppDelegate AlertAndLog:@"Failed to set status changed handler" status:handlerStatus];
    }
    
    self.deleteAclAlertView = [[UIAlertView alloc] initWithTitle:@"" message:@"Delete selected acl?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles: @"OK", nil];
    
    self.deleteAclAlertView.tag = 1;
    
    [self.aclsTableView reloadData];
}

- (void)retrieveAcls
{
    QStatus status = ER_FAIL;
    self.acls = [[NSMutableArray alloc] initWithArray:[self.tpApplication retrieveAclsUsingSessionId:self.sessionId status:status]];
    
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve Acl(s)" status:status];
    } else {
        NSLog(@"got %lu acl(s)", (unsigned long)[self.acls count]);
    }
    
}
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[ACLTableViewController class]]) {
		ACLTableViewController *aclTVC = segue.destinationViewController;
        aclTVC.sessionId = self.sessionId;
        aclTVC.acl = [self.acls objectAtIndex:[self.aclsTableView indexPathForSelectedRow].row];
        aclTVC.tpApplication = self.tpApplication;
    } else if ([segue.destinationViewController isKindOfClass:[ManifestTabBarController class]]) {
        ManifestTabBarController *manifestVC = segue.destinationViewController;
        manifestVC.sessionId = self.sessionId;
        manifestVC.tpApplication = self.tpApplication;
    } else if ([segue.destinationViewController isKindOfClass:[CreateAclViewController class]]) {
        CreateAclViewController *createAclVC = segue.destinationViewController;
        createAclVC.sessionId = self.sessionId;
        createAclVC.tpApplication = self.tpApplication;
    }
}
- (IBAction)didTouUpInsideOptionsBtn:(id)sender {
    UIActionSheet *optionsActionSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:
                                         @"Create ACL",
                                         @"Restart",
                                         @"Refresh",
                                         @"Show Manifest",
                                         nil];
    optionsActionSheet.tag = 1;
    [optionsActionSheet showInView:self.view];
}


- (void)createACL
{
    [self performSegueWithIdentifier:@"CreateAclSegue" sender:self];
}

- (void)retrieveManifestFile
{
    QStatus status = ER_FAIL;
    self.manifestFileText = [self.tpApplication retrieveManifestFileUsingSessionId:self.sessionId status:status];
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve manifest file" status:status];
    } else {
        NSLog(@"Successfully retrieved manifest file.");
        [self performSegueWithIdentifier:@"ManifestSegue" sender:self];
    }
}

- (BOOL)AJGWCAclStatusToBOOL:(AJGWCAclStatus) aclStatus
{
    return aclStatus == GW_AS_ACTIVE ?  YES :  NO;
}

#pragma mark - UIActionSheetDelegate method
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex;
{
    switch (actionSheet.tag) {
        case 1: { //optionsActionSheet
            switch (buttonIndex) {
                case 0: // Create ACL
                    [self createACL];
                    break;
                case 1: // Restart
                {
                    NSLog(@"Calling restart");
                    
                    QStatus status = ER_FAIL;
                    [self.tpApplication restartUsingSessionId:self.sessionId status:status];
                    if (ER_OK != status) {
                        [AppDelegate AlertAndLog:@"Failed to restart application"  status:status];
                    }
                }
                    break;
                case 2: // Refresh
                {
                    NSLog(@"Calling refresh");
                    [self retrieveAcls];
                    [self.aclsTableView reloadData];
                }
                    break;
                case 3: // Show Manifest
                    [self performSegueWithIdentifier:@"ManifestSegue" sender:self];
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

#pragma mark - UITableViewDataSource

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

// Swipe to delete.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        self.indexPathToDelete = indexPath;
        [self.deleteAclAlertView show];
    }
}

- (void)deleteAcl
{
    QStatus status;
    AJGWCAclResponseCode resCode;
    
    NSString* aclId = [[self.acls objectAtIndex:self.indexPathToDelete.row] aclId];
    resCode = [self.tpApplication deleteAclUsingSessionId:self.sessionId aclId:aclId status:status];
    
    if (ER_OK != status || resCode != GW_ACL_RC_SUCCESS) {
        NSLog(@"Failed to delete acl. status:%@ responseCode:%@", [AJNStatus descriptionForStatusCode:status], [AJGWCGatewayCtrlEnums AJGWCAclResponseCodeToString:resCode]);
        [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to delete acl." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
    } else {
        [self.acls removeObjectAtIndex:self.indexPathToDelete.row];
        [self.aclsTableView deleteRowsAtIndexPaths:@[self.indexPathToDelete] withRowAnimation:UITableViewRowAnimationAutomatic];
    }
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
        case 1: // self.deleteAclAlertView
            if (buttonIndex == 1) // OK
            {
                [self deleteAcl];
            }
            
            break;
        default:
			NSLog(@"alertView.tag is wrong");
			break;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [self.acls count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    TPAppInfoAclsTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"AclCell" forIndexPath:indexPath];
    
    AJGWCGatewayCtrlAccessControlList* aclList = [self.acls objectAtIndex:indexPath.row];
    cell.aclObject = aclList;
    cell.sessionId = self.sessionId;
    cell.aclNameLbl.text = [aclList aclName];
    [cell.aclActiveSwitch setOn:[self AJGWCAclStatusToBOOL:[aclList status]]];
    return cell;
}

#pragma mark -  AJGWCGatewayCtrlApplicationStatusSignalHandler method
- (void)onStatusChanged:(NSString*) appId status:(AJGWCGatewayCtrlTPApplicationStatus*) status
{
    NSLog(@"AppID %@ status has changed", appId);
    if ([[self.tpApplication appId] isEqualToString:appId])
    {
        [self updateLabelsStatus:status];
    }
}

- (void)retrieveStatus
{
    NSLog(@"Retrieving application status for %@", [self.tpApplication appId]);
    QStatus status = ER_FAIL;
    
    AJGWCGatewayCtrlTPApplicationStatus* tpAppStatus = [self.tpApplication retrieveStatusUsingSessionId:self.sessionId status:status];
    
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve application status" status:status];
    } else {
        [self updateLabelsStatus:tpAppStatus];
    }
}

- (void)updateLabelsStatus:(AJGWCGatewayCtrlTPApplicationStatus*) tpAppStatus
{
    [TPAppInfoViewController setLabelTextColor:self.connectivityLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCConnectionStatusToString:[tpAppStatus connectionStatus]]];
    [TPAppInfoViewController setLabelTextColor:self.operationLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCOperationalStatusToString:[tpAppStatus operationalStatus]]];
    [TPAppInfoViewController setLabelTextColor:self.installationLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCInstallStatusToString:[tpAppStatus installStatus]]];
}

+ (void)setLabelTextColor:(UILabel*) label forStatus:(NSString*) statusString
{
    label.text = statusString;
    [label setTextColor: [STATUS_COLOR objectForKey:statusString]];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self.tpApplication unsetStatusChangedHandler];
    [self startTPAppInfoViewController];
}

@end