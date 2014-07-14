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
#import "AJGWCGatewayCtrlConnAppObjectPath.h"
#import "AJGWCGatewayCtrlConnAppInterface.h"
#import "alljoyn/gateway/GatewayCtrlManifestObjectDescription.h"

@interface AJGWCGatewayCtrlManifestObjectDescription : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp GatewayCtrlManifestObjectDescription object
 */
- (id)initWithHandle:(ajn::services::GatewayCtrlManifestObjectDescription *) handle;

/**
 * Constructor
 * @param objectPath AllJoyn object identification
 * @param interfaces Set of the interfaces related to the given object path
 */
- (id)initWithObjectPath:(AJGWCGatewayCtrlConnAppObjectPath*) objectPath interfaces:(NSSet*) interfaces;

/**
 * Constructor
 * @param objectPath AllJoyn object identification {@link AJGWCGatewayCtrlConnAppObjectPath}
 * @param interfaces Set of the interfaces related to the given object path
 * @param isConfigured Set TRUE to permit this {@link AJGWCGatewayCtrlManifestObjectDescription} object path and interfaces
 * to be remoted by the Third Party Application
 */
- (id)initWithObjectPath:(AJGWCGatewayCtrlConnAppObjectPath*) objectPath interfaces:(NSSet*) interfaces isConfigured:(bool) isConfigured;

/**
 * The object path supported by the Service Provider Application manifest
 * @return Object path
 */
- (AJGWCGatewayCtrlConnAppObjectPath*)objectPath;

/**
 * The interfaces supported by the Service Provider Application manifest
 * @return Set of the {@link AJGWCGatewayCtrlConnAppInterface} objects
 */
- (NSSet*)interfaces;

/**
 * The flag has TRUE if this {@link AJGWCGatewayCtrlManifestObjectDescription} is configured
 * to permit the object path and interfaces to be remoted by the Third Party Application
 * @return configured state
 */
- (bool)isConfigured;

/**
 * Set TRUE to permit this {@link AJGWCGatewayCtrlManifestObjectDescription} object path and interfaces
 * to be remoted by the Third Party Application
 * @param configured configured state
 */
- (void)setConfigured:(bool) configured;

/**
 * Returns the cpp handle of this class
 * @return {@link ajn::services::GatewayCtrlManifestObjectDescription*}
 */
- (ajn::services::GatewayCtrlManifestObjectDescription*)handle;
@end
