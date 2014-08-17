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

#import "AJGWCGatewayCtrlRuleObjectDescription.h"
#import "AJGWCGatewayCtrlRuleInterface.h"

@interface AJGWCGatewayCtrlRuleObjectDescription ()

@property (nonatomic) ajn::gwcontroller::GatewayCtrlRuleObjectDescription* handle;

@end

@implementation AJGWCGatewayCtrlRuleObjectDescription

- (id)initWithHandle:(ajn::gwcontroller::GatewayCtrlRuleObjectDescription *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwcontroller::GatewayCtrlRuleObjectDescription *)handle;
    }
    return self;
}

- (id)initWithObjectPath:(AJGWCGatewayCtrlRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces
{
    self = [super init];
    std::set<ajn::gwcontroller::GatewayCtrlRuleInterface> interfacesSet;

    if (self) {
        // Populate std::set with NSArray data
        for (AJGWCGatewayCtrlRuleInterface* connAppInterface in interfaces) {
            interfacesSet.insert(*[connAppInterface handle]);
        }

        self.handle = new ajn::gwcontroller::GatewayCtrlRuleObjectDescription(*[objectPath handle], interfacesSet);
    }
    return self;
}

- (id)initWithObjectPath:(AJGWCGatewayCtrlRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces isConfigured:(bool) isConfigured
{
    self = [super init];
    std::set<ajn::gwcontroller::GatewayCtrlRuleInterface> interfacesSet;
    if (self) {
        // Populate std::set with NSSet data
        for (AJGWCGatewayCtrlRuleInterface* connAppInterface in interfaces) {
            interfacesSet.insert(*[connAppInterface handle]);
        }
        self.handle = new ajn::gwcontroller::GatewayCtrlRuleObjectDescription(*[objectPath handle], interfacesSet, isConfigured);
    }
    return self;
}

- (AJGWCGatewayCtrlRuleObjectPath*)objectPath
{
    return [[AJGWCGatewayCtrlRuleObjectPath alloc] initWithHandle:self.handle->getObjectPath()];
}

- (NSSet*)interfaces
{
    NSMutableSet* interfacesSet = [[NSMutableSet alloc] init];
    const std::set<ajn::gwcontroller::GatewayCtrlRuleInterface>* interfacesVect = self.handle->getInterfaces();

    // Populate NSArray with std::set data
    for (std::set<ajn::gwcontroller::GatewayCtrlRuleInterface>::const_iterator it = (*interfacesVect).begin(); it != (*interfacesVect).end(); it++) {
        [interfacesSet addObject:[[AJGWCGatewayCtrlRuleInterface alloc] initWithHandle:*it]];
    }

    return interfacesSet;
}

- (bool)isConfigured
{
    return self.handle->isConfigured();
}

- (void)setConfigured:(bool) configured
{
    self.handle->setConfigured(configured);
}

- (ajn::gwcontroller::GatewayCtrlRuleObjectDescription*)handle
{
    return _handle;
}
@end
