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

#include "AppBusObject.h"
#include "../GatewayConstants.h"
#include "AclAdapter.h"
#include <alljoyn/gateway/GatewayMgmt.h>

namespace ajn {
namespace gw {
using namespace services;
using namespace qcc;
using namespace gwConsts;

AppBusObject::AppBusObject(BusAttachment* bus, GatewayConnectorApp* connectorApp, String const& objectPath, QStatus* status) :
    BusObject(objectPath.c_str()), m_ConnectorApp(connectorApp), m_ObjectPath(objectPath), m_AppStatusChanged(NULL),
    m_AclUpdated(NULL), m_ShutdownApp(NULL)
{
    *status = createAppInterface(bus);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not create the AppInterface"));
        return;
    }

    *status = createAppConnectorInterface(bus);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not create the AppConnectorInterface"));
        return;
    }

    *status = createAclMgmtInterface(bus);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not create the AclMgmtInterface"));
        return;
    }

    QCC_DbgTrace((GenerateIntrospection(true).c_str()));
}

QStatus AppBusObject::createAppInterface(BusAttachment* bus)
{
    QStatus status = ER_OK;

    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_GW_APP_INTERFACE.c_str());
    if (!interfaceDescription) {
        status = bus->CreateInterface(AJ_GW_APP_INTERFACE.c_str(), interfaceDescription, true);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_GET_APP_STATUS.c_str(), AJ_GET_APP_STATUS_PARAMS_IN.c_str(),
                                                 AJ_GET_APP_STATUS_PARAMS_OUT.c_str(), AJ_GET_APP_STATUS_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_RESTART_APP.c_str(), AJ_RESTART_APP_PARAMS_IN.c_str(),
                                                 AJ_RESTART_APP_PARAMS_OUT.c_str(), AJ_RESTART_APP_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_GET_MANIFEST_FILE.c_str(), AJ_GET_MANIFEST_FILE_PARAMS_IN.c_str(),
                                                 AJ_GET_MANIFEST_FILE_PARAMS_OUT.c_str(), AJ_GET_MANIFEST_FILE_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_GET_MANIFEST_INTERFACES.c_str(), AJ_GET_MANIFEST_INTERFACES_PARAMS_IN.c_str(),
                                                 AJ_GET_MANIFEST_INTERFACES_PARAMS_OUT.c_str(), AJ_GET_MANIFEST_INTERFACES_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddSignal(AJ_SIGNAL_APP_STATUS_CHANGED.c_str(), AJ_APP_STATUS_CHANGED_PARAMS.c_str(), AJ_APP_STATUS_CHANGED_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        interfaceDescription->Activate();
    }

postCreate:
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return status;
    }

    status = AddInterface(*interfaceDescription, ANNOUNCED);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return status;
    }

    const ajn::InterfaceDescription::Member* methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_APP_STATUS.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::GetAppStatus));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the GetAppStatus MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_RESTART_APP.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::RestartApp));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the RestartApp MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_MANIFEST_FILE.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::GetManifestFile));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the GetManifestFile MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_MANIFEST_INTERFACES.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::GetManifestInterfaces));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the GetManifestInterfaces MethodHandler"));
        return status;
    }

    m_AppStatusChanged = interfaceDescription->GetMember(AJ_SIGNAL_APP_STATUS_CHANGED.c_str());

    QCC_DbgTrace(("Created AppBusObject successfully"));

    return status;
}

QStatus AppBusObject::createAppConnectorInterface(BusAttachment* bus)
{
    QStatus status = ER_OK;

    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_GW_APP_CONNECTOR_INTERFACE.c_str());
    if (!interfaceDescription) {
        status = bus->CreateInterface(AJ_GW_APP_CONNECTOR_INTERFACE.c_str(), interfaceDescription);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_GET_MERGED_ACL.c_str(), AJ_GET_MERGED_ACL_PARAMS_IN.c_str(),
                                                 AJ_GET_MERGED_ACL_PARAMS_OUT.c_str(), AJ_GET_MERGED_ACL_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_UPDATE_CONNECTION_STATUS.c_str(), AJ_UPDATE_CONNECTION_STATUS_PARAMS_IN.c_str(),
                                                 AJ_UPDATE_CONNECTION_STATUS_PARAMS_OUT.c_str(), AJ_UPDATE_CONNECTION_STATUS_PARAM_NAMES.c_str(), MEMBER_ANNOTATE_NO_REPLY);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddSignal(AJ_SIGNAL_ACL_UPDATED.c_str(), AJ_ACL_UPDATED_PARAMS.c_str(), AJ_ACL_UPDATED_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddSignal(AJ_SIGNAL_SHUTDOWN_APP.c_str(), AJ_SHUTDOWN_APP_PARAMS.c_str(), AJ_SHUTDOWN_APP_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        interfaceDescription->Activate();
    }
postCreate:
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return status;
    }

    status = AddInterface(*interfaceDescription, ANNOUNCED);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return status;
    }

    const ajn::InterfaceDescription::Member* methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_MERGED_ACL.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::GetMergedAcl));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the GetMergedAcl MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_UPDATE_CONNECTION_STATUS.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::UpdateConnectionStatus));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the UpdateConnectionStatus MethodHandler"));
        return status;
    }

    m_AclUpdated = interfaceDescription->GetMember(AJ_SIGNAL_ACL_UPDATED.c_str());
    m_ShutdownApp = interfaceDescription->GetMember(AJ_SIGNAL_SHUTDOWN_APP.c_str());

    QCC_DbgTrace(("Created AppBusObject successfully"));

    return status;
}

QStatus AppBusObject::createAclMgmtInterface(BusAttachment* bus)
{
    QStatus status = ER_OK;

    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_GW_ACL_MGMT_INTERFACE.c_str());
    if (!interfaceDescription) {
        status = bus->CreateInterface(AJ_GW_ACL_MGMT_INTERFACE.c_str(), interfaceDescription, true);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_CREATE_ACL.c_str(), AJ_CREATE_ACL_PARAMS_IN.c_str(),
                                                 AJ_CREATE_ACL_PARAMS_OUT.c_str(), AJ_CREATE_ACL_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_DELETE_ACL.c_str(), AJ_DELETE_ACL_PARAMS_IN.c_str(),
                                                 AJ_DELETE_ACL_PARAMS_OUT.c_str(), AJ_DELETE_ACL_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        status = interfaceDescription->AddMethod(AJ_METHOD_LIST_ACLS.c_str(), AJ_LIST_ACLS_PARAMS_IN.c_str(),
                                                 AJ_LIST_ACLS_PARAMS_OUT.c_str(), AJ_LIST_ACLS_PARAM_NAMES.c_str());
        if (status != ER_OK) {
            goto postCreate;
        }
        interfaceDescription->Activate();
    }
postCreate:
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create interface"));
        return status;
    }

    status = AddInterface(*interfaceDescription, ANNOUNCED);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not add interface"));
        return status;
    }

    const ajn::InterfaceDescription::Member* methodMember = interfaceDescription->GetMember(AJ_METHOD_CREATE_ACL.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::CreateAcl));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the CreateAcl MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_DELETE_ACL.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::DeleteAcl));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the DeleteAcl MethodHandler"));
        return status;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_LIST_ACLS.c_str());
    status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppBusObject::ListAcls));
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register the ListAcls MethodHandler"));
        return status;
    }

    QCC_DbgTrace(("Created GatewayAclBusObject successfully"));
    return status;
}

AppBusObject::~AppBusObject()
{
}

QStatus AppBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called in AppBusObject class:"));

    if (0 == strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        return val.Set(AJPARAM_UINT16.c_str(), GATEWAY_MANAGEMENT_VERSION);
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus AppBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

void AppBusObject::GetAppStatus(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetAppStatus method call"));

    ajn::MsgArg replyArg[4];
    int indx = 0;

    QStatus status;
    status = replyArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getInstallStatus());
    if (status != ER_OK) {
        goto ReplyError;
    }

    status = replyArg[indx++].Set(AJPARAM_STR.c_str(), m_ConnectorApp->getInstallDescription().c_str());
    if (status != ER_OK) {
        goto ReplyError;
    }

    status = replyArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getConnectionStatus());
    if (status != ER_OK) {
        goto ReplyError;
    }

    status = replyArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getOperationalStatus());
    if (status != ER_OK) {
        goto ReplyError;
    }

    status = MethodReply(msg, replyArg, indx);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetAppStatus reply call failed"));
    }
    return;

ReplyError:

    QCC_LogError(status, ("Can't retrieve AppStatus - responding with error "));
    MethodReply(msg, status);
}

void AppBusObject::RestartApp(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received RestartApp method call"));

    uint16_t responseCode = m_ConnectorApp->restartConnectorApp();

    ajn::MsgArg replyArg[1];
    QStatus status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to RestartApp"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("RestartApp reply call failed"));
    }
}

void AppBusObject::GetManifestFile(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetManifestFile method call"));

    qcc::String manifestData = m_ConnectorApp->getManifest().getManifestData();
    ajn::MsgArg replyArg[1];
    QStatus status = replyArg[0].Set(AJPARAM_STR.c_str(), manifestData.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to GetManifestFile"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetManifestFile reply call failed"));
    }
}

void AppBusObject::GetManifestInterfaces(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetManifestInterfaces method call"));

    ajn::MsgArg replyArg[2];
    QStatus status = marshalCapabilities(m_ConnectorApp->getManifest().getExposedServices(), &replyArg[0]);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal ExposedServices for response to GetManifestInterfaces"));
        MethodReply(msg, status);
        return;
    }

    status = marshalCapabilities(m_ConnectorApp->getManifest().getRemotedServices(), &replyArg[1]);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal RemotedServices for response to GetManifestInterfaces"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 2);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetManifestInterfaces reply call failed"));
    }
}

QStatus AppBusObject::marshalCapabilities(const GatewayConnectorAppManifest::Capabilities& capabilities, MsgArg* msgArg)
{
    QStatus status = ER_OK;
    GatewayConnectorAppManifest::Capabilities::const_iterator it;
    MsgArg* objectsArray = new MsgArg[capabilities.size()];
    size_t objectsIndex = 0;

    for (it = capabilities.begin(); it != capabilities.end(); it++) {

        const std::vector<GatewayConnectorAppCapability::InterfaceDesc>& interfaces = it->getInterfaces();
        MsgArg* interfacesArray = new MsgArg[interfaces.size()];
        size_t interfaceIndex = 0;

        for (size_t i = 0; i < interfaces.size(); i++) {
            status = interfacesArray[interfaceIndex++].Set(AJPARAM_MANIFEST_INTERFACE_STRUCT.c_str(), interfaces[i].interfaceName.c_str(),
                                                           interfaces[i].interfaceFriendlyName.c_str(), interfaces[i].isSecured);
            if (status != ER_OK) {
                delete[] objectsArray;
                delete[] interfacesArray;
                return status;
            }
        }

        status = objectsArray[objectsIndex].Set(AJPARAM_MANIFEST_INTERFACE_INFO.c_str(), it->getObjectPath().c_str(),
                                                it->getIsObjectPathPrefix(), it->getObjectPathFriendlyName().c_str(),
                                                interfaceIndex, interfacesArray);
        if (status != ER_OK) {
            delete[] objectsArray;
            delete[] interfacesArray;
            return status;
        }
        objectsArray[objectsIndex++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }
    status = msgArg->Set(AJPARAM_MANIFEST_INTERFACE_INFO_ARRAY.c_str(), objectsIndex, objectsArray);
    if (status != ER_OK) {
        delete[] objectsArray;
        return status;
    }

    msgArg->SetOwnershipFlags(MsgArg::OwnsArgs, true);
    return status;
}

QStatus AppBusObject::SendAppStatusChangedSignal()
{
    QCC_DbgTrace(("In SendAppStatusChangedSignal"));

    GatewayBusListener* busListener = GatewayMgmt::getInstance()->getBusListener();
    QStatus status = ER_BUS_PROPERTY_VALUE_NOT_SET;

    if (!m_AppStatusChanged) {
        QCC_DbgHLPrintf(("Can't send m_AppStatusChanged signal. Signal not set"));
        return status;
    }

    if (!busListener) {
        QCC_DbgHLPrintf(("Can't send m_AppStatusChanged signal. BusListener not set"));
        return status;
    }

    ajn::MsgArg msgArg[4];
    int indx = 0;

    status = msgArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getInstallStatus());
    if (status != ER_OK) {
        return status;
    }

    status = msgArg[indx++].Set(AJPARAM_STR.c_str(), m_ConnectorApp->getInstallDescription().c_str());
    if (status != ER_OK) {
        return status;
    }

    status = msgArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getConnectionStatus());
    if (status != ER_OK) {
        return status;
    }

    status = msgArg[indx++].Set(AJPARAM_UINT16.c_str(), m_ConnectorApp->getOperationalStatus());
    if (status != ER_OK) {
        return status;
    }

    const std::vector<SessionId>& sessionIds = busListener->getSessionIds();
    for (size_t i = 0; i < sessionIds.size(); i++) {
        status = Signal(NULL, sessionIds[i], *m_AppStatusChanged, msgArg, indx);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not send m_AppStatusChanged Signal for sessionId: %i" + sessionIds[i]));
        }
    }
    return status;
}

void AppBusObject::GetMergedAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetMergedAcl method call"));

    ajn::MsgArg replyArg[2];
    QStatus status = AclAdapter::marshalMergedAcl(m_ConnectorApp->getAcls(), replyArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Acl for GetMergedAcl method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 2);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetMergedAcl reply call failed"));
    }
}

void AppBusObject::UpdateConnectionStatus(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received UpdateConnectionStatus method call"));

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    if (numArgs < 1) {
        QCC_DbgHLPrintf(("Could not receive UpdateConnectionStatus"));
        return;
    }

    uint16_t connectionStatus = 0;
    QStatus status = args[0].Get(AJPARAM_UINT16.c_str(), &connectionStatus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal argument of UpdateConnectionStatus method"));
        return;
    }

    if (connectionStatus > GW_CS_MAX_CONNECTION_STATUS) {
        QCC_DbgHLPrintf(("ConnectionStatus is not a valid ConnectionStatus"));
        return;
    }

    m_ConnectorApp->setConnectionStatus((ConnectionStatus)connectionStatus);
    QCC_DbgPrintf(("Connection Status updated successfully"));
}

QStatus AppBusObject::SendAclUpdatedSignal()
{
    QStatus status = ER_BUS_PROPERTY_VALUE_NOT_SET;
    QCC_DbgTrace(("Received SendAclUpdatedSignal method call"));

    if (!m_AclUpdated) {
        QCC_DbgHLPrintf(("Can't send AclUpdated signal. Signal not set"));
        return status;
    }

    qcc::String destination = AJ_GW_APP_WKN_PREFIX + m_ConnectorApp->getConnectorId();
    status = Signal(destination.c_str(), 0, *m_AclUpdated);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not send AclUpdated Signal"));
    }
    return status;
}

QStatus AppBusObject::SendShutdownAppSignal()
{
    QStatus status = ER_BUS_PROPERTY_VALUE_NOT_SET;
    QCC_DbgTrace(("Received SendShutdownAppSignal method call"));

    if (!m_ShutdownApp) {
        QCC_DbgHLPrintf(("Can't send ShutdownApp signal. Signal not set"));
        return status;
    }

    qcc::String destination = AJ_GW_APP_WKN_PREFIX + m_ConnectorApp->getConnectorId();
    status = Signal(destination.c_str(), 0, *m_ShutdownApp);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not send ShutdownApp Signal"));
    }
    return status;
}

void AppBusObject::CreateAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received CreateAcl method call"));

    qcc::String aclName;
    GatewayAclRules aclRules;
    std::map<qcc::String, qcc::String> metadata;
    std::map<qcc::String, qcc::String> customMetadata;
    QStatus status = AclAdapter::unmarshalAcl(msg, &aclName, &aclRules, &metadata, &customMetadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal CreateAcl method call"));
        MethodReply(msg, status);
        return;
    }

    qcc::String aclId;
    uint16_t resultStatus = m_ConnectorApp->createAcl(&aclId, aclName, aclRules, metadata, customMetadata);

    ajn::MsgArg replyArg[3];
    status = replyArg[0].Set(AJPARAM_UINT16.c_str(), resultStatus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to createAcl"));
        MethodReply(msg, status);
        return;
    }

    status = replyArg[1].Set(AJPARAM_STR.c_str(), aclId.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to createAcl"));
        MethodReply(msg, status);
        return;
    }

    qcc::String objectPath = m_ObjectPath + "/" + aclId;
    status = replyArg[2].Set(AJPARAM_OBJECTPATH.c_str(), objectPath.c_str());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to createAcl"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 3);
    if (status != ER_OK) {
        QCC_LogError(status, ("CreateAcl reply call failed"));
    }
}

void AppBusObject::DeleteAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received DeleteAcl method call"));

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    if (numArgs < 1) {
        QCC_DbgHLPrintf(("Could not handle DeleteAcl - no arguments given"));
        MethodReply(msg, ER_INVALID_DATA);
        return;
    }

    char* aclId = NULL;
    QStatus status = args[0].Get(AJPARAM_STR.c_str(), &aclId);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to DeleteAcl"));
        MethodReply(msg, status);
        return;
    }

    uint16_t responseCode = m_ConnectorApp->deleteAcl(aclId);

    ajn::MsgArg replyArg[1];
    status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response to DeleteAcl"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("DeleteAcl reply call failed"));
    }
}

void AppBusObject::ListAcls(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received ListAcls method call"));

    QStatus status;
    ajn::MsgArg replyArg[1];

    std::map<String, GatewayAcl*>::iterator it;
    std::map<String, GatewayAcl*> acls = m_ConnectorApp->getAcls();
    std::vector<MsgArg> aclInfo(acls.size());
    size_t aclInfoSize = 0;
    for (it = acls.begin(); it != acls.end(); it++) {
        status = aclInfo[aclInfoSize++].Set(AJPARAM_ACLS_STRUCT.c_str(), it->first.c_str(), it->second->getAclName().c_str(),
                                            it->second->getAclStatus(), it->second->getObjectPath().c_str());
        if (status != ER_OK) {
            QCC_LogError(status, ("Can't marshal response to ListAcls - responding with error "));
            MethodReply(msg, status);
            return;
        }
    }

    status = replyArg[0].Set(AJPARAM_ACLS_STRUCT_ARRAY.c_str(), aclInfoSize, aclInfo.data());
    if (status != ER_OK) {
        QCC_LogError(status, ("Can't marshal response to ListAcls - responding with error "));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("ListAcls reply call failed"));
    }
    return;
}

} /* namespace gw */
} /* namespace ajn */
