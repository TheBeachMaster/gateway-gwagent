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
#include <alljoyn/gateway/GatewayAppManifest.h>

namespace ajn {
namespace services {

//forward declaration
class AppBusObject;

/**
 * Class that represents an App on the Gateway
 */
class GatewayApp {
  public:

    /**
     * Constructor for the GatewayApp
     * @param appId - Id of app
     * @param manifest - manifest of app
     */
    GatewayApp(qcc::String const& appId, GatewayAppManifest const& manifest);

    /**
     * Destructor for the GatewayApp
     */
    virtual ~GatewayApp();

    /**
     * Initialize this App
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * Shutdown this App
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * Restart the App
     * @return a response code - success/failure
     */
    RestartAppResponseCode restartApp();

    /**
     * Function that starts the Application
     * @return success - true/false
     */
    bool startApp();

    /**
     * Create an Acl for this App
     * @param aclId - id of Acl
     * @param aclName - name of Acl
     * @param policy - policy of Acl
     * @param metaData - metaData  of Acl
     * @param customMetaData - customMetaData  of Acl
     * @return response code - success/failure
     */
    AclResponseCode createAcl(qcc::String* aclId, qcc::String const& aclName, GatewayPolicy const& policy,
                              std::map<qcc::String, qcc::String> const& metaData, std::map<qcc::String, qcc::String> const& customMetaData);

    /**
     * Delete an Acl for this App
     * @param aclId - id of acl to be deleted
     * @return response code - success/failure
     */
    AclResponseCode deleteAcl(qcc::String const& aclId);

    /**
     * Get the appId of the app
     * @return appId
     */
    const qcc::String& getAppId() const;

    /**
     * Get the connection status of the App
     * @return connectionStatus
     */
    ConnectionStatus getConnectionStatus() const;

    /**
     * Get the InstallDescription of the App
     * @return install Description
     */
    const qcc::String& getInstallDescription() const;

    /**
     * Get the InstallStatus of the App
     * @return install Status
     */
    InstallStatus getInstallStatus() const;

    /**
     * Get the ObjectPath of the App
     * @return objectPath
     */
    const qcc::String& getObjectPath() const;

    /**
     * Get the OperationStatus of the App
     * @return the OperationalStatus
     */
    OperationalStatus getOperationalStatus() const;

    /**
     * Get the Acl of this App
     * @return map of acls
     */
    const std::map<qcc::String, GatewayAcl*>& getAcls() const;

    /**
     * Get the Manifest of the App
     * @return manifest
     */
    const GatewayAppManifest& getManifest() const;

    /**
     * Get the BusObject of this App
     * @return busObject
     */
    AppBusObject* getAppBusObject() const;

    /**
     * get the processID of the App
     * @return pid
     */
    pid_t getProcessId() const;

    /**
     * Set the ConnectionStatus of the app
     * @param connectionStatus
     */
    void setConnectionStatus(ConnectionStatus connectionStatus);

    /**
     * This app has shutdown
     */
    void sigChildReceived();

    /**
     * Update the Policy Manager with new policies
     * @return success/failure
     */
    QStatus updatePolicies();

    /**
     * Function that shuts down the Application in separate thread
     * @param thread - the thread that should be used to shutdown the App
     * @return success - true/false
     */
    bool shutdownApp(pthread_t* thread);

    /**
     * Function that returns whether this App has an active Acl
     * @return true/false
     */
    bool hasActiveAcl();

    /**
     * static Restart function for new thread
     * @param app
     */
    static void* restart(void* app);

    /**
     * static Stop function for new thread
     * @param app
     */
    static void* stop(void* app);

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
    QStatus loadAppAcls();

    /**
     * Function that shuts down the Application
     * @return success - true/false
     */
    bool shutdownApp();

    /**
     * The AppId of the App
     */
    qcc::String m_AppId;

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
    GatewayAppManifest m_Manifest;

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

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYAPP_H_ */
