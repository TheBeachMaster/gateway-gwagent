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

#ifndef AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter_H
#define AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter_H

#import "alljoyn/gateway/GatewayCtrlApplicationStatusSignalHandler.h"
#import "alljoyn/gateway/GatewayCtrlConnectorApplicationStatus.h"
#import "AJGWCGatewayCtrlApplicationStatusSignalHandler.h"
/**
 AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter class
 */

class AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter : public ajn::services::GatewayCtrlApplicationStatusSignalHandler {
public:
    /**
     Constructor
     @param handle {@link AJGWCGatewayCtrlApplicationStatusSignalHandler} handle
     */
    AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter(id <AJGWCGatewayCtrlApplicationStatusSignalHandler> handle);

    /**
     * Destructor for AJGWCGatewayCtrlSessionListenerAdapter
     */
    ~AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter();

    /**
     * The event is emitted when the status of the Third Party Application
     * changes. Avoid blocking the thread on which the method is called.
     * @param appId The application id
     * @param status {@link ajn::services::GatewayCtrlConnectorApplicationStatus}
     */
    void onStatusChanged(const qcc::String &appId, const ajn::services::GatewayCtrlConnectorApplicationStatus *ConnectorApplicationStatus);

    void onError(const qcc::String& appId, const QStatus &status);

private:
    /**
     {@link AJGWCGatewayCtrlApplicationStatusSignalHandlerAdapter} handle
     */
    id <AJGWCGatewayCtrlApplicationStatusSignalHandler> applicationStatusSignalHandler;
};

#endif
