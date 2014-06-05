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
#import "alljoyn/gateway/GatewayCtrlTPApplication.h"
#import "AJNSessionOptions.h"
#import "AJGWCGatewayCtrlManifestRules.h"
#import "AJGWCGatewayCtrlAccessRules.h"
#import "AJGWCGatewayCtrlTPApplicationStatus.h"
#import "AJGWCGatewayCtrlEnums.h"
#import "AJGWCGatewayCtrlAclWriteResponse.h"
#import "AJGWCGatewayCtrlApplicationStatusSignalHandler.h"

@interface AJGWCGatewayCtrlTPApplication : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlTPApplication object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlTPApplication *) handle;

/**
 * Constructor
 * @param gwBusName The name of the gateway {@link AJNBusAttachment} the application is installed on
 * @param appObjPath The object path to reach the third party application on the gateway
 */
//- (id)initWithGwBusName:(NSString*) gwBusName appObjPath:(NSString*) appObjPath;

/**
 * Constructor
 * @param appInfo
 */
//- (id)initWithGwBusName:(NSString*) gwBusName appInfo:(AJNMessageArgument*) appInfo;

/**
 * @return gwBusName the {@link AJGWCGatewayCtrlTPApplication} is installed on
 */
- (NSString*)gwBusName;

/**
 * @return The id of the {@link AJGWCGatewayCtrlTPApplication}
 */
- (NSString*)appId;

/**
 * @return The name of the {@link AJGWCGatewayCtrlTPApplication}.
 */
- (NSString*)friendlyName;

/**
 * @return The object path to reach the application on the gateway
 */
- (NSString*)objectPath;

/**
 * @return The application version
 */
- (NSString*)appVersion;

/**
 * Retrieves the Manifest file of the application.
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return qcc::String representation of the Manifest file in XML format.
 */
- (NSString*)retrieveManifestFileUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;

/**
 * Retrieves the Manifest rules of the application
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link AJGWCGatewayCtrlManifestRules}
 */
- (AJGWCGatewayCtrlManifestRules*)retrieveManifestRulesUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;

/**
 * Retrieves the configurable rules of the application
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @param announcements Array of {@link AJGWCAnnouncementData} objects
 * @return {@link AJGWCGatewayCtrlAccessRules}
 */
- (AJGWCGatewayCtrlAccessRules*)retrieveConfigurableRulesUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status announcements:(NSArray*) announcements;

/**
 * Retrieves the state of the application
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link GatewayCtrlTPApplicationStatus}
 */
- (AJGWCGatewayCtrlTPApplicationStatus*)retrieveStatusUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;

/**
 * Restarts the application
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link AJGWCRestartStatus}
 */
- (AJGWCRestartStatus)restartUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;

/**
 * Set an {@link AJGWCGatewayCtrlApplicationStatusSignalHandler} to receive application
 * related events. In order to receive the events, in addition to calling this method,
 * a session should be successfully established with the gateway hosting the application.
 * Use {@link AJGWCGatewayCtrlTPApplication#unsetStatusChangedHandler()} to stop receiving the events.
 * @param handler Signal handler
 * @return {@link QStatus}
 */
- (QStatus)setStatusChangedHandler:(id<AJGWCGatewayCtrlApplicationStatusSignalHandler>) handler;

/**
 * Stop receiving Service Provider Application related signals
 */
- (void)unsetStatusChangedHandler;

/**
 * Sends request to create {@link AJGWCGatewayCtrlAccessControlList} object with the received name and
 * the {@link AJGWCGatewayCtrlAccessRules}. The {@link AJGWCGatewayCtrlAccessRules} are validated against the {@link AJGWCGatewayCtrlManifestRules}.
 * Only valid rules will be sent for the ACL creation. The invalid rules could be received from the
 * returned {@link AJGWCGatewayCtrlAclWriteResponse} object.
 * @param sessionId The id of the session established with the gateway
 * @param name The ACL name
 * @param accessRules The ACL access rules
 * @param status return status of operation
 * @return {@link AJGWCGatewayCtrlAclWriteResponse}
 */
- (AJGWCGatewayCtrlAclWriteResponse*)createAclUsingSessionId:(AJNSessionId) sessionId name:(NSString*) name accessRules:(AJGWCGatewayCtrlAccessRules*) accessRules status:(QStatus&) status;

/**
 * Retrieves a list of the Access Control Lists installed on the application
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return List of the {@link AJGWCGatewayCtrlAccessControlList}
 */
- (NSArray*)retrieveAclsUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;

/**
 * Delete the Access Control List of this application
 * @param sessionId The id of the session established with the gateway
 * @param aclId The id of the ACL to be deleted
 * @param status return status of operation
 * @return {@link AJGWCAclResponseCode}
 */
- (AJGWCAclResponseCode)deleteAclUsingSessionId:(AJNSessionId) sessionId aclId:(NSString*) aclId status:(QStatus&) status;

@end
