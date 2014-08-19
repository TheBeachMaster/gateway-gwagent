/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#import "ViewController.h"
#import "AJNStatus.h"
#import "alljoyn/about/AJNAnnouncement.h"
#import "alljoyn/about/AJNAboutDataConverter.h"
#import "alljoyn/about/AJNAnnouncementReceiver.h"
#import "alljoyn/gateway/AJGWCConnectorApp.h"
#import "alljoyn/gateway/AJGWCConnectorAppStatus.h"
#import "alljoyn/gateway/AJGWCGatewayController.h"
#import "alljoyn/gateway/AJGWCAnnouncementData.h"
#import "AnnounceTextViewController.h"
#import "GetAboutCallViewController.h"
#import "ConnectorAppTableViewController.h"
#import "ClientInformation.h"
#import "AnnouncementManager.h"
#import "AppDelegate.h"

static bool ALLOWREMOTEMESSAGES = true; // About Client -  allow Remote Messages flag
static NSString *const APPNAME = @"AboutClientMain";  //About Client - default application name
static NSString *const DAEMON_QUIET_PREFIX = @"quiet@";    //About Client - quiet advertising
static NSString *const DEFAULT_PASSCODE = @"000000";
static NSString *const KEYSTORE_FILE_PATH = @"Documents/alljoyn_keystore/s_central.ks";
static NSString *const GW_OBJECT_PATH = @"/gw";  // GW Service
static NSString *const GW_INTERFACE_NAME = @"org.alljoyn.gwagent.ctrl";  //GW Service
static NSString * const AUTH_MECHANISM = @"ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX ALLJOYN_ECDHE_PSK";

@interface ViewController ()

// About Client properties
@property (strong, nonatomic) AJNBusAttachment *clientBusAttachment;
@property (strong, nonatomic) AJNAnnouncementReceiver *announcementReceiver;
@property (strong, nonatomic) NSString *realmBusName;
@property (nonatomic) bool isAboutClientConnected;
@property (strong, atomic) NSMutableDictionary *clientInformationDict; // Store the client related information
@property (strong, nonatomic) NSMutableDictionary *peersPasscodes; // Store the peers passcodes

// Announcement
@property (strong, nonatomic) NSString *announcementButtonCurrentTitle; // The pressed button's announcementUniqueName
@property (strong, nonatomic) dispatch_queue_t annBtnCreationQueue;

// About Client strings
@property (strong, nonatomic) NSString *ajconnect;
@property (strong, nonatomic) NSString *ajdisconnect;

// About Client alerts
@property (strong, nonatomic) UIAlertView *announcementOptionsAlertWithGW;

/* Security */
@property (strong, nonatomic) UIAlertView *setPassCodeAlert;
@property (strong, nonatomic) NSString *passCodeText;
@property (strong, nonatomic) NSString *peerName;
@property (nonatomic, strong) NSString *password;

@end

@implementation ViewController

#pragma mark - Built In methods
- (void)viewDidLoad
{
    [super viewDidLoad];

    [self loadNewSession];
}

// Get the user's input from the alert dialog
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
        case 4: //announcementOptionsAlertNoGW
        {
            [self performAnnouncemnetAction:buttonIndex];
        }
            break;

        case 5: // passcode alert
        {
            [self.setPassCodeAlert dismissWithClickedButtonIndex:buttonIndex animated:NO];

            if (buttonIndex == 1) { // User pressed OK
                // get the input pass
                self.passCodeText = [self.setPassCodeAlert textFieldAtIndex:0].text;
                NSLog(@"Passcode is: %@",  self.passCodeText);
                bool foundPeer = false;

                // check that peername is not empty
                if ([self.peerName length]) {
                    if (![self.passCodeText length]) {
                        // set the pass to default if input is empty
                        self.passCodeText = DEFAULT_PASSCODE;
                    }
                    // Iterate over the dictionary and add/update
                    for (NSString *key in self.peersPasscodes.allKeys) {
                        if ([key isEqualToString:self.peerName]) {
                            // Update passcode for key
                            (self.peersPasscodes)[self.peerName] = self.passCodeText;
                            NSLog(@"Update peer %@ with passcode %@",  self.peerName, self.passCodeText);
                            // Set flag
                            foundPeer = true;
                            break;
                        }
                    }
                    if (!foundPeer) {
                        // Add new set of key/value
                        [self.peersPasscodes setValue:self.passCodeText forKey:self.peerName];
                        NSLog(@"add new peers %@ %@", self.peerName, self.passCodeText);
                    }

                    [[NSNotificationCenter defaultCenter] postNotificationName:@"passcodeForBus" object:self.peerName];
                }
            }
            else {     // User pressed Cancel
            }
        }
            break;

        default:
            NSLog(@"alertView.tag is wrong");
            break;
    }
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[GetAboutCallViewController class]]) {
        GetAboutCallViewController *getAboutCallView = segue.destinationViewController;
        getAboutCallView.clientInformation = self.clientInformationDict[self.announcementButtonCurrentTitle];
        getAboutCallView.clientBusAttachment = self.clientBusAttachment;
    }
    else if ([segue.destinationViewController isKindOfClass:[AnnounceTextViewController class]]) {
        AnnounceTextViewController *announceTextViewController = segue.destinationViewController;
        announceTextViewController.ajnAnnouncement = [(ClientInformation *)(self.clientInformationDict)[self.announcementButtonCurrentTitle] announcement];
    }
    else if ([segue.destinationViewController isKindOfClass:[ConnectorAppTableViewController class]]) {
        ConnectorAppTableViewController *connectorAppTableViewController = segue.destinationViewController;
        connectorAppTableViewController.title = @"Controller Apps"; //[AJNAboutDataConverter messageArgumentToString:[[(ClientInformation *)(self.clientInformationDict)[self.announcementButtonCurrentTitle] announcement] aboutData][@"DeviceName"]];
        connectorAppTableViewController.busAttachment = self.clientBusAttachment;
        connectorAppTableViewController.ajnAnnouncement = [(ClientInformation *)(self.clientInformationDict)[self.announcementButtonCurrentTitle] announcement];
        NSLog(@"AnnouncementManager entries: [%lu]",(unsigned long)[[[AnnouncementManager sharedInstance] getAnnouncements] count]);
    }
} /* prepareForSegue: sender: */

#pragma mark - IBAction Methods
- (IBAction)connectButtonDidTouchUpInside:(id)sender
{
    // Connect to the bus with the default realm bus name
    if (!self.isAboutClientConnected) {
        [self startAboutClient];
        self.loadingGWLabel.alpha = 1;
    } else {
        [self stopAboutClient];
        self.loadingGWLabel.alpha = 0;
    }
}

// IBAction triggered by a dynamic announcement button
- (void)didTouchAnnSubvOkButton:(id)subvOkButton
{
    [[subvOkButton superview] removeFromSuperview];
}

#pragma mark - AJNAnnouncementListener protocol method
// Here we receive an announcement from AJN and add it to the client's list of services avaialble
- (void)announceWithVersion:(uint16_t)version port:(uint16_t)port busName:(NSString *)busName objectDescriptions:(NSMutableDictionary *)objectDescs aboutData:(NSMutableDictionary **)aboutData
{

    bool hasGWInterface = false;
    // iterate over the object descriptions dictionary
    for (NSString *key in objectDescs.allKeys) {
        if ([key isEqualToString:GW_OBJECT_PATH]) {
            // iterate over the NSMutableArray
            for (NSString *intfc in[objectDescs valueForKey:key]) {
                if ([intfc hasPrefix:GW_INTERFACE_NAME]) {
                    hasGWInterface = true;
                    NSLog(@"Announcement has key:%@  intfc:%@", key, intfc);
                }
            }
        }
    }

    if (!hasGWInterface) {
        NSLog(@"Announcement does not support gw controller interface.");
        AJGWCAnnouncementData* announcementData = [[AJGWCAnnouncementData alloc] initWithPort:port busName:busName aboutData:*aboutData objectDescriptions:objectDescs];
        [[AnnouncementManager sharedInstance] addNewAnnouncement:announcementData];
        return;
    }

    NSString *announcementUniqueName; // Announcement unique name in a format of <busName DeviceName>
    ClientInformation *clientInformation = [[ClientInformation alloc] init];

    // Save the announcement in a AJNAnnouncement
    clientInformation.announcement = [[AJNAnnouncement alloc] initWithVersion:version port:port busName:busName objectDescriptions:objectDescs aboutData:aboutData];

    // Generate an announcement unique name in a format of <busName DeviceName>
    announcementUniqueName = [NSString stringWithFormat:@"%@ %@", [clientInformation.announcement busName], [AJNAboutDataConverter messageArgumentToString:[clientInformation.announcement aboutData][@"DeviceName"]]];
    NSLog(@"Announcement unique name [%@]", announcementUniqueName);

    AJNMessageArgument *annObjMsgArg = [clientInformation.announcement aboutData][@"AppId"];
    uint8_t *appIdBuffer;
    size_t appIdNumElements;
    QStatus status;
    status = [annObjMsgArg value:@"ay", &appIdNumElements, &appIdBuffer];

    // Add the received announcement
    if (status != ER_OK) {
        NSLog(@"Failed to read appId for key [%@] :%@", announcementUniqueName, [AJNStatus descriptionForStatusCode:status]);
        return;
    }

    // Dealing with announcement entries should be syncronized, so we add it to a queue
    dispatch_sync(self.annBtnCreationQueue, ^{
        bool isAppIdExists = false;
        uint8_t *tmpAppIdBuffer;
        size_t tmpAppIdNumElements;
        QStatus tStatus;
        int res;

        // Iterate over the announcements dictionary
        for (NSString *key in self.clientInformationDict.allKeys) {
            ClientInformation *clientInfo = [self.clientInformationDict valueForKey:key];
            AJNAnnouncement *announcement = [clientInfo announcement];
            AJNMessageArgument *tmpMsgrg = [announcement aboutData][@"AppId"];

            tStatus = [tmpMsgrg value:@"ay", &tmpAppIdNumElements, &tmpAppIdBuffer];
            if (tStatus != ER_OK) {
                NSLog(@"Failed to read appId for key [%@] :%@", key, [AJNStatus descriptionForStatusCode:tStatus]);
                return;
            }

            res = 1;
            if (appIdNumElements == tmpAppIdNumElements) {
                res = memcmp(appIdBuffer, tmpAppIdBuffer, appIdNumElements);
            }

            // Found a matched appId - res=0
            if (!res) {
                NSLog(@"Got an announcement from a known device/appID");
                isAppIdExists = true;
                // Same AppId and the same announcementUniqueName
                if ([key isEqualToString:announcementUniqueName]) {
                    // Update only announcements dictionary
                    (self.clientInformationDict)[announcementUniqueName] = clientInformation;
                }
                else {
                    // Same AppId but *different* Bus name/Device name
                    NSLog(@"Same AppId but *different* Bus name/Device name - updating the announcement object and UI");
                    NSString *prevBusName = [announcement busName];
                    // Check if bus name has changed
                    if (!([busName isEqualToString:prevBusName])) {
                        NSLog(@"Bus name has changed - calling cancelFindAdvertisedName");
                        // Cancel advertise name
                        tStatus = [self.clientBusAttachment cancelFindAdvertisedName:prevBusName];
                        if (status != ER_OK) {
                            NSLog(@"Failed to cancelAdvertisedName for %@ :%@", prevBusName, [AJNStatus descriptionForStatusCode:tStatus]);
                        }
                    }
                    // Remove existed record from the announcements dictionary
                    [self.clientInformationDict removeObjectForKey:key];
                    // Add new record to the announcements dictionary
                    [self.clientInformationDict setValue:clientInformation forKey:announcementUniqueName];
                    // Update UI
                    NSLog(@"Update UI with the announcementUniqueName: %@", announcementUniqueName);

                    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
                }
                break;
            } //if
        } //for

        //appId doesn't exist and  there is no match announcementUniqueName
        if (!(self.clientInformationDict)[announcementUniqueName] && !isAppIdExists) {
            // Add new pair with this AboutService information (version,port,bus name, object description and about data)
            [self.clientInformationDict setValue:clientInformation forKey:announcementUniqueName];
            [self addNewAnnouncemetEntry];
        }
    });

    // Register interest in a well-known name prefix for the purpose of discovery (didLoseAdertise)
    [self.clientBusAttachment enableConcurrentCallbacks];
    status = [self.clientBusAttachment findAdvertisedName:busName];
    if (status != ER_OK) {
        NSLog(@"Failed to findAdvertisedName for %@ :%@", busName, [AJNStatus descriptionForStatusCode:status]);
    }
}

#pragma mark AJNBusListener protocol methods
- (void)didFindAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
    NSLog(@"didFindAdvertisedName has been called");
}

- (void)didLoseAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
    NSLog(@"didLoseAdvertisedName has been called");
    QStatus status;
    // Find the button title that should be removed
    for (NSString *key in[self.clientInformationDict allKeys]) {
        if ([[[[self.clientInformationDict valueForKey:key] announcement] busName] isEqualToString:name]) {
            // Cancel advertise name for that bus
            status = [self.clientBusAttachment cancelFindAdvertisedName:name];
            if (status != ER_OK) {
                NSLog(@"Failed to cancelFindAdvertisedName for %@. status:%@", name, [AJNStatus descriptionForStatusCode:status]);
            }
            // Remove the anouncement from the dictionary
            [self.clientInformationDict removeObjectForKey:key];
            [self.servicesTable reloadData];
        }
    }
}

#pragma mark - util methods
- (void)loadNewSession
{
    // About Client flags
    self.isAboutClientConnected  = false;

    self.annBtnCreationQueue = dispatch_queue_create("org.alljoyn.announcementbuttoncreationQueue", NULL);

    // Set About Client strings
    self.ajconnect = @"Connect to AllJoyn";
    self.ajdisconnect = @"Disconnect from AllJoyn";
    self.realmBusName = @"org.alljoyn.BusNode.gwController";
    // Set About Client connect button
    self.connectButton.backgroundColor = [UIColor darkGrayColor]; //button bg color
    [self.connectButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal]; //button font color
    [self.connectButton setTitle:self.ajconnect forState:UIControlStateNormal]; //default text
    self.loadingGWLabel.alpha = 0;
    [self prepareAlerts];
}

// Initialize alerts
- (void)prepareAlerts
{
    // announcementOptionsAlert.tag = 4
    self.announcementOptionsAlertWithGW = [[UIAlertView alloc] initWithTitle:@"Choose option:" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Show Announce", @"About",@"Gateway Mgmt", nil];
    self.announcementOptionsAlertWithGW.alertViewStyle = UIAlertViewStyleDefault;
    self.announcementOptionsAlertWithGW.tag = 4;

    // setPassCodeAlert.tag = 5
    self.setPassCodeAlert = [[UIAlertView alloc] initWithTitle:@"" message:@"Enter device password" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
    self.setPassCodeAlert.alertViewStyle = UIAlertViewStylePlainTextInput;
    self.setPassCodeAlert.tag = 5;
}

- (void)performAnnouncemnetAction:(NSInteger)opt
{
    switch (opt) {
        case 0: // "Cancel"
            break;

        case 1: // "Show Announce"
        {
            [self performSegueWithIdentifier:@"AboutShowAnnounceSegue" sender:self];
        }
            break;

        case 2: // "About"
        {
            [self performSegueWithIdentifier:@"AboutClientSegue" sender:self]; // get the announcment object
        }
            break;

        case 3: //"Gateway"
        {
            [self performSegueWithIdentifier:@"ConnectorAppSegue" sender:self];
        }
        default:
            break;
    }
}

- (void)setPassCode:(NSString *)passCode
{
    self.password = passCode;
}

#pragma mark - AboutClient
#pragma mark start AboutClient
- (void)startAboutClient
{
    QStatus status;

    NSLog(@"Start About Client");

    // Create a dictionary to contain announcements using a key in the format of: "announcementUniqueName + announcementObj"
    self.clientInformationDict = [[NSMutableDictionary alloc] init];

    self.clientBusAttachment = [[AJNBusAttachment alloc] initWithApplicationName:(APPNAME) allowRemoteMessages:(ALLOWREMOTEMESSAGES)];

    status = [self.clientBusAttachment start];
    if (status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed at AJNBusAttachment start" status:status];
        [self stopAboutClient];
        return;
    }

    status = [self.clientBusAttachment connectWithArguments:(@"")];
    if (status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed at AJNBusAttachment connectWithArguments" status:status];
        [self stopAboutClient];
        return;
    }

    NSLog(@"aboutClientListener");

    [self.clientBusAttachment registerBusListener:self];

    self.announcementReceiver = [[AJNAnnouncementReceiver alloc] initWithAnnouncementListener:self andBus:self.clientBusAttachment];
    status = [self.announcementReceiver registerAnnouncementReceiverForInterfaces:nil withNumberOfInterfaces:0];
    if (status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed to register Announcement Receiver" status:status];
        [self stopAboutClient];
        return;
    }

    [AnnouncementManager sharedInstance]; //Initialize announcement manager for all annoncements other than gateway management app

    status = [self.clientBusAttachment requestWellKnownName:self.realmBusName withFlags:kAJNBusNameFlagDoNotQueue];
    if (status == ER_OK) {
        NSUUID *UUID = [NSUUID UUID];
        NSString *stringUUID = [UUID UUIDString];

        self.realmBusName = [self.realmBusName stringByAppendingFormat:@"-%@", stringUUID];

        status = [self.clientBusAttachment advertiseName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
        if (status != ER_OK) {
            [AppDelegate AlertAndLog:@"Failed at AJNBusAttachment advertiseName" status:status];
            [self stopAboutClient];
            return;
        }
        else {
            NSLog(@"Successfully advertised: %@%@", DAEMON_QUIET_PREFIX, self.realmBusName);
        }
    } else {
        [AppDelegate AlertAndLog:@"Failed at AJNBusAttachment requestWellKnownName" status:status];
        [self stopAboutClient];
        return;
    }

    [self.connectButton setTitle:self.ajdisconnect forState:UIControlStateNormal]; //change title to "Disconnect from AllJoyn"


    // Create NSMutableDictionary dictionary of peers passcodes
    self.peersPasscodes = [[NSMutableDictionary alloc] init];

    // Enable Client Security
    status = [self enableClientSecurity];

    if (ER_OK != status) {
        NSLog(@"Failed to enable security on the bus. %@", [AJNStatus descriptionForStatusCode:status]);
    }
    else {
        NSLog(@"Successfully enabled security for the bus");
    }
    self.isAboutClientConnected = true;
    [self.servicesTable reloadData];
}

- (QStatus)enableClientSecurity
{
    QStatus status;
    status = [self.clientBusAttachment enablePeerSecurity:AUTH_MECHANISM authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:YES];

    if (status != ER_OK) { //try to delete the keystore and recreate it, if that fails return failure
        NSError *error;
        NSString *keystoreFilePath = [NSString stringWithFormat:@"%@/alljoyn_keystore/s_central.ks", [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]];
        [[NSFileManager defaultManager] removeItemAtPath:keystoreFilePath error:&error];
        if (error) {
            NSLog(@"ERROR: Unable to delete keystore. %@", error);
            return ER_AUTH_FAIL;
        }
        status = [self.clientBusAttachment enablePeerSecurity:AUTH_MECHANISM authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:YES];
    }

    return status;
}

- (void)addNewAnnouncemetEntry
{
    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
}

// IBAction triggered by pressing a dynamic announcement entry
- (void)announcementGetMoreInfo:(NSInteger)requestedRow
{
    self.announcementButtonCurrentTitle = [self.clientInformationDict allKeys][requestedRow];   // set the announcementButtonCurrentTitle

    NSLog(@"Getting data for [%@]", self.announcementButtonCurrentTitle);

    [self.announcementOptionsAlertWithGW show]; // Event is forward to alertView: clickedButtonAtIndex:

}


#pragma mark stop AboutClient
- (void)stopAboutClient
{
    QStatus status;
    NSLog(@"Stop About Client");

    status = [self.clientBusAttachment cancelAdvertisedName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
    if (status == ER_OK) {
        NSLog(@"Successfully cancel advertised name");
    }
    status = [self.clientBusAttachment releaseWellKnownName:self.realmBusName];
    if (status == ER_OK) {
        NSLog(@"Successfully release WellKnownName");
    }
    status = [self.clientBusAttachment removeMatchRule:@"sessionless='t',type='error'"];
    if (status == ER_OK) {
        NSLog(@"Successfully remove MatchRule");
    }

    for (NSString *key in[self.clientInformationDict allKeys]) {
        ClientInformation *clientInfo = (self.clientInformationDict)[key];
        status = [self.clientBusAttachment cancelFindAdvertisedName:[[clientInfo announcement] busName]];
        if (status != ER_OK) {
            NSLog(@"Failed to cancelAdvertisedName for %@: %@", key, [AJNStatus descriptionForStatusCode:status]);
        }
    }
    self.clientInformationDict = nil;

    [[AnnouncementManager sharedInstance] destroyInstance];

    status = [self.announcementReceiver unRegisterAnnouncementReceiverForInterfaces:nil withNumberOfInterfaces:0];
    if (status == ER_OK) {
        NSLog(@"Successfully unregistered AnnouncementReceiver");
    }

    self.announcementReceiver = nil;

    status = [self.clientBusAttachment stop];
    if (status == ER_OK) {
        NSLog(@"Successfully stopped bus");
    }
    self.clientBusAttachment = nil;

    self.peersPasscodes = nil;


    // Set flag
    self.isAboutClientConnected  = false;

    // UI cleanup
    [self.connectButton setTitle:self.ajconnect forState:UIControlStateNormal];

    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];

    NSLog(@"About Client is stopped");

}

#pragma mark UITableView delegates

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSUInteger numberOfRows = [self.clientInformationDict count];

    return numberOfRows;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *MyIdentifier = @"AnnouncementCell";

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];

    cell.selectionStyle = UITableViewCellSelectionStyleNone;

    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:MyIdentifier];
    }

    // Here we use the provided setImageWithURL: method to load the web image
    // Ensure you use a placeholder image otherwise cells will be initialized with no image
    cell.textLabel.text = [self.clientInformationDict allKeys][indexPath.row];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self announcementGetMoreInfo:indexPath.row];
}

#pragma mark - AJNAuthenticationListener protocol methods
- (AJNSecurityCredentials *)requestSecurityCredentialsWithAuthenticationMechanism:(NSString *)authenticationMechanism peerName:(NSString *)peerName authenticationCount:(uint16_t)authenticationCount userName:(NSString *)userName credentialTypeMask:(AJNSecurityCredentialType)mask
{
    AJNSecurityCredentials *creds = nil;
    bool credFound = false;

    NSLog(@"requestSecurityCredentialsWithAuthenticationMechanism:%@ forRemotePeer%@ userName:%@", authenticationMechanism, peerName, userName);

    if ([authenticationMechanism isEqualToString:@"ALLJOYN_SRP_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_PIN_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_ECDHE_PSK"]) {
        if (mask & kAJNSecurityCredentialTypePassword) {
            if (authenticationCount <= 3) {
                creds = [[AJNSecurityCredentials alloc] init];

                // Check if the password stored in peersPasscodes
                for (NSString *key in self.peersPasscodes.allKeys) {
                    if ([key isEqualToString:peerName]) {
                        creds.password = (self.peersPasscodes)[key];
                        NSLog(@"Found password %@ for peer %@", creds.password, key);
                        credFound = true;
                        break;
                    }
                }
                // Use the default password
                if (!credFound) {
                    creds.password = DEFAULT_PASSCODE;
                    NSLog(@"Using default password %@ for peer %@", DEFAULT_PASSCODE, peerName);
                }
            }
        }
    }
    return creds;
}

- (void)authenticationUsing:(NSString *)authenticationMechanism forRemotePeer:(NSString *)peerName didCompleteWithStatus:(BOOL)success
{
    NSString *status;
    status = (success == YES ? @"was successful" : @"failed");

    NSLog(@"authenticationUsing:%@ forRemotePeer%@ %@", authenticationMechanism, peerName, status);

    //get the passcpde for this bus
    if (!success) {
        self.peerName = peerName;
        self.passCodeText = nil;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.setPassCodeAlert show];
        });
    }
}

//- (BOOL)alertViewShouldEnableFirstOtherButton:(UIAlertView *)alertView
//{
//    return YES;
//}


@end
