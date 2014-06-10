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
#include <alljoyn/gateway/GatewayManagement.h>
#include <map>

namespace ajn {
namespace services {

//forward declarations
class AppMgmtBusObject;
class GatewayApp;

/**
 * Class used to manage Applications
 */
class GatewayAppManager {
  public:

    /**
     * Constructor for GatewayAppManager
     */
    GatewayAppManager();

    /**
     * Destructor for GatewayAppManager
     */
    virtual ~GatewayAppManager();

    /**
     * Initialize the GatewayAppManager
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * Shutdown the GatewayAppManager
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
    std::map<qcc::String, GatewayApp*> getApps() const;

  private:

    /**
     * Load the installed Apps by parsing the apps directory
     * @return
     */
    QStatus loadInstalledApps();

    /**
     * BusObject used for AppManagement
     */
    AppMgmtBusObject* m_AppMgmtBusObject;

    /**
     * The map storing the Apps
     */
    std::map<qcc::String, GatewayApp*> m_Apps;
};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYAPPMANAGER_H_ */
