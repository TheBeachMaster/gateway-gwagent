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
#import "alljoyn/gateway/AJGWCGatewayController.h"
#import "alljoyn/gateway/AJGWCConnectorApp.h"
#import "alljoyn/gateway/AJGWCConnectorAppStatusSignalHandler.h"
#import "alljoyn/gateway/AJGWCConnectorAppStatus.h"
#import "alljoyn/gateway/AJGWCEnums.h"
#import "ConnectorAppTableViewCell.h"
#import "ConnectorAppInfoViewController.h"
#import "AppDelegate.h"

@interface ConnectorAppTableViewController () <AJGWCConnectorAppStatusSignalHandler, UIActionSheetDelegate, AJGWCSessionListener>

@property (strong, nonatomic) AJGWCGatewayMgmtApp* gatewayMgmtApp;
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

- (void)stopGWController
{
    QStatus status = [self.gatewayMgmtApp leaveSession];
    if (status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed to leaveSession" status:status];
    } else {
        NSLog(@"Successfully leave Session");
    }
    [[AJGWCGatewayController sharedInstance] shutdown];
}
- (void)startGWController
{
    UIBarButtonItem *optionsBtn = [[UIBarButtonItem alloc] initWithTitle:@"+" style:UIBarButtonItemStylePlain target:self action:@selector(didTouUpInsideOptionsBtn:)];
    self.navigationItem.rightBarButtonItem = optionsBtn;

    [AJGWCGatewayController startWithBus:self.busAttachment];

    AJGWCGatewayController* gwController = [AJGWCGatewayController sharedInstance];

    self.gatewayMgmtApp = [gwController createGatewayWithBusName:self.ajnAnnouncement.busName objectDescs:self.ajnAnnouncement.objectDescriptions aboutData:self.ajnAnnouncement.aboutData];

    AJGWCSessionResult *sessionResult = [self.gatewayMgmtApp joinSession:self];

    if (sessionResult.status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed to retrieve installed apps" status:sessionResult.status];
    } else {
        self.sessionId = sessionResult.sid;
    }

    NSLog(@"SessionId is %u", self.sessionId);

    [self retrieveConnectorApps];
}

- (void)retrieveConnectorApps
{
    NSMutableArray* connectorApps = [[NSMutableArray alloc] init];
    QStatus status = [self.gatewayMgmtApp retrieveConnectorApps:self.sessionId connectorApps:connectorApps];
    self.gwApps = [[NSArray alloc] initWithArray:connectorApps];
    NSLog(@"retrieveConnectorApps return [%lu] applications", (unsigned long)[self.gwApps count]);
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve installed apps" status:status];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

- (void)statusSignalHandler:(BOOL) flag
{
    for(AJGWCConnectorApp* connectorApp in self.gwApps)
    {
        if (flag == YES) {
            QStatus handlerStatus = [connectorApp setStatusSignalHandler:self];
            if (ER_OK != handlerStatus) {
                NSLog(@"Failed to set status changed handler");
            } else {
                NSLog(@"Successfully set status changed handler for %@", [connectorApp friendlyName]);
            }
        } else {
            NSLog(@"calling unset status changed handler for %@", [connectorApp friendlyName]);
            [connectorApp unsetStatusSignalHandler];
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
                    [self retrieveConnectorApps]; //refresh
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

#pragma mark -  AJGWCConnectorAppStatusSignalHandler method
- (void)onStatusSignal:(NSString*) appId status:(AJGWCConnectorAppStatus*) status
{
    NSLog(@"AppID %@ status has changed:", appId);
    NSLog(@"installStatus: %@", [AJGWCEnums AJGWCInstallStatusToString:[status installStatus]]);
    NSLog(@"installDescriptions: %@", [status installDescriptions]);
    NSLog(@"ConnectionStatus: %@", [AJGWCEnums AJGWCConnectionStatusToString:[status connectionStatus]]);
    NSLog(@"operationalStatus: %@", [AJGWCEnums AJGWCOperationalStatusToString:[status operationalStatus]]);
    NSLog(@"---------------------");

    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

- (void)onError:(NSString *)appId errorCode:(QStatus)code
{
    [AppDelegate AlertAndLog:[NSString stringWithFormat:@"AppID %@ status event - an error occured in creation of the event data",appId] status:code];
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

    AJGWCConnectorApp* connectorApp = [self.gwApps objectAtIndex:indexPath.row];
    cell.ConnectorAppNameLbl.text = [connectorApp friendlyName];

    /* Retrieve Status */
    QStatus status = ER_FAIL;
    AJGWCConnectorAppStatus* connectorAppStatus;
    status = [connectorApp retrieveStatusUsingSessionId:self.sessionId status:&connectorAppStatus];

    if (status == ER_OK) {

        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppInstallLbl forStatus:[AJGWCEnums AJGWCInstallStatusToString:[connectorAppStatus installStatus]]];

        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppConnectionLbl forStatus:[AJGWCEnums AJGWCConnectionStatusToString:[connectorAppStatus connectionStatus]]];

        [ConnectorAppInfoViewController setLabelTextColor:cell.ConnectorAppOperationalLbl forStatus:[AJGWCEnums AJGWCOperationalStatusToString:[connectorAppStatus operationalStatus]]];

        cell.ConnectorAppId = [connectorApp appId];

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
        connectorAppInfo.connectorApp =  [self.gwApps objectAtIndex:[self.tableView indexPathForSelectedRow].row];
        connectorAppInfo.sessionId = self.sessionId;
    }
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self statusSignalHandler:NO];
}

- (void)didMoveToParentViewController:(UIViewController *)parent
{
    if (parent==nil) { //only on the way back
        [self stopGWController];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [self statusSignalHandler:YES];
}

- (void)sessionEstablished:(AJGWCGatewayMgmtApp*) gatewayMgmtApp
{
    NSLog(@"Session established");
}

- (void)sessionLost:(AJGWCGatewayMgmtApp*) gatewayMgmtApp
{
    NSLog(@"Session lost");
    [[[UIAlertView alloc]initWithTitle:@"Session lost" message:@"Connection to the gateway lost" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];

    [self.navigationController popToRootViewControllerAnimated:YES];

}

@end
