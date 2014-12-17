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

#include <sys/wait.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayConnectorAppManager.h>
#include <alljoyn/gateway/GatewayMetadataManager.h>
#include <alljoyn/gateway/GatewayRouterPolicyManager.h>
#include "GatewayConstants.h"

namespace ajn {
namespace gw {

using namespace qcc;
using namespace gwConsts;

GatewayMgmt* GatewayMgmt::s_Instance(NULL);

GatewayMgmt* GatewayMgmt::getInstance()
{
    if (!s_Instance) {
        s_Instance = new GatewayMgmt();
    }

    return s_Instance;
}

GatewayMgmt::GatewayMgmt() : m_Bus(NULL), m_BusListener(NULL),
    m_RouterPolicyManager(NULL), m_ConnectorAppManager(NULL), m_MetadataManager(NULL)
{
}

GatewayMgmt::~GatewayMgmt()
{
    QCC_DbgTrace(("Shutting down"));

    if (this == s_Instance) {
        s_Instance = NULL;
    }
}

uint16_t GatewayMgmt::getVersion()
{
    return GATEWAY_MANAGEMENT_VERSION;
}

void GatewayMgmt::sigChildCallback(int32_t signum)
{
    if (!s_Instance) {
        return;
    }

    GatewayConnectorAppManager* appManager = s_Instance->getConnectorAppManager();
    if (!appManager) {
        return;
    }

    pid_t pid = wait(NULL);
    QCC_DbgPrintf(("Received SigChild for Process %i", pid));
    appManager->sigChildReceived(pid);
}

QStatus GatewayMgmt::initGatewayMgmt(BusAttachment* bus)
{
    QStatus status = ER_OK;
    QCC_DbgTrace(("Initializing GatewayManagementApp"));

    if (!bus) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Bus cannot be NULL"));
        return status;
    }

    if (!bus->IsStarted()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Bus is not started"));
        return status;
    }

    if (!bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Bus is not connected"));
        return status;
    }

    if (m_Bus && m_Bus->GetUniqueName().compare(bus->GetUniqueName()) != 0) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Bus is already set to different BusAttachment"));
        return status;
    }

    m_Bus = bus;

    if (m_MetadataManager || m_RouterPolicyManager || m_ConnectorAppManager || m_BusListener) {
        QCC_DbgPrintf(("Objects already started. Ignoring request"));
        return status;
    }

    m_BusListener = new GatewayBusListener(m_Bus);
    m_BusListener->setSessionPort(GATEWAY_PORT);
    m_Bus->RegisterBusListener(*m_BusListener);

    SessionPort servicePort = GATEWAY_PORT;
    SessionOpts sessionOpts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);

    status = m_Bus->BindSessionPort(servicePort, sessionOpts, *m_BusListener);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not bind Session Port successfully"));
        return status;
    }

    m_MetadataManager = new GatewayMetadataManager();
    status = m_MetadataManager->init();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the Metadata Manager"));
        return status;
    }

    m_RouterPolicyManager = new GatewayRouterPolicyManager();
    status = m_RouterPolicyManager->init(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the Policy Manager"));
        return status;
    }

    m_ConnectorAppManager = new GatewayConnectorAppManager();
    status = m_ConnectorAppManager->init(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the App Manager"));
        return status;
    }

    m_RouterPolicyManager->setAutoCommit(true);
    status = m_RouterPolicyManager->commit();
    if (status != ER_OK) {
        QCC_LogError(status, ("Initial commit of the Policies did not succeed"));
        return status;
    }

    status = m_MetadataManager->cleanup();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not cleanup the MetadataManager"));
        return status;
    }

    QCC_DbgPrintf(("Initialized GatewayConnectorApp successfully"));
    return status;
}

QStatus GatewayMgmt::shutdownGatewayMgmt()
{
    QStatus returnStatus = ER_OK;
    if (!m_Bus) {
        returnStatus = ER_BUS_BUS_NOT_STARTED;
        QCC_LogError(returnStatus, ("Bus not set."));
        return returnStatus;
    }

    if (m_RouterPolicyManager) {
        m_RouterPolicyManager->setAutoCommit(false);
    }

    if (m_ConnectorAppManager) {
        QStatus status = m_ConnectorAppManager->shutdown(m_Bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not shutdown the GatewayManager"));
            returnStatus = status;
        }

        delete m_ConnectorAppManager;
        m_ConnectorAppManager = NULL;
    }

    if (m_RouterPolicyManager) {
        QStatus status = m_RouterPolicyManager->commit();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not commit the Policies"));
            returnStatus = status;
        }

        status = m_RouterPolicyManager->shutdown(m_Bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not shutdown the GatewayRouterPolicyManager"));
            returnStatus = status;
        }

        delete m_RouterPolicyManager;
        m_RouterPolicyManager = NULL;
    }

    if (m_MetadataManager) {
        delete m_MetadataManager;
        m_MetadataManager = NULL;
    }

    if (m_BusListener) {
        m_Bus->UnregisterBusListener(*m_BusListener);
        delete m_BusListener;
        m_BusListener = NULL;

        SessionPort sp = GATEWAY_PORT;

        QStatus status = m_Bus->UnbindSessionPort(sp);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not unbind the SessionPort"));
            returnStatus = status;
        }
    }

    xmlCleanupParser();
    m_Bus = NULL;
    return returnStatus;
}

BusAttachment* GatewayMgmt::getBusAttachment() const
{
    return m_Bus;
}

GatewayRouterPolicyManager* GatewayMgmt::getRouterPolicyManager() const
{
    return m_RouterPolicyManager;
}

GatewayConnectorAppManager* GatewayMgmt::getConnectorAppManager() const
{
    return m_ConnectorAppManager;
}

GatewayMetadataManager* GatewayMgmt::getMetadataManager() const
{
    return m_MetadataManager;
}

GatewayBusListener* GatewayMgmt::getBusListener() const
{
    return m_BusListener;
}

} /* namespace gw */
} /* namespace ajn */

