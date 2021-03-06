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

#import "AJGWCConnectorCapabilities.h"
//#import "AJNMessageArgument.h"
#import "AJGWCRuleObjectDescription.h"

@interface AJGWCConnectorCapabilities ()

@property (nonatomic) ajn::gwc::ConnectorCapabilities* handle;

@end

@implementation AJGWCConnectorCapabilities

- (id)initWithHandle:(ajn::gwc::ConnectorCapabilities *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::ConnectorCapabilities *)handle;
    }
    return self;

}

//- (id)initWithConnectorCapabilities:(AJNMessageArgument*) manifRules
//{
//    self = [super init];
//    if (self) {
//        self.handle = new ajn::gwc::ConnectorCapabilities((ajn::MsgArg*)manifRules.handle);
//    }
//    return self;
//}

- (NSArray*)exposedServices
{
    std::vector<ajn::gwc::RuleObjectDescription*> exposedServicesVect = self.handle->getExposedServices();
    NSMutableArray* exposedServicesArray = [[NSMutableArray alloc] init];

    // Populate NSMutableArray with std::vector data;
    for (std::vector<ajn::gwc::RuleObjectDescription*>::const_iterator vectIt = exposedServicesVect.begin(); vectIt != exposedServicesVect.end(); vectIt++) {
        [exposedServicesArray addObject:[[AJGWCRuleObjectDescription alloc] initWithHandle:*vectIt]];
    }

    return exposedServicesArray;
}

- (NSArray*)remotedServices
{
    std::vector<ajn::gwc::RuleObjectDescription*> remotedServicesVect = self.handle->getRemotedServices();

    NSMutableArray* remotedServicesArray = [[NSMutableArray alloc] init];;
    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::gwc::RuleObjectDescription*>::const_iterator vectIt = remotedServicesVect.begin(); vectIt != remotedServicesVect.end(); vectIt++) {
        [remotedServicesArray addObject:[[AJGWCRuleObjectDescription alloc] initWithHandle:*vectIt]];
    }
    return remotedServicesArray;
}

- (ajn::gwc::ConnectorCapabilities*)handle
{
    return _handle;
}
@end
