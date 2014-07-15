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

#import "AJGWCGatewayCtrlConnectorApplication.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "alljoyn/gateway/AnnouncementData.h"
//#import "AJNMessageArgument.h"
#import "AJGWCGatewayCtrlManifestRules.h"
#import "AJGWCGatewayCtrlAccessControlList.h"
#import "AJGWCAnnouncementData.h"
#import "AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter.h"

@interface AJGWCGatewayCtrlConnectorApplication ()

@property (nonatomic) ajn::services::GatewayCtrlConnectorApplication* handle;

@property (nonatomic) AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter* adapter;
@end

@implementation AJGWCGatewayCtrlConnectorApplication

- (void)dealloc
{
    delete self.adapter;
    self.adapter = NULL;
}

//- (id)initWithGwBusName:(NSString*) gwBusName appObjPath:(NSString*) appObjPath
//{
//    self = [super init];
//    if (self) {
//        self.handle = new ajn::services::GatewayCtrlConnectorApplication([AJNConvertUtil convertNSStringToQCCString:gwBusName], [AJNConvertUtil convertNSStringToQCCString:appObjPath]);
//    }
//    return self;
//}

- (id)initWithHandle:(ajn::services::GatewayCtrlConnectorApplication *) handle
{
    self = [super init];
    if (self) {
        self.handle = (ajn::services::GatewayCtrlConnectorApplication*)handle;
    }
    return self;
}

//- (id)initWithGwBusName:(NSString*) gwBusName appInfo:(AJNMessageArgument*) appInfo
//{
//    self = [super init];
//    if (self) {
//        self.handle = new ajn::services::GatewayCtrlConnectorApplication([AJNConvertUtil convertNSStringToQCCString:gwBusName], (ajn::MsgArg*)appInfo.handle);
//    }
//    return self;
//}

- (NSString*)gwBusName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetGwBusName()];
}

- (NSString*)appId
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetAppId()];
}

- (NSString*)friendlyName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetFriendlyName()];
}

- (NSString*)objectPath
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetObjectPath()];
}

- (NSString*)appVersion
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->GetAppVersion()];
}

- (NSString*)retrieveManifestFileUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->RetrieveManifestFile(sessionId, status)];
}

- (AJGWCGatewayCtrlManifestRules*)retrieveManifestRulesUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    ajn::services::GatewayCtrlManifestRules* mRules = self.handle->RetrieveManifestRules(sessionId, status);

    return [[AJGWCGatewayCtrlManifestRules alloc] initWithHandle:mRules];
}

- (AJGWCGatewayCtrlAccessRules*)retrieveConfigurableRulesUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status announcements:(NSArray*) announcements
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

    ajn::services::GatewayCtrlAccessRules* aRules = self.handle->RetrieveConfigurableRules(sessionId, announcementsVect, status);

    return [[AJGWCGatewayCtrlAccessRules alloc] initWithHandle:aRules];
}

- (AJGWCGatewayCtrlConnectorApplicationStatus*)retrieveStatusUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    ajn::services::GatewayCtrlConnectorApplicationStatus* connectorAppStatus = self.handle->RetrieveStatus(sessionId, status);

    return [[AJGWCGatewayCtrlConnectorApplicationStatus alloc] initWithHandle:connectorAppStatus];
}

- (AJGWCRestartStatus)restartUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    return (AJGWCRestartStatus)self.handle->Restart(sessionId, status);
}

- (QStatus)setStatusChangedHandler:(id<AJGWCGatewayCtrlApplicationStatusSignalHandler>) handler
{
    self.adapter = new AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter(handler);

    return self.handle->SetStatusChangedHandler(self.adapter);
}

- (void)unsetStatusChangedHandler
{
    self.handle->UnsetStatusChangedHandler();
}

- (AJGWCGatewayCtrlAclWriteResponse*)createAclUsingSessionId:(AJNSessionId) sessionId name:(NSString*) name accessRules:(AJGWCGatewayCtrlAccessRules*) accessRules status:(QStatus&) status
{
    ajn::services::GatewayCtrlAclWriteResponse* aclWRespose = self.handle->CreateAcl(sessionId, [AJNConvertUtil convertNSStringToQCCString:name], [accessRules handle], status);

    return [[AJGWCGatewayCtrlAclWriteResponse alloc] initWithHandle:aclWRespose];
}

- (NSArray*)retrieveAclsUsingSessionId:(AJNSessionId) sessionId status:(QStatus&) status
{
    NSMutableArray* aclListArray =  [[NSMutableArray alloc] init];
    std::vector <ajn::services::GatewayCtrlAccessControlList*> aclListVect =  self.handle->RetrieveAcls(sessionId, status);
    // Populate NSArray with std::vector data
    for (std::vector<ajn::services::GatewayCtrlAccessControlList*>::const_iterator vectIt = aclListVect.begin(); vectIt != aclListVect.end(); vectIt++) {
        [aclListArray addObject:[[AJGWCGatewayCtrlAccessControlList alloc] initWithHandle:*vectIt]];
    }

    return aclListArray;
}

- (AJGWCAclResponseCode)deleteAclUsingSessionId:(AJNSessionId) sessionId aclId:(NSString*) aclId status:(QStatus&) status
{
    return (AJGWCAclResponseCode)self.handle->DeleteAcl(sessionId, [AJNConvertUtil convertNSStringToQCCString:aclId], status);
}
@end
