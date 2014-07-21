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

#include <alljoyn/gateway/GatewayCtrlGatewayController.h>
#include "GatewayCtrlConstants.h"
#include <qcc/Debug.h>

namespace ajn {
namespace services {

using namespace gwcConsts;

GatewayCtrlGatewayController* GatewayCtrlGatewayController::m_instance = NULL;
BusAttachment* GatewayCtrlGatewayController::m_Bus = NULL;

GatewayCtrlGatewayController::GatewayCtrlGatewayController()
{

}

GatewayCtrlGatewayController*GatewayCtrlGatewayController::getInstance()
{
    if (!m_instance) {
        m_instance = new GatewayCtrlGatewayController();
    }
    return m_instance;
}

void GatewayCtrlGatewayController::init(BusAttachment*bus)
{
    m_Bus = bus;
}

void GatewayCtrlGatewayController::shutdown()
{
    release();

    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}


BusAttachment* GatewayCtrlGatewayController::getBusAttachment()
{
    return m_Bus;
}


GatewayCtrlGateway* GatewayCtrlGatewayController::createGateway(qcc::String const& gatewayBusName, const AnnounceHandler::ObjectDescriptions& objectDescs, const AnnounceHandler::AboutData& aboutData)
{
    GatewayCtrlGateway* gateway = NULL;

    for (AboutClient::ObjectDescriptions::const_iterator it = objectDescs.begin(); it != objectDescs.end(); ++it) {
        qcc::String key = it->first;
        std::vector<qcc::String> vector = it->second;
        for (std::vector<qcc::String>::const_iterator itv = vector.begin(); itv != vector.end(); ++itv) {
            if (itv->compare(AJ_GATEWAYCONTROLLER_APPMGMT_INTERFACE) == 0) {
                if (key.compare(AJ_OBJECTPATH_PREFIX) == 0) {
                    gateway = new GatewayCtrlGateway(gatewayBusName, aboutData);
                    break;
                }
            }
        }
    }

    if (gateway) {
        m_Gateways[gatewayBusName] = gateway;
    }

    return gateway;
}

GatewayCtrlGateway* GatewayCtrlGatewayController::getGateway(qcc::String const& gatewayBusName)
{
    std::map<qcc::String, GatewayCtrlGateway*>::const_iterator gateway = m_Gateways.find(gatewayBusName);

    if (gateway != m_Gateways.end()) {
        return gateway->second;
    }
    return NULL;
}

QStatus GatewayCtrlGatewayController::deleteGateway(qcc::String const& gatewayBusName)
{
    QStatus status = ER_OK;

    std::map<qcc::String, GatewayCtrlGateway*>::iterator gateway = m_Gateways.find(gatewayBusName);
    if (gateway != m_Gateways.end()) {
        status = gateway->second->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("While releasing a gateway"));
            return status;
        }

        delete gateway->second;

        m_Gateways.erase(gateway);  //code rev

    }

    return status;
}

QStatus GatewayCtrlGatewayController::deleteAllGateways()
{
    emptyMap();

    return ER_OK;
}

const std::map<qcc::String, GatewayCtrlGateway*>& GatewayCtrlGatewayController::getGateways() const
{
    return m_Gateways;
}

void GatewayCtrlGatewayController::emptyMap()
{
    while (!m_Gateways.empty()) {
        std::map<qcc::String, GatewayCtrlGateway*>::iterator itr = m_Gateways.begin();
        GatewayCtrlGateway*gateway = (*itr).second;
        m_Gateways.erase(itr);
        gateway->release();
        delete gateway;
    }

}


QStatus GatewayCtrlGatewayController::release()
{
    emptyMap();

    // static member variables are being taken care of in ShutDown

    return ER_OK;

}
}
}