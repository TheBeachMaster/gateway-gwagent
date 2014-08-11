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

#ifndef ConnectorApp_H
#define ConnectorApp_H

#include <vector>
#include <qcc/String.h>
#include <alljoyn/gateway/AclRules.h>
#include <alljoyn/gateway/ConnectorCapabilities.h>
#include <alljoyn/gateway/Acl.h>
#include <alljoyn/gateway/ConnectorAppStatus.h>
#include <alljoyn/gateway/ConnectorAppStatusSignalHandler.h>
#include <alljoyn/gateway/AclWriteResponse.h>
#include <alljoyn/gateway/Enums.h>
#include "alljoyn/gateway/AsyncTaskQueue.h"
#include <alljoyn/gateway/LogModule.h>
#include <alljoyn/gateway/AnnouncementData.h>

namespace ajn {
namespace gwc {

class ChangedSignalData : public TaskData {
  public:
    /**
     * Constructor
     * @param returnArgs MsgArg with the signal information
     * @param AppId
     */
    ChangedSignalData(const ajn::MsgArg* returnArgs, const qcc::String& AppId);

    /**
     * Get the connector application status
     * @return ConnectorAppStatus
     */
    const ConnectorAppStatus*getConnectorAppStatus() const { return &m_ConnectorAppStatus; }

    const qcc::String& getAppId() const { return m_AppId; }

    QStatus getStatus() const { return m_Status; }

  private:
    ConnectorAppStatus m_ConnectorAppStatus;

    qcc::String m_AppId;

    QStatus m_Status;

};

class ChangedSignalTask : public AsyncTask {
  public:

    /**
     * set the signal handler
     * @param handler signal handler
     */
    void setHandler(const ConnectorAppStatusSignalHandler*handler) { m_Handler = (ConnectorAppStatusSignalHandler*)handler; }

    /**
     * unset the signal handler
     */
    void unSetHandler() { m_Handler = NULL; }

  private:

    virtual void OnEmptyQueue() { }

    virtual void OnTask(TaskData const* taskdata);

    ConnectorAppStatusSignalHandler*m_Handler;
};

class ConnectorApp : public MessageReceiver {

  public:

    /**
     * Constructor - must call init
     */
    ConnectorApp() { }

    /**
     * init
     * @param gwBusName bus name
     * @param appInfo MsgArg containing the application info
     * @return {@link QStatus}
     */
    QStatus init(const qcc::String& gwBusName, ajn::MsgArg*appInfo);

    /**
     * Destructor
     */
    virtual ~ConnectorApp();

    /**
     * @return gwBusName the {@link ConnectorApp} is installed on
     */
    const qcc::String& getGwBusName();

    /**
     * @return The id of the {@link ConnectorApp}
     */
    const qcc::String& getAppId();

    /**
     * @return The name of the {@link ConnectorApp}.
     */
    const qcc::String& getFriendlyName();

    /**
     * @return The object path to reach the application on the gateway
     */
    const qcc::String& getObjectPath();

    /**
     * @return The application version
     */
    const qcc::String& getAppVersion();

    /**
     * Retrieves the Manifest file of the application.
     * @param sid The id of the session established with the gateway
     * @param xml representation of the Manifest file in XML format.
     * @return {@link QStatus}
     */
    QStatus retrieveManifestFile(SessionId sessionId, qcc::String& xml);

    /**
     * Retrieves the Manifest rules of the application
     * @param sessionId The id of the session established with the gateway
     * @param connectorCapabilities {@link AclRules}
     * @return {@QStatus}
     */
    QStatus retrieveConnectorCapabilities(SessionId sessionId, ConnectorCapabilities** connectorCapabilities);

    /**
     * @param sessionId The id of the session established with the gateway
     * @param manifests About clients in the controllers vicinity
     * @param announcements a vector of all of the announcements in the controller's area
     * @param configurableRules {@link AclRules}
     * @return {@QStatus}
     */
    QStatus retrieveApplicableConnectorCapabilities(SessionId sessionId, std::vector<AnnouncementData*> const& announcements, AclRules** configurableRules);


    /**
     * Retrieves the state of the application
     * @param sessionId The id of the session established with the gateway
     * @param applicationStatus {@link ConnectorAppStatus}
     * @return {@QStatus}
     */
    QStatus retrieveStatus(SessionId sessionId, ConnectorAppStatus** applicationStatus);

    /**
     * Restarts the application
     * @param sessionId The id of the session established with the gateway
     * @param restartStatus {@link RestartStatus}
     * @return {@QStatus}
     */
    QStatus restart(SessionId sessionId,  RestartStatus& restartStatus);

    /**
     * Set an {@link ConnectorAppStatusSignalHandler} to receive application
     * related events. In order to receive the events, in addition to calling this method,
     * a session should be successfully established with the gateway hosting the application.
     * Use {@link ConnectorApp#unsetStatusSignalHandler()} to stop receiving the events.
     * @param handler Signal handler
     * @return {@QStatus}
     */
    QStatus setStatusSignalHandler(const ConnectorAppStatusSignalHandler*handler);

    /**
     * Stop handler from receiving Service Provider Application related signals
     */
    void unsetStatusSignalHandler();

    /**
     * Sends request to create {@link Acl} object with the received name and
     * the {@link AclRules}. The {@link AclRules} are validated against the {@link ConnectorCapabilities}.
     * Only valid rules will be sent for the ACL creation. The invalid rules could be received from the
     * returned {@link AclWriteResponse} object.
     * @param sessionId The id of the session established with the gateway
     * @param name The ACL name
     * @param aclRules The ACL access rules
     * @param aclWriteResponse {@link AclWriteResponse}
     * @return {@QStatus}
     */

    QStatus createAcl(SessionId sessionId, const qcc::String& name, AclRules* aclRules, AclWriteResponse** aclWriteResponse);

    /**
     * Retrieves a list of the Access Control Lists installed on the application
     * @param sessionId The id of the session established with the gateway
     * @param acls vector of the {@link Acl}
     * @return {@QStatus}
     */
    QStatus retrieveAcls(SessionId sessionId, std::vector <Acl*>& acls);

    /**
     * Delete the Access Control List of this application
     * @param sessionId The id of the session established with the gateway
     * @param aclId The id of the ACL to be deleted
     * @param responseCode {@link AclResponseCode}
     * @return {@QStatus}
     */
    QStatus deleteAcl(SessionId sessionId, const qcc::String& aclId, AclResponseCode& responseCode);

    /**
     * Intersects {@link AnnouncementData} with the received remotedServices, creates
     * a list of {@link RemotedApp}
     * @param remotedServices The remotedServices from the application manifest
     * @param announcements up to date vector of AnnouncementData
     * @param remotedApps vector of {@link RemotedApp}
     * @return {@QStatus}
     */
    static QStatus  extractRemotedApps(const std::vector<RuleObjectDescription*>& remotedServices,
                                       std::vector<AnnouncementData*> const& announcements,
                                       std::vector<RemotedApp*>& remotedApps);

    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return {@link QStatus}
     */
    QStatus release();


    static bool stringStartWith(const qcc::String& prefix, const qcc::String& inString)
    {
        return (inString.compare(0, prefix.size(), prefix) == 0);
    }

  private:

    void maintainProxyBusObject(SessionId sessionId);

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

    std::vector <Acl*> m_Acls;

    ConnectorCapabilities*m_ConnectorCapabilities;

    AclRules*m_ConfigurableRules;

    ConnectorAppStatus*m_ConnectorAppStatus;

    AclWriteResponse*m_AclWriteResponse;

    const ajn::InterfaceDescription::Member* m_SignalMethod;

    static AsyncTaskQueue m_ApplicationSignalQueue;

    static ChangedSignalTask m_ChangedSignalTask;




    /**
     * Intersects received {@link AnnouncementData} with the received remotedServices, creates
     * a {@link RemotedApp}. <br>
     * Important, for the correct work of this algorithm the list of the remoted services must be sorted with the
     * {@link RuleObjectDescriptionComparator}.
     * @param remotedServices The remotedServices from the application manifest
     * @param ann {@link AnnouncementData} to be intersected with the remotedServices
     * @return {@link RemotedApp} or NULL if the {@link BusObjectDescription}s of the received
     * {@link AnnouncementData} do not have any object path or interfaces that match the remotedServices.
     * Additionally NULL is returned if the {@link AnnouncementData} doesn't have mandatory values
     * for {@link RemotedApp} creation.
     */
    static RemotedApp* extractRemotedApp(const std::vector<RuleObjectDescription*>& remotedServices, const AnnouncementData*ann, QStatus& status);

    void emptyVector();

};
}
}
#endif /* defined(ConnectorApp_H) */
