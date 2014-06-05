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
#import "ManifestRulesViewController.h"
#import "ManifestFileViewController.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlManifestObjectDescription.h"

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
        NSString* manifestFile = [self.tpApplication retrieveManifestFileUsingSessionId:self.sessionId status:status];
        if (ER_OK != status) {
            NSLog(@"Failed to read manifest file. status:%@", [AJNStatus descriptionForStatusCode:status]);
            [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to read manifest file." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
        } else {
            [((ManifestFileViewController*)self.selectedViewController).manifestFileTextView setText:manifestFile];
        }
        
    } else {
        AJGWCGatewayCtrlManifestRules* manifestRules = [self.tpApplication retrieveManifestRulesUsingSessionId:self.sessionId status:status];
        if (ER_OK != status) {
            NSLog(@"Failed to read manifest rules. status:%@", [AJNStatus descriptionForStatusCode:status]);
            [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to read manifest rules." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
        } else {
            ((ManifestRulesViewController*)self.selectedViewController).exposedServicesTextView.attributedText = [self manifestObjectDescriptionArrayToString:[manifestRules exposedServices]];
            ((ManifestRulesViewController*)self.selectedViewController).remotedServicesTextView.attributedText = [self manifestObjectDescriptionArrayToString:[manifestRules remotedServices]];
        }
    }
}


- (NSAttributedString*)manifestObjectDescriptionArrayToString:(NSArray*) objDescArray
{
    NSMutableAttributedString* objDescArrayStr = [[NSMutableAttributedString alloc] init];
    
    for (AJGWCGatewayCtrlManifestObjectDescription* mObjDesc in objDescArray)
    {
        //objectPath
        AJGWCGatewayCtrlTPObjectPath* tpObjPath = [mObjDesc objectPath];
        NSString* tpObjPathStr = [NSString stringWithFormat:@"%@\n%@\n%@\n", [tpObjPath friendlyName], [tpObjPath path], [tpObjPath isPrefix] ? @"Prefix" : @"Not Prefix"];
        NSLog(@"Final string(tpObjPathStr):\n%@", tpObjPathStr); // final str
        
        //interfaces
        NSMutableString* tpInterfaceStr = [[NSMutableString alloc] init];
        NSSet* interfaces = [mObjDesc interfaces];
        for (AJGWCGatewayCtrlTPInterface* tpInterface in interfaces) {
            tpInterfaceStr = [NSMutableString stringWithFormat:@"%@%@", tpInterfaceStr, [NSString stringWithFormat:@"    %@\n    %@\n    %@\n\n", [tpInterface friendlyName], [tpInterface interfaceName], [tpInterface isSecured] ? @"Secured" : @"Not Secured"]];
        }
        NSLog(@"Final string(tpInterfaceStr):\n%@", tpInterfaceStr); // final str
        
        // add tpObjPathStr and set color
        [objDescArrayStr appendAttributedString:[[NSAttributedString alloc] initWithString:tpObjPathStr]];
        NSRange tpObjPathStrRange = NSMakeRange([objDescArrayStr length] ? [objDescArrayStr length] - [tpObjPathStr length] : [objDescArrayStr length], [tpObjPathStr length]);
        [objDescArrayStr addAttribute:NSForegroundColorAttributeName value:[UIColor orangeColor] range:tpObjPathStrRange];
        
        // add tpInterfaceStr and set color
        [objDescArrayStr appendAttributedString:[[NSAttributedString alloc] initWithString:tpInterfaceStr]];
        NSRange tpInterfaceStrRange = NSMakeRange([objDescArrayStr length] ? [objDescArrayStr length] - [tpInterfaceStr length] : [objDescArrayStr length], [tpInterfaceStr length]);
        [objDescArrayStr addAttribute:NSForegroundColorAttributeName value:[UIColor blueColor] range:tpInterfaceStrRange];
        
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
