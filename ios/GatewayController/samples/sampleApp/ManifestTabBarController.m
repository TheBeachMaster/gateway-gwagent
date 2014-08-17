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

#import "ManifestTabBarController.h"
#import "AJNStatus.h"
#import "ConnectorCapabilitiesViewController.h"
#import "ManifestFileViewController.h"
#import "alljoyn/gateway/AJGWCRuleObjectDescription.h"

@interface ManifestTabBarController () <UITabBarControllerDelegate>

@end

@implementation ManifestTabBarController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self startManifestTabBarController];
}

- (void)startManifestTabBarController
{
    self.delegate = self;
    self.selectedViewController = [self.viewControllers objectAtIndex:0];
    [self loadManifestContent];
}

-(void)loadManifestContent
{
    QStatus status;

    if([self.selectedViewController isKindOfClass:[ManifestFileViewController class]]) {
        NSString* manifestFile;
        status = [self.connectorApp retrieveManifestFileUsingSessionId:self.sessionId fileContent:&manifestFile];
        if (ER_OK != status) {
            NSLog(@"Failed to read manifest file. status:%@", [AJNStatus descriptionForStatusCode:status]);
            [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to read manifest file." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
        } else {
            [((ManifestFileViewController*)self.selectedViewController).manifestFileTextView setText:manifestFile];
        }

    } else {
        AJGWCConnectorCapabilities* connectorCapabilities;

        status = [self.connectorApp retrieveConnectorCapabilitiesUsingSessionId:self.sessionId connectorCapabilities:&connectorCapabilities];
        if (ER_OK != status) {
            NSLog(@"Failed to read manifest rules. status:%@", [AJNStatus descriptionForStatusCode:status]);
            [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to read manifest rules." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
        } else {
            ((ConnectorCapabilitiesViewController*)self.selectedViewController).exposedServicesTextView.attributedText = [self manifestObjectDescriptionArrayToString:[connectorCapabilities exposedServices]];
            ((ConnectorCapabilitiesViewController*)self.selectedViewController).remotedServicesTextView.attributedText = [self manifestObjectDescriptionArrayToString:[connectorCapabilities remotedServices]];
        }
    }
}


- (NSAttributedString*)manifestObjectDescriptionArrayToString:(NSArray*) objDescArray
{
    NSMutableAttributedString* objDescArrayStr = [[NSMutableAttributedString alloc] init];

    for (AJGWCRuleObjectDescription* mObjDesc in objDescArray)
    {
        //objectPath
        AJGWCRuleObjectPath* RuleObjPath = [mObjDesc objectPath];
        NSString* RuleObjPathStr = [NSString stringWithFormat:@"%@\n%@\n%@\n", [RuleObjPath friendlyName], [RuleObjPath path], [RuleObjPath isPrefix] ? @"Prefix" : @"Not Prefix"];
        NSLog(@"Final string(RuleObjPathPathStr):\n%@", RuleObjPathStr); // final str

        //interfaces
        NSMutableString* connAppInterfaceStr = [[NSMutableString alloc] init];
        NSSet* interfaces = [mObjDesc interfaces];
        for (AJGWCRuleInterface* connAppInterface in interfaces) {
            connAppInterfaceStr = [NSMutableString stringWithFormat:@"%@%@", connAppInterfaceStr, [NSString stringWithFormat:@"    %@\n    %@\n    %@\n\n", [connAppInterface friendlyName], [connAppInterface interfaceName], [connAppInterface isSecured] ? @"Secured" : @"Not Secured"]];
        }
        NSLog(@"Final string(connAppInterfaceStr):\n%@", connAppInterfaceStr); // final str

        // add RuleObjPathPathStr and set color
        [objDescArrayStr appendAttributedString:[[NSAttributedString alloc] initWithString:RuleObjPathStr]];
        NSRange RuleObjPathStrRange = NSMakeRange([objDescArrayStr length] ? [objDescArrayStr length] - [RuleObjPathStr length] : [objDescArrayStr length], [RuleObjPathStr length]);
        [objDescArrayStr addAttribute:NSForegroundColorAttributeName value:[UIColor orangeColor] range:RuleObjPathStrRange];

        // add connAppInterfaceStr and set color
        [objDescArrayStr appendAttributedString:[[NSAttributedString alloc] initWithString:connAppInterfaceStr]];
        NSRange connAppInterfaceStrRange = NSMakeRange([objDescArrayStr length] ? [objDescArrayStr length] - [connAppInterfaceStr length] : [objDescArrayStr length], [connAppInterfaceStr length]);
        [objDescArrayStr addAttribute:NSForegroundColorAttributeName value:[UIColor blueColor] range:connAppInterfaceStrRange];

    }

    // set string font size
    [objDescArrayStr addAttribute:NSFontAttributeName value:[UIFont systemFontOfSize:7] range:NSMakeRange(0, [objDescArrayStr length])];

    return objDescArrayStr;
}

#pragma mark - UITabBarControllerDelegate method
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
    [self loadManifestContent];
}
@end
