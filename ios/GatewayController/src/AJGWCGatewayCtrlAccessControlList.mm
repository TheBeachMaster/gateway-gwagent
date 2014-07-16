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

#import "AJGWCGatewayCtrlAccessControlList.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJGWCAnnouncementData.h"

@interface AJGWCGatewayCtrlAccessControlList ()

@property (nonatomic) ajn::services::GatewayCtrlAccessControlList* handle;

@end

@implementation AJGWCGatewayCtrlAccessControlList

- (id)initWithHandle:(ajn::services::GatewayCtrlAccessControlList *)handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlAccessControlList *)handle;
    }
    return self;
}

//- (id)initWithGwBusName:(NSString*) gwBusName aclInfo:(AJNMessageArgument*) aclInfo
//{
//    self = [super init];
//    if (self) {
//        self.handle = new ajn::services::GatewayCtrlAccessControlList([AJNConvertUtil convertNSStringToQCCString:gwBusName], (ajn::MsgArg*)aclInfo.handle);
//    }
//    return self;
//}

- (NSString*)aclName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getName()];
}

- (void)setAclName:(NSString*) name
{
    self.handle->setName([AJNConvertUtil convertNSStringToQCCString:name]);
}

- (NSString*)aclId
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getId()];
}

- (NSString*)aclObjectPath
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}

- (NSString*)gwBusName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getGwBusName()];
}

- (AJGWCAclResponseCode)activateUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    return (AJGWCAclResponseCode)self.handle->activate(sessionId, status);
}

- (AJGWCAclResponseCode)deactivateUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    return (AJGWCAclResponseCode)self.handle->deactivate(sessionId, status);
}

- (AJGWCGatewayCtrlAclWriteResponse*)updateAcl:(AJNSessionId) sessionId accessRules:(AJGWCGatewayCtrlAccessRules*) accessRules manifestRules:(AJGWCGatewayCtrlManifestRules*) manifestRules status:(QStatus&) status
{
    return [[AJGWCGatewayCtrlAclWriteResponse alloc] initWithHandle:self.handle->updateAcl(sessionId,
                                                                                           [accessRules handle],
                                                                                           [manifestRules handle],
                                                                                           status)];
}

- (AJGWCAclResponseCode)updateCustomMetadata:(AJNSessionId) sessionId metadata:(NSDictionary*) metadata status:(QStatus&) status
{
    std::map<qcc::String, qcc::String> metadataMap;
    // Populate std::map with NSDictionary data
    for (NSString* key in metadata.allKeys) {
        metadataMap.insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], [AJNConvertUtil convertNSStringToQCCString:[metadata objectForKey:key]]));
    }
    return (AJGWCAclResponseCode)self.handle->updateCustomMetadata(sessionId, metadataMap, status);
}
- (AJGWCAclResponseCode)updateAclMetadata:(AJNSessionId) sessionId metadata:(NSDictionary*) metadata status:(QStatus&) status
{
    std::map<qcc::String, qcc::String> metadataMap;
    // Populate std::map with NSDictionary data
    for (NSString* key in metadata.allKeys) {
        metadataMap.insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], [AJNConvertUtil convertNSStringToQCCString:[metadata objectForKey:key]]));
    }

    return (AJGWCAclResponseCode)self.handle->updateAclMetadata(sessionId, metadataMap, status);
}


- (AJGWCAclStatus)status
{
    return (AJGWCAclStatus)self.handle->getStatus();
}


- (AJGWCAclStatus)retrieveStatusUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status;
{
    return (AJGWCAclStatus)self.handle->retrieveStatus(sessionId, status);
}

- (AJGWCGatewayCtrlAccessRules *)retrieveAclUsingSessionId:(AJNSessionId) sessionId manifestRules:(AJGWCGatewayCtrlManifestRules*) manifestRules announcements:(NSArray*) announcements status:(QStatus&) status
{
    std::vector<ajn::services::AnnouncementData *>  announcementsVect;

    // Populate std::vector of AnnouncementData with NSArray of AJGWCAnnouncementData
    for(AJGWCAnnouncementData* announcementData in announcements)
    {
        // port
        uint16_t port = [announcementData port];

        // Populate AboutData (std::map<qcc::String, ajn::MsgArg> AboutData)
        NSDictionary* aboutDataDict = [announcementData aboutData];
        ajn::services::AboutClient::AboutData aboutDataMap;
        for(NSString* key in aboutDataDict.allKeys)
        {
            qcc::String aboutDataMapKey = [AJNConvertUtil convertNSStringToQCCString:key]; // key
            ajn::MsgArg* aboutDataMapVal = (ajn::MsgArg*)[[aboutDataDict objectForKey:key] handle]; //value
            aboutDataMap.insert(std::make_pair(aboutDataMapKey, *aboutDataMapVal));
        }

        // ObjectDescriptions (std::map<qcc::String, std::vector<qcc::String> > )
        NSDictionary* objectDescriptionsDict = [announcementData objectDescriptions];
        ajn::services::AboutClient::ObjectDescriptions objectDescriptionsMap;

        for(NSString* key in objectDescriptionsDict.allKeys)
        {
            std::vector<qcc::String> objDescVect;

            for (NSString* str in [objectDescriptionsDict objectForKey:key])
            {
                objDescVect.insert(objDescVect.end(), [AJNConvertUtil convertNSStringToQCCString:str]); // add the strings to std::vector
            }
            objectDescriptionsMap.insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], objDescVect)); //insert into objectDescriptionMap
        }

        ajn::services::AnnouncementData* annData = new ajn::services::AnnouncementData(port, aboutDataMap, objectDescriptionsMap);
        announcementsVect.insert(announcementsVect.end(), annData);
    } //for

    const ajn::services::GatewayCtrlManifestRules* gwManifestRules = const_cast<ajn::services::GatewayCtrlManifestRules*>([manifestRules handle]);

    ajn::services::GatewayCtrlAccessRules* gwAccessRule = self.handle->retrieveAcl(sessionId, *gwManifestRules, announcementsVect, status);

    AJGWCGatewayCtrlAccessRules* gwAccessRules = [[AJGWCGatewayCtrlAccessRules alloc] initWithHandle:gwAccessRule];

    return gwAccessRules;
}
@end
