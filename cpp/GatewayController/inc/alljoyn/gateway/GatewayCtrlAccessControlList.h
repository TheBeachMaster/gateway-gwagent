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

#ifndef GatewayCtrlAccessControlList_H
#define GatewayCtrlAccessControlList_H


#include <vector>
#include <map>
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>
#include <alljoyn/gateway/GatewayCtrlDiscoveredApp.h>
#include <alljoyn/gateway/GatewayCtrlEnums.h>
#include <alljoyn/gateway/GatewayCtrlAccessRules.h>
#include <alljoyn/gateway/GatewayCtrlManifestRules.h>
#include <alljoyn/gateway/GatewayCtrlAclWriteResponse.h>
#include <alljoyn/gateway/AnnouncementData.h>

namespace ajn {
namespace services {
class GatewayCtrlAccessControlList {
    friend class GatewayCtrlTPApplication;

  public:

    /**
     * Constructor
     * @param gwBusName The name of the gateway {@link BusAttachment} hosting a Third Party Application
     * that is related to this ACL
     * @param aclInfoAJ The ACL information
     */
    GatewayCtrlAccessControlList(qcc::String gwBusName, const ajn::MsgArg*aclInfoAJ);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlAccessControlList();

    /**
     * @return The name of the Access Control List
     */
    qcc::String GetName();

    /**
     * Set the name of the Access Control List
     * @param name The ACL name
     */
    void SetName(qcc::String name);

    /**
     * @return Id of the Access Control List
     */
    qcc::String GetId();

    /**
     * @return Object path of the Access Control List
     */
    qcc::String GetObjectPath();

    /**
     * @return The name of the gateway {@link BusAttachment} hosting a Third Party Application
     * that is related to this Access Control List
     */
    qcc::String GetGwBusName();


    /**
     * Activate the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link AclResponseCode}
     */
    AclResponseCode Activate(SessionId sessionId, QStatus& status);


    /**
     * Deactivate the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link AclResponseCode}
     */
    AclResponseCode Deactivate(SessionId sessionId, QStatus& status);

    /**
     * Sends request to update Access Control List with the received {@link GatewayCtrlAccessRules}.
     * The {@link GatewayCtrlAccessRules} are validated against the provided {@link GatewayCtrlManifestRules}.
     * Only valid rules will be sent to update the ACL. The invalid rules could be received from the
     * returned {@link GatewayCtrlAclWriteResponse} object.
     * @param sessionId The id of the session established with the gateway
     * @param accessRules The ACL access rules
     * @param status return status of operation
     * @return {@link GatewayCtrlAclWriteResponse}
     */
    GatewayCtrlAclWriteResponse* UpdateAcl(SessionId sessionId, GatewayCtrlAccessRules* accessRules, GatewayCtrlManifestRules* manifestRules, QStatus& status);

    /**
     * Updates custom metadata of the Access Control List. The ACL metadata is rewritten following the
     * request.
     * @param sessionId The id of the session established with the gateway
     * @param metadata The metadata to update the ACL
     * @param status return status of operation
     * @return {@link AclResponseCode}
     */
    AclResponseCode UpdateCustomMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, QStatus& status);

    /**
     * Updates metadata of the internal Access Control List. The ACL metadata is rewritten following the
     * request.
     * @param sessionId The id of the session established with the gateway
     * @param metadata The metadata to update the ACL
     * @param status return status of operation
     * @return {@link AclResponseCode}
     */
    AclResponseCode UpdateAclMetadata(SessionId sessionId, const std::map<qcc::String, qcc::String>& metadata, QStatus& status);

    /**
     * Return the current state of the {@link GatewayCtrlAccessControlList}
     * @return {@link AclStatus}
     */
    AclStatus GetStatus();

    /**
     * Retrieve from the gateway status of the Access Control List
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link AclStatus}
     */
    AclStatus RetrieveStatus(SessionId sessionId, QStatus& status);


    /**
     * Refreshes the {@link AccessControlList} object and returns its {@link AccessRules}
     * @param sessionId The id of the session established with the gateway
     * @param manifestRules {@link ManifestRules} that is used for the {@link AccessRules} creation
     * @param announcements A vector of AnnounementData objects with the current set of announcements in the network
     * @param status return status of operation
     * @return {@link AccessRules}
     */
    GatewayCtrlAccessRules*retrieveAcl(SessionId sessionId, const GatewayCtrlManifestRules& manifestRules,
                                       std::vector<AnnouncementData*> const& announcements, QStatus& status);
    /**
     * @return Status of release
     */
    QStatus Release();

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
     * The name of the gateway {@link BusAttachment} a Third Party Application which is related to this ACL
     * is installed on
     */
    qcc::String m_GwBusName;

    GatewayCtrlAclWriteResponse*m_AclWriteResponse;

    GatewayCtrlAccessRules*m_AccessRules;

    std::map<qcc::String, qcc::String> m_InternalMetadata;

    /**
     * Validate received {@link ManifestObjectDescription} list to the list of {@link ManifestObjectDescriptionAJ}
     * which is used for ACL creation or an update.
     * The list of the invalid rules is returned by this method.
     * @param toValidate The list of {@link ManifestObjectDescription} rules to be validated
     * @param target The list of {@link ManifestObjectDescriptionAJ} that is populated with the valid rules
     * from the "toValidate" list
     * @param manifestRules Check validation of the "toValidate" rules against the list of this manifest rules
     * @return List of an invalid rules. The rules that weren't found in the manifest rules
     */
    static std::vector<GatewayCtrlManifestObjectDescription*> ValidateManifObjDescs(
        const std::vector<GatewayCtrlManifestObjectDescription*>& toValidate,
        std::vector<GatewayCtrlManifestObjectDescription*>& target,
        const std::vector<GatewayCtrlManifestObjectDescription*>& manifestRules);
    /**
     * Checks that received toValidate rule complies with received manifestRules.
     * The method removes from toValidate interfaces the interfaces that are not valid (not in the
     * manifestRules interfaces). Not valid interfaces are added to the received notValid set.
     * @param toValidate {@link GatewayCtrlManifestObjectDescription} rules to be validated
     * @param notValid An empty Set that will be populated with  interfaces that don't comply
     * with the manifest interfaces
     * @param manifestRules validate received toValidate rules against this list of the manifest rules
     * @return Returns TRUE if there is at least one rule (interface) that complies with the manifest rules.
     * FALSE means that toValidate rule doesn't comply with the manifest rules not by the object path
     * and not by the interfaces. As a result of this method execution, toValidate will contain
     * only valid interfaces all the invalid interfaces will be moved to the notValid set.
     */
    static bool IsValidRule(GatewayCtrlManifestObjectDescription*toValidate, std::set<GatewayCtrlTPInterface>& notValid, const std::vector<GatewayCtrlManifestObjectDescription*>& manifestRules);

    static bool isValidObjPath(const GatewayCtrlTPObjectPath*manifOp, qcc::String toValidOP, bool isPrefix);

    /**
     * Gets exposed services of the ACL, intersects it with the manifest exposed services in order to create
     * the exposed services of the {@link AccessRules}. In addition adds the exposed services rules
     * that haven't configured yet.
     * @param aclExpServicesAJ Exposed services retrieved from the ACL
     * @param manifExpServices Manifest exposed services
     * @return List of {@link ManifestObjectDescription} of the exposed services
     */
    std::vector<GatewayCtrlManifestObjectDescription*> ConvertExposedServices(
        const std::vector<GatewayCtrlManifestObjectDescription*>& aclExpServices,
        const std::vector<GatewayCtrlManifestObjectDescription*>& manifExpServices);

    /**
     * Fills the received list of {@link RemotedApp}s from the array of {@link RemotedAppAJ}.
     * The appName and deviceName that are required for creating the {@link RemotedApp} object are taken
     * from the internal metadata or the {@link AnnouncementData}. If the {@link AnnouncementData} has appName or deviceName
     * that are different from the metadata values, the metadata is updated. The object description rules of the
     * created {@link RemotedApp} are completed from the rules which are returned by the
     * {@link TPApplication#extractRemotedApp(List, AnnouncementData)}
     * @param inputRemotedApps The source for filling the remotedApps list
     * @param outputRemotedApps The list to be filled
     * @param remotedServices The manifest data that is required for creation of the {@link RemotedApp}
     * @return TRUE if the internal metadata was updated
     */
    bool ConvertRemotedApps(const std::vector<GatewayCtrlRemotedApp*>& inputRemotedApps,
                            std::vector<GatewayCtrlRemotedApp*>& outputRemotedApps,
                            std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices,
                            std::vector<AnnouncementData*> const& announcements, QStatus& status);


    /**
     * Converts {@link ManifestObjectDescriptionAJ} array in to {@link ManifestObjectDescription} list. The converted rules
     * are validated against the received manifest rules. Manifest rules that were used for the validation and
     * the {@link ManifestObjectDescription}  construction are stored in the given usedManRules map.
     * Created {@link ManifestObjectDescription} rules are marked as configured. <br>
     * Important, for the correct work of this algorithm the manifest list must be sorted with the
     * {@link ManifObjDescComparator}.
     * @param objDescsAJ to be converted
     * @param manifest manifest rules that are used for the validation and {@link ManifestObjectDescription} construction
     * @param usedManRules manifest rules that were used for validation and the {@link ManifestObjectDescription} construction
     * @return {@link ManifestObjectDescription} list converted from the {@link ManifestObjectDescriptionAJ} array
     */
    std::vector<GatewayCtrlManifestObjectDescription*> ConvertObjectDescription(const std::vector<GatewayCtrlManifestObjectDescription*>& objDescs,
                                                                                const std::vector<GatewayCtrlManifestObjectDescription*>& manifest,
                                                                                std::map<GatewayCtrlTPObjectPath, std::set<GatewayCtrlTPInterface> >& usedManRules);

    /**
     * Search for the {@link RemotedApp} in the given list of the remotedApps with the given
     * deviceId and appId. If the {@link RemotedApp} is found it's removed from the remotedApps
     * and is returned
     * @param remotedApps To look for the {@link RemotedApp}
     * @param deviceId
     * @param appId
     * @return {@link RemotedApp} if found or NULL if NOT
     */
    GatewayCtrlRemotedApp*GetRemotedApp(std::vector<GatewayCtrlRemotedApp*>*remotedApps, qcc::String deviceId, const uint8_t*appId)
    {
        std::vector<GatewayCtrlRemotedApp*>::iterator iter;

        for (iter = remotedApps->begin(); iter != remotedApps->end(); iter++) {

            GatewayCtrlRemotedApp*currApp = (*iter);
            if ((currApp->GetDeviceId().compare(deviceId) == 0)
                &&
                (::memcmp(currApp->GetAppId(), appId, UUID_LENGTH) == 0)) {

                remotedApps->erase(iter);
                return currApp;
            }
        }

        return NULL;
    }

    void AddUnconfiguredRemotedAppRules(const std::vector<GatewayCtrlManifestObjectDescription*>& unconfRules, std::vector<GatewayCtrlManifestObjectDescription*>& confRules);

    /**
     * Check whether the deviceNameMeta and appNameMeta are equal to the annApp,
     * if not return TRUE
     * @param deviceNameMeta
     * @param appNameMeta
     * @param annApp
     * @param key metadata prefix key
     * @return TRUE if the metadata needs to be updated
     */
    bool MetadataUpdated(qcc::String deviceNameMeta, qcc::String appNameMeta, const GatewayCtrlRemotedApp& annApp, qcc::String key);
};
}
}
#endif /* defined(GatewayCtrlAccessControlList_H) */
