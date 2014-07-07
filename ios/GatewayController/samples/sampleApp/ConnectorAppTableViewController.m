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

#import "ConnectorAppTableViewController.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlGatewayController.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlConnectorApplication.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlApplicationStatusSignalHandler.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlConnectorApplicationStatus.h"
#import "ConnectorAppTableViewCell.h"
#import "ConnectorAppInfoViewController.h"
#import "AppDelegate.h"

@interface ConnectorAppTableViewController () <AJGWCGatewayCtrlApplicationStatusSignalHandler, UIActionSheetDelegate>

@property (strong, nonatomic) AJGWCGatewayCtrlGateway* gateway;
@property (nonatomic) AJNSessionId sessionId;
@property (strong, nonatomic) NSArray* gwApps;

@end

@implementation ConnectorAppTableViewController

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
    for(AJGWCGatewayCtrlConnectorApplication* connectorApp in self.gwApps)
    {
        if (flag == YES) {
            QStatus handlerStatus = [connectorApp setStatusChangedHandler:self];
            if (ER_OK != handlerStatus) {
                NSLog(@"Failed to set status changed handler");
            }
        } else {
            [connectorApp unsetStatusChangedHandler];
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
- (void)onStatusChanged:(NSString*) appId status:(AJGWCGatewayCtrlConnectorApplicationStatus*) status
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
    ConnectorAppTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"ConnectorAppCell" forIndexPath:indexPath];
    
    AJGWCGatewayCtrlConnectorApplication* connectorApp = [self.gwApps objectAtIndex:indexPath.row];
    cell.ConnectorAppNameLbl.text = [connectorApp friendlyName];
    
    /* Retrieve Status */
    QStatus status = ER_FAIL;
    AJGWCGatewayCtrlConnectorApplicationStatus* connectorAppStatus = [connectorApp retrieveStatusUsingSessionId:self.sessionId status:status];
    
    if (status == ER_OK) {
        
        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppInstallLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCInstallStatusToString:[connectorAppStatus installStatus]]];
        
        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppConnectionLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCConnectionStatusToString:[connectorAppStatus connectionStatus]]];

        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppOperationalLbl forStatus:[AJGWCGatewayCtrlEnums AJGWCOperationalStatusToString:[connectorAppStatus operationalStatus]]];        
    } else {
        [AppDelegate AlertAndLog:@"RetrieveStatus error happened, check log" status:status];
        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppInstallLbl forStatus:@"Error"];
        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppConnectionLbl forStatus:@"Error"];
        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppOperationalLbl forStatus:@"Error"];
    }
    return cell;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[ConnectorAppInfoViewController class]]) {
		ConnectorAppInfoViewController *connectorAppInfo = segue.destinationViewController;
        connectorAppInfo.connectorApplication =  [self.gwApps objectAtIndex:[self.tableView indexPathForSelectedRow].row];
        connectorAppInfo.sessionId = self.sessionId;
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
