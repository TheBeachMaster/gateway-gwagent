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

#include <alljoyn/gateway/GatewayCtrlGateway.h>
#include <alljoyn/gateway/GatewayCtrlGatewayController.h>
#include <alljoyn/gateway/LogModule.h>
#include <alljoyn/Status.h>
#include "GatewayCtrlConstants.h"
#include <qcc/Log.h>

using namespace ajn::services::gwcConsts;

namespace ajn {
namespace services {


QStatus GatewayCtrlGateway::init(const qcc::String& gwBusName, AboutClient::AboutData const& aboutData)
{
    m_SessionHandler  = this;
    m_Listener = 0;


    return GatewayCtrlDiscoveredApp::init(gwBusName, aboutData);
}

GatewayCtrlGateway::~GatewayCtrlGateway()
{

}

void GatewayCtrlGateway::emptyVector()
{
    for (size_t indx = 0; indx < m_InstalledApps.size(); indx++) {
        QStatus status = m_InstalledApps[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_InstalledApps[indx];
    }

    m_InstalledApps.clear();
}

QStatus GatewayCtrlGateway::retrieveInstalledApps(SessionId sessionId, std::vector<GatewayCtrlConnectorApplication*>& installedApps)
{
    QStatus status;
    {
        //Release the current vector
        emptyVector();

        BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->getBusAttachment();

        // create proxy bus object
        ProxyBusObject proxy(*busAttachment, getBusName().c_str(), AJ_OBJECTPATH_PREFIX.c_str(), sessionId, true);

        qcc::String interfaceName = AJ_GATEWAYCONTROLLER_APPMGMT_INTERFACE;
        InterfaceDescription* interfaceDescription = (InterfaceDescription*) busAttachment->GetInterface(interfaceName.c_str());
        if (!interfaceDescription) {

            status = busAttachment->CreateInterface(interfaceName.c_str(), interfaceDescription);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not create interface"));
                goto end;
            }

            status = interfaceDescription->AddMethod(AJ_METHOD_GETINSTALLEDAPPS.c_str(), NULL, "a(ssos)", "installedAppsInfoArray");
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddMethod"));
                goto end;
            }

            status = interfaceDescription->AddProperty(AJ_PROPERTY_VERSION.c_str(), AJPARAM_UINT16.c_str(), PROP_ACCESS_READ);
            if (status != ER_OK) {
                QCC_LogError(status, ("Could not AddProperty"));
                goto end;
            }

            interfaceDescription->Activate();
        }

        if (!interfaceDescription) { //if for some reason we still don't have interfaceDescription
            status = ER_FAIL;
            QCC_LogError(status, ("Interface description missing"));
            goto end;
        }

        status = proxy.AddInterface(*interfaceDescription);
        if (status != ER_OK) {
            QCC_LogError(status, ("AddInterface failed"));
            goto end;
        }

        Message replyMsg(*busAttachment);
        status = proxy.MethodCall(interfaceName.c_str(), AJ_METHOD_GETINSTALLEDAPPS.c_str(), NULL, 0, replyMsg);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to getInstalledApps failed"));
            goto end;
        }

        const ajn::MsgArg* returnArgs = NULL;
        size_t numArgs = 0;
        replyMsg->GetArgs(numArgs, returnArgs);
        if (numArgs != 1) {
            QCC_DbgHLPrintf(("Received unexpected amount of returnArgs"));
            status = ER_BUS_UNEXPECTED_SIGNATURE;
            goto end;
        }

        int numApplications;
        MsgArg* tempEntries;
        status = returnArgs[0].Get("a(ssos)", &numApplications, &tempEntries);
        if (status != ER_OK) {
            QCC_LogError(status, ("Call to Get failed"));
            goto end;
        }

        for (int i = 0; i < numApplications; i++) {

            GatewayCtrlConnectorApplication*connectorApp = new GatewayCtrlConnectorApplication();
            status = connectorApp->init(getBusName(), &tempEntries[i]);

            if (status != ER_OK) {
                QCC_LogError(status, ("Call to connectorApp->init failed"));

                delete connectorApp;
                connectorApp = NULL;

                goto end;
            }

            m_InstalledApps.push_back(connectorApp);

        }
    }
end:

    installedApps = m_InstalledApps;
    return status;
}


GatewayCtrlSessionResult GatewayCtrlGateway::joinSession() {
    return joinSession(NULL);
}


GatewayCtrlSessionResult GatewayCtrlGateway::joinSession(GatewayCtrlControllerSessionListener*listener) {

    m_Listener = listener;

    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return { ER_BUS_BUS_NOT_STARTED, -1 };
    }
    busAttachment->EnableConcurrentCallbacks();

    if (m_SessionHandler.getSessionId() != 0) {
        QCC_DbgPrintf(("Session already started"));

        if (listener) {
            QCC_DbgPrintf(("Firing Listener"));

            listener->sessionEstablished(this);
        }
        return { ER_OK, static_cast<int>(m_SessionHandler.getSessionId()) };
    }

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    SessionId sessionId = 0;
    ajn::SessionPort port = GATEWAYSERVICE_PORT;
    QStatus status = busAttachment->JoinSession(getBusName().c_str(), port, &m_SessionHandler, sessionId, opts);
    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to JoinSession with %s", getBusName().c_str()));
    }
    m_SessionHandler.JoinSessionCB(status, sessionId, opts, this);
    return { status, static_cast<int>(sessionId) };
}

QStatus GatewayCtrlGateway::joinSessionAsync(GatewayCtrlControllerSessionListener*listener)
{
    m_Listener = listener;

    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->getBusAttachment();
    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }
    busAttachment->EnableConcurrentCallbacks();

    if (m_SessionHandler.getSessionId() != 0) {
        QCC_DbgPrintf(("Session already started, firing Listener"));

        if (listener) {
            listener->sessionEstablished(this);
        }

        return ER_OK;
    }

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    QStatus status = busAttachment->JoinSessionAsync(getBusName().c_str(), (ajn::SessionPort)GATEWAYSERVICE_PORT, &m_SessionHandler,
                                                     opts, &m_SessionHandler, NULL);

    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to JoinSession with %s", getBusName().c_str()));
    }
    return status;


}

QStatus GatewayCtrlGateway::leaveSession()
{
    BusAttachment* busAttachment = GatewayCtrlGatewayController::getInstance()->getBusAttachment();

    if (!busAttachment) {
        QCC_DbgHLPrintf(("BusAttachment is not set"));
        return ER_BUS_BUS_NOT_STARTED;
    }

    SessionId sessionId = m_SessionHandler.getSessionId();
    if (sessionId == 0) {
        QCC_DbgPrintf(("Session not started. Can't end Session"));
        return ER_OK;
    }

    QStatus status = busAttachment->LeaveSession(sessionId);

    if (status != ER_OK) {
        QCC_LogError(status, ("Unable to LeaveSession. Error:%d", status));
    }

    return status;
}

GatewayCtrlControllerSessionListener* GatewayCtrlGateway::getListener() const
{
    return m_Listener;
}


QStatus GatewayCtrlGateway::release()
{
    emptyVector();

    // m_Listener is NOT deleted here since it comes from the outside

    return ER_OK;

}
}
}
