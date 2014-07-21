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

#import "AJGWCGatewayCtrlGateway.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCGatewayCtrlConnectorApplication.h"
#import "AJGWCGatewayCtrlControllerSessionListenerAdapter.h"
#import "AJNStatus.h"

@interface AJGWCGatewayCtrlGateway ()

@property (nonatomic) ajn::services::GatewayCtrlGateway* handle;
@property (nonatomic) AJGWCGatewayCtrlControllerSessionListenerAdapter* adapter;

@end

@implementation AJGWCGatewayCtrlGateway

- (void)dealloc
{
    delete self.adapter;
    self.adapter = NULL;
}

- (id)initWithHandle:(ajn::services::GatewayCtrlGateway*)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlGateway *)handle;
    }
    return self;
}

- (id)initWithbusUniqueName:(NSString*) busUniqueName aboutData:(NSDictionary *) aboutData
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
        self.handle = new ajn::services::GatewayCtrlGateway([AJNConvertUtil convertNSStringToQCCString:busUniqueName], aboutDataMap);
    }
    return self;
}

- (NSArray*)retrieveInstalledApps:(AJNSessionId) sessionId status:(QStatus&) status
{
    NSMutableArray*  installedAppsArray = [[NSMutableArray alloc] init];
    std::vector<ajn::services::GatewayCtrlConnectorApplication*> installedAppsVect = self.handle->retrieveInstalledApps(sessionId, status);

    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::services::GatewayCtrlConnectorApplication*>::const_iterator vectIt = installedAppsVect.begin(); vectIt != installedAppsVect.end(); vectIt++) {
        [installedAppsArray addObject:[[AJGWCGatewayCtrlConnectorApplication alloc] initWithHandle:*vectIt]];
    }
    return installedAppsArray;
}

- (AJGWCGatewayCtrlSessionResult*)joinSession
{
    ajn::services::GatewayCtrlSessionResult ret = self.handle->joinSession();
    return [[AJGWCGatewayCtrlSessionResult alloc]initWithStatus:ret.m_status sid:ret.m_sid];
}

- (AJGWCGatewayCtrlSessionResult*)joinSession:(id<AJGWCGatewayCtrlControllerSessionListener>) listener
{
    self.adapter = new AJGWCGatewayCtrlControllerSessionListenerAdapter(listener);
    ajn::services::GatewayCtrlSessionResult ret = self.handle->joinSession(self.adapter);
    return [[AJGWCGatewayCtrlSessionResult alloc]initWithStatus:ret.m_status sid:ret.m_sid];
}

- (QStatus)joinSessionAsync:(id<AJGWCGatewayCtrlControllerSessionListener>) listener
{
    self.adapter = new AJGWCGatewayCtrlControllerSessionListenerAdapter(listener);
    return self.handle->joinSessionAsync(self.adapter);
}

- (QStatus)leaveSession
{
    return self.handle->leaveSession();
}

- (id<AJGWCGatewayCtrlControllerSessionListener>)listener
{
    return self.adapter->getListener();
}

@end
