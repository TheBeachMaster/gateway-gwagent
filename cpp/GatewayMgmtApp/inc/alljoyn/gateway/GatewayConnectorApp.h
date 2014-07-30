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

#ifndef GATEWAYAPP_H_
#define GATEWAYAPP_H_

#include <map>
#include <qcc/String.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/gateway/GatewayEnums.h>
#include <alljoyn/gateway/GatewayAcl.h>
#include <alljoyn/gateway/GatewayConnectorAppManifest.h>

namespace ajn {
namespace gw {

//forward declaration
class AppBusObject;

/**
 * Class that represents an App on the Gateway
 */
class GatewayConnectorApp {
  public:

    /**
     * Constructor for the GatewayConnectorApp
     * @param connectorId - Id of app
     * @param manifest - manifest of app
     */
    GatewayConnectorApp(qcc::String const& connectorId, GatewayConnectorAppManifest const& manifest);

    /**
     * Destructor for the GatewayConnectorApp
     */
    virtual ~GatewayConnectorApp();

    /**
     * Initialize this Connector App
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * Shutdown this Connector App
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * Restart the Connector App
     * @return a response code - success/failure
     */
    RestartAppResponseCode restartConnectorApp();

    /**
     * Function that starts the Connector App
     * @return success - true/false
     */
    bool startConnectorApp();

    /**
     * Create an Acl for this App
     * @param aclId - id of the Acl
     * @param aclName - name of the Acl
     * @param aclRules - rules of the Acl
     * @param metadata - metadata  of the Acl
     * @param customMetadata - customMetadata  of Acl
     * @return response code - success/failure
     */
    AclResponseCode createAcl(qcc::String* aclId, qcc::String const& aclName, GatewayAclRules const& aclRules,
                              std::map<qcc::String, qcc::String> const& metadata, std::map<qcc::String, qcc::String> const& customMetadata);

    /**
     * Delete an Acl for this App
     * @param aclId - id of acl to be deleted
     * @return response code - success/failure
     */
    AclResponseCode deleteAcl(qcc::String const& aclId);

    /**
     * Get the connectorId of the Connector App
     * @return connectorId
     */
    const qcc::String& getConnectorId() const;

    /**
     * Get the connection status of the Connector App
     * @return connectionStatus
     */
    ConnectionStatus getConnectionStatus() const;

    /**
     * Get the InstallDescription of the Connector App
     * @return install Description
     */
    const qcc::String& getInstallDescription() const;

    /**
     * Get the InstallStatus of the Connector App
     * @return install Status
     */
    InstallStatus getInstallStatus() const;

    /**
     * Get the ObjectPath of the Connector App
     * @return objectPath
     */
    const qcc::String& getObjectPath() const;

    /**
     * Get the OperationStatus of the Connector App
     * @return the OperationalStatus
     */
    OperationalStatus getOperationalStatus() const;

    /**
     * Get the Acl of this Connector App
     * @return map of acls
     */
    const std::map<qcc::String, GatewayAcl*>& getAcls() const;

    /**
     * Get the Manifest of the Connector App
     * @return manifest
     */
    const GatewayConnectorAppManifest& getManifest() const;

    /**
     * Get the BusObject of this Connector App
     * @return busObject
     */
    AppBusObject* getAppBusObject() const;

    /**
     * get the processID of the Connector App
     * @return pid
     */
    pid_t getProcessId() const;

    /**
     * Set the ConnectionStatus of the Connector App
     * @param connectionStatus
     */
    void setConnectionStatus(ConnectionStatus connectionStatus);

    /**
     * This Connector app has shutdown
     */
    void sigChildReceived();

    /**
     * Update the Policy Manager with new AclRules
     * @return success/failure
     */
    QStatus updatePolicyManager();

    /**
     * Function that shuts down the Application in separate thread
     * @param thread - the thread that should be used to shutdown the Connector App
     * @return success - true/false
     */
    bool shutdownConnectorApp(pthread_t* thread);

    /**
     * Function that returns whether this Connector App has an active Acl
     * @return true/false
     */
    bool hasActiveAcl();

    /**
     * static Restart function for new thread
     * @param app
     */
    static void* Restart(void* app);

    /**
     * static Stop function for new thread
     * @param app
     */
    static void* Stop(void* app);

  private:

    /**
     * Generate an AclId based on the aclName
     * @param aclName - the AclName
     * @return the AclId
     */
    qcc::String generateAclId(qcc::String const& aclName);

    /**
     * load the Acls of this App
     * @return status - success/failure
     */
    QStatus loadAcls();

    /**
     * Function that shuts down the Application
     * @return success - true/false
     */
    bool shutdownConnectorApp();

    /**
     * The connectorId of the App
     */
    qcc::String m_ConnectorId;

    /**
     * The ObjectPath of the App
     */
    qcc::String m_ObjectPath;

    /**
     * The ConnectionStatus of the App
     */
    ConnectionStatus m_ConnectionStatus;

    /**
     * The OperationalStatus of the App
     */
    OperationalStatus m_OperationalStatus;

    /**
     * The InstallStatus of the App
     */
    InstallStatus m_InstallStatus;

    /**
     * The InstallDescription of the App
     */
    qcc::String m_InstallDescription;

    /**
     * The Manifest of the App
     */
    GatewayConnectorAppManifest m_Manifest;

    /**
     * The BusObject of the App
     */
    AppBusObject* m_AppBusObject;

    /**
     * The PID of the App
     */
    pid_t m_ProcessId;

    /**
     * The Acls of this App
     */
    std::map<qcc::String, GatewayAcl*> m_Acls;
};

} /* namespace gw */
} /* namespace ajn */

#endif /* GATEWAYAPP_H_ */
