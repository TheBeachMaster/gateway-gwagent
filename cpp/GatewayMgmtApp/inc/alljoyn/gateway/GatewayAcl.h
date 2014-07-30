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

#ifndef GATEWAYACL_H_
#define GATEWAYACL_H_

#include <alljoyn/gateway/GatewayAclRules.h>
#include <alljoyn/gateway/GatewayEnums.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace ajn {
namespace gw {

//forward declaration
class AclBusObject;
class GatewayConnectorApp;

/**
 * Class to define an Acl
 */
class GatewayAcl {

  public:

    /**
     * Constructor for GatewayAcl
     * @param aclId - Id for acl
     * @param connectorApp - app that contains this acl
     */
    GatewayAcl(qcc::String const& aclId, GatewayConnectorApp* connectorApp);

    /**
     * Constructor for GatewayAcl
     * @param aclId - Id for acl
     * @param aclName - name of acl
     * @param connectorApp - app that contains this acl
     * @param aclRules - rules for this acl
     * @param customMetadata - customMetadata for this acl
     * @param aclStatus - status of the acl
     */
    GatewayAcl(qcc::String const& aclId, qcc::String const& aclName, GatewayConnectorApp* connectorApp, GatewayAclRules const& aclRules,
               std::map<qcc::String, qcc::String> const& customMetadata, AclStatus aclStatus);

    /**
     * Destructor for GatewayAcl
     */
    virtual ~GatewayAcl();

    /**
     * Load the values of this Acl from a file
     * @param fileName - file used to parse
     * @return status - success/failure
     */
    QStatus loadFromFile(qcc::String const& fileName);

    /**
     * Write the Acl to its file
     * @return status - success/failure
     */
    QStatus writeToFile();

    /**
     * Initialize this Acl
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * Shutdown this Acl
     * @param bus - bus used to register
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * Get the rules of the Acl
     * @return AclRules
     */
    const GatewayAclRules& getAclRules() const;

    /**
     * Get the AclId of the Acl
     * @return the aclId
     */
    const qcc::String& getAclId() const;

    /**
     * Get the AclName of the Acl
     * @return aclName
     */
    const qcc::String& getAclName() const;

    /**
     * Get the AclStatus of the Acl
     * @return aclName
     */
    AclStatus getAclStatus() const;

    /**
     * Get the objectPath of the Acl
     * @return objectPath
     */
    const qcc::String& getObjectPath() const;

    /**
     * Get the CustomMetadata map of the Acl
     * @return CustomMetadata
     */
    const std::map<qcc::String, qcc::String>& getCustomMetadata() const;

    /**
     * Update the Acl
     * @param aclName - name of Acl
     * @param aclRules - rules to update to
     * @param metadata - metadata to update to
     * @param customMetadata - customMetadata to update to
     * @return status - success/failure
     */
    AclResponseCode updateAcl(qcc::String const& aclName, GatewayAclRules const& aclRules, std::map<qcc::String, qcc::String> const& metadata,
                              std::map<qcc::String, qcc::String> const& customMetadata);

    /**
     * Update the metadata of the Acl
     * @param metadata - metadata to update to
     * @return status - success/failure
     */
    AclResponseCode updateMetadata(std::map<qcc::String, qcc::String> const& metadata);

    /**
     * Update the customMetadata of the Acl
     * @param customMetadata - customMetadata to update to
     * @return status - success/failure
     */
    AclResponseCode updateCustomMetadata(std::map<qcc::String, qcc::String> const& customMetadata);

    /**
     * Update the AclStatus of the Acl
     * @param aclStatus - status to update to
     * @return status - success/failure
     */
    AclResponseCode updateAclStatus(AclStatus aclStatus);

  private:

    /**
     * The AclId of the Acl
     */
    qcc::String m_AclId;

    /**
     * The AclName of the Acl
     */
    qcc::String m_AclName;

    /**
     * The ObjectPath of the Acl
     */
    qcc::String m_ObjectPath;

    /**
     * The Rules of the Acl
     */
    GatewayAclRules m_AclRules;

    /**
     * The AclStatus of the Acl
     */
    AclStatus m_AclStatus;

    /**
     * Map of customMetadata received in acl
     */
    std::map<qcc::String, qcc::String> m_CustomMetadata;

    /**
     * The busObject of the Acl
     */
    AclBusObject* m_AclBusObject;

    /**
     * The Connector App that contains this Acl
     */
    GatewayConnectorApp* m_ConnectorApp;

    /**
     * Parse Metadata - helper function to parse an xml
     * @param currentKey - current key in the xml
     * @param metadata - metadata map to fill
     */
    void parseMetadata(xmlNode* currentKey, std::map<qcc::String, qcc::String>& metadata);

    /**
     * Parse Objects - helper function to parse an xml
     * @param currentKey - current key in xml
     * @param objects - objects to fill
     */
    void parseObjects(xmlNode* currentKey, GatewayRuleObjectDescriptions& objects);

    /**
     * parse the RemotedApps - helper function to parse an xml
     * @param currentKey - current key in xml
     * @param remoteAppRules - remoteAppRules to fill
     */
    void parseRemotedApp(xmlNode* currentKey, GatewayRemoteAppRules& remoteAppRules);

    /**
     * Helper function to write Objects to a file
     * @param writer - the writer to use
     * @param objects - the gateway Objects to write
     * @return rc - success/failure
     */
    int writeObjectsToFile(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects);

    /**
     * Helper function to write remotePermissions to a file
     * @param writer - the writer to use
     * @param remoteAppRules - the gateway remoteAppRules to write
     * @return rc - success/failure
     */
    int writeRemotedAppsToFile(xmlTextWriterPtr writer, const GatewayRemoteAppRules& remoteAppRules);

};

} /* namespace gw */
} /* namespace ajn */

#endif /* GATEWAYACL_H_ */
