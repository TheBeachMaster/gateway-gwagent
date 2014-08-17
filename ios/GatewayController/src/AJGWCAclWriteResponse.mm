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

#import "AJGWCAclWriteResponse.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJGWCAclWriteResponse ()

@property (nonatomic) ajn::gwc::AclWriteResponse* handle;

@end

@implementation AJGWCAclWriteResponse

- (id)initWithHandle:(ajn::gwc::AclWriteResponse *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::AclWriteResponse *)handle;
    }
    return self;
}

- (id)initWithAclId:(NSString*) aclId code:(AJGWCAclResponseCode) code invalidRules:(AJGWCAclRules *) invalidRules objPath:(NSString*) objPath
{
    self = [super init];
    if (self) {
        self.handle = new ajn::gwc::AclWriteResponse([AJNConvertUtil convertNSStringToQCCString:aclId], (ajn::gwc::AclResponseCode)code, [invalidRules handle], [AJNConvertUtil convertNSStringToQCCString:objPath]);
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

- (AJGWCAclRules*)invalidRules
{
    return [[AJGWCAclRules alloc] initWithHandle:self.handle->getInvalidRules()];
}

- (NSString*)objectPath
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}


- (ajn::gwc::AclWriteResponse*)handle
{
    return _handle;
}
@end
