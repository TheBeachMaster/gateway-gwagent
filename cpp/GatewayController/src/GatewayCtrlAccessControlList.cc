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

#include <alljoyn/gateway/GatewayCtrlAccessControlList.h>
#include <alljoyn/gateway/GatewayCtrlGatewayController.h>
#include <alljoyn/gateway/LogModule.h>
#include <qcc/StringUtil.h>
#include <algorithm>

#include "GatewayCtrlConstants.h"
#include "PayloadAdapter.h"

namespace ajn {
namespace services {

using namespace gwcConsts;

GatewayCtrlAccessControlList::GatewayCtrlAccessControlList(qcc::String gwBusName, const ajn::MsgArg*aclInfoAJ) : m_GwBusName(gwBusName), m_AclWriteResponse(NULL), m_AccessRules(NULL)
{
    char* AclID;
    char* AclName;
    short RetAclStatus;
    char* ObjectPath;

    QStatus status = aclInfoAJ->Get("(ssqo)", &AclID, &AclName, &RetAclStatus, &ObjectPath);

    if (status == ER_OK) {
        m_AclId = AclID;

        m_AclName = AclName;

        m_AclStatus = (AclStatus)RetAclStatus;

        m_ObjectPath = ObjectPath;
    }

    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

    {
        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_ACTIVATEACL.c_str(), NULL, "q", "AclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_DEACTIVATEACL.c_str(), NULL, "q", "AclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETACL.c_str(), NULL, "sa(obas)a(saya(obas))a{ss}a{ss}", "aclName,exposedServices,remotedApps,metaData");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETACLSTATUS.c_str(), NULL, "q", "aclStatus");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_UPDATEACL.c_str(), "sa(obas)a(saya(obas))a{ss}a{ss}", "q", "aclName,exposedServices,remotedApps,metaData,restartResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }
            status = interfaceDescription->AddMethod(AJ_METHOD_UPDATEACLMETADATA.c_str(), "a{ss}", "q", "metaData,aclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_UPDATECUSTOMMETADATA.c_str(), "a{ss}", "q", "metaData,aclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddProperty"));
                return;
            }

            interfaceDescription->Activate();
        }
    }

}

GatewayCtrlAccessControlList::~GatewayCtrlAccessControlList() {
}

qcc::String GatewayCtrlAccessControlList::GetName()
{
    return m_AclName;
}

void GatewayCtrlAccessControlList::SetName(qcc::String name)
{
    m_AclName = name;
}


qcc::String GatewayCtrlAccessControlList::GetId()
{
    return m_AclId;
}


qcc::String GatewayCtrlAccessControlList::GetObjectPath()
{
    return m_ObjectPath;
}

qcc::String GatewayCtrlAccessControlList::GetGwBusName()
{
    return m_GwBusName;
}

AclResponseCode GatewayCtrlAccessControlList::Activate(SessionId sessionId, QStatus& status)
{
    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_ACTIVATEACL.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to ActivateAcl failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }

        if (aclResponse == GW_ACL_RC_SUCCESS) {
            m_AclStatus = GW_AS_ACTIVE;
        }

        return (AclResponseCode)aclResponse;
    }
end:

    return GW_ACL_RC_INVALID;
}

AclResponseCode GatewayCtrlAccessControlList::Deactivate(SessionId sessionId, QStatus& status)
{
    status = ER_OK;

    {
        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_DEACTIVATEACL.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to DeactivateAcl failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }

        if (aclResponse == GW_ACL_RC_SUCCESS) {
            m_AclStatus = GW_AS_INACTIVE;
        }

        return (AclResponseCode)aclResponse;
    }
end:

    return GW_ACL_RC_INVALID;

}

GatewayCtrlAclWriteResponse* GatewayCtrlAccessControlList::UpdateAcl(SessionId sessionId,
                                                                     GatewayCtrlAccessRules* accessRules,
                                                                     GatewayCtrlManifestRules* manifestRules,
                                                                     QStatus& status)
{

    if (m_AclWriteResponse) {
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        // Validate the rules in the ACL

        std::vector<GatewayCtrlManifestObjectDescription*> expServicesTargetOut;

        std::vector<GatewayCtrlManifestObjectDescription*> invalidExpServices = ValidateManifObjDescs(accessRules->GetExposedServices(),
                                                                                                      expServicesTargetOut,
                                                                                                      manifestRules->GetExposedServices());

        const std::vector<GatewayCtrlRemotedApp*> remotedAppsIn =  accessRules->GetRemotedApps();

        std::vector<GatewayCtrlRemotedApp*> invalidRemotedApps;

        std::vector<GatewayCtrlRemotedApp*> remotedAppsOut;

        for (std::vector<GatewayCtrlRemotedApp*>::const_iterator itr = remotedAppsIn.begin(); itr != remotedAppsIn.end(); itr++) {
            std::vector<GatewayCtrlManifestObjectDescription*> remotedAppsTarget;

            std::vector<GatewayCtrlManifestObjectDescription*> invalidRemAppRules =
                ValidateManifObjDescs((*itr)->GetObjDescRules(),
                                      remotedAppsTarget,
                                      manifestRules->GetRemotedServices());

            if (invalidRemAppRules.size() > 0) {
                invalidRemotedApps.push_back(new GatewayCtrlRemotedApp((*itr), invalidRemAppRules));
            }

            //If there is no any marshaled rule, no valid rule was found -> continue
            if (remotedAppsTarget.size() == 0) {
                continue;
            }

            //Populate the RemotedApp
            GatewayCtrlRemotedApp*remotedApp = new GatewayCtrlRemotedApp((*itr), remotedAppsTarget);

            remotedAppsOut.push_back(remotedApp);

            //Store this application data in the internal metadata
            qcc::String AppId;
            const uint8_t*binary_appid = (*itr)->GetAppId();

            AppId = qcc::BytesToHexString(binary_appid, UUID_LENGTH);

            qcc::String keyPrefix = (*itr)->GetDeviceId() + "_" + AppId;
            m_InternalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_APP_NAME, remotedApp->GetAppName()));
            m_InternalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_DEVICE_NAME, remotedApp->GetDeviceName()));
        }

        GatewayCtrlAccessRules*transmittedAcessRules = new GatewayCtrlAccessRules(expServicesTargetOut, remotedAppsOut);


        std::vector<MsgArg*> inputArgsVector;

        status = PayloadAdapter::MarshalAccessRules(*transmittedAcessRules, inputArgsVector);

        transmittedAcessRules->Release();
        delete transmittedAcessRules;
        transmittedAcessRules = NULL;

        if (status != ER_OK) {
            QCC_LogError(status, ("PayloadAdapter::MarshalAccessRules failed"));
            goto end;
        }

        MsgArg*aclNameArg = new MsgArg("s", m_AclName.c_str());

        inputArgsVector.insert(inputArgsVector.begin(), aclNameArg);

        MsgArg*internalMetaDataKeyValueArg = PayloadAdapter::MarshalMetaData(m_InternalMetadata, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] internalMetaDataKeyValueArg;

            goto end;
        }

        MsgArg*internalMetaDataKeyValueMapArg = new MsgArg("a{ss}", m_InternalMetadata.size(), internalMetaDataKeyValueArg);

        inputArgsVector.push_back(internalMetaDataKeyValueMapArg);

        for (std::vector<MsgArg*>::const_iterator itr = inputArgsVector.begin(); itr != inputArgsVector.end(); itr++) {
            (*itr)->SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        MsgArg*args = new MsgArg[inputArgsVector.size()];

        for (unsigned int x = 0; x != inputArgsVector.size(); x++) {
            args[x] = *inputArgsVector[x];
        }


        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_UPDATEACL.c_str(), args, 5, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to UpdateAcl failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }

        GatewayCtrlAccessRules*invalidRules = new GatewayCtrlAccessRules(invalidExpServices, invalidRemotedApps);

        m_AclWriteResponse = new GatewayCtrlAclWriteResponse(m_AclId, (AclResponseCode)aclResponse, invalidRules, m_ObjectPath);

        return m_AclWriteResponse;
    }
end:

    return NULL;
}

AclResponseCode GatewayCtrlAccessControlList::UpdateCustomMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, QStatus& status)
{
    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        ajn::MsgArg*metadataArg = PayloadAdapter::MarshalMetaData(metadata, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            delete [] metadataArg;
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_UPDATECUSTOMMETADATA.c_str(), metadataArg, 1, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to updateMetaData failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }
        return (AclResponseCode)aclResponse;
    }
end:

    return GW_ACL_RC_INVALID;
}

AclResponseCode GatewayCtrlAccessControlList::UpdateAclMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, QStatus& status)
{
    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        MsgArg*metadataArg = PayloadAdapter::MarshalMetaData(metadata, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            delete [] metadataArg;
            goto end;
        }

        MsgArg*internalMetaDataKeyValueMapArg = new MsgArg("a{ss}", metadata.size(), metadataArg);

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_UPDATEACLMETADATA.c_str(), internalMetaDataKeyValueMapArg, 1, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to updateMetaData failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }
        return (AclResponseCode)aclResponse;
    }
end:

    return GW_ACL_RC_INVALID;
}

AclStatus GatewayCtrlAccessControlList::GetStatus()
{
    return m_AclStatus;
}

AclStatus GatewayCtrlAccessControlList::RetrieveStatus(SessionId sessionId, QStatus& status)
{
    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_GETACLSTATUS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to getAclStatus failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        short aclResponse;

        status = returnArgs[0].Get("q", &aclResponse);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed calling getAclStatus"));
            goto end;
        }
        return (AclStatus)aclResponse;
    }
end:

    return GW_AS_INACTIVE;
}

/**
 * Compare {@link ManifestObjectDescription} according to their {@link TPObjectPath}.
 * The algorithm performs lexicographical comparison of the object paths
 * with the condition that for equal object paths the object path that is not defined
 * as prefix is less than the object path that is prefix.
 */

int ManifestObjectDescriptionComparator(const GatewayCtrlManifestObjectDescription*lhs, const GatewayCtrlManifestObjectDescription*rhs) {

    qcc::String lhsOP = lhs->GetObjectPath()->GetPath();
    qcc::String rhsOP = rhs->GetObjectPath()->GetPath();

    int compRes  = rhsOP.compare(lhsOP);
    if (compRes == 0) {

        if (lhs->GetObjectPath()->IsPrefix()) {
            return 1;
        } else {
            return -1;
        }
    }

    return compRes;
}

GatewayCtrlAccessRules*GatewayCtrlAccessControlList::retrieveAcl(SessionId sessionId,
                                                                 const GatewayCtrlManifestRules& manifestRules,
                                                                 std::vector<AnnouncementData*> const& announcements,
                                                                 QStatus& status)
{
    if (m_AccessRules) {
        m_AccessRules->Release();
        delete m_AccessRules;
        m_AccessRules = NULL;
    }

    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERACL_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {
            status = ER_FAIL;
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }


        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_GETACL.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetAcl failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 5) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        char*name;

        status = returnArgs[0].Get("s", &name);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed Get"));
            goto end;
        }

        m_AclName = name;

        status = PayloadAdapter::unmarshalMetaData(&returnArgs[3], &m_InternalMetadata);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed unmarshaling InternalMetaData"));
            goto end;
        }

        std::map<qcc::String, qcc::String> customMetadata;

        status = PayloadAdapter::unmarshalMetaData(&returnArgs[4], &customMetadata);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed unmarshaling custom MetaData"));
            goto end;
        }


        // prepare the returned accessRules by intersecting them with the manifest rules
        std::vector<GatewayCtrlManifestObjectDescription*> expServices     = manifestRules.GetExposedServices();
        std::vector<GatewayCtrlManifestObjectDescription*> remServices     = manifestRules.GetRemotedServices();
        std::sort(expServices.begin(), expServices.end(), ManifestObjectDescriptionComparator);
        std::sort(remServices.begin(), remServices.end(), ManifestObjectDescriptionComparator);

        GatewayCtrlAccessRules*tmpAccessRules = new GatewayCtrlAccessRules(&returnArgs[1], &returnArgs[2], manifestRules, m_InternalMetadata);

        // The information retreived from the gateway manager will always include configured rules and configured rules only.
        for (std::vector<GatewayCtrlRemotedApp*>::const_iterator remotedAppIter = tmpAccessRules->GetRemotedApps().begin(); remotedAppIter != tmpAccessRules->GetRemotedApps().end(); remotedAppIter++) {
            const std::vector<GatewayCtrlManifestObjectDescription*> objDescs = ((GatewayCtrlRemotedApp*)*remotedAppIter)->GetObjDescRules();

            for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator objDescsIter = objDescs.begin(); objDescsIter != objDescs.end(); objDescsIter++) {
                GatewayCtrlManifestObjectDescription*objDesc = *objDescsIter;
                objDesc->SetConfigured(true);
            }


        }

        for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator objDescsIter = tmpAccessRules->GetExposedServices().begin(); objDescsIter != tmpAccessRules->GetExposedServices().end(); objDescsIter++) {
            GatewayCtrlManifestObjectDescription*objDesc = *objDescsIter;
            objDesc->SetConfigured(true);
        }

        std::vector<GatewayCtrlManifestObjectDescription*> exposedServices = ConvertExposedServices(tmpAccessRules->GetExposedServices(), expServices);

        std::vector<GatewayCtrlRemotedApp*> remotedApps;
        bool updateMetadata       =  ConvertRemotedApps(tmpAccessRules->GetRemotedApps(),
                                                        remotedApps,
                                                        remServices,
                                                        announcements, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("ConvertRemotedApps failed"));
            goto end;
        }


        delete tmpAccessRules;
        tmpAccessRules = NULL;

        if (updateMetadata) {
            if (m_InternalMetadata.size() > 0) {
                UpdateAclMetadata(sessionId, m_InternalMetadata, status);

                if (status != ER_OK) {
                    QCC_LogError(status, ("Failed UpdateAclMetadata"));
                }
            }
        }

        m_AccessRules = new GatewayCtrlAccessRules(exposedServices, remotedApps);
        m_AccessRules->SetMetadata(customMetadata);

        return m_AccessRules;
    }
end:

    return NULL;

}


QStatus GatewayCtrlAccessControlList::Release()
{
    if (m_AclWriteResponse) {
        m_AclWriteResponse->Release();
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    if (m_AccessRules) {
        m_AccessRules->Release();
        delete m_AccessRules;
        m_AccessRules = NULL;
    }

    return ER_OK;
}

std::vector<GatewayCtrlManifestObjectDescription*>
GatewayCtrlAccessControlList::ValidateManifObjDescs(const std::vector<GatewayCtrlManifestObjectDescription*>& toValidate,
                                                    std::vector<GatewayCtrlManifestObjectDescription*>& target,
                                                    const std::vector<GatewayCtrlManifestObjectDescription*>& manifestRules) {

    std::vector<GatewayCtrlManifestObjectDescription*> invalidRules;


    for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator itr = toValidate.begin(); itr != toValidate.end(); itr++) {

        std::set<GatewayCtrlTPInterface> invInterfaces;
        bool isValid                = IsValidRule(*itr, invInterfaces, manifestRules);

        //If current ManifestObjectDescription is NOT valid it need to be added to the invalid rules
        //OR the ManifestObjectDescription could be valid but some of its interfaces are not
        if (!isValid || invInterfaces.size() > 0) {
            invalidRules.push_back(new GatewayCtrlManifestObjectDescription(*(*itr)->GetObjectPath(), invInterfaces));
        }

        if (!isValid) {
            continue;
        }

        //Marshal and add the valid rules to the target
        target.push_back(*itr);
    }

    return invalidRules;
}

bool GatewayCtrlAccessControlList::IsValidRule(GatewayCtrlManifestObjectDescription*toValidate, std::set<GatewayCtrlTPInterface>& notValid, const std::vector<GatewayCtrlManifestObjectDescription*>& manifestRules)
{
    bool validRuleFound = false;
    std::set<GatewayCtrlTPInterface>*validIfaces = (std::set<GatewayCtrlTPInterface>*)toValidate->GetInterfaces();           //this pointer to interfaces in "toValidate" is important - we change the set inside of the "toValidate" in/out variable.

    notValid.insert(validIfaces->begin(), validIfaces->end());

    validIfaces->empty();

    //If toValidate is not configured it considered as an invalid rule and won't be sent to the gateway
    if (!toValidate->IsConfigured()) {
        return false;
    }

    std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator itr;

    for (itr = manifestRules.begin(); itr != manifestRules.end(); itr++) {

        GatewayCtrlManifestObjectDescription* mRule = (*itr);

        GatewayCtrlTPObjectPath*manop          = mRule->GetObjectPath();
        const std::set<GatewayCtrlTPInterface>*manifs = mRule->GetInterfaces();

        //Check object path validity
        if (isValidObjPath(manop, toValidate->GetObjectPath()->GetPath(), toValidate->GetObjectPath()->IsPrefix())) {

            //If the the list of the manifest interfaces is empty, it means that all the interfaces
            //of the toValidate object path are supported, so toValidate object is fully valid ==> return true
            if (manifs->size() == 0) {

                validIfaces->insert(notValid.begin(), notValid.end());
                notValid.clear();
                return true;
            }

            //Validate interfaces
            std::set<GatewayCtrlTPInterface>::const_iterator notValidIter = notValid.begin();
            while (notValidIter != notValid.end()) {

                GatewayCtrlTPInterface ifaceToTest = *notValidIter;

                if (manifs->find(ifaceToTest) != manifs->end()) {                      // Check if the interface is valid
                    validRuleFound = true;
                    notValid.erase(notValidIter++);                       // Remove the interface from notValid group
                    validIfaces->insert(ifaceToTest);                   // Add the interface to the valid group
                    continue;
                }

                if ((notValid.size() == 0) || (notValidIter == notValid.end())) {
                    break;
                }

                notValidIter++;
            }

            //All the interfaces toValidate are valid
            if (validRuleFound && notValid.size() == 0) {
                return true;
            }

        }        //if :: objPath

    }        //for :: manifestRules

    return validRuleFound;
}

bool _stringStartWith(const qcc::String& prefix, const qcc::String& inString)
{     //TODO: I copied this from somewhere, there hsould only be one function
    bool ret = (inString.compare(0, prefix.size(), prefix) == 0);
    return ret;
}

bool GatewayCtrlAccessControlList::isValidObjPath(const GatewayCtrlTPObjectPath*manifOp, qcc::String toValidOP, bool isPrefix)
{
    if ((manifOp->IsPrefix() && _stringStartWith(manifOp->GetPath(), toValidOP)) ||
        (!manifOp->IsPrefix() && !isPrefix && (manifOp->GetPath() == toValidOP))) {

        return true;
    }

    return false;
}

std::vector<GatewayCtrlManifestObjectDescription*>
GatewayCtrlAccessControlList::ConvertExposedServices(const std::vector<GatewayCtrlManifestObjectDescription*>& aclExpServicesAJ,
                                                     const std::vector<GatewayCtrlManifestObjectDescription*>& manifExpServices)
{

    std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> > usedManRules;
    std::vector<GatewayCtrlManifestObjectDescription*> aclExpServices   = ConvertObjectDescription(aclExpServicesAJ,
                                                                                                   manifExpServices,
                                                                                                   usedManRules);

    //Find out the manifest exposed services rules that weren't used
    for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator manifExpSrvc = manifExpServices.begin(); manifExpSrvc != manifExpServices.end(); manifExpSrvc++) {

        GatewayCtrlTPObjectPath*manop       = (*manifExpSrvc)->GetObjectPath();
        std::set<GatewayCtrlTPInterface>*manifs      = (std::set<GatewayCtrlTPInterface>*)(*manifExpSrvc)->GetInterfaces();                 // we manipulate this array


        std::set<GatewayCtrlTPInterface>*usedIfaces = NULL;


        if (usedManRules.find(*manop) != usedManRules.end()) {
            usedIfaces = &usedManRules.find(*manop)->second;
        }
        GatewayCtrlTPObjectPath*storeOp = new GatewayCtrlTPObjectPath(manop->GetPath(), manop->GetFriendlyName(), false, manop->isPrefixAllowed());


        //Check if this rule was NOT used then add it to the resExpServices
        if (usedIfaces == NULL) {
            aclExpServices.push_back(new GatewayCtrlManifestObjectDescription(*storeOp, *manifs, false));
            continue;
        }

        //Remove from the manifest interfaces the interfaces that have been used
        for (std::set<GatewayCtrlTPInterface>::const_iterator itr = usedIfaces->begin(); itr != usedIfaces->end(); itr++) {
            QCC_SyncPrintf("erasing %s", itr->GetName().c_str());
            manifs->erase(manifs->find(*itr));
        }


        //Add to the resExpServices the object path and the interfaces that weren't used
        if (manifs->size() > 0) {
            aclExpServices.push_back(new GatewayCtrlManifestObjectDescription(*storeOp, *manifs, false));
        }
    }

    return aclExpServices;
}        //convertExposedServices



std::vector<GatewayCtrlManifestObjectDescription*>
GatewayCtrlAccessControlList::ConvertObjectDescription(const std::vector<GatewayCtrlManifestObjectDescription*>& objDescs,
                                                       const std::vector<GatewayCtrlManifestObjectDescription*>& manifest,
                                                       std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >& usedManRules)

{

    std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >  resRules;

    std::set<qcc::String> objDescInterfaceNames;

    // create a unique list of interface strings
    for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator objDesc = objDescs.begin(); objDesc != objDescs.end(); objDesc++) {
        for (std::set<GatewayCtrlTPInterface>::const_iterator itr = (*objDesc)->GetInterfaces()->begin(); itr != (*objDesc)->GetInterfaces()->end(); itr++) {
            objDescInterfaceNames.insert(itr->GetName());
        }
    }

    for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator objDesc = objDescs.begin(); objDesc != objDescs.end(); objDesc++) {

        std::set<qcc::String> ifacesToConvert = objDescInterfaceNames;

        for (std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator manifRule = manifest.begin(); manifRule != manifest.end(); manifRule++) {
            GatewayCtrlTPObjectPath*manop               = (*manifRule)->GetObjectPath();
            const std::set<GatewayCtrlTPInterface>*manifs              = (*manifRule)->GetInterfaces();
            int manifsSize          = (int)manifs->size();

            if (!isValidObjPath(manop, (*objDesc)->GetObjectPath()->GetPath(), (*objDesc)->GetObjectPath()->IsPrefix())) {
                continue;
            }

            GatewayCtrlTPObjectPath*resObjPath = NULL;

            if (manop->GetPath().compare((*objDesc)->GetObjectPath()->GetPath()) == 0) {
                resObjPath = new GatewayCtrlTPObjectPath((*objDesc)->GetObjectPath()->GetPath(), manop->GetFriendlyName(), (*objDesc)->GetObjectPath()->IsPrefix(), manop->isPrefixAllowed());
            } else {
                resObjPath = new GatewayCtrlTPObjectPath((*objDesc)->GetObjectPath()->GetPath(), "", (*objDesc)->GetObjectPath()->IsPrefix(), manop->isPrefixAllowed());
            }

            //Add used manifest rules for the empty manifest interfaces array
            if (manifsSize == 0) {
                std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >::const_iterator usedManOP = usedManRules.find(*manop);
                if (usedManOP != usedManRules.end()) {
                    std::set<GatewayCtrlTPInterface> usedIfaces = usedManOP->second;
                    if (usedIfaces.size() == 0) {
                        std::set<GatewayCtrlTPInterface> empty;
                        usedManRules.insert(std::pair<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >(*manop, empty));
                    }
                }
            }

            std::set<qcc::String>::const_iterator ifacesToConvertIter = ifacesToConvert.begin();

            std::set<GatewayCtrlTPInterface> resInterfaces;         //result interfaces

            while (ifacesToConvertIter != ifacesToConvert.end()) {

                qcc::String ajIface = (*ifacesToConvertIter);

                //If there are not interfaces in the manifest, it means that all the interfaces are supported
                //add them without display names
                if (manifsSize == 0) {
                    resInterfaces.insert(GatewayCtrlTPInterface(ajIface, "", false));
                    ifacesToConvert.erase(ifacesToConvertIter++);
                }

                for (std::set<GatewayCtrlTPInterface>::const_iterator manIface = manifs->begin(); manIface != manifs->end(); manIface++) {

                    //aclInterface is found in manifest
                    if (ajIface.compare(manIface->GetName()) == 0) {


                        resInterfaces.insert(GatewayCtrlTPInterface(ajIface, (*manIface).GetFriendlyName(), manIface->IsSecured()));
                        ifacesToConvert.erase(ifacesToConvertIter++);
                        continue;
                    }
                }        //for :: manifest interfaces

                if ((ifacesToConvert.size() == 0) || (ifacesToConvertIter == ifacesToConvert.end())) {
                    break;
                }

                ifacesToConvertIter++;
            }


            //Not found any matched interfaces, continue to the next manifest rule
            if (resInterfaces.size() == 0) {
                continue;
            }

            //Add the interfaces to the resObjPath
            std::set<GatewayCtrlTPInterface> ifaces;

            if (resRules.find(*resObjPath) != resRules.end()) {
                ifaces = resRules.find(*resObjPath)->second;

                //Merge interfaces
                ifaces.insert(resInterfaces.begin(), resInterfaces.end());

            } else {

                resRules.insert(std::pair<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >(*resObjPath, resInterfaces));
            }


            //Add used manifest rules
            if (manifsSize > 0) {

                std::set<GatewayCtrlTPInterface> usedIfaces;
                if (usedManRules.find(*manop) != usedManRules.end()) {
                    usedIfaces.insert(resInterfaces.begin(), resInterfaces.end());
                } else {
                    usedManRules.insert(std::pair<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >(*manop, resInterfaces));
                }
            }

            //If all the objDescAJ interfaces have been handled, no need to continue iterating
            //over the manifest rules
            if (ifacesToConvert.size() == 0) {
                break;
            }

        }        //for :: manifest

    }        //for :: objDescsAJ

    //Create final list of the configured rules

    std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >::const_iterator itr;
    std::vector<GatewayCtrlManifestObjectDescription*> rules;
    for (itr = resRules.begin(); itr != resRules.end(); itr++) {
        const GatewayCtrlTPObjectPath*path = &itr->first;
        std::set<GatewayCtrlTPInterface> tpInterfacesSet = itr->second;

        QCC_SyncPrintf("size:%d", resRules.size());


        GatewayCtrlManifestObjectDescription*newOD = new GatewayCtrlManifestObjectDescription(*path, tpInterfacesSet, true);
        rules.push_back(newOD);
    }

    return rules;
}        //convertObjectDescription


bool GatewayCtrlAccessControlList::ConvertRemotedApps(const std::vector<GatewayCtrlRemotedApp*>& inputRemotedApps,
                                                      std::vector<GatewayCtrlRemotedApp*>& outputRemotedApps,
                                                      std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices,
                                                      std::vector<AnnouncementData*> const& announcements, QStatus& status)
{
    //Gets TRUE if the metadata needs to be updated
    bool updatedMeta               = false;
    std::vector<GatewayCtrlRemotedApp*> configurableApps = GatewayCtrlTPApplication::ExtractRemotedApps(remotedServices, announcements, status);

    if (status != ER_OK) {
        QCC_LogError(status, ("ExtractRemotedApps failed"));
        return updatedMeta;
    }

    std::vector<GatewayCtrlRemotedApp*>::const_iterator remAppIter;

    //Iterate over the remoted apps
    for (remAppIter = inputRemotedApps.begin(); remAppIter != inputRemotedApps.end(); remAppIter++) {

        GatewayCtrlRemotedApp*remApp = (*remAppIter);

        //Retrieve announcement data to check whether the aclRemApps should be completed
        if (remApp->GetAppId() == NULL) {
            QCC_SyncPrintf("retrieveRemotedApps - remotedApp with a bad appId has been received, objPath: '%s'", m_ObjectPath.c_str());
            continue;
        }


        std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> > usedManRules;
        //Convert the acl remoted app object descriptions to the list of ManifestObjectDescriptions
        //by intersecting with the manifest data.
        std::vector<GatewayCtrlManifestObjectDescription*> configuredRules = ConvertObjectDescription(
            remApp->GetObjDescRules(), remotedServices, usedManRules);

        //Construct the standard deviceId_appId key
        qcc::String AppId;
        const uint8_t*binary_appid = remApp->GetAppId();


        AppId = qcc::BytesToHexString(binary_appid, UUID_LENGTH);

        qcc::String keyPrefix = remApp->GetDeviceId() + "_" + AppId;

        int confRulesSize = (int)configuredRules.size();
        QCC_SyncPrintf("retrieveRemotedApps - Created ObjDesc rules of the remoted app: '%s' rules size: '%d', objPath: '%s'", keyPrefix.c_str(), confRulesSize, m_ObjectPath.c_str());

        //Retrieve appName and deviceName from the metadata
        bool findMeta      = true;

        qcc::String key =  keyPrefix + AJSUFFIX_APP_NAME;

        std::map<qcc::String, qcc::String>::const_iterator itr = m_InternalMetadata.find(key);

        qcc::String appNameMeta;

        if (itr != m_InternalMetadata.end()) {
            appNameMeta = itr->second;
        }


        key = keyPrefix + AJSUFFIX_DEVICE_NAME;

        itr = m_InternalMetadata.find(key);

        qcc::String deviceNameMeta;

        if (itr != m_InternalMetadata.end()) {
            deviceNameMeta = itr->second;
        }

        if (deviceNameMeta.empty() || deviceNameMeta.length() == 0 ||
            appNameMeta.empty()  || appNameMeta.length() == 0) {

            QCC_SyncPrintf("retrieveRemotedApps - metadata is corrupted!!!. deviceName or appName weren't found, objPath: '%s'", m_ObjectPath.c_str());

            findMeta = false;
        }

        //Look for the configurable RemotedApp from intersection of the manifest
        //with announcement data
        GatewayCtrlRemotedApp*configurableApp = GetRemotedApp(configurableApps, remApp->GetDeviceId(), binary_appid);

        //If there is no configurableApp, but aclMetadata has appName and deviceName to construct the RemotedApp object
        //and the acl configuredRules were created successfully, then create the RemotedApp object
        if (configurableApp == NULL) {

            QCC_SyncPrintf("retrieveRemotedApps - not found any ConfigurableApp for the remoted app: '%s', objPath: '%s'", key.c_str(), m_ObjectPath.c_str());

            if  (findMeta && confRulesSize > 0) {

                QCC_SyncPrintf("retrieveRemotedApps - metadata has the required values, creating the remoted app");
                //Create RemotedApp
                outputRemotedApps.push_back(new GatewayCtrlRemotedApp(qcc::String(""), appNameMeta, (uint8_t*)binary_appid, deviceNameMeta, remApp->GetDeviceId(), configuredRules));
            }
        } else {         //There is a configurableApp

            QCC_SyncPrintf("retrieveRemotedApps - found announcement for the remoted app: '%s', objPath: '%s'", key.c_str(), m_ObjectPath.c_str());


            if (MetadataUpdated(deviceNameMeta, appNameMeta, *configurableApp, key)) {
                updatedMeta = true;
            }

            //Completes already configured rules with rules that haven't configured yet
            AddUnconfiguredRemotedAppRules(configurableApp->GetObjDescRules(), configuredRules);

            if (configuredRules.size() > 0) {
                outputRemotedApps.push_back(new GatewayCtrlRemotedApp(configurableApp, configuredRules));
            }

        }        //if :: annData != null

    }        //for :: remotedApp

    //Add to the configured remotedApps the unconfigured remoted apps.
    //These apps remained in the configurableApps after working the algorithm above
    for (std::vector<GatewayCtrlRemotedApp*>::const_iterator itr = configurableApps.begin(); itr != configurableApps.end(); itr++) {
        outputRemotedApps.push_back(*itr);
    }


    return updatedMeta;
}


/**
 * Compares configured rules of the remoted apps with the unconfigured rules.
 * Completes the configured rules with the rules that haven't configured yet.
 * @param unconfRules
 * @param confRules
 */
void GatewayCtrlAccessControlList::AddUnconfiguredRemotedAppRules(const std::vector<GatewayCtrlManifestObjectDescription*>& unconfRules,
                                                                  std::vector<GatewayCtrlManifestObjectDescription*>& confRules) {

    std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator iter;


    for (iter = unconfRules.begin(); iter != unconfRules.end(); iter++) {

        GatewayCtrlManifestObjectDescription*unconfRule = (*iter);


        GatewayCtrlTPObjectPath*unconfOP          = unconfRule->GetObjectPath();
        std::set<GatewayCtrlTPInterface>* unconfIfaces  = (std::set<GatewayCtrlTPInterface>*)unconfRule->GetInterfaces();

        //Gets TRUE if unconfOP was found among the confRules
        bool unconfOpInConf = false;

        std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator confRulesIter;

        for (confRulesIter = confRules.begin(); confRulesIter != confRules.end(); confRulesIter++) {

            GatewayCtrlTPObjectPath* confOP         = (*confRulesIter)->GetObjectPath();
            const std::set<GatewayCtrlTPInterface>* confIfaces = (*confRulesIter)->GetInterfaces();

            //Check if the unconfOP not equals confOP
            if (unconfOP->GetPath().compare(confOP->GetPath())) {
                continue;
            }

            unconfOpInConf = true;

            for (std::set<GatewayCtrlTPInterface>::const_iterator itr = confIfaces->begin(); itr != confIfaces->end(); itr++) {
                QCC_SyncPrintf("erasing %s", itr->GetName().c_str());
                unconfIfaces->erase(unconfIfaces->find(*itr));
            }
            //unconfIfaces->erase(confIfaces->begin(), confIfaces->end());
            break;
        }

        if (!unconfOpInConf || unconfIfaces->size() > 0) {
            confRules.push_back(new GatewayCtrlManifestObjectDescription(*unconfOP, *unconfIfaces, false));
        }

    }        //for::unconfRule
}

bool GatewayCtrlAccessControlList::MetadataUpdated(qcc::String deviceNameMeta, qcc::String appNameMeta, const GatewayCtrlRemotedApp& annApp, qcc::String key) {

    bool updatedMeta      = false;

    qcc::String annAppName        = annApp.GetAppName();
    qcc::String annDeviceName     = annApp.GetDeviceName();

    qcc::String appNameMetaKey    = key + AJSUFFIX_APP_NAME;
    qcc::String deviceNameMetaKey = key + AJSUFFIX_DEVICE_NAME;

    //Check appName, deviceName correctness vs. announcements
    if (!annAppName.compare(appNameMeta)) {

        QCC_SyncPrintf("retrieveRemotedApps - metaAppName differs from the announcement app name, update the metadata with the app name: '%s', objPath: '%s'", annAppName.c_str(), m_ObjectPath.c_str());

        m_InternalMetadata.insert(std::pair<qcc::String, qcc::String>(appNameMetaKey, annAppName));
        updatedMeta = true;
    }

    if (!annDeviceName.compare(deviceNameMeta)) {

        QCC_SyncPrintf("retrieveRemotedApps - metaDeviceName differs from the announcement device name, update the metadata with the device name: '%s', objPath: '%s'", annDeviceName.c_str(), m_ObjectPath.c_str());

        m_InternalMetadata.insert(std::pair<qcc::String, qcc::String>(deviceNameMetaKey, annDeviceName));

        updatedMeta = true;
    }

    return updatedMeta;
}
}
}






















