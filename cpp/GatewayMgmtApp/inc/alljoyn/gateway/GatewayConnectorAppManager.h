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

#ifndef GATEWAYAPPMANAGER_H_
#define GATEWAYAPPMANAGER_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <map>

namespace ajn {
namespace gw {

//forward declarations
class AppMgmtBusObject;
class GatewayConnectorApp;

/**
 * Class used to manage Applications
 */
class GatewayConnectorAppManager {
  public:

    /**
     * Constructor for GatewayConnectorAppManager
     */
    GatewayConnectorAppManager();

    /**
     * Destructor for GatewayConnectorAppManager
     */
    virtual ~GatewayConnectorAppManager();

    /**
     * Initialize the GatewayConnectorAppManager
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * Shutdown the GatewayConnectorAppManager
     * @param bus - bus used to unregister
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * receive Sig Child Signal
     * @param pid - pid of process that died
     */
    void sigChildReceived(pid_t pid);

    /**
     * Get the Apps stored by the App Manager
     * @return apps
     */
    std::map<qcc::String, GatewayConnectorApp*> getConnectorApps() const;

  private:

    /**
     * Load the installed Apps by parsing the apps directory
     * @return
     */
    QStatus loadConnectorApps();

    /**
     * BusObject used for AppMgmt
     */
    AppMgmtBusObject* m_AppMgmtBusObject;

    /**
     * The map storing the Apps
     */
    std::map<qcc::String, GatewayConnectorApp*> m_ConnectorApps;
};

} /* namespace gw */
} /* namespace ajn */

#endif /* GATEWAYAPPMANAGER_H_ */
