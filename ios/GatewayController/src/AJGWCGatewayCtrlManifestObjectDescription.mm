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

#import "AJGWCGatewayCtrlManifestObjectDescription.h"
#import "AJGWCGatewayCtrlTPInterface.h"

@interface AJGWCGatewayCtrlManifestObjectDescription ()

@property (nonatomic) ajn::services::GatewayCtrlManifestObjectDescription* handle;

@end

@implementation AJGWCGatewayCtrlManifestObjectDescription

- (id)initWithHandle:(ajn::services::GatewayCtrlManifestObjectDescription *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlManifestObjectDescription *)handle;
    }
    return self;
}

- (id)initWithObjectPath:(AJGWCGatewayCtrlTPObjectPath*) objectPath interfaces:(NSSet*) interfaces
{
    self = [super init];
    std::set<ajn::services::GatewayCtrlTPInterface> interfacesSet;
    
	if (self) {
        // Populate std::set with NSArray data
        for (AJGWCGatewayCtrlTPInterface* tpInterface in interfaces) {
            interfacesSet.insert(*[tpInterface handle]);
        }
        
		self.handle = new ajn::services::GatewayCtrlManifestObjectDescription(*[objectPath handle], interfacesSet);
	}
	return self;
}

- (id)initWithObjectPath:(AJGWCGatewayCtrlTPObjectPath*) objectPath interfaces:(NSSet*) interfaces isConfigured:(bool) isConfigured
{
    self = [super init];
    std::set<ajn::services::GatewayCtrlTPInterface> interfacesSet;
	if (self) {
        // Populate std::set with NSSet data
        for (AJGWCGatewayCtrlTPInterface* tpInterface in interfaces) {
            interfacesSet.insert(*[tpInterface handle]);
        }
		self.handle = new ajn::services::GatewayCtrlManifestObjectDescription(*[objectPath handle], interfacesSet, isConfigured);
	}
	return self;
}

- (AJGWCGatewayCtrlTPObjectPath*)objectPath
{
    return [[AJGWCGatewayCtrlTPObjectPath alloc] initWithHandle:self.handle->GetObjectPath()];
}

- (NSSet*)interfaces
{
    NSMutableSet* interfacesSet = [[NSMutableSet alloc] init];
    const std::set<ajn::services::GatewayCtrlTPInterface>* interfacesVect = self.handle->GetInterfaces();
    
    // Populate NSArray with std::set data
    for (std::set<ajn::services::GatewayCtrlTPInterface>::const_iterator it = (*interfacesVect).begin(); it != (*interfacesVect).end(); it++) {
        [interfacesSet addObject:[[AJGWCGatewayCtrlTPInterface alloc] initWithHandle:*it]];
    }
    
    return interfacesSet;
}

- (bool)isConfigured
{
    return self.handle->IsConfigured();
}

- (void)setConfigured:(bool) configured
{
    self.handle->SetConfigured(configured);
}

- (ajn::services::GatewayCtrlManifestObjectDescription*)handle
{
    return _handle;
}
@end