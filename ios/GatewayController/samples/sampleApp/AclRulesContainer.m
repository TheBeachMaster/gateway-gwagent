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
#import "alljoyn/gateway/AJGWCAclRules.h"
#import "alljoyn/gateway/AJGWCRemotedApp.h"

@interface AclRulesContainer ()

@property (strong,nonatomic) AJGWCAclRules *ajgwcAclRules;
@property (strong,nonatomic) NSMutableArray *aclRulesArray; // array of arrays of AclRules objects. position 0 is "Exposed Services" AclRules objects. each entry from 1 corresponds to a remotedApp from ajgwcAclRules

@end

@implementation AclRulesContainer
- (instancetype)initWithACL:(AJGWCAcl *)acl UsingSessionId:(AJNSessionId)sessionId connectorCapabilities:(AJGWCConnectorCapabilities *)connectorCapabilities announcements:(NSArray *)announcements status:(QStatus &)status
{
    self = [super init];
    if (self) {
        self.aclRulesArray = [[NSMutableArray alloc]init];

        AJGWCAclRules *tmpAclRules;
        status = [acl retrieveUsingSessionId:sessionId connectorCapabilities:connectorCapabilities announcements:announcements aclRules:&tmpAclRules];
        self.ajgwcAclRules = tmpAclRules;
        if (status != ER_OK) {
            return nil;
        }

        VisualAclRules *exposeServicesRules = [[VisualAclRules alloc]initWithArrayOfRuleObjectDescription:[self.ajgwcAclRules exposedServices]];

        self.aclRulesArray[0] = exposeServicesRules;

        int pos = 1;
        for (AJGWCRemotedApp *remotedApp in [self.ajgwcAclRules  remotedApps]) {
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
    return [self.ajgwcAclRules.remotedApps[pos-1] appName];
    }
}

- (VisualAclRules *)aclRulesForSection:(NSInteger)section;
{
    return self.aclRulesArray[section];
}

- (AJGWCRemotedApp *)findRemotedAppInAjgwcAclRules:(NSInteger)pos
{
    return self.ajgwcAclRules.remotedApps[pos -1];
}

- (AJGWCAclRules *)createAJGWCAclRules
{
    AJGWCAclRules *rules = nil;

    // Create a list of AJGWCRuleObjectDescription based on the configured exposed services rules
    NSArray *exposedServices;
    NSMutableArray *remotedApps = [[NSMutableArray alloc]init]; // array of AJGWCRemotedApp

    exposedServices = [((VisualAclRules *)_aclRulesArray[0]) createAJGWCRuleObjectDescriptions];

    for (NSInteger pos = 1; pos != [_aclRulesArray count]; pos++) {
        VisualAclRules *aclRules = _aclRulesArray[pos];

        NSArray *remotedServicesObjectDescriptions = [aclRules createAJGWCRuleObjectDescriptions];

        AJGWCRemotedApp *remotedApp = [[AJGWCRemotedApp alloc] initWithAnnouncedApp:[self findRemotedAppInAjgwcAclRules:pos] ruleObjDescriptions:&remotedServicesObjectDescriptions];

        remotedApps[pos-1] = remotedApp;

    }


    rules = [[AJGWCAclRules alloc] initWithExposedServices:exposedServices remotedApps:remotedApps];

    return rules;
}


@end
