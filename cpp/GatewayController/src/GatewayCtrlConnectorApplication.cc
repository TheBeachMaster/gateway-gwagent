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

#include <alljoyn/gateway/GatewayCtrlConnectorApplication.h>
#include <alljoyn/gateway/GatewayCtrlGatewayController.h>
#include <qcc/StringUtil.h>
#include "GatewayCtrlConstants.h"
#include "PayloadAdapter.h"


namespace ajn {
namespace services {
using namespace gwcConsts;
//            GatewayCtrlConnectorApplication::GatewayCtrlConnectorApplication(qcc::String gwBusName, qcc::String appObjPath){ }

ChangedSignalTask GatewayCtrlConnectorApplication::m_ChangedSignalTask;
AsyncTaskQueue GatewayCtrlConnectorApplication::m_ApplicationSignalQueue(&GatewayCtrlConnectorApplication::m_ChangedSignalTask);

GatewayCtrlConnectorApplication::GatewayCtrlConnectorApplication(qcc::String gwBusName, ajn::MsgArg*appInfo) : m_ManifestRules(NULL), m_ConfigurableRules(NULL), m_ConnectorApplicationStatus(NULL), m_AclWriteResponse(NULL), m_SignalMethod(NULL)
{
    GatewayCtrlConnectorApplication::m_ApplicationSignalQueue.Start();

    m_GwBusName = gwBusName;

    char* AppId;

    char* FriendlyName;

    char* ObjectPath;

    char* AppVersion;

    QStatus status = appInfo->Get("(ssos)", &AppId, &FriendlyName, &ObjectPath, &AppVersion);

    if (status == ER_OK) {
        m_AppId = AppId;

        m_FriendlyName = FriendlyName;

        m_ObjectPath = ObjectPath;

        m_AppVersion = AppVersion;
    } else { QCC_LogError(status, ("MsgArg get failed")); return; }

    QCC_DbgTrace(("In GatewayCtrlConnectorApplication Constructor"));


    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

    {
        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETMANIFESTFILE.c_str(), NULL, "s", "ManifestFile");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETMANIFESTINTERFACES.c_str(), NULL, "a((obs)a(ssb))a((obs)a(ssb))", "exposedServices,remotedServices");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }


            status = interfaceDescription->AddMethod(AJ_METHOD_GETAPPSTATUS.c_str(), NULL, "qsqq", "installStatus,installDescription,connectionStatus,operationalStatus");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_RESTARTAPP.c_str(), NULL, "q", "restartResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddSignal(AJ_SIGNAL_APPSTATUSCHANGED.c_str(), "qsqq", "installStatus,installDescription,connectionStatus,operationalStatus");
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
        m_SignalMethod = interfaceDescription->GetMember(AJ_SIGNAL_APPSTATUSCHANGED.c_str());
    }

    {
        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_CREATEACL.c_str(), "sa(obas)a(saya(obas))a{ss}a{ss}", "qso", "aclName,exposedServices,remotedApps,metaData,aclResponseCode,aclId,objectPath");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_DELETEACL.c_str(), "s", "q", "aclId,aclResponseCode");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                return;
            }


            status = interfaceDescription->AddMethod(AJ_METHOD_LISTACLS.c_str(), NULL, "a(ssqo)", "aclsList");
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


    QCC_DbgTrace(("org.alljoyn.gwagent.ctrl.App created."));


}

GatewayCtrlConnectorApplication::~GatewayCtrlConnectorApplication() {
    GatewayCtrlConnectorApplication::m_ApplicationSignalQueue.Stop();
}

qcc::String GatewayCtrlConnectorApplication::GetGwBusName() {
    return m_GwBusName;
}

qcc::String GatewayCtrlConnectorApplication::GetAppId() {
    return m_AppId;
}

qcc::String GatewayCtrlConnectorApplication::GetFriendlyName() {
    return m_FriendlyName;
}

qcc::String GatewayCtrlConnectorApplication::GetObjectPath() {
    return m_ObjectPath;
}

qcc::String GatewayCtrlConnectorApplication::GetAppVersion() {
    return m_AppVersion;
}

void GatewayCtrlConnectorApplication::EmptyVector()
{
    for (size_t indx = 0; indx < m_Acls.size(); indx++) {
        QStatus status = m_Acls[indx]->Release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_Acls[indx];
    }

    m_Acls.clear();
}

qcc::String GatewayCtrlConnectorApplication::RetrieveManifestFile(SessionId sessionId, QStatus& status) {
    {

        status = ER_OK;

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
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
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETMANIFESTFILE.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to getInstalledApps failed"));
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

        char*manifestFile;

        status = returnArgs[0].Get("s", &manifestFile);

        return qcc::String(manifestFile);

    }
end:

    return "";
}

//Important: sessionId is used only to connect to the gateway, the access rules retrieved will always erase the previous access rules.
GatewayCtrlManifestRules*GatewayCtrlConnectorApplication::RetrieveManifestRules(SessionId sessionId, QStatus& status) {
    {
        if (m_ManifestRules) {
            delete m_ManifestRules;
            m_ManifestRules = NULL;
        }

        status = ER_OK;

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
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
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETMANIFESTINTERFACES.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 2) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        m_ManifestRules = new GatewayCtrlManifestRules(returnArgs);

    }
end:

    return m_ManifestRules;

}

GatewayCtrlAccessRules* GatewayCtrlConnectorApplication::RetrieveConfigurableRules(SessionId sessionId, std::vector<AnnouncementData*> const& announcements, QStatus& status) {

    if (m_ConfigurableRules) {
        delete m_ConfigurableRules;
        m_ConfigurableRules = NULL;
    }

    RetrieveManifestRules(sessionId, status);

    if (status != ER_OK) {
        QCC_LogError(status, ("RetrieveManifestRules failed in RetrieveConfigurableRules"));
        return NULL;
    }

    std::vector<GatewayCtrlRemotedApp*> remotedApps;

    remotedApps = ExtractRemotedApps(m_ManifestRules->GetRemotedServices(), announcements, status);

    m_ConfigurableRules = new GatewayCtrlAccessRules(m_ManifestRules->GetExposedServices(), remotedApps);

    return m_ConfigurableRules;
}

GatewayCtrlConnectorApplicationStatus*GatewayCtrlConnectorApplication::RetrieveStatus(SessionId sessionId, QStatus& status) {
    {
        if (m_ConnectorApplicationStatus) {
            delete m_ConnectorApplicationStatus;
            m_ConnectorApplicationStatus = NULL;
        }

        status = ER_OK;

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
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
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_GETAPPSTATUS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 4) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        m_ConnectorApplicationStatus = new GatewayCtrlConnectorApplicationStatus(returnArgs);

        return m_ConnectorApplicationStatus;

    }
end:

    return NULL;

}

RestartStatus GatewayCtrlConnectorApplication::Restart(SessionId sessionId, QStatus& status) {

    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLERAPP_INTERFACE;
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
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLERAPP_INTERFACE.c_str(), AJ_METHOD_RESTARTAPP.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
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

        short restartStatus;

        status = returnArgs[0].Get("q", &restartStatus);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting restartStatus"));
            goto end;
        }
        return (RestartStatus)restartStatus;
    }
end:

    return GW_RESTART_APP_RC_INVALID;
}


void GatewayCtrlConnectorApplication::handleSignal(const ajn::InterfaceDescription::Member* member,
                                                   const char* srcPath, ajn::Message& msg)
{
    QCC_DbgHLPrintf((srcPath));

    const ajn::MsgArg* returnArgs;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, returnArgs);
    if (numArgs != 4) {
        QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
    } else {
        ChangedSignalData*taskdata = new ChangedSignalData(returnArgs, m_AppId);

        m_ApplicationSignalQueue.Enqueue(taskdata);
    }
}


QStatus GatewayCtrlConnectorApplication::SetStatusChangedHandler(const GatewayCtrlApplicationStatusSignalHandler*handler)
{

    QStatus status = ER_OK;

    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

    m_ChangedSignalTask.SetHandler(handler);

    status = busAttachment->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&GatewayCtrlConnectorApplication::handleSignal), m_SignalMethod, m_ObjectPath.c_str());

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the SignalHandler"));
        return status;
    } else {
        QCC_DbgPrintf(("Registered the SignalHandler successfully"));
    }

    qcc::String AJ_DISMISSER_INTERFACE_MATCH = "interface='" + AJ_GATEWAYCONTROLLERAPP_INTERFACE + "'";
    QCC_DbgPrintf(("StatusChangedHandler Match String is: %s", AJ_DISMISSER_INTERFACE_MATCH.c_str()));

    status = busAttachment->AddMatch(AJ_DISMISSER_INTERFACE_MATCH.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add filter match."));
    }

    return status;
}

void GatewayCtrlConnectorApplication::UnsetStatusChangedHandler() {
    m_ApplicationSignalQueue.Stop();

    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

    QStatus status = busAttachment->UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&GatewayCtrlConnectorApplication::handleSignal), m_SignalMethod, m_ObjectPath.c_str());

    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister the SignalHandler"));
    } else {
        QCC_DbgPrintf(("Unregistered the SignalHandler successfully"));
    }

    m_ChangedSignalTask.UnSetHandler();
    m_ApplicationSignalQueue.Start();
}



GatewayCtrlAclWriteResponse* GatewayCtrlConnectorApplication::CreateAcl(SessionId sessionId, qcc::String name, GatewayCtrlAccessRules* accessRules, QStatus& status) {

    if (m_AclWriteResponse) {
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    {
        status = ER_OK;

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
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

        std::map<qcc::String, qcc::String> internalMetadata;

        // Validate the rules in the ACL

        GatewayCtrlManifestRules*manifestRules = RetrieveManifestRules(sessionId, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Retreiving manifest rules failed"));
            goto end;
        }

        std::vector<GatewayCtrlManifestObjectDescription*> expServicesTargetOut;

        std::vector<GatewayCtrlManifestObjectDescription*> invalidExpServices =
            GatewayCtrlAccessControlList::ValidateManifObjDescs(accessRules->GetExposedServices(),
                                                                expServicesTargetOut,
                                                                manifestRules->GetExposedServices());

        const std::vector<GatewayCtrlRemotedApp*> remotedAppsIn =  accessRules->GetRemotedApps();

        std::vector<GatewayCtrlRemotedApp*> invalidRemotedApps;

        std::vector<GatewayCtrlRemotedApp*> remotedAppsOut;

        for (std::vector<GatewayCtrlRemotedApp*>::const_iterator itr = remotedAppsIn.begin(); itr != remotedAppsIn.end(); itr++) {
            std::vector<GatewayCtrlManifestObjectDescription*> remotedAppsTarget;

            std::vector<GatewayCtrlManifestObjectDescription*> invalidRemAppRules = GatewayCtrlAccessControlList::ValidateManifObjDescs((*itr)->GetObjDescRules(), remotedAppsTarget, manifestRules->GetRemotedServices());

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
            internalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_APP_NAME, remotedApp->GetAppName()));
            internalMetadata.insert(std::pair<qcc::String, qcc::String>(keyPrefix + AJSUFFIX_DEVICE_NAME, remotedApp->GetDeviceName()));
        }

        GatewayCtrlAccessRules*transmittedAcessRules = new GatewayCtrlAccessRules(expServicesTargetOut, remotedAppsOut);


        std::vector<MsgArg*> accessRulesVector;

        status = PayloadAdapter::MarshalAccessRules(*transmittedAcessRules, accessRulesVector);

        transmittedAcessRules->Release();
        delete transmittedAcessRules;
        transmittedAcessRules = NULL;


        // end of marshalling

        if (status != ER_OK) {
            QCC_LogError(status, ("PayloadAdapter::MarshalAccessRules failed"));
            goto end;
        }

        MsgArg*aclNameArg = new MsgArg("s", name.c_str());

        accessRulesVector.insert(accessRulesVector.begin(), aclNameArg);

        MsgArg*internalMetaDataKeyValueArg = PayloadAdapter::MarshalMetaData(internalMetadata, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] internalMetaDataKeyValueArg;

            goto end;
        }

        MsgArg*internalMetaDataKeyValueMapArg = new MsgArg("a{ss}", internalMetadata.size(), internalMetaDataKeyValueArg);

        accessRulesVector.push_back(internalMetaDataKeyValueMapArg);

        for (std::vector<MsgArg*>::const_iterator itr = accessRulesVector.begin(); itr != accessRulesVector.end(); itr++) {
            (*itr)->SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        MsgArg*args = new MsgArg[accessRulesVector.size()];

        for (int x = 0; x != accessRulesVector.size(); x++) {
            args[x] = *accessRulesVector[x];
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_CREATEACL.c_str(), args, 5, replyMsg);

        delete [] args;
        args = NULL;

        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetACLs failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 3) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        AclResponseCode aclResponseCode;
        char*aclId;
        char*objectPath;
        status = returnArgs[0].Get("q", &aclResponseCode);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }
        status = returnArgs[1].Get("s", &aclId);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }
        status = returnArgs[2].Get("o", &objectPath);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }



        m_AclWriteResponse = new GatewayCtrlAclWriteResponse(aclId, aclResponseCode, NULL, objectPath);

    }
end:

    return m_AclWriteResponse;
}


const std::vector <GatewayCtrlAccessControlList*>& GatewayCtrlConnectorApplication::RetrieveAcls(SessionId sessionId, QStatus& status) {

    EmptyVector();

    {
        status = ER_OK;

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
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
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_LISTACLS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetACLs failed"));
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

        int numACLs;
        MsgArg* tempEntries;
        status = returnArgs[0].Get("a(ssqo)", &numACLs, &tempEntries);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }

        for (int i = 0; i < numACLs; i++) {

            GatewayCtrlAccessControlList*accessControlList = new GatewayCtrlAccessControlList(m_GwBusName, &tempEntries[i]);

            m_Acls.push_back(accessControlList);

        }
    }
end:

    return m_Acls;
}

AclResponseCode GatewayCtrlConnectorApplication::DeleteAcl(SessionId sessionId, qcc::String aclId, QStatus& status) {

    status = ER_OK;

    {

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->GetBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, m_GwBusName.c_str(), m_ObjectPath.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE;
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



        MsgArg aclIdArg;

        aclIdArg.Set("s", aclId.c_str());

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE.c_str(), AJ_METHOD_DELETEACL.c_str(), &aclIdArg, 1, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to GetManifestInterfaces failed"));
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

QStatus GatewayCtrlConnectorApplication::Release() {

    EmptyVector();

    if (m_ManifestRules) {
        m_ManifestRules->Release();
        delete m_ManifestRules;
        m_ManifestRules = NULL;
    }

    if (m_ConfigurableRules) {
        m_ConfigurableRules->Release();
        delete m_ConfigurableRules;
        m_ConfigurableRules = NULL;
    }

    if (m_ConnectorApplicationStatus) {
        delete m_ConnectorApplicationStatus;
        m_ConnectorApplicationStatus = NULL;
    }

    if (m_AclWriteResponse) {
        m_AclWriteResponse->Release();
        delete m_AclWriteResponse;
        m_AclWriteResponse = NULL;
    }

    return ER_OK;
}

std::vector<GatewayCtrlRemotedApp*>
GatewayCtrlConnectorApplication::ExtractRemotedApps(const std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices,
                                                    std::vector<AnnouncementData*> const& announcements,
                                                    QStatus& status)
{

    std::vector<AnnouncementData*>::const_iterator annIter;
    std::vector<GatewayCtrlRemotedApp*> extractedRemotedApps;


    for (annIter = announcements.begin(); annIter != announcements.end(); annIter++) {

        GatewayCtrlRemotedApp*remotedApp = ExtractRemotedApp(remotedServices, *annIter, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("ExtractRemotedApp "));
        }

        if (remotedApp != NULL) {
            extractedRemotedApps.push_back(remotedApp);
        }
    }

    return extractedRemotedApps;
}

bool stringStartWith(const qcc::String& prefix, const qcc::String& inString)
{

    return (inString.compare(0, prefix.size(), prefix) == 0);
}

GatewayCtrlRemotedApp* GatewayCtrlConnectorApplication::ExtractRemotedApp(const std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices, const AnnouncementData*ann, QStatus& status) {

    if (ann == NULL) {
        status = ER_BAD_ARG_2;
        return NULL;
    }

    std::map<GatewayCtrlConnAppObjectPath, std::set<GatewayCtrlConnAppInterface> > remotedRules;

    AboutClient::ObjectDescriptions::const_iterator bod;

    for (bod = ann->GetObjectDescriptions().begin(); bod !=  ann->GetObjectDescriptions().end(); bod++) {

        const qcc::String objPath = bod->first;

        std::vector<qcc::String> ifacesToMatch = (*bod).second;

        std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator moj;

        for (moj = remotedServices.begin(); moj != remotedServices.end(); moj++) {

            GatewayCtrlManifestObjectDescription*curr_moj = (*moj);

            GatewayCtrlConnAppObjectPath*manop       = curr_moj->GetObjectPath();
            const std::set<GatewayCtrlConnAppInterface>*manifs      = curr_moj->GetInterfaces();
            int manifsSize  = (int)manifs->size();

            //Check object path suitability: if manifest objPath is a prefix of BusObjDesc objPath
            //or both object paths are equal



            if ((manop->IsPrefix() && stringStartWith(manop->GetPath(), objPath)) ||
                (manop->GetPath().compare(objPath) == 0)) {

                std::set<GatewayCtrlConnAppInterface>  resIfaces;

                std::vector<qcc::String>::iterator ifacesToMatchIter = ifacesToMatch.begin();

                //Search for the interfaces that comply with the manifest interfaces
                while (ifacesToMatchIter != ifacesToMatch.end()) {

                    qcc::String iface = *ifacesToMatchIter;

                    //If there are no interfaces in the manifest, it means that all the interfaces are supported
                    //add them without display names
                    if (manifsSize == 0) {
                        resIfaces.insert(GatewayCtrlConnAppInterface(iface, "", false));
                        ifacesToMatch.erase(ifacesToMatchIter++);
                        continue;
                    }

                    std::set<GatewayCtrlConnAppInterface>::iterator manIface;


                    for (manIface = manifs->begin(); manIface != manifs->end(); manIface++) {

                        if ((*manIface).GetName().compare(iface) == 0) {

                            resIfaces.insert(*manIface);          //found interface -> add it to the results
                            ifacesToMatch.erase(ifacesToMatchIter++);
                            break;
                        }
                    }

                    if ((ifacesToMatch.size() == 0) || (ifacesToMatchIter == ifacesToMatch.end())) {
                        break;
                    }

                    ifacesToMatchIter++;

                }        //while :: ifacesToMatch

                //not found any matched interfaces, continue to the next manifest rule
                if (resIfaces.size() == 0) {
                    continue;
                }

                //We add the manifest rule, if the manifest OP is the prefix of the BOD.objPath
                //or both object paths are equal
                GatewayCtrlConnAppObjectPath storeOp(manop->GetPath(), manop->GetFriendlyName(), false, manop->isPrefixAllowed());
                std::map<GatewayCtrlConnAppObjectPath, std::set<GatewayCtrlConnAppInterface> >::const_iterator remotedIfaces = remotedRules.find(storeOp);
                if (remotedIfaces == remotedRules.end()) {
                    remotedRules.insert(std::pair<GatewayCtrlConnAppObjectPath, std::set<GatewayCtrlConnAppInterface> >(storeOp,  resIfaces));
                } else {
                    resIfaces.insert(remotedIfaces->second.begin(), remotedIfaces->second.end());
                }


                //We add the BOD.objPath if the manifest OP is not equal to the  BOD.objPath
                if (manop->GetPath().compare(objPath)) {

                    //bodOp starts with the manOp but itself it's not a prefix
                    GatewayCtrlConnAppObjectPath bodOp(objPath, "", false, manop->isPrefixAllowed());
                    remotedIfaces      = remotedRules.find(bodOp);
                    if (remotedIfaces == remotedRules.end()) {
                        remotedRules.insert(std::pair<GatewayCtrlConnAppObjectPath, std::set<GatewayCtrlConnAppInterface> >(bodOp,  resIfaces));
                    } else {
                        resIfaces.insert(remotedIfaces->second.begin(), remotedIfaces->second.end());
                    }
                }

            }        //if :: objPath

            //If all the BusObjectDescription interfaces have been handled, no need to continue iterating
            //over the manifest rules
            if (ifacesToMatch.size() == 0) {
                break;
            }

        }        //for :: manifest

    }        //for :: BusObjectDesc

    int rulesSize = (int)remotedRules.size();

    //Check if this announcement complies with the manifest rules
    if (rulesSize == 0) {
        return NULL;
    }

    //Create Remoted rules list
    std::vector<GatewayCtrlManifestObjectDescription*> rules;

    std::map<GatewayCtrlConnAppObjectPath, std::set<GatewayCtrlConnAppInterface> >::const_iterator op;

    for (op = remotedRules.begin(); op != remotedRules.end(); op++) {
        rules.push_back(new GatewayCtrlManifestObjectDescription(op->first,  op->second));
    }

    GatewayCtrlRemotedApp*remotedApp = NULL;

    remotedApp = new GatewayCtrlRemotedApp(ann->GetAboutData(), rules);

    return remotedApp;
}
}
}
