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

#ifndef GATEWAYMANAGEMENT_H_
#define GATEWAYMANAGEMENT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/gateway/GatewayBusListener.h>
#include <map>

#define GW_WELLKNOWN_NAME "org.alljoyn.GWAgent.GMApp"

namespace ajn {
namespace services {

class GatewayPolicyManager;
class GatewayAppManager;
class GatewayMetaDataManager;

/**
 * GatewayManagement class. Used to initialize and shutdown the GatewayManagement instance
 */
class GatewayManagement {

  public:

    /**
     * Get Instance of GatewayManagement - singleton implementation
     * @return instance
     */
    static GatewayManagement* getInstance();

    /**
     * Callback when child dies
     * @param signum
     */
    static void sigChildCallback(int32_t signum);

    /**
     * Destructor for GatewayManagement
     */
    ~GatewayManagement();

    /**
     * Initialize the GatewayManagement instance
     * @param bus - bus used for GatewayManagement
     * @return status
     */
    QStatus initGatewayManagement(BusAttachment* bus);

    /**
     * Shutdown the GatewayManagement instance. Allows a new call to initGatewayManagement to be made
     * @return status
     */
    QStatus shutdownGatewayManagement();

    /**
     * Get the Version of the GatewayManagement instance
     * @return the GatewayManagement version
     */
    static uint16_t getVersion();

    /**
     * Get the BusAttachment of the GatewayManagement
     * @return the BusAttachment
     */
    BusAttachment* getBusAttachment() const;

    /**
     * Get the PolicyManager of the GatewayManagement
     * @return the getPolicyManager
     */
    GatewayPolicyManager* getPolicyManager() const;

    /**
     * Get the AppManager of the GatewayManagement
     * @return the AppManager
     */
    GatewayAppManager* getAppManager() const;

    /**
     * Get the MetaDataManager of the GatewayManagement
     * @return metaDataManager
     */
    GatewayMetaDataManager* getMetaDataManager() const;

    /**
     * Get the BusListener of the GatewayManagement
     * @return bus Listener
     */
    GatewayBusListener* getBusListener() const;

  private:

    /**
     * Default constructor for GatewayManagement
     * Private to allow for singleton implementation
     */
    GatewayManagement();

    /**
     * Instance variable - GatewayManagement is a singleton
     */
    static GatewayManagement* s_Instance;

    /**
     * BusAttachement used in GatewayManagement instance
     */
    BusAttachment* m_Bus;

    /**
     * The Buslistener of the GatewayManagement instance
     */
    GatewayBusListener* m_BusListener;

    /**
     * The PolicyManager of the GatewayManagement instance
     */
    GatewayPolicyManager* m_PolicyManager;

    /**
     * The GatewayAppManager of the GatewayManagement instance
     */
    GatewayAppManager* m_AppManager;

    /**
     * The MetaDataManager of the GatewayManagement instance
     */
    GatewayMetaDataManager* m_MetaDataManager;

};

} //namespace services
} //namespace ajn

#endif /* GATEWAYMANAGEMENT_H_ */
