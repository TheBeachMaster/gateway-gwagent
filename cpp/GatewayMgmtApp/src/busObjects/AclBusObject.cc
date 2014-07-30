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

#include "AclBusObject.h"
#include "../GatewayConstants.h"
#include "AclAdapter.h"
#include <alljoyn/about/AboutServiceApi.h>
#include <alljoyn/gateway/GatewayMgmt.h>

namespace ajn {
namespace gw {
using namespace qcc;
using namespace services;
using namespace gwConsts;

AclBusObject::AclBusObject(BusAttachment* bus, GatewayAcl* acl, String const& objectPath, QStatus* status) :
    BusObject(objectPath.c_str()), m_Acl(acl), m_ObjectPath(objectPath)
{
    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_GW_ACL_INTERFACE.c_str());
    if (!interfaceDescription) {
        *status = bus->CreateInterface(AJ_GW_ACL_INTERFACE.c_str(), interfaceDescription, true);
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_ACTIVATE_ACL.c_str(), AJ_ACTIVATE_ACL_PARAMS_IN.c_str(),
                                                  AJ_ACTIVATE_ACL_PARAMS_OUT.c_str(), AJ_ACTIVATE_ACL_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_GET_ACL.c_str(), AJ_GET_ACL_PARAMS_IN.c_str(),
                                                  AJ_GET_ACL_PARAMS_OUT.c_str(), AJ_GET_ACL_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_GET_ACL_STATUS.c_str(), AJ_GET_ACL_STATUS_PARAMS_IN.c_str(),
                                                  AJ_GET_ACL_STATUS_PARAMS_OUT.c_str(), AJ_GET_ACL_STATUS_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_UPDATE_ACL.c_str(), AJ_UPDATE_ACL_PARAMS_IN.c_str(),
                                                  AJ_UPDATE_ACL_PARAMS_OUT.c_str(), AJ_UPDATE_ACL_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_UPDATE_METADATA.c_str(), AJ_UPDATE_METADATA_PARAMS_IN.c_str(),
                                                  AJ_UPDATE_METADATA_PARAMS_OUT.c_str(), AJ_UPDATE_METADATA_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_UPDATE_CUSTOM_METADATA.c_str(), AJ_UPDATE_METADATA_PARAMS_IN.c_str(),
                                                  AJ_UPDATE_METADATA_PARAMS_OUT.c_str(), AJ_UPDATE_METADATA_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_DEACTIVATE_ACL.c_str(), AJ_DEACTIVATE_ACL_PARAMS_IN.c_str(),
                                                  AJ_DEACTIVATE_ACL_PARAMS_OUT.c_str(), AJ_DEACTIVATE_ACL_PARAM_NAMES.c_str());
        if (*status != ER_OK) {
            goto postCreate;
        }
        interfaceDescription->Activate();
    }

postCreate:
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not create interface"));
        return;
    }

    *status = AddInterface(*interfaceDescription);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not add interface"));
        return;
    }

    const ajn::InterfaceDescription::Member* methodMember = interfaceDescription->GetMember(AJ_METHOD_ACTIVATE_ACL.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::ActivateAcl));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the ActivateAcl MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_ACL.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::GetAcl));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the GetAcl MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_ACL_STATUS.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::GetAclStatus));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the GetAclStatus MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_UPDATE_ACL.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::UpdateAcl));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the UpdateAcl MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_UPDATE_METADATA.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::UpdateMetadata));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the UpdateMetadata MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_UPDATE_CUSTOM_METADATA.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::UpdateCustomMetadata));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the UpdateCustomMetadata MethodHandler"));
        return;
    }

    methodMember = interfaceDescription->GetMember(AJ_METHOD_DEACTIVATE_ACL.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AclBusObject::DeactivateAcl));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the DeactivateAcl MethodHandler"));
        return;
    }

    QCC_DbgTrace(("Created GatewayAclBusObject successfully"));

    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        QCC_DbgHLPrintf(("AboutService is not defined"));
        *status = ER_FAIL;
        return;
    }

    std::vector<String> interfaces;
    interfaces.push_back(AJ_GW_ACL_INTERFACE);
    *status = aboutService->AddObjectDescription(m_ObjectPath, interfaces);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not add interface to ObjectDescription"));
    }

    QCC_DbgTrace((GenerateIntrospection(true).c_str()));
}

AclBusObject::~AclBusObject()
{
    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        QCC_DbgPrintf(("AboutService is not defined"));
        return;
    }

    std::vector<String> interfaces;
    interfaces.push_back(AJ_GW_ACL_INTERFACE);
    aboutService->RemoveObjectDescription(m_ObjectPath, interfaces);
}

QStatus AclBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called in GatewayAclBusObject class:"));

    if (0 == strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        return val.Set(AJPARAM_UINT16.c_str(), GATEWAY_MANAGEMENT_VERSION);
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus AclBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

void AclBusObject::ActivateAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received ActivateAcl method call"));
    uint16_t responseCode = m_Acl->updateAclStatus(GW_AS_ACTIVE);

    ajn::MsgArg replyArg[1];
    QStatus status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal responseCode for ActivateAcl method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("ActivateAcl reply call failed"));
    }
}

void AclBusObject::GetAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetAcl method call"));

    ajn::MsgArg replyArg[5];
    QStatus status = AclAdapter::marshalAcl(m_Acl, replyArg);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal Acl for GetAcl method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 5);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetAcl reply call failed."));
    }
}

void AclBusObject::GetAclStatus(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetAclStatus method call"));

    ajn::MsgArg replyArg[1];
    QStatus status = replyArg[0].Set(AJPARAM_UINT16.c_str(), m_Acl->getAclStatus());
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal response for GetAclStatus method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetAclStatus reply call failed"));
    }
}

void AclBusObject::UpdateAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received UpdateAcl method call"));

    qcc::String aclName;
    GatewayAclRules aclRules;
    std::map<qcc::String, qcc::String> metadata;
    std::map<qcc::String, qcc::String> customMetadata;
    QStatus status = AclAdapter::unmarshalAcl(msg, &aclName, &aclRules, &metadata, &customMetadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal UpdateAcl method call"));
        MethodReply(msg, status);
        return;
    }

    uint16_t responseCode = m_Acl->updateAcl(aclName, aclRules, metadata, customMetadata);

    ajn::MsgArg replyArg[1];
    status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal responseCode for UpdateAcl method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("UpdateAcl reply call failed"));
    }
}

void AclBusObject::UpdateMetadata(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received UpdateMetadata method call"));

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    if (numArgs < 1) {
        QCC_DbgHLPrintf(("Could not UpdateMetadata"));
        return;
    }

    std::map<qcc::String, qcc::String> metadata;
    QStatus status = AclAdapter::unmarshalMetadata(&args[0], &metadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal UpdateMetadata method call"));
        MethodReply(msg, status);
        return;
    }

    uint16_t responseCode = m_Acl->updateMetadata(metadata);

    ajn::MsgArg replyArg[1];
    status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal responseCode for UpdateMetadata method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("UpdateMetadata reply call failed"));
    }
}

void AclBusObject::UpdateCustomMetadata(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received UpdateCustomMetadata method call"));

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    if (numArgs < 1) {
        QCC_DbgHLPrintf(("Could not UpdateCustomMetadata"));
        return;
    }

    std::map<qcc::String, qcc::String> customMetadata;
    QStatus status = AclAdapter::unmarshalMetadata(&args[0], &customMetadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unmarshal UpdateCustomMetadata method call"));
        MethodReply(msg, status);
        return;
    }

    uint16_t responseCode = m_Acl->updateCustomMetadata(customMetadata);

    ajn::MsgArg replyArg[1];
    status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal responseCode for UpdateCustomMetadata method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("UpdateCustomMetadata reply call failed"));
    }
}

void AclBusObject::DeactivateAcl(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received DeactivateAcl method call"));
    uint16_t responseCode = m_Acl->updateAclStatus(GW_AS_INACTIVE);

    ajn::MsgArg replyArg[1];
    QStatus status = replyArg[0].Set(AJPARAM_UINT16.c_str(), responseCode);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not marshal responseCode for DeactivateAcl method"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("DeactivateAcl reply call failed"));
    }
}

} /* namespace gw */
} /* namespace ajn */



