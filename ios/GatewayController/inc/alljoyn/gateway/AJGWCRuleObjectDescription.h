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
#import "AJGWCRuleObjectPath.h"
#import "AJGWCRuleInterface.h"
#import "alljoyn/gateway/RuleObjectDescription.h"

@interface AJGWCRuleObjectDescription : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp RuleObjectDescription object
 */
- (id)initWithHandle:(ajn::gwc::RuleObjectDescription *) handle;

/**
 * Constructor
 * @param objectPath AllJoyn object identification
 * @param interfaces Set of the interfaces related to the given object path
 */
- (id)initWithObjectPath:(AJGWCRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces;

/**
 * Constructor
 * @param objectPath AllJoyn object identification {@link AJGWCRuleObjectPath}
 * @param interfaces Set of the interfaces related to the given object path
 * @param isConfigured Set TRUE to permit this {@link AJGWCRuleObjectDescription} object path and interfaces
 * to be remoted by the Connector App
 */
- (id)initWithObjectPath:(AJGWCRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces isConfigured:(bool) isConfigured;

/**
 * The object path supported by the Service Provider Application manifest
 * @return Object path
 */
- (AJGWCRuleObjectPath*)objectPath;

/**
 * The interfaces supported by the Service Provider Application manifest
 * @return Set of the {@link AJGWCRuleInterface} objects
 */
- (NSSet*)interfaces;

/**
 * The flag has TRUE if this {@link AJGWCRuleObjectDescription} is configured
 * to permit the object path and interfaces to be remoted by the Connector App
 * @return configured state
 */
- (bool)isConfigured;

/**
 * Set TRUE to permit this {@link AJGWCRuleObjectDescription} object path and interfaces
 * to be remoted by the Connector App
 * @param configured configured state
 */
- (void)setConfigured:(bool) configured;

/**
 * Returns the cpp handle of this class
 * @return {@link ajn::gwc::RuleObjectDescription*}
 */
- (ajn::gwc::RuleObjectDescription*)handle;
@end
