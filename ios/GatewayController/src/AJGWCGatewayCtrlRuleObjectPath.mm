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

#import "AJGWCGatewayCtrlRuleObjectPath.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJGWCGatewayCtrlRuleObjectPath ()

@property (nonatomic) ajn::gwcontroller::GatewayCtrlRuleObjectPath* handle;

@end

@implementation AJGWCGatewayCtrlRuleObjectPath

- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlRuleObjectPath *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwcontroller::GatewayCtrlRuleObjectPath *)handle;
    }
    return self;
}

- (id)initWithObjectPath:(NSString*) objectPath friendlyName:(NSString*) friendlyName isPrefix:(bool) isPrefix isPrefixAllowed:(bool)isPrefixAllowed
{
    self = [super init];
    if (self) {
        self.handle = new ajn::gwcontroller::GatewayCtrlRuleObjectPath([AJNConvertUtil convertNSStringToQCCString:objectPath], [AJNConvertUtil convertNSStringToQCCString:friendlyName], isPrefix, isPrefixAllowed);
    }
    return self;
}

- (NSString*)path
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getPath()];
}

- (NSString*)friendlyName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getFriendlyName()];
}

- (bool)isPrefix
{
    return self.handle->isPrefix();
}

- (void)setPrefix:(bool) isPrefix
{
    self.handle->setPrefix(isPrefix);
}

- (bool)isPrefixAllowed
{
    return self.handle->isPrefixAllowed();
}

- (void)setPrefixAllowed:(bool)isPrefixAllowed
{
    self.handle->setPrefixAllowed(isPrefixAllowed);
}

- (ajn::gwcontroller::GatewayCtrlRuleObjectPath*)handle
{
    return _handle;
}

@end
