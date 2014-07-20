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

#import "AJGWCGatewayCtrlAclWriteResponse.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJGWCGatewayCtrlAclWriteResponse ()

@property (nonatomic) ajn::services::GatewayCtrlAclWriteResponse* handle;

@end

@implementation AJGWCGatewayCtrlAclWriteResponse

- (id)initWithHandle:(ajn::services::GatewayCtrlAclWriteResponse *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlAclWriteResponse *)handle;
    }
    return self;
}

- (id)initWithAclId:(NSString*) aclId code:(AJGWCAclResponseCode) code invalidRules:(AJGWCGatewayCtrlAccessRules *) invalidRules objPath:(NSString*) objPath
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::GatewayCtrlAclWriteResponse([AJNConvertUtil convertNSStringToQCCString:aclId], (ajn::services::AclResponseCode)code, [invalidRules handle], [AJNConvertUtil convertNSStringToQCCString:objPath]);
    }
    return self;
}

- (NSString*)aclId
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getAclId()];
}

- (AJGWCAclResponseCode)responseCode
{
    return (AJGWCAclResponseCode)self.handle->getResponseCode();
}

- (AJGWCGatewayCtrlAccessRules*)invalidRules
{
    return [[AJGWCGatewayCtrlAccessRules alloc] initWithHandle:self.handle->getInvalidRules()];
}

- (NSString*)objectPath
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}


- (ajn::services::GatewayCtrlAclWriteResponse*)handle
{
    return _handle;
}
@end
