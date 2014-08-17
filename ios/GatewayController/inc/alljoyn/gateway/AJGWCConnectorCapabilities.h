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
#import "alljoyn/gateway/ConnectorCapabilities.h"
//@class AJNMessageArgument;

/**
 * The manifest rules of the Connector App
 */
@interface AJGWCConnectorCapabilities : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp ConnectorCapabilities object
 */
- (id)initWithHandle:(ajn::gwc::ConnectorCapabilities *) handle;

/**
 * Constructor
 * @param manifRules
 */
//- (id)initWithConnectorCapabilities:(AJNMessageArgument*) manifRules;

/**
 * The {@link AJGWCRuleObjectDescription} objects that the Connector App
 * exposes to its clients
 * @return List of exposed services
 */
- (NSArray*)exposedServices;

/**
 * The {@link AJGWCRuleObjectDescription} objects that the Connector App
 * supports for being remoted
 * @return List of remoted interfaces
 */
- (NSArray*)remotedServices;

/**
 * Returns the cpp handle of this class
 * @return AclRules
 */
- (ajn::gwc::ConnectorCapabilities*)handle;

@end
