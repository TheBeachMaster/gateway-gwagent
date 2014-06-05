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

#import <UIKit/UIKit.h>
#import "alljoyn/gateway/AJGWCGatewayCtrlTPApplication.h"
#import "AJNSessionOptions.h"

static NSDictionary* STATUS_COLOR = [[NSDictionary alloc]
                                     initWithObjectsAndKeys:
                                     //AJGWCConnectionStatus
                                     [UIColor purpleColor], @"Not initialized",
                                     [UIColor blueColor], @"In progress",
                                     [UIColor greenColor],  @"Connected",
                                     [UIColor orangeColor], @"Not connected",
                                     [UIColor redColor], @"Error",
                                     //AJGWCInstallStatus
                                     [UIColor greenColor], @"Installed",
                                     [UIColor blueColor],@"Install in progress",
                                     [UIColor blueColor], @"Upgrade in progress",
                                     [UIColor blueColor], @"Uninstall in progress",
                                     [UIColor redColor], @"Installation failed",
                                     //AJGWCOperationalStatus
                                     [UIColor greenColor], @"Running",
                                     [UIColor orangeColor], @"Stopped",
                                     nil];

@interface TPAppInfoViewController : UIViewController

@property (weak, nonatomic) AJGWCGatewayCtrlTPApplication* tpApplication;
@property (nonatomic) AJNSessionId sessionId;
@property (strong, nonatomic) IBOutlet UILabel *friendlyNameLbl;
@property (strong, nonatomic) IBOutlet UILabel *appVersionLbl;

@property (weak, nonatomic) IBOutlet UILabel *connectivityLbl;
@property (weak, nonatomic) IBOutlet UILabel *operationLbl;
@property (weak, nonatomic) IBOutlet UILabel *installationLbl;

@property (weak, nonatomic) IBOutlet UITableView *aclsTableView;


+ (void)setLabelTextColor:(UILabel*) label forStatus:(NSString*) statusString;
@end
