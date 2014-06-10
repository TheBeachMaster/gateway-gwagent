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
#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/gateway/GatewayAppManager.h>
#include <alljoyn/gateway/GatewayMetaDataManager.h>
#include <alljoyn/gateway/GatewayPolicyManager.h>
#include "GatewayConstants.h"

namespace ajn {
namespace services {

using namespace qcc;
using namespace gwConsts;

GatewayManagement* GatewayManagement::s_Instance(NULL);

GatewayManagement* GatewayManagement::getInstance()
{
    if (!s_Instance) {
        s_Instance = new GatewayManagement();
    }

    return s_Instance;
}

GatewayManagement::GatewayManagement() : m_Bus(NULL), m_BusListener(NULL),
    m_PolicyManager(NULL), m_AppManager(NULL), m_MetaDataManager(NULL)
{
}

GatewayManagement::~GatewayManagement()
{
    QCC_DbgTrace(("Shutting down"));

    if (this == s_Instance) {
        s_Instance = NULL;
    }
}

uint16_t GatewayManagement::getVersion()
{
    return GATEWAY_MANAGEMENT_VERSION;
}

void GatewayManagement::sigChildCallback(int32_t signum)
{
    if (!s_Instance) {
        return;
    }

    GatewayAppManager* appManager = s_Instance->getAppManager();
    if (!appManager) {
        return;
    }

    pid_t pid = wait(NULL);
    QCC_DbgPrintf(("Received SigChild for Process %i", pid));
    appManager->sigChildReceived(pid);
}

QStatus GatewayManagement::initGatewayManagement(BusAttachment* bus)
{
    QStatus status = ER_OK;
    QCC_DbgTrace(("Initializing GatewayApp"));

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

    if (m_MetaDataManager || m_PolicyManager || m_AppManager || m_BusListener) {
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

    m_MetaDataManager = new GatewayMetaDataManager();
    status = m_MetaDataManager->init();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the MetaData Manager"));
        return status;
    }

    m_PolicyManager = new GatewayPolicyManager();
    status = m_PolicyManager->init(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the Policy Manager"));
        return status;
    }

    m_AppManager = new GatewayAppManager();
    status = m_AppManager->init(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize the App Manager"));
        return status;
    }

    m_PolicyManager->setAutoCommit(true);
    status = m_PolicyManager->commitPolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not commit the Policies"));
        return status;
    }

    status = m_MetaDataManager->cleanup();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not cleanup the MetaDataManager"));
        return status;
    }

    QCC_DbgPrintf(("Initialized GatewayApp successfully"));
    return status;
}

QStatus GatewayManagement::shutdownGatewayManagement()
{
    QStatus returnStatus = ER_OK;
    if (!m_Bus) {
        returnStatus = ER_BUS_BUS_NOT_STARTED;
        QCC_LogError(returnStatus, ("Bus not set."));
        return returnStatus;
    }

    if (m_PolicyManager) {
        m_PolicyManager->setAutoCommit(false);
    }

    if (m_AppManager) {
        QStatus status = m_AppManager->shutdown(m_Bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not shutdown the GatewayManager"));
            returnStatus = status;
        }

        delete m_AppManager;
        m_AppManager = NULL;
    }

    if (m_PolicyManager) {
        QStatus status = m_PolicyManager->commitPolicies();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not commit the Policies"));
            returnStatus = status;
        }

        status = m_PolicyManager->shutdown(m_Bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not shutdown the GatewayPolicyManager"));
            returnStatus = status;
        }

        delete m_PolicyManager;
        m_PolicyManager = NULL;
    }

    if (m_MetaDataManager) {
        delete m_MetaDataManager;
        m_MetaDataManager = NULL;
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

    m_Bus = NULL;
    return returnStatus;
}

BusAttachment* GatewayManagement::getBusAttachment() const
{
    return m_Bus;
}

GatewayPolicyManager* GatewayManagement::getPolicyManager() const
{
    return m_PolicyManager;
}

GatewayAppManager* GatewayManagement::getAppManager() const
{
    return m_AppManager;
}

GatewayMetaDataManager* GatewayManagement::getMetaDataManager() const
{
    return m_MetaDataManager;
}

GatewayBusListener* GatewayManagement::getBusListener() const
{
    return m_BusListener;
}

} /* namespace services */
} /* namespace ajn */

