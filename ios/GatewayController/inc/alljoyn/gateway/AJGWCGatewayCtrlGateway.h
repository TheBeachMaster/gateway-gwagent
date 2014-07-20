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
#import "alljoyn/Status.h"
#import "AJNSessionOptions.h"
#import "alljoyn/gateway/GatewayCtrlGateway.h"
#import "AJGWCGatewayCtrlDiscoveredApp.h"
#import "AJGWCGatewayCtrlGateway.h"
#import "AJGWCGatewayCtrlSessionResult.h"
#import "AJGWCGatewayCtrlControllerSessionListener.h"

@interface AJGWCGatewayCtrlGateway : AJGWCGatewayCtrlDiscoveredApp

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlGateway object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlGateway*) handle;

/**
 * Constructor
 * @param busUniqueName The name of the {@link AJNBusAttachment} of the gateway that sent
 * the Announcement
 * @param aboutData The data sent with the Announcement
 */
- (id)initWithbusUniqueName:(NSString*) busUniqueName aboutData:(NSDictionary *) aboutData;

/**
 * Retrieve the list of applications installed on the gateway identified by the given gwBusName
 * @param sessionId The id of the session established with the gateway
 * @param installedApps A reference to an array of The {@link AJGWCGatewayCtrlConnectorApplication} installed applications
 * @return status of operation
 */
- (QStatus)retrieveInstalledApps:(AJNSessionId) sessionId installedApps:(NSMutableArray*) installedApps;

/**
 * Join session synchronously with the given gateway identified by the gwBusName.
 * This method doesn't require {@link AJGWCGatewayCtrlControllerSessionListener}. Use this method
 * when there is no need to receive any session related event.
 * @return {@link AJGWCGatewayCtrlSessionResult}
 */
- (AJGWCGatewayCtrlSessionResult*)joinSession;

/**
 * Join session synchronously with the given gateway identified by the gwBusName.
 * The session related events will be sent to the given listener.
 * @param listener The listener is used to be notified about the session related events
 * @return {@link AJGWCGatewayCtrlSessionResult}
 */
- (AJGWCGatewayCtrlSessionResult*)joinSession:(id<AJGWCGatewayCtrlControllerSessionListener>) listener;

/**
 * Join session asynchronously with the given gwBusName.
 * @param listener The listener is used to be notified about the session related events
 * @return Returns the joinSessionAsync status {@link QStatus}
 */
- (QStatus)joinSessionAsync:(id<AJGWCGatewayCtrlControllerSessionListener>) listener;

/**
 * Disconnect the given session
 * @return Returns the leave session {@link QStatus}
 */
- (QStatus)leaveSession;

/**
 * Get the Listener defined for this SessionHandler
 * @return listener
 */
- (id<AJGWCGatewayCtrlControllerSessionListener>)listener;

@end
