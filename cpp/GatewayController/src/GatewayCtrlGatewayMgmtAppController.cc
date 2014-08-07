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

#include <alljoyn/gateway/GatewayCtrlGatewayMgmtAppController.h>
#include "GatewayCtrlConstants.h"
#include <qcc/Debug.h>

namespace ajn {
namespace gwcontroller {

using namespace gwcConsts;

GatewayCtrlGatewayMgmtAppController* GatewayCtrlGatewayMgmtAppController::m_instance = NULL;
BusAttachment* GatewayCtrlGatewayMgmtAppController::m_Bus = NULL;

GatewayCtrlGatewayMgmtAppController::GatewayCtrlGatewayMgmtAppController()
{

}

GatewayCtrlGatewayMgmtAppController*GatewayCtrlGatewayMgmtAppController::getInstance()
{
    if (!m_instance) {
        m_instance = new GatewayCtrlGatewayMgmtAppController();
    }
    return m_instance;
}

void GatewayCtrlGatewayMgmtAppController::init(BusAttachment*bus)
{
    m_Bus = bus;
}

void GatewayCtrlGatewayMgmtAppController::shutdown()
{
    release();

    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}


BusAttachment* GatewayCtrlGatewayMgmtAppController::getBusAttachment()
{
    return m_Bus;
}


QStatus GatewayCtrlGatewayMgmtAppController::createGateway(const qcc::String& gatewayBusName, const ajn::services::AnnounceHandler::ObjectDescriptions& objectDescs, const ajn::services::AnnounceHandler::AboutData& aboutData, GatewayCtrlGatewayMgmtApp** gatewayMgmtApp)
{

    for (ajn::services::AboutClient::ObjectDescriptions::const_iterator it = objectDescs.begin(); it != objectDescs.end(); ++it) {
        qcc::String key = it->first;
        std::vector<qcc::String> vector = it->second;
        for (std::vector<qcc::String>::const_iterator itv = vector.begin(); itv != vector.end(); ++itv) {
            if (itv->compare(AJ_GATEWAYCONTROLLER_APPMGMT_INTERFACE) == 0) {
                if (key.compare(AJ_OBJECTPATH_PREFIX) == 0) {
                    *gatewayMgmtApp = new GatewayCtrlGatewayMgmtApp();
                    QStatus status = (*gatewayMgmtApp)->init(gatewayBusName, aboutData);

                    if (status != ER_OK) {
                        QCC_LogError(status, ("GatewayCtrlGatewayMgmtApp init failed"));
                        return status;
                    }
                    break;
                }
            }
        }
    }

    if (gatewayMgmtApp) {
        m_Gateways[gatewayBusName] = *gatewayMgmtApp;
    }

    return ER_OK;
}

GatewayCtrlGatewayMgmtApp* GatewayCtrlGatewayMgmtAppController::getGateway(const qcc::String& gatewayBusName)
{
    std::map<qcc::String, GatewayCtrlGatewayMgmtApp*>::const_iterator gateway = m_Gateways.find(gatewayBusName);

    if (gateway != m_Gateways.end()) {
        return gateway->second;
    }
    return NULL;
}

const std::map<qcc::String, GatewayCtrlGatewayMgmtApp*>& GatewayCtrlGatewayMgmtAppController::getGateways() const
{
    return m_Gateways;
}

void GatewayCtrlGatewayMgmtAppController::emptyMap()
{
    while (!m_Gateways.empty()) {
        std::map<qcc::String, GatewayCtrlGatewayMgmtApp*>::iterator itr = m_Gateways.begin();
        GatewayCtrlGatewayMgmtApp*gateway = (*itr).second;
        m_Gateways.erase(itr);
        gateway->release();
        delete gateway;
    }
}


QStatus GatewayCtrlGatewayMgmtAppController::release()
{
    emptyMap();

    // static member variables are being taken care of in ShutDown

    return ER_OK;

}
}
}