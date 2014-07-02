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

#ifndef GatewayCtrlTPApplication_H
#define GatewayCtrlTPApplication_H

#include <vector>
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayCtrlAccessRules.h>
#include <alljoyn/gateway/GatewayCtrlManifestRules.h>
#include <alljoyn/gateway/GatewayCtrlAccessControlList.h>
#include <alljoyn/gateway/GatewayCtrlTPApplicationStatus.h>
#include <alljoyn/gateway/GatewayCtrlApplicationStatusSignalHandler.h>
#include <alljoyn/gateway/GatewayCtrlAclWriteResponse.h>
#include <alljoyn/gateway/GatewayCtrlEnums.h>
#include <alljoyn/services_common/AsyncTaskQueue.h>
#include <alljoyn/gateway/LogModule.h>
#include <alljoyn/gateway/AnnouncementData.h>

namespace ajn {
namespace services {

class ChangedSignalData : public TaskData {
  public:
    ChangedSignalData(const ajn::MsgArg* returnArgs, qcc::String AppId) : m_TPApplicationStatus(returnArgs) { m_AppId = AppId; }

    const GatewayCtrlTPApplicationStatus*GetTPApplicationStatus() const { return &m_TPApplicationStatus; }

    qcc::String GetAppId() const { return m_AppId; }

  private:
    GatewayCtrlTPApplicationStatus m_TPApplicationStatus;

    qcc::String m_AppId;

};

class ChangedSignalTask : public AsyncTask {
  public:

    void SetHandler(const GatewayCtrlApplicationStatusSignalHandler*handler) { m_Handler = (GatewayCtrlApplicationStatusSignalHandler*)handler; }

    void UnSetHandler() { m_Handler = NULL; }

  private:

    virtual void OnEmptyQueue() { }

    virtual void OnTask(TaskData const* taskdata)
    {
        const ChangedSignalData* d = static_cast<const ChangedSignalData*>(taskdata);
        if (m_Handler) {
            m_Handler->onStatusChanged(d->GetAppId(), d->GetTPApplicationStatus());
        } else {
            QCC_DbgHLPrintf(("Got signal, no handler"));
        }
    }

    GatewayCtrlApplicationStatusSignalHandler*m_Handler;
};

class GatewayCtrlTPApplication : public MessageReceiver {



  public:
    /**
     * Constructor
     * @param gwBusName The name of the gateway {@link BusAttachment} the application is installed on
     * @param appObjPath The object path to reach the third party application on the gateway
     */
// this may not be needed	GatewayCtrlTPApplication(qcc::String gwBusName, qcc::String appObjPath);

    /**
     * Constructor
     * @param appInfo
     */
    GatewayCtrlTPApplication(qcc::String gwBusName, ajn::MsgArg*appInfo);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlTPApplication();

    /**
     * @return gwBusName the {@link TPApplication} is installed on
     */
    qcc::String GetGwBusName();

    /**
     * @return The id of the {@link TPApplication}
     */
    qcc::String GetAppId();

    /**
     * @return The name of the {@link TPApplication}.
     */
    qcc::String GetFriendlyName();

    /**
     * @return The object path to reach the application on the gateway
     */
    qcc::String GetObjectPath();

    /**
     * @return The application version
     */
    qcc::String GetAppVersion();

    /**
     * Retrieves the Manifest file of the application.
     * @param sid The id of the session established with the gateway
     * @param status return status of operation
     * @return qcc::String representation of the Manifest file in XML format.
     */
    qcc::String RetrieveManifestFile(SessionId sessionId, QStatus& status);

    /**
     * Retrieves the Manifest rules of the application
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link GatewayCtrlAccessRules}
     */
    GatewayCtrlManifestRules*RetrieveManifestRules(SessionId sessionId, QStatus& status);

    /**
     * @param sessionId The id of the session established with the gateway
     * @param manifests About clients in the controllers vicinity
     * @param announcements a vector of all of the announcements in the controller's area
     * @param status return status of operation
     * @return {@link GatewayCtrlAccessRules}
     */
    GatewayCtrlAccessRules* RetrieveConfigurableRules(SessionId sessionId, std::vector<AnnouncementData*> const& announcements, QStatus& status);


    /**
     * Retrieves the state of the application
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link GatewayCtrlTPApplicationStatus}
     */
    GatewayCtrlTPApplicationStatus*RetrieveStatus(SessionId sessionId, QStatus& status);

    /**
     * Restarts the application
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return {@link RestartStatus}
     */
    RestartStatus Restart(SessionId sessionId, QStatus& status);

    /**
     * Set an {@link ApplicationStatusSignalHandler} to receive application
     * related events. In order to receive the events, in addition to calling this method,
     * a session should be successfully established with the gateway hosting the application.
     * Use {@link TPApplication#unsetStatusChangedHandler()} to stop receiving the events.
     * @param handler Signal handler
     */
    QStatus SetStatusChangedHandler(const GatewayCtrlApplicationStatusSignalHandler*handler);

    /**
     * Stop handler from receiving Service Provider Application related signals
     */
    void UnsetStatusChangedHandler();

    /**
     * Sends request to create {@link AccessControlList} object with the received name and
     * the {@link AccessRules}. The {@link AccessRules} are validated against the {@link ManifestRules}.
     * Only valid rules will be sent for the ACL creation. The invalid rules could be received from the
     * returned {@link AclWriteResponse} object.
     * @param sessionId The id of the session established with the gateway
     * @param name The ACL name
     * @param accessRules The ACL access rules
     * @param status return status of operation
     * @return {@link AclWriteResponse}
     */

    GatewayCtrlAclWriteResponse* CreateAcl(SessionId sessionId, qcc::String name, GatewayCtrlAccessRules* accessRules, QStatus& status);

    /**
     * Retrieves a list of the Access Control Lists installed on the application
     * @param sessionId The id of the session established with the gateway
     * @param status return status of operation
     * @return List of the {@link AccessControlList}
     */
    const std::vector <GatewayCtrlAccessControlList*>& RetrieveAcls(SessionId sessionId, QStatus& status);

    /**
     * Delete the Access Control List of this application
     * @param sessionId The id of the session established with the gateway
     * @param aclId The id of the ACL to be deleted
     * @param status return status of operation
     * @return {@link AclResponseCode}
     */
    AclResponseCode DeleteAcl(SessionId sessionId, qcc::String aclId, QStatus& status);

    /**
     * Intersects {@link AnnouncementData} with the received remotedServices, creates
     * a list of {@link RemotedApp}
     * @param remotedServices The remotedServices from the application manifest
     * @param announcements up to date vector of AnnouncementData
     * @return List of {@link RemotedApp}
     */
    static std::vector<GatewayCtrlRemotedApp*> ExtractRemotedApps(const std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices,
                                                                  std::vector<AnnouncementData*> const& announcements,
                                                                  QStatus& status);

    /**
     * @return Status of release
     */
    QStatus Release();

  private:

    void MaintainProxyBusObject(SessionId sessionId);

    void handleSignal(const ajn::InterfaceDescription::Member* member,
                      const char* srcPath, ajn::Message& msg);


    /**
     * The name of the gateway {@link BusAttachment} the application is installed on
     */
    qcc::String m_GwBusName;

    /**
     * Application id
     */
    qcc::String m_AppId;

    /**
     * The application friendly name or description
     */
    qcc::String m_FriendlyName;

    /**
     * The identification of the application object
     */
    qcc::String m_ObjectPath;

    /**
     * The application version
     */
    qcc::String m_AppVersion;

    /**
     * internal memory management objects
     */

    std::vector <GatewayCtrlAccessControlList*> m_Acls;

    GatewayCtrlManifestRules*m_ManifestRules;

    GatewayCtrlAccessRules*m_ConfigurableRules;

    GatewayCtrlTPApplicationStatus*m_TPApplicationStatus;

    GatewayCtrlAclWriteResponse*m_AclWriteResponse;

    const ajn::InterfaceDescription::Member* m_SignalMethod;

    static AsyncTaskQueue m_ApplicationSignalQueue;

    static ChangedSignalTask m_ChangedSignalTask;




    /**
     * Intersects received {@link AnnouncementData} with the received remotedServices, creates
     * a {@link RemotedApp}. <br>
     * Important, for the correct work of this algorithm the list of the remoted services must be sorted with the
     * {@link ManifObjDescComparator}.
     * @param remotedServices The remotedServices from the application manifest
     * @param ann {@link AnnouncementData} to be intersected with the remotedServices
     * @return {@link RemotedApp} or NULL if the {@link BusObjectDescription}s of the received
     * {@link AnnouncementData} do not have any object path or interfaces that match the remotedServices.
     * Additionally NULL is returned if the {@link AnnouncementData} doesn't have mandatory values
     * for {@link RemotedApp} creation.
     */
    static GatewayCtrlRemotedApp* ExtractRemotedApp(const std::vector<GatewayCtrlManifestObjectDescription*>& remotedServices, const AnnouncementData*ann, QStatus& status);

    void EmptyVector();

};
}
}
#endif /* defined(GatewayCtrlTPApplication_H) */
