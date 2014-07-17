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
#import "AJGWCGatewayCtrlManifestObjectDescription.h"


@interface AJGWCGatewayCtrlRemotedApp ()

@property (nonatomic) ajn::services::GatewayCtrlRemotedApp* handle;

@end

@implementation AJGWCGatewayCtrlRemotedApp

- (id)initWithHandle:(ajn::services::GatewayCtrlRemotedApp *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlRemotedApp *)handle;
    }
    return self;
}

- (id)initWithBusUniqueName:(NSString*) busUniqueName appName:(NSString*) appName appId:(uint8_t*) appId deviceName:(NSString*) deviceName deviceId:(NSString*) deviceId objDescRules:(NSArray**) objDescRules
{
    self = [super init];
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> objDescRulesVect;
    if (self) {
        // Populate std::vector with NSArray data
        for(AJGWCGatewayCtrlManifestObjectDescription* manifestObjDesc in *objDescRules) {
            objDescRulesVect.insert(objDescRulesVect.end(), [manifestObjDesc handle]);
        }
        self.handle = new ajn::services::GatewayCtrlRemotedApp();

        QStatus status = self.handle->init([AJNConvertUtil convertNSStringToQCCString:busUniqueName],
                                           [AJNConvertUtil convertNSStringToQCCString:appName],
                                           appId,
                                           [AJNConvertUtil convertNSStringToQCCString:deviceName],
                                           [AJNConvertUtil convertNSStringToQCCString:deviceId],
                                           objDescRulesVect);

        if (status!=ER_OK) {
            delete self.handle;
            self.handle = NULL;
            NSLog(@"failed init of GatewayCtrlRemotedApp");
            return nil;
        }


    }
    return self;
}

- (id)initWithAboutData:(NSDictionary*) aboutData objDescRules:(NSArray*) objDescRules
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
        std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> objDescRulesVect;
        // Populate std::vector with the NSArray data
        for(AJGWCGatewayCtrlManifestObjectDescription* manifestObjDesc in objDescRules) {
            objDescRulesVect.insert(objDescRulesVect.end(), [manifestObjDesc handle]);
        }
    }
    return self;
}

- (id)initWithDiscoveredApp:(AJGWCGatewayCtrlDiscoveredApp*) discoveredApp objDescRules:(NSArray**) objDescRules
{
    self = [super init];
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> objDescRulesVect;

    if (self) {
        // Populate std::vector with NSArray data
        for(AJGWCGatewayCtrlManifestObjectDescription* manifestObjDesc in *objDescRules) {
            objDescRulesVect.insert(objDescRulesVect.end(), [manifestObjDesc handle]);
        }
        self.handle = new ajn::services::GatewayCtrlRemotedApp();

        QStatus status = self.handle->init([discoveredApp handle], objDescRulesVect);

        if (status!=ER_OK) {
            delete self.handle;
            self.handle = NULL;

            NSLog(@"failed init of GatewayCtrlRemotedApp in initWithDiscoveredApp");
            return nil;
        }
    }
    return self;
}

- (NSArray*)objDescRules
{
    NSMutableArray* objDescRulesArray = [[NSMutableArray alloc] init];
    //std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> objDescRulesVect = self.handle->GetObjDescRules();
    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::services::GatewayCtrlManifestObjectDescription*>::const_iterator vectIt = self.handle->getObjDescRules().begin(); vectIt != self.handle->getObjDescRules().end(); vectIt++) {
        [objDescRulesArray addObject:[[AJGWCGatewayCtrlManifestObjectDescription alloc] initWithHandle:*vectIt]];
    }
    return objDescRulesArray;
}

- (ajn::services::GatewayCtrlRemotedApp*)handle
{
    return _handle;
}
@end
