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

#ifndef GATEWAY_CONNECTOR_H_
#define GATEWAY_CONNECTOR_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/InterfaceDescription.h>
#include <qcc/String.h>
#include "alljoyn/gateway/GatewayEnums.h"

#include <list>

#include "GatewayMergedAcl.h"

namespace ajn {
namespace services {

class GatewayConnector : public MessageReceiver {
  public:
    GatewayConnector(BusAttachment* bus, qcc::String const& appName);
    virtual ~GatewayConnector();

    QStatus init();

    QStatus UpdateConnectionStatus(ConnectionStatus connStatus);

    QStatus GetMergedAcl(MergedAcl& response);
    QStatus GetMergedAclAsync(MergedAcl* response);
  protected:
    virtual void ReceiveGetMergedAclAsync(QStatus unmarshalStatus, MergedAcl* response) { }

    virtual void MergedAclUpdated() = 0;
    virtual void ShutdownApp() = 0;

  private:
    const InterfaceDescription* initInterface(QStatus& status);

    void GetMergedAclReplyHandler(Message& msg, void* mergedAcl);

    void MergedAclUpdatedSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg);
    void ShutdownAppSignalHandler(const InterfaceDescription::Member* member, const char* sourcePath, Message& msg);

    BusAttachment* bus;
    qcc::String objectPath;
    qcc::String wellKnownName;
    ProxyBusObject* remoteAppAccess;
};

} //namespace services
} //namespace ajn

#endif
