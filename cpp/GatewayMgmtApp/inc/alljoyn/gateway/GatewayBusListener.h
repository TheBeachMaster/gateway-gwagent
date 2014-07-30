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

#ifndef GATEWAYBUSLISTENER_H_
#define GATEWAYBUSLISTENER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusListener.h>
#include <alljoyn/SessionPortListener.h>
#include <vector>

namespace ajn {
namespace gw {

/**
 * Class that implements BusListener, SessionPortListener and SessionListener
 */
class GatewayBusListener : public BusListener, public SessionPortListener,
    public SessionListener {

  public:

    /**
     * Constructor of GatewayBusListener
     * @param bus - optional. the bus to use if a SessionListener should be set
     * @param daemonDisconnectCB - optional. callback for when daemon is disconnected
     */
    GatewayBusListener(BusAttachment* bus = 0, void(*daemonDisconnectCB)() = 0);

    /**
     * Destructor of GatewayBusListener
     */
    ~GatewayBusListener();

    /**
     * Set the Value of the SessionPort associated with this SessionPortListener
     * @param sessionPort
     */
    void setSessionPort(SessionPort sessionPort);

    /**
     * Get the SessionPort of the listener
     * @return
     */
    SessionPort getSessionPort();

    /**
     * AcceptSessionJoiner - Receive request to join session and decide whether to accept it or not
     * @param sessionPort - the port of the request
     * @param joiner - the name of the joiner
     * @param opts - the session options
     * @return true/false
     */
    bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);

    /**
     * SessionJoined
     * @param sessionPort
     * @param sessionId
     * @param joiner
     */
    void SessionJoined(SessionPort sessionPort, SessionId sessionId, const char* joiner);

    /**
     * SessionMemberAdded
     * @param sessionId
     * @param uniqueName
     */
    void SessionMemberAdded(SessionId sessionId, const char* uniqueName);

    /**
     * SessionMemberRemoved
     * @param sessionId
     * @param uniqueName
     */
    void SessionMemberRemoved(SessionId sessionId, const char* uniqueName);

    /**
     * SessionLost
     * @param sessionId
     * @param reason
     */
    void SessionLost(SessionId sessionId, SessionLostReason reason);

    /**
     * Get the SessionIds associated with this Listener
     * @return vector of sessionIds
     */
    const std::vector<SessionId>& getSessionIds() const;

    /**
     * Function for when Bus has been disconnected
     */
    void BusDisconnected();

  private:

    /**
     * The port used as part of the join session request
     */
    SessionPort m_SessionPort;

    /**
     * The busAttachment to use
     */
    BusAttachment* m_Bus;

    /**
     * The sessionIds for the port
     */
    std::vector<SessionId> m_SessionIds;

    /**
     * Callback when daemon is disconnected
     */
    void (*m_DaemonDisconnectCB)();
};
}
}

#endif /* GATEWAYBUSLISTENER_H_ */
