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
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getGwBusName()];
}

- (NSString*)appId
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getAppId()];
}

- (NSString*)friendlyName
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getFriendlyName()];
}

- (NSString*)objectPath
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}

- (NSString*)appVersion
{
    return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getAppVersion()];
}

- (QStatus) retrieveManifestFileUsingSessionId:(AJNSessionId) sessionId fileContent:(NSString **)xml
{
    qcc::String in_xml;

    QStatus status = self.handle->retrieveManifestFile(sessionId, in_xml);

    if (status == ER_OK) {
        *xml = [AJNConvertUtil convertQCCStringtoNSString:in_xml];
    }

    return status;
}

- (QStatus)retrieveManifestRulesUsingSessionId:(AJNSessionId) sessionId manifestRules:(AJGWCGatewayCtrlManifestRules**)manifestRules
{
    ajn::services::GatewayCtrlManifestRules* mRules;

    QStatus status = self.handle->retrieveManifestRules(sessionId, &mRules );


    if (status == ER_OK) {
        *manifestRules = [[AJGWCGatewayCtrlManifestRules alloc] initWithHandle:mRules];
    }

    return status;
}

- (QStatus)retrieveConfigurableRulesUsingSessionId:(AJNSessionId) sessionId rules:(AJGWCGatewayCtrlAccessRules**)rules announcements:(NSArray*) announcements
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

    ajn::services::GatewayCtrlAccessRules* aRules;
    QStatus status = self.handle->retrieveConfigurableRules(sessionId, announcementsVect, &aRules);

    if (status == ER_OK) {
        *rules = [[AJGWCGatewayCtrlAccessRules alloc] initWithHandle:aRules];
    }

    return status;
}

- (QStatus)retrieveStatusUsingSessionId:(AJNSessionId) sessionId status:(AJGWCGatewayCtrlConnectorApplicationStatus**)connectorAppStatus
{
    ajn::services::GatewayCtrlConnectorApplicationStatus *localConnectorAppStatus;

    QStatus status = self.handle->retrieveStatus(sessionId, &localConnectorAppStatus);

    if (status==ER_OK) {
        *connectorAppStatus = [[AJGWCGatewayCtrlConnectorApplicationStatus alloc] initWithHandle:localConnectorAppStatus];
    }

    return status;
}

- (QStatus)restartUsingSessionId:(AJNSessionId) sessionId status:(AJGWCRestartStatus&) restartStatus
{
    ajn::services::RestartStatus localRestartStatus;

    QStatus status = self.handle->restart(sessionId, localRestartStatus);

    restartStatus = (AJGWCRestartStatus)localRestartStatus;

    return status;
}

- (QStatus)setStatusChangedHandler:(id<AJGWCGatewayCtrlApplicationStatusSignalHandler>) handler
{
    self.adapter = new AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter(handler);

    return self.handle->setStatusChangedHandler(self.adapter);
}

- (void)unsetStatusChangedHandler
{
    self.handle->unsetStatusChangedHandler();
}

- (QStatus)createAclUsingSessionId:(AJNSessionId) sessionId name:(NSString*) name accessRules:(AJGWCGatewayCtrlAccessRules*) accessRules aclStatus:(AJGWCGatewayCtrlAclWriteResponse**)aclStatus
{
    ajn::services::GatewayCtrlAclWriteResponse* aclWResponse;
    QStatus status = self.handle->createAcl(sessionId, [AJNConvertUtil convertNSStringToQCCString:name], [accessRules handle], &aclWResponse);

    if (status==ER_OK) {
        *aclStatus = [[AJGWCGatewayCtrlAclWriteResponse alloc] initWithHandle:aclWResponse];
    }
    return status;
}

- (QStatus)retrieveAclsUsingSessionId:(AJNSessionId) sessionId acls:(NSMutableArray *)aclListArray
{
    std::vector <ajn::services::GatewayCtrlAccessControlList*> aclListVect;
    QStatus status =  self.handle->retrieveAcls(sessionId, aclListVect);

    if (status==ER_OK) {
        // Populate NSArray with std::vector data
        for (std::vector<ajn::services::GatewayCtrlAccessControlList*>::const_iterator vectIt = aclListVect.begin(); vectIt != aclListVect.end(); vectIt++) {
            [aclListArray addObject:[[AJGWCGatewayCtrlAccessControlList alloc] initWithHandle:*vectIt]];
        }

    }
    return status;
}

- (QStatus)deleteAclUsingSessionId:(AJNSessionId) sessionId aclId:(NSString*) aclId status:(AJGWCAclResponseCode &)responseCode
{
    ajn::services::AclResponseCode localResponseCode;
    QStatus status = self.handle->deleteAcl(sessionId, [AJNConvertUtil convertNSStringToQCCString:aclId], localResponseCode);


    responseCode = (AJGWCAclResponseCode)localResponseCode;

    return status;
}
@end
