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

#ifndef GatewayCtrlSessionListener_H
#define GatewayCtrlSessionListener_H

#include <alljoyn/SessionListener.h>
#include <alljoyn/SessionPortListener.h>

namespace ajn {
namespace services {

class GatewayCtrlGateway;

/**
 *  This class is responsible for handling session related events from the AllJoyn system.
 *  Extend this class to receive the events of:
 *      - sessionEstablished
 *      - sessionLost
 *
 *  The events are called on the AllJoyn thread, so avoid blocking them with
 *  long running tasks.
 */
class GatewayCtrlSessionListener {
  public:

    /**
     * Constructor for GatewayCtrlSessionListener
     */
    GatewayCtrlSessionListener() { };

    /**
     * Destructor for GatewayCtrlSessionListener
     */
    virtual ~GatewayCtrlSessionListener() { };

    /**
     * sessionEstablished - callback when a session is established with a device
     * @param device - the device that the session was established with
     */
    virtual void sessionEstablished(GatewayCtrlGateway* gateway) = 0;

    /**
     * sessionLost - callback when a session is lost with a device
     * @param device - device that the session was lost with
     */
    virtual void sessionLost(GatewayCtrlGateway* gateway) = 0;
};
}
}
#endif /* defined(GatewayCtrlSessionListener_H) */