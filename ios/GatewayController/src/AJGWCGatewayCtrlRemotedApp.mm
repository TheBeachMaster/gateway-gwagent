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

#import "AJGWCGatewayCtrlRemotedApp.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCGatewayCtrlRuleObjectDescription.h"


@interface AJGWCGatewayCtrlRemotedApp ()

@property (nonatomic) ajn::gwcontroller::GatewayCtrlRemotedApp* handle;

@end

@implementation AJGWCGatewayCtrlRemotedApp

- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlRemotedApp *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwcontroller::GatewayCtrlRemotedApp *)handle;
    }
    return self;
}

- (id)initWithBusUniqueName:(NSString*) busUniqueName appName:(NSString*) appName appId:(uint8_t*) appId deviceName:(NSString*) deviceName deviceId:(NSString*) deviceId ruleObjDescriptions:(NSArray**) ruleObjDescriptions
{
    self = [super init];
    std::vector<ajn::gwcontroller::GatewayCtrlRuleObjectDescription*> ruleObjDescriptionsVect;
    if (self) {
        // Populate std::vector with NSArray data
        for(AJGWCGatewayCtrlRuleObjectDescription* manifestObjDesc in *ruleObjDescriptions) {
            ruleObjDescriptionsVect.insert(ruleObjDescriptionsVect.end(), [manifestObjDesc handle]);
        }
        self.handle = new ajn::gwcontroller::GatewayCtrlRemotedApp();

        QStatus status = self.handle->init([AJNConvertUtil convertNSStringToQCCString:busUniqueName],
                                           [AJNConvertUtil convertNSStringToQCCString:appName],
                                           appId,
                                           [AJNConvertUtil convertNSStringToQCCString:deviceName],
                                           [AJNConvertUtil convertNSStringToQCCString:deviceId],
                                           ruleObjDescriptionsVect);

        if (status!=ER_OK) {
            delete self.handle;
            self.handle = NULL;
            NSLog(@"failed init of GatewayCtrlRemotedApp");
            return nil;
        }


    }
    return self;
}

- (id)initWithAboutData:(NSDictionary*) aboutData ruleObjDescriptions:(NSArray*) ruleObjDescriptions
{
    self = [super init];
    if (self) {
        ajn::services::AboutClient::AboutData aboutDataMap;
        //Populate AboutData with NSDictionary data
        for(NSString* key in aboutData.allKeys) {
            qcc::String aboutDataMapKey = [AJNConvertUtil convertNSStringToQCCString:key]; // key
            ajn::MsgArg* aboutDataMapVal = (ajn::MsgArg*)[[aboutData objectForKey:key] handle]; //value
            aboutDataMap.insert(std::make_pair(aboutDataMapKey, *aboutDataMapVal));
        }
        std::vector<ajn::gwcontroller::GatewayCtrlRuleObjectDescription*> ruleObjDescriptionsVect;
        // Populate std::vector with the NSArray data
        for(AJGWCGatewayCtrlRuleObjectDescription* manifestObjDesc in ruleObjDescriptions) {
            ruleObjDescriptionsVect.insert(ruleObjDescriptionsVect.end(), [manifestObjDesc handle]);
        }
    }
    return self;
}

- (id)initWithAnnouncedApp:(AJGWCGatewayCtrlAnnouncedApp*) announcedApp ruleObjDescriptions:(NSArray**) ruleObjDescriptions
{
    self = [super init];
    std::vector<ajn::gwcontroller::GatewayCtrlRuleObjectDescription*> ruleObjDescriptionsVect;

    if (self) {
        // Populate std::vector with NSArray data
        for(AJGWCGatewayCtrlRuleObjectDescription* manifestObjDesc in *ruleObjDescriptions) {
            ruleObjDescriptionsVect.insert(ruleObjDescriptionsVect.end(), [manifestObjDesc handle]);
        }
        self.handle = new ajn::gwcontroller::GatewayCtrlRemotedApp();

        QStatus status = self.handle->init([announcedApp handle], ruleObjDescriptionsVect);

        if (status!=ER_OK) {
            delete self.handle;
            self.handle = NULL;

            NSLog(@"failed init of GatewayCtrlRemotedApp in initWithAnnouncedApp");
            return nil;
        }
    }
    return self;
}

- (NSArray*)ruleObjDescriptions
{
    NSMutableArray* ruleObjDescriptionsArray = [[NSMutableArray alloc] init];
    //std::vector<ajn::gwcontroller::GatewayCtrlRuleObjectDescription*> ruleObjDescriptionsVect = self.handle->getRuleObjDesciptions();
    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::gwcontroller::GatewayCtrlRuleObjectDescription*>::const_iterator vectIt = self.handle->getRuleObjDesciptions().begin(); vectIt != self.handle->getRuleObjDesciptions().end(); vectIt++) {
        [ruleObjDescriptionsArray addObject:[[AJGWCGatewayCtrlRuleObjectDescription alloc] initWithHandle:*vectIt]];
    }
    return ruleObjDescriptionsArray;
}

- (ajn::gwcontroller::GatewayCtrlRemotedApp*)handle
{
    return _handle;
}
@end
