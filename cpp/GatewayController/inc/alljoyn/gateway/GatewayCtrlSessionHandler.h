/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

#ifndef GATEWAYCTRLSESSIONHANDLER_H_
#define GATEWAYCTRLSESSIONHANDLER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/SessionListener.h>

namespace ajn {
namespace services {

class GatewayCtrlGateway;

/**
 * class GatewayCtrlSessionHandler
 */
class GatewayCtrlSessionHandler : public ajn::BusAttachment::JoinSessionAsyncCB, public ajn::SessionListener {
  public:

    /**
     * Constructor for GatewayCtrlSessionHandler
     */
    GatewayCtrlSessionHandler(GatewayCtrlGateway* gateway);

    /**
     * Destructor for GatewayCtrlSessionHandler
     */
    virtual ~GatewayCtrlSessionHandler();

    /**
     * SessionLost called when a session is lost
     * @param sessionId - the session Id of the lost session
     */
    void SessionLost(ajn::SessionId sessionId);


    /**
     * Called when JoinSessionAsync() completes.
     *
     * @param status       ER_OK if successful
     * @param sessionId    Unique identifier for session.
     * @param opts         Session options.
     * @param context      User defined context which will be passed as-is to callback.
     */
    void JoinSessionCB(QStatus status, ajn::SessionId id, const ajn::SessionOpts& opts, void* context);

    /**
     * getSessionId
     * @return SessionId
     */
    ajn::SessionId getSessionId() const;

  private:

    /**
     * SessionId for this Device
     */
    ajn::SessionId m_SessionId;

    /**
     * The device of this Session Handler
     */
    GatewayCtrlGateway* m_Gateway;

};

}     /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYCTRLSESSIONHANDLER_H_ */
