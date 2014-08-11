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

#ifndef ConnectorAppStatusSignalHandler_H
#define ConnectorAppStatusSignalHandler_H

#include <alljoyn/gateway/ConnectorAppStatus.h>

namespace ajn {
namespace gwc {
/**
 * Implement this interface to be notified about changes in the Connector App status
 */
class ConnectorAppStatusSignalHandler {
  public:
    ConnectorAppStatusSignalHandler() { }
    virtual ~ConnectorAppStatusSignalHandler() { }
    /**
     * The event is emitted when the status of the Connector App
     * changes. Avoid blocking the thread on which the method is called.
     * @param appId The application id
     * @param ConnectorAppStatus {@link ConnectorAppStatus}
     */
    virtual void onStatusSignal(const qcc::String& appId, const ConnectorAppStatus*ConnectorAppStatus) = 0;

    /**
     * An event could not be emitted because of an error creating its data
     * @param appId The application id
     * @param status {@link ConnectorAppStatus}
     */
    virtual void onError(const qcc::String& appId, const QStatus& status) = 0;
};
}
}

#endif /* defined(ConnectorAppStatusSignalHandler_H) */
