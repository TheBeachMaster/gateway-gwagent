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

#import "AJGWCGatewayMgmtApp.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCConnectorApp.h"
#import "AJGWCSessionListenerAdapter.h"
#import "AJNStatus.h"

@interface AJGWCGatewayMgmtApp ()

@property (nonatomic) ajn::gwc::GatewayMgmtApp* handle;
@property (nonatomic) AJGWCSessionListenerAdapter* adapter;

@end

@implementation AJGWCGatewayMgmtApp

- (void)dealloc
{
    delete self.adapter;
    self.adapter = NULL;
}

- (id)initWithHandle:(ajn::gwc::GatewayMgmtApp*)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::GatewayMgmtApp *)handle;
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
        self.handle = new ajn::gwc::GatewayMgmtApp();
        QStatus status = self.handle->init([AJNConvertUtil convertNSStringToQCCString:busUniqueName], aboutDataMap);
        if (status != ER_OK) {
            return nil;
        }
    }
    return self;
}

- (QStatus)retrieveConnectorApps:(AJNSessionId) sessionId connectorApps:(NSMutableArray*) connectorApps

{
    std::vector<ajn::gwc::ConnectorApp*> connectorAppsVect;
    QStatus status = self.handle->retrieveConnectorApps(sessionId, connectorAppsVect);

    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::gwc::ConnectorApp*>::const_iterator vectIt = connectorAppsVect.begin(); vectIt != connectorAppsVect.end(); vectIt++) {
        [connectorApps addObject:[[AJGWCConnectorApp alloc] initWithHandle:*vectIt]];
    }
    return status;
}

- (AJGWCSessionResult*)joinSession
{
    ajn::gwc::SessionResult ret = self.handle->joinSession();
    return [[AJGWCSessionResult alloc]initWithStatus:ret.m_status sid:ret.m_sid];
}

- (AJGWCSessionResult*)joinSession:(id<AJGWCSessionListener>) listener
{
    self.adapter = new AJGWCSessionListenerAdapter(listener);
    ajn::gwc::SessionResult ret = self.handle->joinSession(self.adapter);
    return [[AJGWCSessionResult alloc]initWithStatus:ret.m_status sid:ret.m_sid];
}

- (QStatus)joinSessionAsync:(id<AJGWCSessionListener>) listener
{
    self.adapter = new AJGWCSessionListenerAdapter(listener);
    return self.handle->joinSessionAsync(self.adapter);
}

- (QStatus)leaveSession
{
    return self.handle->leaveSession();
}

- (id<AJGWCSessionListener>)listener
{
    return self.adapter->getListener();
}

@end
