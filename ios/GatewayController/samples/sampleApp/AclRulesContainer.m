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

#import "AclRulesContainer.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlAclRules.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlRemotedApp.h"

@interface AclRulesContainer ()

@property (strong,nonatomic) AJGWCGatewayCtrlAclRules *ajgwcGatewayCtrlAclRules;
@property (strong,nonatomic) NSMutableArray *aclRulesArray; // array of arrays of AclRules objects. position 0 is "Exposed Services" AclRules objects. each entry from 1 corresponds to a remotedApp from ajgwcGatewayCtrlAclRules

@end

@implementation AclRulesContainer
- (instancetype)initWithACL:(AJGWCGatewayCtrlAcl *)acl UsingSessionId:(AJNSessionId)sessionId connectorCapabilities:(AJGWCGatewayCtrlConnectorCapabilities *)connectorCapabilities announcements:(NSArray *)announcements status:(QStatus &)status
{
    self = [super init];
    if (self) {
        self.aclRulesArray = [[NSMutableArray alloc]init];

        AJGWCGatewayCtrlAclRules *tmpGatewayCtrlAclRules;
        status = [acl retrieveUsingSessionId:sessionId connectorCapabilities:connectorCapabilities announcements:announcements aclRules:&tmpGatewayCtrlAclRules];
        self.ajgwcGatewayCtrlAclRules = tmpGatewayCtrlAclRules;
        if (status != ER_OK) {
            return nil;
        }

        VisualAclRules *exposeServicesRules = [[VisualAclRules alloc]initWithArrayOfRuleObjectDescription:[self.ajgwcGatewayCtrlAclRules exposedServices]];

        self.aclRulesArray[0] = exposeServicesRules;

        int pos = 1;
        for (AJGWCGatewayCtrlRemotedApp *remotedApp in [self.ajgwcGatewayCtrlAclRules  remotedApps]) {
            VisualAclRules *remotedAppRules = [[VisualAclRules alloc]initWithArrayOfRuleObjectDescription:[remotedApp ruleObjDescriptions]];

            self.aclRulesArray[pos] = remotedAppRules;
            pos++;
        }
    }
    return self;
}

- (NSUInteger) numberOfEntries
{
    return [self.aclRulesArray count];
}

- (NSString *) entryNameAt:(NSUInteger)pos
{
    if (pos == 0) {
        return @"Exposed Services";
    } else {
    return [self.ajgwcGatewayCtrlAclRules.remotedApps[pos-1] appName];
    }
}

- (VisualAclRules *)aclRulesForSection:(NSInteger)section;
{
    return self.aclRulesArray[section];
}

- (AJGWCGatewayCtrlRemotedApp *)findRemotedAppInAjgwcGatewayCtrlAclRules:(NSInteger)pos
{
    return self.ajgwcGatewayCtrlAclRules.remotedApps[pos -1];
}

- (AJGWCGatewayCtrlAclRules *)createAJGWCGatewayCtrlAclRules
{
    AJGWCGatewayCtrlAclRules *rules = nil;

    // Create a list of AJGWCGatewayCtrlRuleObjectDescription based on the configured exposed services rules
    NSArray *exposedServices;
    NSMutableArray *remotedApps = [[NSMutableArray alloc]init]; // array of AJGWCGatewayCtrlRemotedApp

    exposedServices = [((VisualAclRules *)_aclRulesArray[0]) createAJGWCGatewayCtrlRuleObjectDescriptions];

    for (NSInteger pos = 1; pos != [_aclRulesArray count]; pos++) {
        VisualAclRules *aclRules = _aclRulesArray[pos];

        NSArray *remotedServicesObjectDescriptions = [aclRules createAJGWCGatewayCtrlRuleObjectDescriptions];

        AJGWCGatewayCtrlRemotedApp *remotedApp = [[AJGWCGatewayCtrlRemotedApp alloc] initWithAnnouncedApp:[self findRemotedAppInAjgwcGatewayCtrlAclRules:pos] ruleObjDescriptions:&remotedServicesObjectDescriptions];

        remotedApps[pos-1] = remotedApp;

    }


    rules = [[AJGWCGatewayCtrlAclRules alloc] initWithExposedServices:exposedServices remotedApps:remotedApps];

    return rules;
}


@end
