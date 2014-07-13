/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#include <alljoyn/gateway/GatewayCtrlSessionHandler.h>
#include <alljoyn/gateway/GatewayCtrlGateway.h>
#include "GatewayCtrlConstants.h"
#include <sstream>
#include <alljoyn/gateway/LogModule.h>

namespace ajn {
namespace services {
using namespace gwcConsts;


GatewayCtrlSessionHandler::GatewayCtrlSessionHandler(GatewayCtrlGateway* gateway) : m_SessionId(0), m_Gateway(gateway)
{

}

GatewayCtrlSessionHandler::~GatewayCtrlSessionHandler()
{
}


void GatewayCtrlSessionHandler::SessionLost(ajn::SessionId sessionId)
{
    QCC_DbgPrintf(("Session lost for sessionId: %u", sessionId));
    m_SessionId = 0;

    GatewayCtrlControllerSessionListener* listener = m_Gateway->getListener();
    if (listener) {
        listener->sessionLost(m_Gateway);
    }
}

void GatewayCtrlSessionHandler::JoinSessionCB(QStatus status, ajn::SessionId id, const ajn::SessionOpts& opts, void* context)
{
    GatewayCtrlControllerSessionListener* listener = m_Gateway->getListener();

    if (status != ER_OK) {
        QCC_LogError(status, ("Joining session failed."));

        if (listener) {
            listener->sessionLost(m_Gateway);
        }
        return;
    }

    QCC_DbgPrintf(("Joining session succeeded. SessionId: %u", id));

    m_SessionId = id;

    if (listener) {
        listener->sessionEstablished(m_Gateway);
    }
}

ajn::SessionId GatewayCtrlSessionHandler::getSessionId() const
{
    return m_SessionId;
}

}     /* namespace services */
} /* namespace ajn */
