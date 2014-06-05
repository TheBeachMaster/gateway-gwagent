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
#import "alljoyn/gateway/GatewayCtrlManifestObjectDescription.h"

@interface AJGWCGatewayCtrlTPObjectPath : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlTPObjectPath object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlTPObjectPath *) handle;

/**
 * Constructor
 * @param objectPath AllJoyn object identification
 * @param friendlyName The friendly name of the object path. This name may be presented
 * @param isPrefix TRUE if the object path is a prefix for the full object path
 * to the end user.
 */
- (id)initWithObjectPath:(NSString*) objectPath friendlyName:(NSString*) friendlyName isPrefix:(bool) isPrefix isPrefixAllowed:(bool)isPrefixAllowed;

/**
 * AllJoyn object identification
 * @return The value of the object path
 */
- (NSString*)path;

/**
 * Returns the friendly name  of the object path.
 * This name may be presented to the end user.
 * @return Object path friendly name
 */
- (NSString*)friendlyName;

/**
 * @return Returns whether this object path is a prefix
 */
- (bool)isPrefix;

/**
 * Set whether this object path is a prefix
 * @param isPrefix set the prefix flag to isPrefix
 */
- (void)setPrefix:(bool) isPrefix;

/**
 * @return Returns whether this object path prefix is allowed
 */
- (bool)isPrefixAllowed;

/**
 * Set whether this object path prefix is allowed to be set
 * @param isPrefixAllowed set the prefix flag permissions
 */
- (void)setPrefixAllowed:(bool) isPrefixAllowed;

/**
 * Returns the cpp handle of this class
 * @return GatewayCtrlAccessRules
 */
- (ajn::services::GatewayCtrlTPObjectPath*)handle;


@end