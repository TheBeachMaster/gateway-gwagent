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
#import "alljoyn/Status.h"
#import "AJGWCGatewayCtrlEnums.h"
#import "AJGWCGatewayCtrlAclWriteResponse.h"
#import "AJGWCGatewayCtrlAccessRules.h"
#import "AJGWCGatewayCtrlManifestRules.h"
#import "alljoyn/gateway/GatewayCtrlAccessControlList.h"

@interface AJGWCGatewayCtrlAccessControlList : NSObject

/* Constructor
 * @param handle A handle to a cpp GatewayCtrlAccessControlList object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlAccessControlList *)handle;

/**
 * Constructor
 * @param gwBusName The name of the gateway {@link AJNBusAttachment} hosting a Third Party Application
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
 * @return The name of the gateway {@link AJNBusAttachment} hosting a Third Party Application
 * that is related to this Access Control List
 */
- (NSString*)gwBusName;

/**
 * Activate the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link AJGWCAclResponseCode}
 */
- (AJGWCAclResponseCode)activate:(int) sessionId status:(QStatus&) status;


/**
 * Deactivate the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link AJGWCAclResponseCode}
 */
- (AJGWCAclResponseCode)deactivate:(int) sessionId status:(QStatus&) status;

/**
 * Sends request to update Access Control List with the received {@link AJGWCGatewayCtrlAccessRules}.
 * The {@link AJGWCGatewayCtrlAccessRules} are validated against the provided {@link AJGWCGatewayCtrlManifestRules}.
 * Only valid rules will be sent to update the ACL. The invalid rules could be received from the
 * returned {@link AJGWCGatewayCtrlAclWriteResponse} object.
 * @param sessionId The id of the session established with the gateway
 * @param accessRules The ACL access rules
 * @param manifestRules {@link AJGWCGatewayCtrlManifestRules} that is used for the {@link AJGWCAccessRules} creation
 * @param status return status of operation
 * @return {@link AJGWCGatewayCtrlAclWriteResponse}
 */
- (AJGWCGatewayCtrlAclWriteResponse*)updateAcl:(int) sessionId accessRules:(AJGWCGatewayCtrlAccessRules*) accessRules manifestRules:(AJGWCGatewayCtrlManifestRules*) manifestRules status:(QStatus&) status;

/**
 * Updates custom metadata of the Access Control List. The ACL metadata is rewritten following the
 * request.
 * @param sessionId The id of the session established with the gateway
 * @param metadata The metadata to update the ACL
 * @param status return status of operation
 * @return {@link AJGWCAclResponseCode}
 */
- (AJGWCAclResponseCode)updateCustomMetadata:(int) sessionId metadata:(NSDictionary*) metadata status:(QStatus&) status;

/**
 * Updates metadata of the internal Access Control List. The ACL metadata is rewritten following the
 * request.
 * @param sessionId The id of the session established with the gateway
 * @param metadata The metadata to update the ACL
 * @param status return status of operation
 * @return {@link AJGWCAclResponseCode}
 */
- (AJGWCAclResponseCode)updateAclMetadata:(int) sessionId metadata:(NSDictionary*) metadata status:(QStatus&) status;

/**
 * Return the current state of the {@link AJGWCGatewayCtrlAccessControlList}
 * @return {@link AJGWCAclStatus}
 */
- (AJGWCAclStatus)status;

/**
 * Retrieve from the gateway status of the Access Control List
 * @param sessionId The id of the session established with the gateway
 * @param status return status of operation
 * @return {@link AJGWCAclStatus}
 */
- (AJGWCAclStatus)retrieveStatusUsingSessionId:(int) sessionId status:(QStatus&) status;

/**
 * Refreshes the {@link AJGWCAccessControlList} object and returns its {@link AJGWCGatewayCtrlAccessRules}
 * @param sessionId The id of the session established with the gateway
 * @param manifestRules {@link AJGWCGatewayCtrlManifestRules} that is used for the {@link AJGWCGatewayCtrlAccessRules} creation
 * @param announcements An array of {@link AJGWCAnnouncementData} objects with the current set of announcements in the network
 * @param status return status of operation
 * @return {@link AJGWCGatewayCtrlAccessRules}
 */
- (AJGWCGatewayCtrlAccessRules *)retrieveAclUsingSessionId:(int) sessionId manifestRules:(AJGWCGatewayCtrlManifestRules*) manifestRules announcements:(NSArray*) announcements status:(QStatus&) status;

@end
