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

#import "AJGWCGatewayCtrlAccessRules.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCGatewayCtrlManifestObjectDescription.h"
#import "AJGWCGatewayCtrlRemotedApp.h"

@interface AJGWCGatewayCtrlAccessRules ()

@property (nonatomic) ajn::services::GatewayCtrlAccessRules* handle;

@end

@implementation AJGWCGatewayCtrlAccessRules

- (id)initWithHandle:(ajn::services::GatewayCtrlAccessRules *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlAccessRules *)handle;
    }
    return self;
}

- (id)initWithExposedServices:(NSArray*) exposedServices remotedApps:(NSArray*) remotedApps
{
    self = [super init];
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> exposedServicesVect;
    std::vector<ajn::services::GatewayCtrlRemotedApp*> remotedAppsVect;
	
    if (self) {
        // Populate std::vector with the NSArray data
        for(AJGWCGatewayCtrlManifestObjectDescription* manifestObjDesc in exposedServices) {
            exposedServicesVect.insert(exposedServicesVect.end(), [manifestObjDesc handle]);
        }
        
        // Populate std::vector with the NSArray data
        for(AJGWCGatewayCtrlRemotedApp* remoteApp in remotedApps) {
            remotedAppsVect.insert(remotedAppsVect.end(), [remoteApp handle]);
        }
	}
    
    self.handle = new ajn::services::GatewayCtrlAccessRules(exposedServicesVect, remotedAppsVect);
	return self;
}

-(NSArray*)remotedApps
{
    std::vector<ajn::services::GatewayCtrlRemotedApp*> remotedAppsVect = self.handle->GetRemotedApps();
    NSMutableArray* remotedAppsArray = [[NSMutableArray alloc] init];
    
    // Populate NSArray with std::vector data
    for(std::vector<ajn::services::GatewayCtrlRemotedApp*>::iterator it = remotedAppsVect.begin(); it != remotedAppsVect.end(); ++it) {
        [remotedAppsArray addObject:[[AJGWCGatewayCtrlRemotedApp alloc] initWithHandle:*it]];
    }
    
    return remotedAppsArray;
}

-(NSArray*)exposedServices
{
    std::vector<ajn::services::GatewayCtrlManifestObjectDescription*> exposedServicesVect = self.handle->GetExposedServices();
    NSMutableArray* exposedServicesArray = [[NSMutableArray alloc] init];
    
    // Populate NSArray with std::vector data
    for(std::vector<ajn::services::GatewayCtrlManifestObjectDescription*>::iterator it = exposedServicesVect.begin(); it != exposedServicesVect.end(); ++it) {
        [exposedServicesArray addObject:[[AJGWCGatewayCtrlManifestObjectDescription alloc] initWithHandle:*it]];
    }
    return exposedServicesArray;
}

- (void)updateMetadata:(NSDictionary*) metadata
{
    std::map<qcc::String, qcc::String> metadataMap;
    // Populate std::map with NSDictionary data
    for (NSString* key in metadata.allKeys) {
        metadataMap.insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], [AJNConvertUtil convertNSStringToQCCString:[metadata objectForKey:key]]));
    }
    self.handle->SetMetadata(metadataMap);
}

- (NSString*)metadata:(NSString*) key
{
    qcc::String *value = self.handle->GetMetadata([AJNConvertUtil convertNSStringToQCCString:key]);

    if (value) {
        return [AJNConvertUtil convertQCCStringtoNSString:*value];
    } else {
        return nil;
    }
    
}

- (NSDictionary*)metadata
{
    NSMutableDictionary* metadataDict = [[NSMutableDictionary alloc] init];
    std::map<qcc::String, qcc::String> metadataMap = self.handle->GetMetadata();
    // Populate NSMutableDictionary with std::map data
    for (std::map <qcc::String, qcc::String>::iterator it = metadataMap.begin(); it != metadataMap.end(); ++it) {
        [metadataDict setValue:[AJNConvertUtil convertQCCStringtoNSString:it->second] forKey:[AJNConvertUtil convertQCCStringtoNSString:it->first]];
    }
    
    return metadataDict;
}

- (ajn::services::GatewayCtrlAccessRules*)handle
{
    return _handle;
}

@end