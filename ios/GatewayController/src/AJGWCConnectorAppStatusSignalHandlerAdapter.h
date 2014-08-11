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

#ifndef AJGWCConnectorAppStatusSignalHandlerAdapter_H
#define AJGWCConnectorAppStatusSignalHandlerAdapter_H

#import "alljoyn/gateway/ConnectorAppStatusSignalHandler.h"
#import "alljoyn/gateway/ConnectorAppStatus.h"
#import "AJGWCConnectorAppStatusSignalHandler.h"
/**
 AJGWCConnectorAppStatusSignalHandlerAdapter class
 */

class AJGWCConnectorAppStatusSignalHandlerAdapter : public ajn::gwc::ConnectorAppStatusSignalHandler {
public:
    /**
     Constructor
     @param handle {@link AJGWCConnectorAppStatusSignalHandler} handle
     */
    AJGWCConnectorAppStatusSignalHandlerAdapter(id <AJGWCConnectorAppStatusSignalHandler> handle);

    /**
     * Destructor for AJGWCSessionListenerAdapter
     */
    ~AJGWCConnectorAppStatusSignalHandlerAdapter();

    /**
     * The event is emitted when the status of the Connector App
     * changes. Avoid blocking the thread on which the method is called.
     * @param appId The application id
     * @param status {@link ajn::gwc::ConnectorAppStatus}
     */
    void onStatusSignal(const qcc::String &appId, const ajn::gwc::ConnectorAppStatus *ConnectorAppStatus);

    void onError(const qcc::String& appId, const QStatus &status);

private:
    /**
     {@link AJGWCConnectorAppStatusSignalHandlerAdapter} handle
     */
    id <AJGWCConnectorAppStatusSignalHandler> connectorAppStatusSignalHandler;
};

#endif
