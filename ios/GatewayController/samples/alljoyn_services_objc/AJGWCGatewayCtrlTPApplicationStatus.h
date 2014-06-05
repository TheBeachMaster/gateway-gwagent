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

#import <Foundation/Foundation.h>
//@class AJNMessageArgument;
#import "AJGWCGatewayCtrlEnums.h"
#import "alljoyn/gateway/GatewayCtrlTPApplicationStatus.h"

@interface AJGWCGatewayCtrlTPApplicationStatus : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlTPApplicationStatus object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlTPApplicationStatus*) handle;

/**
 * Constructor
 */
//- (id)initWithAppStatus:(AJNMessageArgument*) appStatus;

/**
 * Constructor
 * @param installStatus install status {@link AJGWCInstallStatus}
 * @param installDescription install description
 * @param connectionStatus connection status {@link AJGWCConnectionStatus}
 * @param operationalStatus operational status  {@link AJGWCOperationalStatus}
 */
- (id)initWithInstallStatus:(AJGWCInstallStatus) installStatus installDescription:(NSString*) installDescription connectionStatus:(AJGWCConnectionStatus) connectionStatus operationalStatus:(AJGWCOperationalStatus) operationalStatus;
/**
 * @return The installation status of the Third Party Application
 */
- (AJGWCInstallStatus)installStatus;

/**
 * @return The installation description of the Third Party Application
 */
- (NSString*)installDescriptions;

/**
 * @return Connection status of the Third Party Application to its cloud service
 */
- (AJGWCConnectionStatus)connectionStatus;

/**
 * @return The state whether the Third Party Application is running
 */
- (AJGWCOperationalStatus)operationalStatus;

@end
