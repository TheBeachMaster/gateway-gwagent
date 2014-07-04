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

#import "TPAppTableViewController.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlGatewayController.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlTPApplication.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlApplicationStatusSignalHandler.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlTPApplicationStatus.h"
#import "TPAppTableViewCell.h"
#import "TPAppInfoViewController.h"
#import "AppDelegate.h"

@interface TPAppTableViewController () <AJGWCGatewayCtrlApplicationStatusSignalHandler, UIActionSheetDelegate>

@property (strong, nonatomic) AJGWCGatewayCtrlGateway* gateway;
@property (nonatomic) AJNSessionId sessionId;
@property (strong, nonatomic) NSArray* gwApps;

@end

@implementation TPAppTableViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self startGWController];
}


- (void)startGWController
{
    UIBarButtonItem *optionsBtn = [[UIBarButtonItem alloc] initWithTitle:@"+" style:UIBarButtonItemStylePlain target:self action:@selector(didTouUpInsideOptionsBtn:)];
    self.navigationItem.rightBarButtonItem = optionsBtn;
    
    [AJGWCGatewayCtrlGatewayController startWithBus:self.busAttachment];
    
    AJGWCGatewayCtrlGatewayController* gwController = [AJGWCGatewayCtrlGatewayController sharedInstance];
    
    self.gateway = [gwController createGatewayWithBusName:self.ajnAnnouncement.busName objectDescs:self.ajnAnnouncement.objectDescriptions aboutData:self.ajnAnnouncement.aboutData];
    
    AJGWCGatewayCtrlSessionResult *sessionResult = [self.gateway joinSession];
    
    if (sessionResult.status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed to retrieve installed apps" status:sessionResult.status];
    } else {
        self.sessionId = sessionResult.sid;
    }

    NSLog(@"SessionId is %u", self.sessionId);
    
    [self retrieveInstalledApps];
}

- (void)retrieveInstalledApps
{
    QStatus status = ER_FAIL;
    self.gwApps = [[NSArray alloc] initWithArray:[self.gateway retrieveInstalledApps:self.sessionId status:status]];
    NSLog(@"retrieveInstalledApps return [%lu] applications", (unsigned long)[self.gwApps count]);
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve installed apps" status:status];
    }
}

- (void)statusChangedHandler:(BOOL) flag
{
    for(AJGWCGatewayCtrlTPApplication* tpApp in self.gwApps)
    {
        if (flag == YES) {
            QStatus handlerStatus = [tpApp setStatusChangedHandler:self];
            if (ER_OK != handlerStatus) {
                NSLog(@"Failed to set status changed handler");
            }
        } else {
            [tpApp unsetStatusChangedHandler];
        }
    }
}

- (IBAction)didTouUpInsideOptionsBtn:(id)sender {
    UIActionSheet *optionsActionSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:
                                         @"Refresh",
                                         nil];
    optionsActionSheet.tag = 1;
    [optionsActionSheet showInView:self.view];
}

#pragma mark - UIActionSheetDelegate method
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex;
{
    switch (actionSheet.tag) {
        case 1: { //optionsActionSheet
            switch (buttonIndex) {
                case 0:
                    NSLog(@"Calling refresh");
                    [self retrieveInstalledApps]; //refresh
                    [self.tableView reloadData];
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

#pragma mark -  AJGWCGatewayCtrlApplicationStatusSignalHandler method
- (void)onStatusChanged:(NSString*) appId status:(AJGWCGatewayCtrlTPApplicationStatus*) status
{
    NSLog(@"AppID %@ status has changed", appId);
    [self.tableView reloadData];
    [self updateLabels];
}

#pragma mark - Table view data source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [self.gwApps count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    TPAppTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"TPAppCell" forIndexPath:indexPath];
    
    AJGWCGatewayCtrlTPApplication* tpApp = [self.gwApps objectAtIndex:indexPath.row];
    cell.TPAppNameLbl.text = [tpApp friendlyName];
    
    /* Retrieve Status */
    QStatus status = ER_FAIL;
    AJGWCGatewayCtrlTPApplicationStatus* tpAppStatus = [tpApp retrieveStatusUsingSessionId:self.sessionId status:status];
    
    if (status == ER_OK) {
        
        [TPAppInfoViewController setLabelTextColor:cell.TPAppInstallLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCInstallStatusToString:[tpAppStatus installStatus]]];
        
        [TPAppInfoViewController setLabelTextColor:cell.TPAppConnectionLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCConnectionStatusToString:[tpAppStatus connectionStatus]]];

        [TPAppInfoViewController setLabelTextColor:cell.TPAppOperationalLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCOperationalStatusToString:[tpAppStatus operationalStatus]]];        
    } else {
        [AppDelegate AlertAndLog:@"RetrieveStatus error happened, check log" status:status];
        [TPAppInfoViewController setLabelTextColor:cell.TPAppInstallLbl forStatus:@"Error"];
        [TPAppInfoViewController setLabelTextColor:cell.TPAppConnectionLbl forStatus:@"Error"];
        [TPAppInfoViewController setLabelTextColor:cell.TPAppOperationalLbl forStatus:@"Error"];
    }
    return cell;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[TPAppInfoViewController class]]) {
		TPAppInfoViewController *tpAppInfo = segue.destinationViewController;
        tpAppInfo.tpApplication =  [self.gwApps objectAtIndex:[self.tableView indexPathForSelectedRow].row];
        tpAppInfo.sessionId = self.sessionId;
    }
}

- (void)updateLabels
{
    //TODO
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self statusChangedHandler:NO];
}
- (void)viewWillAppear:(BOOL)animated
{
    [self statusChangedHandler:YES];
}
@end
