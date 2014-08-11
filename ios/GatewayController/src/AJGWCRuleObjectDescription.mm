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

#import "AJGWCRuleObjectDescription.h"
#import "AJGWCRuleInterface.h"

@interface AJGWCRuleObjectDescription ()

@property (nonatomic) ajn::gwc::RuleObjectDescription* handle;

@end

@implementation AJGWCRuleObjectDescription

- (id)initWithHandle:(ajn::gwc::RuleObjectDescription *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::RuleObjectDescription *)handle;
    }
    return self;
}

- (id)initWithObjectPath:(AJGWCRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces
{
    self = [super init];
    std::set<ajn::gwc::RuleInterface> interfacesSet;

    if (self) {
        // Populate std::set with NSArray data
        for (AJGWCRuleInterface* connAppInterface in interfaces) {
            interfacesSet.insert(*[connAppInterface handle]);
        }

        self.handle = new ajn::gwc::RuleObjectDescription(*[objectPath handle], interfacesSet);
    }
    return self;
}

- (id)initWithObjectPath:(AJGWCRuleObjectPath*) objectPath interfaces:(NSSet*) interfaces isConfigured:(bool) isConfigured
{
    self = [super init];
    std::set<ajn::gwc::RuleInterface> interfacesSet;
    if (self) {
        // Populate std::set with NSSet data
        for (AJGWCRuleInterface* connAppInterface in interfaces) {
            interfacesSet.insert(*[connAppInterface handle]);
        }
        self.handle = new ajn::gwc::RuleObjectDescription(*[objectPath handle], interfacesSet, isConfigured);
    }
    return self;
}

- (AJGWCRuleObjectPath*)objectPath
{
    return [[AJGWCRuleObjectPath alloc] initWithHandle:self.handle->getObjectPath()];
}

- (NSSet*)interfaces
{
    NSMutableSet* interfacesSet = [[NSMutableSet alloc] init];
    const std::set<ajn::gwc::RuleInterface>* interfacesVect = self.handle->getInterfaces();

    // Populate NSArray with std::set data
    for (std::set<ajn::gwc::RuleInterface>::const_iterator it = (*interfacesVect).begin(); it != (*interfacesVect).end(); it++) {
        [interfacesSet addObject:[[AJGWCRuleInterface alloc] initWithHandle:*it]];
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

- (ajn::gwc::RuleObjectDescription*)handle
{
    return _handle;
}
@end
