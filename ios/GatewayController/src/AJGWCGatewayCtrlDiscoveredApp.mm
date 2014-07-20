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

#import "AJGWCGatewayCtrlDiscoveredApp.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJGWCGatewayCtrlDiscoveredApp ()

@property (nonatomic) ajn::services::GatewayCtrlDiscoveredApp* handle;

@end

@implementation AJGWCGatewayCtrlDiscoveredApp

- (id)initWithBusName:(NSString*) busName appName:(NSString*) appName appId:(uint8_t*) appId deviceName:(NSString*) deviceName deviceId:(NSString*)deviceId
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::GatewayCtrlDiscoveredApp();
        self.handle->init([AJNConvertUtil convertNSStringToQCCString:busName],
                          [AJNConvertUtil convertNSStringToQCCString:appName],
                          appId,
                          [AJNConvertUtil convertNSStringToQCCString:deviceName],
                          [AJNConvertUtil convertNSStringToQCCString:deviceId]);
    }
    return self;
}

- (id)initWithBusName:(NSString*) busName aboutData:(NSDictionary*) aboutData
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
        self.handle = new ajn::services::GatewayCtrlDiscoveredApp();
        QStatus status = self.handle->init([AJNConvertUtil convertNSStringToQCCString:busName], aboutDataMap);
        if (status != ER_OK) {
            NSLog(@"Error initializing AJGWCGatewayCtrlDiscoveredApp");
            delete self.handle;
            return nil;
        }
    }
    return self;
}

- (NSString*)busName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getBusName()];
}

- (NSString*)appName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getAppName()];
}

- (uint8_t*)appId
{
    return self.handle->getAppId();
}

- (NSString*)deviceName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getDeviceName()];
}

- (NSString*)deviceId
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getDeviceId()];
}

- (ajn::services::GatewayCtrlDiscoveredApp*)handle
{
    return _handle;
}

@end
