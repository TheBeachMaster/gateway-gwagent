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

#import "AJGWCAclRules.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCRuleObjectDescription.h"
#import "AJGWCRemotedApp.h"
#import "AJNStatus.h"

@interface AJGWCAclRules ()

@property (nonatomic) ajn::gwc::AclRules* handle;

@end

@implementation AJGWCAclRules

- (id)initWithHandle:(ajn::gwc::AclRules *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::gwc::AclRules *)handle;
    }
    return self;
}

- (id)initWithExposedServices:(NSArray*) exposedServices remotedApps:(NSArray*) remotedApps
{
    self = [super init];
    std::vector<ajn::gwc::RuleObjectDescription*> exposedServicesVect;
    std::vector<ajn::gwc::RemotedApp*> remotedAppsVect;

    if (self) {
        // Populate std::vector with the NSArray data
        for(AJGWCRuleObjectDescription* manifestObjDesc in exposedServices) {
            exposedServicesVect.insert(exposedServicesVect.end(), [manifestObjDesc handle]);
        }

        // Populate std::vector with the NSArray data
        for(AJGWCRemotedApp* remoteApp in remotedApps) {
            remotedAppsVect.insert(remotedAppsVect.end(), [remoteApp handle]);
        }

        self.handle = new ajn::gwc::AclRules();

        QStatus status = self.handle->init(exposedServicesVect, remotedAppsVect);

        if (status != ER_OK) {
            delete self.handle;
            self.handle = NULL;
            NSLog(@"Error: Failed to init AJGWCAclRules :%@", [AJNStatus descriptionForStatusCode:status]);
            return nil;
        }
    }


    return self;
}

-(NSArray*)remotedApps
{
    std::vector<ajn::gwc::RemotedApp*> remotedAppsVect = self.handle->getRemotedApps();
    NSMutableArray* remotedAppsArray = [[NSMutableArray alloc] init];

    // Populate NSArray with std::vector data
    for(std::vector<ajn::gwc::RemotedApp*>::iterator it = remotedAppsVect.begin(); it != remotedAppsVect.end(); ++it) {
        [remotedAppsArray addObject:[[AJGWCRemotedApp alloc] initWithHandle:*it]];
    }

    return remotedAppsArray;
}

-(NSArray*)exposedServices
{
    std::vector<ajn::gwc::RuleObjectDescription*> exposedServicesVect = self.handle->getExposedServices();
    NSMutableArray* exposedServicesArray = [[NSMutableArray alloc] init];

    // Populate NSArray with std::vector data
    for(std::vector<ajn::gwc::RuleObjectDescription*>::iterator it = exposedServicesVect.begin(); it != exposedServicesVect.end(); ++it) {
        [exposedServicesArray addObject:[[AJGWCRuleObjectDescription alloc] initWithHandle:*it]];
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
    self.handle->setMetadata(metadataMap);
}

- (NSString*)metadata:(NSString*) key
{
    qcc::String *value = self.handle->getMetadata([AJNConvertUtil convertNSStringToQCCString:key]);

    if (value) {
        return [AJNConvertUtil convertQCCStringtoNSString:*value];
    } else {
        return nil;
    }

}

- (NSDictionary*)metadata
{
    NSMutableDictionary* metadataDict = [[NSMutableDictionary alloc] init];
    std::map<qcc::String, qcc::String> metadataMap = self.handle->getMetadata();
    // Populate NSMutableDictionary with std::map data
    for (std::map <qcc::String, qcc::String>::iterator it = metadataMap.begin(); it != metadataMap.end(); ++it) {
        [metadataDict setValue:[AJNConvertUtil convertQCCStringtoNSString:it->second] forKey:[AJNConvertUtil convertQCCStringtoNSString:it->first]];
    }

    return metadataDict;
}

- (ajn::gwc::AclRules*)handle
{
    return _handle;
}

@end
