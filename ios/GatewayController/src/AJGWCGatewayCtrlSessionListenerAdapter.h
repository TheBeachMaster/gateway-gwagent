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

#ifndef AJGWCGatewayCtrlSessionListenerAdapter_H
#define AJGWCGatewayCtrlSessionListenerAdapter_H

#import "alljoyn/gateway/GatewayCtrlSessionListener.h"
#import "AJGWCGatewayCtrlGateway.h"
#import "AJGWCGatewayCtrlSessionListener.h"

class AJGWCGatewayCtrlSessionListenerAdapter : public ajn::services::GatewayCtrlSessionListener {

public:
    AJGWCGatewayCtrlSessionListenerAdapter() {};

    /**
     * Constructor for AJGWCGatewayCtrlSessionListenerAdapter
     */
    AJGWCGatewayCtrlSessionListenerAdapter(id <AJGWCGatewayCtrlSessionListener> handle);

    /**
     * Destructor for AJGWCGatewayCtrlSessionListenerAdapter
     */
    virtual ~AJGWCGatewayCtrlSessionListenerAdapter() {};

    /**
     * sessionEstablished - callback when a session is established with a device
     * @param device - the device that the session was established with
     */
    void sessionEstablished(ajn::services::GatewayCtrlGateway* gateway);

    /**
     * sessionLost - callback when a session is lost with a device
     * @param device - device that the session was lost with
     */
    void sessionLost(ajn::services::GatewayCtrlGateway* gateway);

    /**
     * Get the Listener defined for this Adapter
     * @return
     */
    id <AJGWCGatewayCtrlSessionListener> getListener() const;


private:
    /**
     {@link AJGWCGatewayCtrlSessionListener} handle
     */
    id <AJGWCGatewayCtrlSessionListener> sessionListenerHandler;

};
#endif
