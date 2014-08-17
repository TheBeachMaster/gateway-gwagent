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
#import "AJGWCEnums.h"
#import "AJGWCAclRules.h"
#import "alljoyn/gateway/AclWriteResponse.h"

@interface AJGWCAclWriteResponse : NSObject

/**
 * Constructor
 * @param handle A handle to a cpp AclWriteResponse object
 */
- (id)initWithHandle:(ajn::gwc::AclWriteResponse*) handle;

/**
 * Constructor
 * @param aclId The ACL id
 * @param code {@link AJGWCAclResponseCode}
 * @param invalidRules {@link AJGWCAclRules}
 * @param objPath Object path
 */
- (id)initWithAclId:(NSString*) aclId code:(AJGWCAclResponseCode) code invalidRules:(AJGWCAclRules *) invalidRules objPath:(NSString*) objPath;

/**
 * @return The id of the ACL that the write operation was referred to
 */
- (NSString*)aclId;

/**
 * @return {@link AJGWCAclResponseCode} of the ACL write action
 */
- (AJGWCAclResponseCode)responseCode;

/**
 * @return {@link AJGWCAclRules} with the rules that don't comply with the {@link AJGWCConnectorCapabilities}
 */
- (AJGWCAclRules*)invalidRules;

/**
 * @return object path
 */
- (NSString*)objectPath;

- (ajn::gwc::AclWriteResponse*)handle;
@end
