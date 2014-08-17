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
#import "alljoyn/gateway/GatewayCtrlAclRules.h"
#import "AJGWCGatewayCtrlConnectorCapabilities.h"

@interface AJGWCGatewayCtrlAclRules : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlAclRules object
 */
- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlAclRules*) handle;

/**
 * Constructor
 * @param exposedServices The interfaces that Connector App exposes to its clients
 * @param remotedApps The applications that may be reached by the Connector App
 * via the configured interfaces and object paths
 */
- (id)initWithExposedServices:(NSArray*) exposedServices  remotedApps:(NSArray*) remotedApps;

/**
 * The applications that may be reached by the Connector App
 * via the configured interfaces and object paths
 * @return List of the remoted applications
 */
- (NSArray*)remotedApps;

/**
 * The interfaces that Connector App exposes to its clients
 * @return List of exposed services
 */
- (NSArray*)exposedServices;

/**
 * Set the given metadata to the existing one
 * @param metadata
 */

/**
 * Update the metadata with the given data
 * @param metadata The metadata to update the Access Rules
 */
- (void)updateMetadata:(NSDictionary*) metadata;

/**
 * Returns metadata value for the given key
 * @param key The metadata key
 * @return metadata The Access Rules metadata for the given key
 */
- (NSString*)metadata:(NSString*) key;

/**
 * Returns current metadata object
 * @return metadata The Access Rules metadata
 */
- (NSDictionary*)metadata;

/**
 * Returns the cpp handle of this class
 * @return GatewayCtrlAclRules
 */
- (ajn::gwcontroller::GatewayCtrlAclRules*)handle;

@end
