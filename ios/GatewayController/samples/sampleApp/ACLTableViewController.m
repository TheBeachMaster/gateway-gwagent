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

#import "ACLTableViewController.h"
#import "alljoyn/gateway/AJGWCRemotedApp.h"
#import "alljoyn/gateway/AJGWCRuleObjectDescription.h"
#import "AclRulesContainer.h"
#import "ObjectPathLevelRulesTableViewController.h"
#import "InterfaceRuleTableViewCell.h"
#import "AJNStatus.h"
#import "AnnouncementManager.h"
#import "AppDelegate.h"

@interface ACLTableViewController () <UIActionSheetDelegate>

@property (weak,nonatomic) AJGWCConnectorCapabilities *connectorCapabilities;
@property (strong,nonatomic) AclRulesContainer *aclRulesContainer;

@end

@implementation ACLTableViewController

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


    UIBarButtonItem *optionsBtn = [[UIBarButtonItem alloc] initWithTitle:@"+" style:UIBarButtonItemStylePlain target:self action:@selector(didTouUpInsideOptionsBtn:)];
    self.navigationItem.rightBarButtonItem = optionsBtn;

    self.refreshControl = [[UIRefreshControl alloc]init];

    self.refreshControl.tintColor = [UIColor blueColor];

    [self.refreshControl addTarget:self action:@selector(refresh:) forControlEvents:UIControlEventValueChanged];

    [self retreiveACL];
}

-(void)refresh:(UIRefreshControl *)refresh
{
    [self retreiveACL];

    [self.refreshControl endRefreshing];
}

-(void)retreiveACL
{
    self.title = [self.acl aclName];

    QStatus status;
    AJGWCConnectorCapabilities* localConnectorCapabilities;
    status = [self.connectorApp retrieveConnectorCapabilitiesUsingSessionId:self.sessionId connectorCapabilities:&localConnectorCapabilities];
    self.connectorCapabilities = localConnectorCapabilities;
    if (ER_OK != status) {
        [AppDelegate AlertAndLog:@"Failed to retrieve manifest rules" status:status];
        return;
    }

    self.aclRulesContainer = [[AclRulesContainer alloc]initWithACL:self.acl UsingSessionId:self.sessionId connectorCapabilities:self.connectorCapabilities announcements:[[AnnouncementManager sharedInstance] getAnnouncements] status:status];

    if (status != ER_OK) {
        [AppDelegate AlertAndLog:@"Failed to retrieve Acl" status:status];
        self.navigationItem.rightBarButtonItem = nil;
        return;
    }

    [self.tableView reloadData];
}

- (void)didTouUpInsideOptionsBtn:(id)sender {
    UIActionSheet *optionsActionSheet = [[UIActionSheet alloc] initWithTitle:@"" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:
                                         @"Update ACL",
                                         @"Rename ACL",
                                         @"Show ACL ID",
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
                {
                    QStatus status = [self update];
                    if (status != ER_OK) {
                        [[[UIAlertView alloc]initWithTitle:@"Failed to Update ACL" message:[NSString stringWithFormat:@"error:%@",[AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
                    }
                }
                    break;
                case 1: //rename ACL
                {
                    UIAlertView *renameAclAlertView = [[UIAlertView alloc] initWithTitle:[NSString stringWithFormat:@"Rename '%@':" , [self.acl aclName]] message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
                    renameAclAlertView.alertViewStyle = UIAlertViewStylePlainTextInput;
                    [renameAclAlertView textFieldAtIndex:0].text = [self.acl aclName];
                    [renameAclAlertView show];
                }
                    break;
                case 2: //show ACL Id
                {
                    [[[UIAlertView alloc]initWithTitle:@"Info" message:[NSString stringWithFormat:@"Selected ACL Id: '%@'",[self.acl aclId] ] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
                }
                    break;
                case 3: // refresh
                {
                    self.tableView.contentOffset = CGPointMake(0, -(self.refreshControl.frame.size.height*2));
                    [self.refreshControl beginRefreshing];
                    [self refresh:nil];

                }
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

- (void)PrintObjectDescriptions:(AJGWCRuleObjectDescription *)objDesc
{
    NSMutableString *interfaces = [[NSMutableString alloc]init];

    for (AJGWCRuleInterface *interface in [objDesc interfaces]) {
        [interfaces appendString:[interface interfaceName]];
    }
    NSLog(@"        ObjPath:%@ - interfaces are[%@]",[[objDesc objectPath] path], interfaces);
}


- (QStatus)update
{
    QStatus status;
    AJGWCConnectorCapabilities *connectorCapabilities;
    status = [self.connectorApp retrieveConnectorCapabilitiesUsingSessionId:self.sessionId connectorCapabilities:&connectorCapabilities];

    if (status != ER_OK) {
        NSLog(@"retrieveConnectorCapabilitiesUsingSessionId failed:%@",[AJNStatus descriptionForStatusCode:status]);
        return status;
    }

    AJGWCAclRules *gwAclRules = [self.aclRulesContainer createAJGWCAclRules];

    AJGWCAclWriteResponse *response;
    status = [self.acl update:self.sessionId aclRules:gwAclRules connectorCapabilities:connectorCapabilities aclWriteResponse:&response];

    if (status != ER_OK) {
        NSLog(@"AJGWCAcl update failed:%@",[AJNStatus descriptionForStatusCode:status]);
        return status;
    }

    if ([response.invalidRules.remotedApps count] > 0) {
        NSArray *invalidRemotedApps = response.invalidRules.remotedApps;

        NSLog(@"found %d invalid remoted app rules:", [invalidRemotedApps count]);

        for (AJGWCRemotedApp *invalidRemotedApp in invalidRemotedApps) {
            NSLog(@"    AppName:%@",[invalidRemotedApp appName]);
            for (AJGWCRuleObjectDescription *objDesc in [invalidRemotedApp ruleObjDescriptions]) {
                [self PrintObjectDescriptions:objDesc];
            }
        }
    }

    if ([response.invalidRules.exposedServices count] > 0) {
        NSLog(@"found invalid exposed services rules");
        for (AJGWCRuleObjectDescription *objDesc in response.invalidRules.exposedServices) {
            [self PrintObjectDescriptions:objDesc];
        }
    }

    if (response.responseCode != GW_ACL_RC_SUCCESS) {
        NSLog(@"update ACL failed with code:%d",response.responseCode);
    }


    return status;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return [self.aclRulesContainer numberOfEntries] ; //we add one for the exposed services
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    VisualAclRules *rules =[self.aclRulesContainer aclRulesForSection:section];

    NSDictionary *dict = [rules aclRulesDictionary];

    NSArray *aclRulesKeys = [dict allKeys];

    return [ aclRulesKeys count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    InterfaceRuleTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"InterfaceRuleCell" forIndexPath:indexPath];

    NSUInteger section = [indexPath section];

    cell.rules = [self.aclRulesContainer aclRulesForSection:section];

    cell.indexPath = indexPath;

    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    NSString *entryName = [self.aclRulesContainer entryNameAt:section];

    return entryName;
}

- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(id)sender
{
    if ([identifier isEqual:@"ObjectPathSegue"]) {
    NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];

    VisualAclRules *rulesForSection = [self.aclRulesContainer aclRulesForSection:[indexPath section]];

    NSArray *object_paths = [rulesForSection objectPathsForInterface:[rulesForSection.aclRulesDictionary allKeys][[indexPath row]]                         ];

    if ([object_paths count] == 0) {
        NSLog(@"error, object path array for interface is 0");

        [[[UIAlertView alloc]initWithTitle:@"no object paths" message:@"error, object path array for interface is 0" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];

        return NO;

    }

    return YES;
    }

    return NO;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    if ([segue.destinationViewController isKindOfClass:[ObjectPathLevelRulesTableViewController class]]){
        NSIndexPath *indexPath = [self.tableView indexPathForSelectedRow];

        VisualAclRules *rulesForSection = [self.aclRulesContainer aclRulesForSection:[indexPath section]];

        ObjectPathLevelRulesTableViewController *vc = segue.destinationViewController;

        vc.aclRules = rulesForSection;

        vc.key = [rulesForSection.aclRulesDictionary allKeys][[indexPath row]];
    }

}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (void) updateConfigButton:(UIButton *)configButton configured:(BOOL)configured
{
    if (configured) {
        [configButton setTitle:@"Added" forState:UIControlStateNormal];
    } else {
        [configButton setTitle:@"Add All" forState:UIControlStateNormal];
    }
}

- (void) serviceButtonCliecked:(UIButton *) sender {
    VisualAclRules *rulesForSection = [self.aclRulesContainer aclRulesForSection:sender.tag];

    [rulesForSection switchAllAclRules];

    [self updateConfigButton:sender configured:[rulesForSection configured]];

    [self.tableView reloadData];

}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {

    VisualAclRules *rulesForSection = [self.aclRulesContainer aclRulesForSection:section];

    CGRect frame = tableView.frame;
    NSString *titleTxt = @"Add All";

    CGSize sizeOfTitle = [titleTxt sizeWithAttributes:@{NSFontAttributeName:
                                                            [UIFont systemFontOfSize:17.0f]}];

    UIButton *configButton = [UIButton buttonWithType: UIButtonTypeSystem];
    [configButton setFrame:CGRectMake(frame.size.width-sizeOfTitle.width - 10 , 10, 50, 30)];

    [configButton setTag:section];
    [configButton addTarget: self  action:@selector(serviceButtonCliecked:) forControlEvents: UIControlEventTouchUpInside];

    [self updateConfigButton:configButton configured:rulesForSection.configured];

    UILabel *title = [[UILabel alloc] initWithFrame:CGRectMake(10, 10, 150, 30)];
    title.text = [self tableView:tableView titleForHeaderInSection:section];

    UIView *headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, frame.size.width, frame.size.height)];
    [headerView addSubview:title];
    [headerView addSubview:configButton];

    return headerView;
}

#pragma mark - UIAlertViewDelegate method
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    // rename ACL
    UITextField *aclNameTextField = [alertView textFieldAtIndex:0];
    if ([aclNameTextField.text isEqualToString:[self.acl aclName]]) {
        return;
    }

    NSLog(@"renaming '%@' to '%@'", [self.acl aclName], aclNameTextField.text);
    [self.acl setAclName:aclNameTextField.text];

    [self update];
    [self.refreshControl beginRefreshing];
    [self refresh:nil];
}
@end
