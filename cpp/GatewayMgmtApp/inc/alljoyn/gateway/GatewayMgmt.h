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
namespace gw {

class GatewayRouterPolicyManager;
class GatewayConnectorAppManager;
class GatewayMetadataManager;

/**
 * GatewayMgmt class. Used to initialize and shutdown the GatewayMgmt instance
 */
class GatewayMgmt {

  public:

    /**
     * Get Instance of GatewayMgmt - singleton implementation
     * @return instance
     */
    static GatewayMgmt* getInstance();

    /**
     * Callback when child dies
     * @param signum
     */
    static void sigChildCallback(int32_t signum);

    /**
     * Destructor for GatewayMgmt
     */
    ~GatewayMgmt();

    /**
     * Initialize the GatewayMgmt instance
     * @param bus - bus used for GatewayMgmt
     * @return status
     */
    QStatus initGatewayMgmt(BusAttachment* bus);

    /**
     * Shutdown the GatewayMgmt instance. Allows a new call to initGatewayMgmt to be made
     * @return status
     */
    QStatus shutdownGatewayMgmt();

    /**
     * Get the Version of the GatewayMgmt instance
     * @return the GatewayMgmt version
     */
    static uint16_t getVersion();

    /**
     * Get the BusAttachment of the GatewayMgmt
     * @return the BusAttachment
     */
    BusAttachment* getBusAttachment() const;

    /**
     * Get the RouterPolicyManager of the GatewayMgmt
     * @return the routerPolicyManager
     */
    GatewayRouterPolicyManager* getRouterPolicyManager() const;

    /**
     * Get the ConnectorAppManager of the GatewayMgmt
     * @return the ConnectorAppManager
     */
    GatewayConnectorAppManager* getConnectorAppManager() const;

    /**
     * Get the MetadataManager of the GatewayMgmt
     * @return metadataManager
     */
    GatewayMetadataManager* getMetadataManager() const;

    /**
     * Get the BusListener of the GatewayMgmt
     * @return bus Listener
     */
    GatewayBusListener* getBusListener() const;

  private:

    /**
     * Default constructor for GatewayMgmt
     * Private to allow for singleton implementation
     */
    GatewayMgmt();

    /**
     * Instance variable - GatewayMgmt is a singleton
     */
    static GatewayMgmt* s_Instance;

    /**
     * BusAttachement used in GatewayMgmt instance
     */
    BusAttachment* m_Bus;

    /**
     * The Buslistener of the GatewayMgmt instance
     */
    GatewayBusListener* m_BusListener;

    /**
     * The RouterPolicyManager of the GatewayMgmt instance
     */
    GatewayRouterPolicyManager* m_RouterPolicyManager;

    /**
     * The GatewayConnectorAppManager of the GatewayMgmt instance
     */
    GatewayConnectorAppManager* m_ConnectorAppManager;

    /**
     * The MetadataManager of the GatewayMgmt instance
     */
    GatewayMetadataManager* m_MetadataManager;

};

} //namespace gw
} //namespace ajn

#endif /* GATEWAYMANAGEMENT_H_ */
