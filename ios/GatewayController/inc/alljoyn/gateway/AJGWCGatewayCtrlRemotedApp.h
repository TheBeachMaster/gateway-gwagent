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
#import "alljoyn/gateway/GatewayCtrlRemotedApp.h"
#import "AJGWCGatewayCtrlAnnouncedApp.h"
#import "AJGWCGatewayCtrlConnectorCapabilities.h"

@interface AJGWCGatewayCtrlRemotedApp : AJGWCGatewayCtrlAnnouncedApp

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlRemotedApp object
 */
- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlRemotedApp *) handle;

/**
 * Constructor
 * @param busUniqueName The name of the {@link AJNBusAttachment} of the remoted application
 * @param appName The name of the application
 * @param appId The application id
 * @param deviceName The name of the device
 * @param deviceId The device id
 * @param ruleObjDescriptions Configuration of the object paths and interfaces that are
 * used by the Connector App to reach this remoted application
 */
- (id)initWithBusUniqueName:(NSString*) busUniqueName appName:(NSString*) appName appId:(uint8_t*) appId deviceName:(NSString*) deviceName deviceId:(NSString*) deviceId ruleObjDescriptions:(NSArray**) ruleObjDescriptions;

/**
 * Constructor
 * @param aboutData The data sent with the Anno uncement
 * @param ruleObjDescriptions Configuration of the object paths and interfaces that are
 * used by the Connector App to reach this remoted application
 */
- (id)initWithAboutData:(NSDictionary*) aboutData ruleObjDescriptions:(NSArray*) ruleObjDescriptions;

/**
 * Constructor
 * @param announcedApp The {@link AJGWCGatewayCtrlAnnouncedApp} to be used to build this {@link AJGWCGatewayCtrlRemotedApp}
 * @param ruleObjDescriptions Configuration of the object paths and interfaces that are
 * used by the Connector App to reach this remoted application
 */
- (id)initWithAnnouncedApp:(AJGWCGatewayCtrlAnnouncedApp*) announcedApp ruleObjDescriptions:(NSArray**) ruleObjDescriptions;

/**
 * Configuration of the object paths and interfaces that are
 * used by the Connector App to reach this remoted application
 * @return List of {@link AJGWCGatewayCtrlRuleObjectDescription} objects
 */
- (NSArray*)ruleObjDescriptions;

/**
 * Returns the cpp handle of this class
 * @return GatewayCtrlRemotedApp
 */
- (ajn::gwcontroller::GatewayCtrlRemotedApp*)handle;
@end
