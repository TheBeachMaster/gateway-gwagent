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

#ifndef GatewayCtrlAcl_H
#define GatewayCtrlAcl_H


#include <vector>
#include <map>
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayCtrlRuleObjectDescription.h>
#include <alljoyn/gateway/GatewayCtrlAnnouncedApp.h>
#include <alljoyn/gateway/GatewayCtrlEnums.h>
#include <alljoyn/gateway/GatewayCtrlAclRules.h>
#include <alljoyn/gateway/GatewayCtrlConnectorCapabilities.h>
#include <alljoyn/gateway/GatewayCtrlAclWriteResponse.h>
#include <alljoyn/gateway/AnnouncementData.h>

namespace ajn {
namespace gwcontroller {
class GatewayCtrlAcl {
    friend class GatewayCtrlConnectorApp;

  public:

    /**
     * Constructor - must call init
     */
    GatewayCtrlAcl();


    /**
     * init
     * @param gwBusName The name of the gateway {@link BusAttachment} hosting a Connector App
     * that is related to this ACL
     * @param aclInfoAJ The ACL information
     */

    QStatus init(const qcc::String& gwBusName, const ajn::MsgArg*aclInfoAJ);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlAcl();

    /**
     * @return The name of the Access Control List
     */
    const qcc::String& getName();

    /**
     * Set the name of the Access Control List
     * @param name The ACL name
     */
    void setName(const qcc::String& name);

    /**
     * @return Id of the Access Control List
     */
    const qcc::String& getId();

    /**
     * @return Object path of the Access Control List
     */
    const qcc::String& getObjectPath();

    /**
     * @return The name of the gateway {@link BusAttachment} hosting a Connector App
     * that is related to this Access Control List
     */
    const qcc::String& getGwBusName();


    /**
     * Activate the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param aclResp A reference to {@link AclResponseCode}
     * @return return status of operation
     */
    QStatus activate(SessionId sessionId, AclResponseCode& aclResp);

    /**
     * Deactivate the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param aclResp A reference to {@link AclResponseCode}
     * @return return status of operation
     */
    QStatus deactivate(SessionId sessionId, AclResponseCode& aclResp);

    /**
     * Sends request to update Access Control List with the received {@link GatewayCtrlAclRules}.
     * The {@link GatewayCtrlAclRules} are validated against the provided {@link GatewayCtrlConnectorCapabilities}.
     * Only valid rules will be sent to update the ACL. The invalid rules could be received from the
     * returned {@link GatewayCtrlAclWriteResponse} object.
     * @param sessionId The id of the session established with the gateway
     * @param aclRules The ACL access rules
     * @param connectorCapabilities the capabilities of this conenctor app
     * @param aclWriteResponse {@link GatewayCtrlAclWriteResponse}
     * @return return status of operation
     */
    QStatus update(SessionId sessionId, GatewayCtrlAclRules* aclRules, GatewayCtrlConnectorCapabilities* connectorCapabilities, GatewayCtrlAclWriteResponse** aclWriteResponse);

    /**
     * Updates custom metadata of the Access Control List. The ACL metadata is rewritten following the
     * request.
     * @param sessionId The id of the session established with the gateway
     * @param metadata The metadata to update the ACL
     * @param aclResponseCode {@link AclResponseCode}
     * @return return status of operation
     */
    QStatus updateCustomMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, AclResponseCode& aclResponseCode);

    /**
     * Updates metadata of the internal Access Control List. The ACL metadata is rewritten following the
     * request.
     * @param sessionId The id of the session established with the gateway
     * @param metadata The metadata to update the ACL
     * @param aclResponseCode {@link AclResponseCode}
     * @return return status of operation
     */
    QStatus updateAclMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, AclResponseCode& aclResponseCode);

    /**
     * Return the current state of the {@link GatewayCtrlAcl}
     * @return {@link AclStatus}
     */
    AclStatus getStatus();

    /**
     * Retrieve from the gateway status of the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param aclStatus {@link AclStatus}
     * @return return status of operation
     */
    QStatus retrieveStatus(SessionId sessionId, AclStatus& aclStatus);

    /**
     * Refreshes the {@link Acl} object and returns its {@link AclRules}
     * @param sessionId The id of the session established with the gateway
     * @param connectorCapabilities {@link ConnectorCapabilities} that is used for the {@link AclRules} creation
     * @param announcements A vector of AnnounementData objects with the current set of announcements in the network
     * @param aclRules {@link AclRules}
     * @return return status of operation

     */
    QStatus retrieve(SessionId sessionId, const GatewayCtrlConnectorCapabilities& connectorCapabilities,
                     std::vector<AnnouncementData*> const& announcements, GatewayCtrlAclRules** aclRules);
    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return {@link QStatus}
     */
    QStatus release();

  private:

    /**
     * ACL id
     */
    qcc::String m_AclId;

    /**
     * Name of the list
     */
    qcc::String m_AclName;

    /**
     * Currect ACL status
     */
    AclStatus m_AclStatus;

    /**
     * ACL objPath
     */
    qcc::String m_ObjectPath;

    /**
     * The name of the gateway {@link BusAttachment} a Connector App which is related to this ACL
     * is installed on
     */
    qcc::String m_GwBusName;

    GatewayCtrlAclWriteResponse*m_AclWriteResponse;

    GatewayCtrlAclRules*m_AclRules;

    std::map<qcc::String, qcc::String> m_InternalMetadata;

    /**
     * Validate received {@link RuleObjectDescription} list to the list of {@link RuleObjectDescriptionAJ}
     * which is used for ACL creation or an update.
     * The list of the invalid rules is returned by this method.
     * @param toValidate The list of {@link RuleObjectDescription} rules to be validated
     * @param target The list of {@link RuleObjectDescriptionAJ} that is populated with the valid rules
     * from the "toValidate" list
     * @param connectorCapabilities Check validation of the "toValidate" rules against the list of this manifest rules
     * @return List of an invalid rules. The rules that weren't found in the manifest rules
     */
    static std::vector<GatewayCtrlRuleObjectDescription*> validateManifObjDescs(
        const std::vector<GatewayCtrlRuleObjectDescription*>& toValidate,
        std::vector<GatewayCtrlRuleObjectDescription*>& target,
        const std::vector<GatewayCtrlRuleObjectDescription*>& connectorCapabilities);
    /**
     * Checks that received toValidate rule complies with received connectorCapabilities.
     * The method removes from toValidate interfaces the interfaces that are not valid (not in the
     * connectorCapabilities interfaces). Not valid interfaces are added to the received notValid set.
     * @param toValidate {@link GatewayCtrlRuleObjectDescription} rules to be validated
     * @param notValid An empty Set that will be populated with  interfaces that don't comply
     * with the manifest interfaces
     * @param connectorCapabilities validate received toValidate rules against this list of the manifest rules
     * @return Returns TRUE if there is at least one rule (interface) that complies with the manifest rules.
     * FALSE means that toValidate rule doesn't comply with the manifest rules not by the object path
     * and not by the interfaces. As a result of this method execution, toValidate will contain
     * only valid interfaces all the invalid interfaces will be moved to the notValid set.
     */
    static bool isValidRule(GatewayCtrlRuleObjectDescription*toValidate, std::set<GatewayCtrlRuleInterface>& notValid, const std::vector<GatewayCtrlRuleObjectDescription*>& connectorCapabilities);

    static bool isValidObjPath(const GatewayCtrlRuleObjectPath*manifOp, const qcc::String& toValidOP, bool isPrefix);

    /**
     * Gets exposed services of the ACL, intersects it with the manifest exposed services in order to create
     * the exposed services of the {@link AclRules}. In addition adds the exposed services rules
     * that haven't configured yet.
     * @param aclExpServicesAJ Exposed services retrieved from the ACL
     * @param manifExpServices Manifest exposed services
     * @return List of {@link RuleObjectDescription} of the exposed services
     */
    std::vector<GatewayCtrlRuleObjectDescription*> convertExposedServices(
        const std::vector<GatewayCtrlRuleObjectDescription*>& aclExpServices,
        const std::vector<GatewayCtrlRuleObjectDescription*>& manifExpServices);

    /**
     * Fills the received list of {@link RemotedApp}s from the array of {@link RemotedAppAJ}.
     * The appName and deviceName that are required for creating the {@link RemotedApp} object are taken
     * from the internal metadata or the {@link AnnouncementData}. If the {@link AnnouncementData} has appName or deviceName
     * that are different from the metadata values, the metadata is updated. The object description rules of the
     * created {@link RemotedApp} are completed from the rules which are returned by the
     * {@link ConnectorApp#extractRemotedApp(List, AnnouncementData)}
     * @param inputRemotedApps The source for filling the remotedApps list
     * @param outputRemotedApps The list to be filled
     * @param remotedServices The manifest data that is required for creation of the {@link RemotedApp}
     * @return TRUE if the internal metadata was updated
     */
    bool convertRemotedApps(const std::vector<GatewayCtrlRemotedApp*>& inputRemotedApps,
                            std::vector<GatewayCtrlRemotedApp*>& outputRemotedApps,
                            std::vector<GatewayCtrlRuleObjectDescription*>& remotedServices,
                            std::vector<AnnouncementData*> const& announcements, QStatus& status);


    /**
     * Converts {@link RuleObjectDescriptionAJ} array in to {@link RuleObjectDescription} list. The converted rules
     * are validated against the received manifest rules. Manifest rules that were used for the validation and
     * the {@link RuleObjectDescription}  construction are stored in the given usedManRules map.
     * Created {@link RuleObjectDescription} rules are marked as configured. <br>
     * Important, for the correct work of this algorithm the manifest list must be sorted with the
     * {@link RuleObjectDescriptionComparator}.
     * @param objDescsAJ to be converted
     * @param manifest manifest rules that are used for the validation and {@link RuleObjectDescription} construction
     * @param usedManRules manifest rules that were used for validation and the {@link RuleObjectDescription} construction
     * @return {@link RuleObjectDescription} list converted from the {@link RuleObjectDescriptionAJ} array
     */
    std::vector<GatewayCtrlRuleObjectDescription*> convertObjectDescription(const std::vector<GatewayCtrlRuleObjectDescription*>& objDescs,
                                                                            const std::vector<GatewayCtrlRuleObjectDescription*>& manifest,
                                                                            std::map<GatewayCtrlRuleObjectPath, std::set<GatewayCtrlRuleInterface> >& usedManRules);

    /**
     * Search for the {@link RemotedApp} in the given list of the remotedApps with the given
     * deviceId and appId. If the {@link RemotedApp} is found it's removed from the remotedApps
     * and is returned
     * @param remotedApps To look for the {@link RemotedApp}
     * @param deviceId
     * @param appId
     * @return {@link RemotedApp} if found or NULL if NOT
     */
    GatewayCtrlRemotedApp*getRemotedApp(std::vector<GatewayCtrlRemotedApp*>*remotedApps, const qcc::String& deviceId, const uint8_t*appId)
    {
        std::vector<GatewayCtrlRemotedApp*>::iterator iter;

        for (iter = remotedApps->begin(); iter != remotedApps->end(); iter++) {

            GatewayCtrlRemotedApp*currApp = (*iter);
            if ((currApp->getDeviceId().compare(deviceId) == 0)
                &&
                (::memcmp(currApp->getAppId(), appId, UUID_LENGTH) == 0)) {

                remotedApps->erase(iter);
                return currApp;
            }
        }

        return NULL;
    }

    void addUnconfiguredRemotedAppRules(const std::vector<GatewayCtrlRuleObjectDescription*>& unconfRules, std::vector<GatewayCtrlRuleObjectDescription*>& confRules);

    /**
     * Check whether the deviceNameMeta and appNameMeta are equal to the annApp,
     * if not return TRUE
     * @param deviceNameMeta
     * @param appNameMeta
     * @param annApp
     * @param key metadata prefix key
     * @return TRUE if the metadata needs to be updated
     */
    bool metadataUpdated(const qcc::String& deviceNameMeta, const qcc::String& appNameMeta, const GatewayCtrlRemotedApp& annApp, const qcc::String& keyPrefix);
};
}
}
#endif /* defined(GatewayCtrlAcl_H) */
