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

#ifndef AJGWCSessionListenerAdapter_H
#define AJGWCSessionListenerAdapter_H

#import "alljoyn/gateway/SessionListener.h"
#import "AJGWCGatewayMgmtApp.h"
#import "AJGWCSessionListener.h"

class AJGWCSessionListenerAdapter : public ajn::gwc::SessionListener {

public:
    AJGWCSessionListenerAdapter() {};

    /**
     * Constructor for AJGWCSessionListenerAdapter
     */
    AJGWCSessionListenerAdapter(id <AJGWCSessionListener> handle);

    /**
     * Destructor for AJGWCSessionListenerAdapter
     */
    virtual ~AJGWCSessionListenerAdapter() {};

    /**
     * sessionEstablished - callback when a session is established with a device
     * @param gatewayMgmtApp - the gateway that the session was established with
     */
    void sessionEstablished(ajn::gwc::GatewayMgmtApp* gatewayMgmtApp);

    /**
     * sessionLost - callback when a session is lost with a device
     * @param gatewayMgmtApp - the gateway that the session was lost with
     */
    void sessionLost(ajn::gwc::GatewayMgmtApp* gatewayMgmtApp);

    /**
     * Get the Listener defined for this Adapter
     * @return
     */
    id <AJGWCSessionListener> getListener() const;


private:
    /**
     {@link AJGWCSessionListener} handle
     */
    id <AJGWCSessionListener> sessionListenerHandler;

};
#endif
