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

#include <alljoyn/gateway/GatewayPolicy.h>
#include <alljoyn/gateway/GatewayEnums.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace ajn {
namespace services {

//forward declaration
class AclBusObject;
class GatewayApp;

/**
 * Class to define an Acl
 */
class GatewayAcl {

  public:

    /**
     * Constructor for GatewayAcl
     * @param aclId - Id for acl
     * @param app - app that contains this acl
     */
    GatewayAcl(qcc::String const& aclId, GatewayApp* app);

    /**
     * Constructor for GatewayAcl
     * @param aclId - Id for acl
     * @param aclName - name of acl
     * @param app - app that contains this acl
     * @param policy - policy for this acl
     * @param customMetaData - customMetaData for this acl
     * @param aclStatus - status of the acl
     */
    GatewayAcl(qcc::String const& aclId, qcc::String const& aclName, GatewayApp* app, GatewayPolicy const& policy,
               std::map<qcc::String, qcc::String> const& customMetaData, AclStatus aclStatus);

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
     *	Write the Acl to its file
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
     * Get the policy of the Acl
     * @return policy
     */
    const GatewayPolicy& getPolicy() const;

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
     * Get the CustomMetaData map of the Acl
     * @return CustomMetaData
     */
    const std::map<qcc::String, qcc::String>& getCustomMetaData() const;

    /**
     * Update the Acl
     * @param aclName - name of Acl
     * @param policy - policy to update to
     * @param metaData - metaData to update to
     * @param customMetaData - customMetaData to update to
     * @return status - success/failure
     */
    AclResponseCode updateAcl(qcc::String const& aclName, GatewayPolicy const& policy, std::map<qcc::String, qcc::String> const& metaData,
                              std::map<qcc::String, qcc::String> const& customMetaData);

    /**
     * Update the metaData of the Acl
     * @param metaData - metaData to update to
     * @return status - success/failure
     */
    AclResponseCode updateMetaData(std::map<qcc::String, qcc::String> const& metaData);

    /**
     * Update the customMetaData of the Acl
     * @param customMetaData - customMetaData to update to
     * @return status - success/failure
     */
    AclResponseCode updateCustomMetaData(std::map<qcc::String, qcc::String> const& customMetaData);

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
     * The Policy of the Acl
     */
    GatewayPolicy m_Policy;

    /**
     * The AclStatus of the Acl
     */
    AclStatus m_AclStatus;

    /**
     * Map of customMetaData received in acl
     */
    std::map<qcc::String, qcc::String> m_CustomMetaData;

    /**
     * The busObject of the Acl
     */
    AclBusObject* m_AclBusObject;

    /**
     * The App that contains this Acl
     */
    GatewayApp* m_App;

    /**
     * Parse MetaData - helper function to parse an xml
     * @param currentKey - current key in the xml
     * @param metaData - metaData map to fill
     */
    void parseMetaData(xmlNode* currentKey, std::map<qcc::String, qcc::String>& metaData);

    /**
     * Parse Objects - helper function to parse an xml
     * @param currentKey - current key in xml
     * @param objects - objects to fill
     */
    void parseObjects(xmlNode* currentKey, GatewayObjectDescriptions& objects);

    /**
     * parse the RemotedServices - helper function to parse an xml
     * @param currentKey - current key in xml
     * @param remotePermissions - remotePermissions to fill
     */
    void parseRemotedServices(xmlNode* currentKey, GatewayRemoteAppPermissions& remotePermissions);

    /**
     * Helper function to write Objects to a file
     * @param writer - the writer to use
     * @param objects - the gateway Objects to write
     * @return rc - success/failure
     */
    int writeObjectsToFile(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects);

    /**
     * Helper function to write remotePermissions to a file
     * @param writer - the writer to use
     * @param remotePermissions - the gateway remotePermissions to write
     * @return rc - success/failure
     */
    int writeRemotedServicesToFile(xmlTextWriterPtr writer, const GatewayRemoteAppPermissions& remotePermissions);

};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYACL_H_ */
