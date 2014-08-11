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

#import "AJGWCConnectorAppStatus.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCEnums.h"

@interface AJGWCConnectorAppStatus ()

@property (nonatomic) ajn::gwc::ConnectorAppStatus* handle;

@end

@implementation AJGWCConnectorAppStatus

- (id)initWithHandle:(ajn::gwc::ConnectorAppStatus *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::ConnectorAppStatus *)handle;
    }
    return self;
}

//- (id)initWithAppStatus:(AJNMessageArgument*) appStatus
//{
//    self = [super init];
//    if (self) {
//        self.handle = new ajn::gwc::ConnectorAppStatus((ajn::MsgArg*)appStatus.handle);
//    }
//    return self;
//}

- (id)initWithInstallStatus:(AJGWCInstallStatus) installStatus installDescription:(NSString*) installDescription connectionStatus:(AJGWCConnectionStatus) connectionStatus operationalStatus:(AJGWCOperationalStatus) operationalStatus
{
    self = [super init];
    if (self) {
        self.handle = new ajn::gwc::ConnectorAppStatus();
        QStatus status = self.handle->init((ajn::gwc::InstallStatus)installStatus, [AJNConvertUtil convertNSStringToQCCString:installDescription], (ajn::gwc::ConnectionStatus)connectionStatus, (ajn::gwc::OperationalStatus)operationalStatus);

        if (status != ER_OK) {
            delete self.handle;
            self.handle = NULL;

            NSLog(@"Failed initializing ajn::gwc::ConnectorAppStatus");
            return nil;
        }


    }
    return self;
}

- (AJGWCInstallStatus)installStatus
{
    return (AJGWCInstallStatus)self.handle->getInstallStatus();
}

- (NSString*)installDescriptions
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getInstallDescriptions()];
}

- (AJGWCConnectionStatus)connectionStatus
{
    return (AJGWCConnectionStatus)self.handle->getConnectionStatus();
}

- (AJGWCOperationalStatus)operationalStatus
{
    return (AJGWCOperationalStatus)self.handle->getOperationalStatus();
}

@end
