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

#import "AccessRulesContainer.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlAccessRules.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlRemotedApp.h"

@interface AccessRulesContainer ()

@property (strong,nonatomic) AJGWCGatewayCtrlAccessRules *ajgwcGatewayCtrlAccessRules;
@property (strong,nonatomic) NSMutableArray *accessRulesArray; // array of arrays of AccessRules objects. position 0 is "Exposed Services" AccessRules objects. each entry from 1 corresponds to a remotedApp from ajgwcGatewayCtrlAccessRules

@end

@implementation AccessRulesContainer
- (instancetype)initWithACL:(AJGWCGatewayCtrlAccessControlList *)acl UsingSessionId:(AJNSessionId)sessionId manifestRules:(AJGWCGatewayCtrlManifestRules *)manifestRules announcements:(NSArray *)announcements status:(QStatus &)status
{
    self = [super init];
    if (self) {
        self.accessRulesArray = [[NSMutableArray alloc]init];
        
        self.ajgwcGatewayCtrlAccessRules = [acl retrieveAclUsingSessionId:sessionId manifestRules:manifestRules announcements:announcements status:status];
        
        if (status != ER_OK) {
            return nil;
        }

        VisualAccessRules *exposeServicesRules = [[VisualAccessRules alloc]initWithArrayOfManifestObjectDescription:[self.ajgwcGatewayCtrlAccessRules exposedServices]];
        
        self.accessRulesArray[0] = exposeServicesRules;

        int pos = 1;
        for (AJGWCGatewayCtrlRemotedApp *remotedApp in [self.ajgwcGatewayCtrlAccessRules  remotedApps]) {
            VisualAccessRules *remotedAppRules = [[VisualAccessRules alloc]initWithArrayOfManifestObjectDescription:[remotedApp objDescRules]];

            self.accessRulesArray[pos] = remotedAppRules;
            pos++;
        }
    }
    return self;
}

- (NSUInteger) numberOfEntries
{
    return [self.accessRulesArray count];
}

- (NSString *) entryNameAt:(NSUInteger)pos
{
    if (pos == 0) {
        return @"Exposed Services";
    } else {
    return [self.ajgwcGatewayCtrlAccessRules.remotedApps[pos-1] appName];
    }
}

- (VisualAccessRules *)accessRulesForSection:(NSInteger)section;
{
    return self.accessRulesArray[section];
}

- (AJGWCGatewayCtrlRemotedApp *)findRemotedAppInAjgwcGatewayCtrlAccessRules:(NSInteger)pos
{
    return self.ajgwcGatewayCtrlAccessRules.remotedApps[pos -1];
}

- (AJGWCGatewayCtrlAccessRules *)createAJGWCGatewayCtrlAccessRules
{
    AJGWCGatewayCtrlAccessRules *rules = nil;
    
    // Create a list of AJGWCGatewayCtrlManifestObjectDescription based on the configured exposed services rules
    NSArray *exposedServices;
    NSMutableArray *remotedApps = [[NSMutableArray alloc]init]; // array of AJGWCGatewayCtrlRemotedApp
    
    exposedServices = [((VisualAccessRules *)_accessRulesArray[0]) createAJGWCGatewayCtrlManifestObjectDescriptions];
    
    for (NSInteger pos = 1; pos != [_accessRulesArray count]; pos++) {
        VisualAccessRules *accessRules = _accessRulesArray[pos];
        
        NSArray *remotedServicesObjectDescriptions = [accessRules createAJGWCGatewayCtrlManifestObjectDescriptions];
            
        AJGWCGatewayCtrlRemotedApp *remotedApp = [[AJGWCGatewayCtrlRemotedApp alloc] initWithDiscoveredApp:[self findRemotedAppInAjgwcGatewayCtrlAccessRules:pos] objDescRules:&remotedServicesObjectDescriptions];
        
        remotedApps[pos-1] = remotedApp;

    }
    
    
    rules = [[AJGWCGatewayCtrlAccessRules alloc] initWithExposedServices:exposedServices remotedApps:remotedApps];
    
    return rules;
}


@end
