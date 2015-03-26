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

#include "AppMgmtBusObject.h"
#include "../GatewayConstants.h"
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayConnectorApp.h>
#include <vector>

namespace ajn {
namespace gw {
using namespace qcc;
using namespace gwConsts;

AppMgmtBusObject::AppMgmtBusObject(BusAttachment* bus, GatewayConnectorAppManager* connectorAppManager, QStatus* status) :
    BusObject(AJ_GW_OBJECTPATH.c_str()), m_ConnectorAppManager(connectorAppManager)
{
    InterfaceDescription* interfaceDescription = (InterfaceDescription*) bus->GetInterface(AJ_GW_APP_MGMT_INTERFACE.c_str());
    if (!interfaceDescription) {
        *status = bus->CreateInterface(AJ_GW_APP_MGMT_INTERFACE.c_str(), interfaceDescription, true);
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
        if (*status != ER_OK) {
            goto postCreate;
        }
        *status = interfaceDescription->AddMethod(AJ_METHOD_GET_INSTALLED_APPS.c_str(), AJ_GET_INSTALLED_APPS_PARAMS_IN.c_str(),
                                                  AJ_GET_INSTALLED_APPS_PARAMS_OUT.c_str(), AJ_GET_INSTALLED_APPS_PARAM_NAMES.c_str());
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

    *status = AddInterface(*interfaceDescription, ANNOUNCED);
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not add interface"));
        return;
    }

    const ajn::InterfaceDescription::Member* methodMember = interfaceDescription->GetMember(AJ_METHOD_GET_INSTALLED_APPS.c_str());
    *status = AddMethodHandler(methodMember, static_cast<MessageReceiver::MethodHandler>(&AppMgmtBusObject::GetInstalledApps));
    if (*status != ER_OK) {
        QCC_LogError(*status, ("Could not register the GetInstalledApps MethodHandler"));
        return;
    }

    std::vector<String> interfaces;
    interfaces.push_back(AJ_GW_APP_MGMT_INTERFACE);

    QCC_DbgTrace(("Created AppMgmtBusObject successfully"));
    QCC_DbgTrace((GenerateIntrospection(true).c_str()));
}

AppMgmtBusObject::~AppMgmtBusObject()
{
}

QStatus AppMgmtBusObject::Get(const char* interfaceName, const char* propName, MsgArg& val)
{
    QCC_DbgTrace(("Get property was called in AppMgmtBusObject class:"));

    if (0 == strcmp(AJ_PROPERTY_VERSION.c_str(), propName)) {
        return val.Set(AJPARAM_UINT16.c_str(), GATEWAY_MANAGEMENT_VERSION);
    }
    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus AppMgmtBusObject::Set(const char* interfaceName, const char* propName, MsgArg& val)
{
    return ER_ALLJOYN_ACCESS_PERMISSION_ERROR;
}

void AppMgmtBusObject::GetInstalledApps(const InterfaceDescription::Member* member, Message& msg)
{
    QCC_DbgTrace(("Received GetInstalledApps method call"));

    QStatus status;
    ajn::MsgArg replyArg[1];

    std::map<String, GatewayConnectorApp*>::iterator it;
    std::map<String, GatewayConnectorApp*> apps = m_ConnectorAppManager->getConnectorApps();
    std::vector<MsgArg> appInfo(apps.size());
    size_t appInfoSize = 0;
    for (it = apps.begin(); it != apps.end(); it++) {
        status = appInfo[appInfoSize++].Set(AJPARAM_INSTALLED_APPS_INFO.c_str(), it->second->getConnectorId().c_str(),
                                            it->second->getManifest().getFriendlyName().c_str(),
                                            it->second->getObjectPath().c_str(),
                                            it->second->getManifest().getVersion().c_str());
        if (status != ER_OK) {
            QCC_LogError(status, ("Can't marshal InstalledAppInfo - responding with error"));
            MethodReply(msg, status);
            return;
        }
    }

    status = replyArg[0].Set(AJPARAM_INSTALLED_APPS_INFO_ARRAY.c_str(), appInfoSize, appInfo.data());
    if (status != ER_OK) {
        QCC_LogError(status, ("Can't marshal InstalledAppInfo - responding with error"));
        MethodReply(msg, status);
        return;
    }

    status = MethodReply(msg, replyArg, 1);
    if (status != ER_OK) {
        QCC_LogError(status, ("GetInstalledApps reply call failed"));
    }
}

} /* namespace gw */
} /* namespace ajn */




