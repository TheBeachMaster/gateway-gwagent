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

#import "AJGWCGatewayCtrlManifestRules.h"
//#import "AJNMessageArgument.h"
#import "AJGWCGatewayCtrlManifestObjectDescription.h"

@interface AJGWCGatewayCtrlManifestRules ()

@property (nonatomic) ajn::services::GatewayCtrlManifestRules* handle;

@end

@implementation AJGWCGatewayCtrlManifestRules

- (id)initWithHandle:(ajn::services::GatewayCtrlManifestRules *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlManifestRules *)handle;
    }
    return self;
    
}

//- (id)initWithManifestRules:(AJNMessageArgument*) manifRules
//{
//    self = [super init];
//	if (self) {
//		self.handle = new ajn::services::GatewayCtrlManifestRules((ajn::MsgArg*)manifRules.handle);
//	}
//	return self;
//}

- (NSArray*)exposedServices
{
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> exposedServicesVect = self.handle->GetExposedServices();
    NSMutableArray* exposedServicesArray = [[NSMutableArray alloc] init];
    
    // Populate NSMutableArray with std::vector data;
    for (std::vector<ajn::services::GatewayCtrlManifestObjectDescription*>::const_iterator vectIt = exposedServicesVect.begin(); vectIt != exposedServicesVect.end(); vectIt++) {
        [exposedServicesArray addObject:[[AJGWCGatewayCtrlManifestObjectDescription alloc] initWithHandle:*vectIt]];
    }
    
    return exposedServicesArray;
}

- (NSArray*)remotedServices
{
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> remotedServicesVect = self.handle->GetRemotedServices();
    
    NSMutableArray* remotedServicesArray = [[NSMutableArray alloc] init];;
    // Populate NSMutableArray with std::vector data
    for (std::vector<ajn::services::GatewayCtrlManifestObjectDescription*>::const_iterator vectIt = remotedServicesVect.begin(); vectIt != remotedServicesVect.end(); vectIt++) {
        [remotedServicesArray addObject:[[AJGWCGatewayCtrlManifestObjectDescription alloc] initWithHandle:*vectIt]];
    }
    return remotedServicesArray;
}

- (ajn::services::GatewayCtrlManifestRules*)handle
{
    return _handle;
}
@end