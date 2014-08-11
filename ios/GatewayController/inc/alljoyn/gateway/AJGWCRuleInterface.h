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
#import "alljoyn/gateway/RuleObjectDescription.h"

@interface AJGWCRuleInterface : NSObject <NSCopying>

/**
 * Constructor
 * @param handle A handle to a cpp RuleInterface object
 */
- (id)initWithHandle:(ajn::gwc::RuleInterface) handle;

/**
 * Constructor
 * @param name AllJoyn name of the interface
 * @param friendlyName The interface friendly name
 * @param isSecured Whether the interface is secured
 */
- (id)initWithInterfaceName:(NSString*) name friendlyName:(NSString*) friendlyName  isSecured:(bool) isSecured;

/**
 * Returns a name of the interface.
 * @return interface name
 */
- (NSString*)interfaceName;

/**
 * Returns a friendly name of the interface.
 * The friendly name may be presented to the end user.
 * @return Interface friendly name
 */
- (NSString*)friendlyName;

/**
 * @return TRUE if the interface is secured
 */
- (bool)isSecured;

/**
 * Returns the cpp handle of this class
 * @return RuleInterface
 */
- (ajn::gwc::RuleInterface*)handle;
@end
