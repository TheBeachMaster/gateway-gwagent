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

GatewayCtrlGatewayController::GatewayCtrlGatewayController(BusAttachment* bus)
{

}

GatewayCtrlGatewayController*GatewayCtrlGatewayController::getInstance()
{
    if (!m_Bus) {
        return NULL;
    }
    if (!m_instance) {
        m_instance = new GatewayCtrlGatewayController(m_Bus);
    }
    return m_instance;
}

void GatewayCtrlGatewayController::Init(BusAttachment*bus)
{
    m_Bus = bus;
}

void GatewayCtrlGatewayController::Shutdown()
{
    Release();

    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}


BusAttachment* GatewayCtrlGatewayController::GetBusAttachment()
{
    return m_Bus;
}


GatewayCtrlGateway* GatewayCtrlGatewayController::CreateGateway(qcc::String const& gatewayBusName, const AnnounceHandler::ObjectDescriptions& objectDescs, const AnnounceHandler::AboutData& aboutData)
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
        status = gateway->second->Release();

        if (status != ER_OK) {
            QCC_LogError(status, ("While releasing a gateway"));
            return status;
        }

        delete gateway->second;
    }



    m_Gateways.erase(gateway);

    return status;
}

QStatus GatewayCtrlGatewayController::deleteAllGateways()
{
    EmptyMap();

    return ER_OK;
}

const std::map<qcc::String, GatewayCtrlGateway*>& GatewayCtrlGatewayController::getGateways() const
{
    return m_Gateways;
}

void GatewayCtrlGatewayController::EmptyMap()
{
    for (std::map<qcc::String, GatewayCtrlGateway*>::iterator itr = m_Gateways.begin(); itr != m_Gateways.end(); itr++) {
        GatewayCtrlGateway*gateway = (*itr).second;
        gateway->Release();
        delete gateway;
    }

    m_Gateways.clear();
}


QStatus GatewayCtrlGatewayController::Release()
{
    EmptyMap();

    // static member variables are being taken care of in ShutDown

    return ER_OK;

}
}
}