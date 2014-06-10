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

#import "AJGWCGatewayCtrlTPApplicationStatus.h"
#import "alljoyn/about/AJNConvertUtil.h"
//#import "AJNMessageArgument.h"
#import "AJGWCGatewayCtrlEnums.h"

@interface AJGWCGatewayCtrlTPApplicationStatus ()

@property (nonatomic) ajn::services::GatewayCtrlTPApplicationStatus* handle;

@end

@implementation AJGWCGatewayCtrlTPApplicationStatus

- (id)initWithHandle:(ajn::services::GatewayCtrlTPApplicationStatus *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlTPApplicationStatus *)handle;
    }
    return self;
}

//- (id)initWithAppStatus:(AJNMessageArgument*) appStatus
//{
//    self = [super init];
//	if (self) {
//		self.handle = new ajn::services::GatewayCtrlTPApplicationStatus((ajn::MsgArg*)appStatus.handle);
//	}
//	return self;
//}

- (id)initWithInstallStatus:(AJGWCInstallStatus) installStatus installDescription:(NSString*) installDescription connectionStatus:(AJGWCConnectionStatus) connectionStatus operationalStatus:(AJGWCOperationalStatus) operationalStatus
{
    self = [super init];
	if (self) {
		self.handle = new ajn::services::GatewayCtrlTPApplicationStatus((ajn::services::InstallStatus)installStatus, [AJNConvertUtil convertNSStringToQCCString:installDescription], (ajn::services::ConnectionStatus)connectionStatus, (ajn::services::OperationalStatus)operationalStatus);
	}
	return self;
}

- (AJGWCInstallStatus)installStatus
{
    return (AJGWCInstallStatus)self.handle->GetInstallStatus();
}

- (NSString*)installDescriptions
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetInstallDescriptions()];
}

- (AJGWCConnectionStatus)connectionStatus
{
    return (AJGWCConnectionStatus)self.handle->GetConnectionStatus();
}

- (AJGWCOperationalStatus)operationalStatus
{
    return (AJGWCOperationalStatus)self.handle->GetOperationalStatus();
}

@end