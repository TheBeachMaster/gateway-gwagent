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

#ifndef APPMGMTBUSOBJECT_H_
#define APPMGMTBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/gateway/GatewayAppManager.h>

namespace ajn {
namespace services {

/**
 * AppMgmtBusObject - BusObject for App Management
 */
class AppMgmtBusObject : public BusObject  {
  public:

    /**
     * Constructor for GatewayAclBusObject class
     * @param bus - the bus to create the interface
     * @param gatewayManager - the gatewayManager that contains this busObject
     * @param status - success/failure
     */
    AppMgmtBusObject(BusAttachment* bus, GatewayAppManager* gatewayManager, QStatus* status);

    /**
     * Destructor for the BusObject
     */
    virtual ~AppMgmtBusObject();

    /**
     * Function callback for getInstalledApps
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetInstalledApps(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Get Property
     * @param interfaceName - name of the interface
     * @param propName - name of the property
     * @param val - msgArg to fill
     * @return status - success/failure
     */
    QStatus Get(const char* interfaceName, const char* propName, MsgArg& val);

    /**
     * Set Property
     * @param interfaceName - name of the interface
     * @param propName - name of the property
     * @param val - msgArg to fill
     * @return status - success/failure
     */
    QStatus Set(const char* interfaceName, const char* propName, MsgArg& val);

  private:

    /**
     * The Manager that contains this BusObject
     */
    GatewayAppManager* m_AppManager;

};

} /* namespace services */
} /* namespace ajn */

#endif /* APPMGMTBUSOBJECT_H_ */
