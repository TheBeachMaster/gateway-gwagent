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
#define GW_CONNECTOR_IFC_NAME "org.alljoyn.gwagent.connector.App"
#define GW_CONNECTOR_SIG_MATCH "type='signal',interface='org.alljoyn.gwagent.connector.App'"
#define GW_MGMNT_APP_WKN "org.alljoyn.GWAgent.GMApp"

using namespace ajn::gw;
using namespace ajn;

GatewayConnector::GatewayConnector(BusAttachment* bus, qcc::String const& appName) :
    m_Bus(bus), m_ObjectPath("/gw/"),
    m_WellKnownName("org.alljoyn.GWAgent.Connector."),
    m_RemoteAppAccess(NULL)
{
    m_ObjectPath.append(appName);
    m_WellKnownName.append(appName);
}

GatewayConnector::~GatewayConnector()
{

}

QStatus GatewayConnector::init()
{
    QStatus status = ER_OK;

    const InterfaceDescription* ifc = initInterface(status);
    if (ER_OK != status) {
        return status;
    }

    status =  m_Bus->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(
                                               &GatewayConnector::mergedAclUpdatedSignalHandler), ifc->GetMember("MergedAclUpdated"), NULL);
    if (ER_OK != status) {
        return status;
    }

    status =  m_Bus->RegisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(
                                               &GatewayConnector::shutdownSignalHandler), ifc->GetMember("ShutdownApp"), NULL);
    if (ER_OK != status) {
        return status;
    }

    status = m_Bus->AddMatch(GW_CONNECTOR_SIG_MATCH);
    if (ER_OK != status) {
        return status;
    }

    m_RemoteAppAccess = new ProxyBusObject(*m_Bus, GW_MGMNT_APP_WKN, m_ObjectPath.c_str(), 0);
    status = m_RemoteAppAccess->AddInterface(*ifc);
    if (ER_OK != status) {
        return status;
    }

    status = m_Bus->RequestName(m_WellKnownName.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE);

    return status;
}

const InterfaceDescription* GatewayConnector::initInterface(QStatus& status)
{
    status = ER_OK;

    const InterfaceDescription* ret = m_Bus->GetInterface(GW_CONNECTOR_IFC_NAME);
    if (ret) {
        return ret;
    }

    InterfaceDescription* ifc;
    status = m_Bus->CreateInterface(GW_CONNECTOR_IFC_NAME, ifc);
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

    return m_Bus->GetInterface(GW_CONNECTOR_IFC_NAME);
}

QStatus GatewayConnector::getMergedAcl(GatewayMergedAcl* response)
{
    QStatus status = ER_OK;

    Message reply(*m_Bus);
    status = m_RemoteAppAccess->MethodCall(GW_CONNECTOR_IFC_NAME, "GetMergedAcl", NULL, 0, reply);
    if (ER_OK != status) {
        return status;
    }

    status = response->unmarshal(reply);

    return status;
}

QStatus GatewayConnector::updateConnectionStatus(ConnectionStatus connStatus)
{
    MsgArg input[1];
    input[0].Set("q", connStatus);
    return m_RemoteAppAccess->MethodCall(GW_CONNECTOR_IFC_NAME, "UpdateConnectionStatus", input, 1);
}

void GatewayConnector::mergedAclUpdatedSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg)
{
    mergedAclUpdated();
}

void GatewayConnector::shutdownSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg)
{
    shutdown();
}

QStatus GatewayConnector::getMergedAclAsync(GatewayMergedAcl* response)
{
    Message reply(*m_Bus);
    return m_RemoteAppAccess->MethodCallAsync(GW_CONNECTOR_IFC_NAME, "GetMergedAcl", this,
                                              static_cast<MessageReceiver::ReplyHandler>(&GatewayConnector::getMergedAclReplyHandler), NULL, 0, response);

}

void GatewayConnector::getMergedAclReplyHandler(Message& msg, void* mergedAcl) {
    GatewayMergedAcl* response = static_cast<GatewayMergedAcl*>(mergedAcl);
    QStatus status = response->unmarshal(msg);
    receiveGetMergedAclAsync(status, response);
}
