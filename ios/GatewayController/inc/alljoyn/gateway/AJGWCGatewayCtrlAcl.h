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
#import "AJGWCGatewayCtrlEnums.h"
#import "AJGWCGatewayCtrlAclWriteResponse.h"
#import "AJGWCGatewayCtrlAclRules.h"
#import "AJGWCGatewayCtrlConnectorCapabilities.h"
#import "alljoyn/gateway/GatewayCtrlAcl.h"

@interface AJGWCGatewayCtrlAcl : NSObject

/* Constructor
 * @param handle A handle to a cpp GatewayCtrlAcl object
 */
- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlAcl *)handle;

/**
 * Constructor
 * @param gwBusName The name of the gateway {@link AJNBusAttachment} hosting a Connector App
 * that is related to this ACL
 * @param aclInfo The ACL information
 */
//- (id)initWithGwBusName:(NSString*) gwBusName aclInfo:(AJNMessageArgument*) aclInfo;

/**
 * @return The name of the Access Control List
 */
- (NSString*)aclName;

/**
 * Set the name of the Access Control List
 * @param name The ACL name
 */
- (void)setAclName:(NSString*) name;

/**
 * @return Id of the Access Control List
 */
- (NSString*)aclId;

/**
 * @return Object path of the Access Control List
 */
- (NSString*)aclObjectPath;

/**
 * @return The name of the gateway {@link AJNBusAttachment} hosting a Connector App
 * that is related to this Access Control List
 */
- (NSString*)gwBusName;

/**
 * Activate the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param aclResponseCode A reference {@link AJGWCAclResponseCode}
 * @return status return status of operation
 */
- (QStatus)activateUsingSessionId:(AJNSessionId) sessionId aclResponseCode:(AJGWCAclResponseCode&) aclResponseCode;

/**
 * Deactivate the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param aclResponseCode A reference {@link AJGWCAclResponseCode}
 * @return status return status of operation
 */
- (QStatus)deactivateUsingSessionId:(AJNSessionId) sessionId aclResponseCode:(AJGWCAclResponseCode&) aclResponseCode;

/**
 * Sends request to update Access Control List with the received {@link AJGWCGatewayCtrlAclRules}.
 * The {@link AJGWCGatewayCtrlAclRules} are validated against the provided {@link AJGWCGatewayCtrlConnectorCapabilities}.
 * Only valid rules will be sent to update the ACL. The invalid rules could be received from the
 * returned {@link AJGWCGatewayCtrlAclWriteResponse} object.
 * @param sessionId The id of the session established with the gateway
 * @param aclRules The ACL access rules
 * @param connectorCapabilities {@link AJGWCGatewayCtrlConnectorCapabilities} that is used for the {@link AJGWCAclRules} creation
 * @param aclWriteResponse {@link AJGWCGatewayCtrlAclWriteResponse}
 * @return status return status of operation
 */
- (QStatus)update:(AJNSessionId) sessionId aclRules:(AJGWCGatewayCtrlAclRules*) aclRules connectorCapabilities:(AJGWCGatewayCtrlConnectorCapabilities*) connectorCapabilities aclWriteResponse:(AJGWCGatewayCtrlAclWriteResponse**) aclWriteResponse;

/**
 * Updates custom metadata of the Access Control List. The ACL metadata is rewritten following the
 * request.
 * @param sessionId The id of the session established with the gateway
 * @param metadata The metadata to update the ACL
 * @param aclResponseCode A reference {@link AJGWCAclResponseCode}
 * @return status return status of operation
 */
- (QStatus)updateCustomMetadata:(AJNSessionId) sessionId metadata:(NSDictionary*) metadata status:(AJGWCAclResponseCode&) aclResponseCode;

/**
 * Updates metadata of the internal Access Control List. The ACL metadata is rewritten following the
 * request.
 * @param sessionId The id of the session established with the gateway
 * @param metadata The metadata to update the ACL
 * @param aclResponseCode A reference {@link AJGWCAclResponseCode}
 * @return status return status of operation
 */
- (QStatus)updateMetadata:(AJNSessionId) sessionId metadata:(NSDictionary*) metadata status:(AJGWCAclResponseCode&) aclResponseCode;

/**
 * Return the current state of the {@link AJGWCGatewayCtrlAcl}
 * @return {@link AJGWCAclStatus}
 */
- (AJGWCAclStatus)status;

/**
 * Retrieve from the gateway status of the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param aclStatus {@link AJGWCAclStatus}
 * @return status return status of operation
 */
- (QStatus)retrieveStatusUsingSessionId:(AJNSessionId) sessionId aclStatus:(AJGWCAclStatus&) aclStatus;

/**
 * Refreshes the {@link AJGWCAcl} object and returns its {@link AJGWCGatewayCtrlAclRules}
 * @param sessionId The id of the session established with the gateway
 * @param connectorCapabilities {@link AJGWCGatewayCtrlConnectorCapabilities} that is used for the {@link AJGWCGatewayCtrlAclRules} creation
 * @param announcements An array of {@link AJGWCAnnouncementData} objects with the current set of announcements in the network
 * @param aclRules {@link AJGWCGatewayCtrlAclRules}
 * @return status return status of operation
 */
- (QStatus)retrieveUsingSessionId:(AJNSessionId) sessionId connectorCapabilities:(AJGWCGatewayCtrlConnectorCapabilities*) connectorCapabilities announcements:(NSArray*) announcements aclRules:(AJGWCGatewayCtrlAclRules**) aclRules;

@end
