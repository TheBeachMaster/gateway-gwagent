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
#import "alljoyn/gateway/GatewayCtrlGatewayMgmtApp.h"
#import "AJGWCGatewayCtrlAnnouncedApp.h"
#import "AJGWCGatewayCtrlGatewayMgmtApp.h"
#import "AJGWCGatewayCtrlSessionResult.h"
#import "AJGWCGatewayCtrlSessionListener.h"

@interface AJGWCGatewayCtrlGatewayMgmtApp : AJGWCGatewayCtrlAnnouncedApp

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlGatewayMgmtApp object
 */
- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlGatewayMgmtApp*) handle;

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
 * @param connectorApps A reference to an array of The {@link AJGWCGatewayCtrlConnectorApp} installed applications
 * @return status of operation
 */
- (QStatus)retrieveConnectorApps:(AJNSessionId) sessionId connectorApps:(NSMutableArray*) connectorApps;

/**
 * Join session synchronously with the given gateway identified by the gwBusName.
 * This method doesn't require {@link AJGWCGatewayCtrlSessionListener}. Use this method
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
- (AJGWCGatewayCtrlSessionResult*)joinSession:(id<AJGWCGatewayCtrlSessionListener>) listener;

/**
 * Join session asynchronously with the given gwBusName.
 * @param listener The listener is used to be notified about the session related events
 * @return Returns the joinSessionAsync status {@link QStatus}
 */
- (QStatus)joinSessionAsync:(id<AJGWCGatewayCtrlSessionListener>) listener;

/**
 * Disconnect the given session
 * @return Returns the leave session {@link QStatus}
 */
- (QStatus)leaveSession;

/**
 * Get the Listener defined for this SessionHandler
 * @return listener
 */
- (id<AJGWCGatewayCtrlSessionListener>)listener;

@end
