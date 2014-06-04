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

#include "alljoyn/gateway/GatewayConnector.h"
#define GW_CONNECTOR_IFC_NAME "org.alljoyn.gwagent.App"
#define GW_CONNECTOR_SIG_MATCH "type='signal',interface='org.alljoyn.gwagent.App'"
#define GW_MGMNT_APP_WKN "org.alljoyn.GWAgent.GMApp"

using namespace ajn::services;
using namespace ajn;

GatewayConnector::GatewayConnector(BusAttachment* bus, qcc::String const& appName) :
    bus(bus), objectPath("/gw/"),
    wellKnownName("org.alljoyn.GWAgent.Connector."), //TODO: confirm name
    remoteAppAccess(NULL) {
    objectPath.append(appName);
    wellKnownName.append(appName);
}

GatewayConnector::~GatewayConnector() {

}

QStatus GatewayConnector::init() {
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = initInterface(status);
    if (ER_OK != status) {
        return status;
    }

    status =  bus->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(
                                             &GatewayConnector::MergedAclUpdatedSignalHandler), ifc->GetMember("MergedAclUpdated"), NULL);
    if (ER_OK != status) {
        return status;
    }

    status =  bus->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(
                                             &GatewayConnector::ShutdownAppSignalHandler), ifc->GetMember("ShutdownApp"), NULL);
    if (ER_OK != status) {
        return status;
    }

    status = bus->AddMatch(GW_CONNECTOR_SIG_MATCH);
    if (ER_OK != status) {
        return status;
    }

    remoteAppAccess = new ProxyBusObject(*bus, GW_MGMNT_APP_WKN, objectPath.c_str(), 0);
    status = remoteAppAccess->AddInterface(*ifc);
    if (ER_OK != status) {
        return status;
    }

    status = bus->RequestName(wellKnownName.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE);

    return status;
}

const InterfaceDescription* GatewayConnector::initInterface(QStatus& status) {
    status = ER_OK;

    const InterfaceDescription* ret = bus->GetInterface(GW_CONNECTOR_IFC_NAME);
    if (ret) {
        return ret;
    }

    InterfaceDescription* ifc;
    status = bus->CreateInterface(GW_CONNECTOR_IFC_NAME, ifc);
    if (ER_OK != status) {
        return NULL;
    }

    status = ifc->AddMethod("GetMergedAcl", NULL,  "a(obas)a(saya(obas))", "exposedServices,remotedApps");
    if (ER_OK != status) {
        return NULL;
    }

    status = ifc->AddMethod("UpdateConnectionStatus", "q", NULL, "connectionStatus", MEMBER_ANNOTATE_NO_REPLY);
    if (ER_OK != status) {
        return NULL;
    }

    status = ifc->AddSignal("MergedAclUpdated", NULL, NULL);
    if (ER_OK != status) {
        return NULL;
    }

    status = ifc->AddSignal("ShutdownApp", NULL, NULL);
    if (ER_OK != status) {
        return NULL;
    }

    ifc->Activate();

    return bus->GetInterface(GW_CONNECTOR_IFC_NAME);
}

QStatus GatewayConnector::GetMergedAcl(MergedAcl& response) {
    QStatus status = ER_OK;

    Message reply(*bus);
    status = remoteAppAccess->MethodCall(GW_CONNECTOR_IFC_NAME, "GetMergedAcl", NULL, 0, reply);
    if (ER_OK != status) {
        return status;
    }

    status = response.unmarshal(reply);

    return status;
}

QStatus GatewayConnector::UpdateConnectionStatus(ConnectionStatus connStatus) {
    MsgArg inputs[1];
    inputs[0].Set("q", connStatus);
    return remoteAppAccess->MethodCall(GW_CONNECTOR_IFC_NAME, "UpdateConnectionStatus", inputs, 1);
}

void GatewayConnector::MergedAclUpdatedSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg) {
    MergedAclUpdated();
}

void GatewayConnector::ShutdownAppSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg) {
    ShutdownApp();
}

QStatus GatewayConnector::GetMergedAclAsync(MergedAcl* response) {
    Message reply(*bus);
    return remoteAppAccess->MethodCallAsync(GW_CONNECTOR_IFC_NAME, "GetMergedAcl", this,
                                            static_cast<MessageReceiver::ReplyHandler>(&GatewayConnector::GetMergedAclReplyHandler), NULL, 0, response);

}

void GatewayConnector::GetMergedAclReplyHandler(Message& msg, void* mergedAcl) {
    MergedAcl* response = static_cast<MergedAcl*>(mergedAcl);
    QStatus status = response->unmarshal(msg);
    ReceiveGetMergedAclAsync(status, response);
}
