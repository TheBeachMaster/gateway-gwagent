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

#ifndef GatewayCtrlGateway_H
#define GatewayCtrlGateway_H

#include <map>
#include <vector>
#include <qcc/String.h>
#include <alljoyn/about/AboutClient.h>
#include <alljoyn/gateway/GatewayCtrlDiscoveredApp.h>
#include <alljoyn/gateway/GatewayCtrlTPApplication.h>
#include <alljoyn/gateway/GatewayCtrlControllerSessionListener.h>
#include <alljoyn/gateway/GatewayCtrlSessionHandler.h>
#include <alljoyn/about/AboutClient.h>


namespace ajn {
namespace services {
typedef  struct  {
    /**
     * Status of joining session
     */
    QStatus m_status;

    /**
     * Session id
     */
    int m_sid;

} GatewayCtrlSessionResult;

class GatewayCtrlGateway : public GatewayCtrlDiscoveredApp {
  public:

    /**
     * Constructor
     * @param busName The name of the {@link BusAttachment} of the gateway that sent
     * the Announcement
     * @param aboutData The data sent with the Announcement
     */
    GatewayCtrlGateway(qcc::String gwBusName, AboutClient::AboutData const& aboutData);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlGateway();

    /**
     * Retrieve the list of applications installed on the gateway identified by the given gwBusName
     * @param sessionId The id of the session established with the gateway
     * @param installedApps The {@link TPApplication}
     * @param status return status of operation
     * @return {@link Status}
     */
    const std::vector<GatewayCtrlTPApplication*>&  RetrieveInstalledApps(SessionId sessionId, QStatus& status);


    /**
     * Join session synchronously with the given gateway identified by the gwBusName.
     * This method doesn't require {@link GatewayCtrlControllerSessionListener}. Use this method
     * when there is no need to receive any session related event.
     * @param gwBusName The bus name of the gateway to connect to.
     * @return {@link SessionResult}
     */
    GatewayCtrlSessionResult JoinSession();

    /**
     * Join session synchronously with the given gateway identified by the gwBusName.
     * The session related events will be sent to the given listener.
     * @param gwBusName The bus name of the gateway to connect to.
     * @param listener The listener is used to be notified about the session related events
     * @return {@link SessionResult}
     */
    GatewayCtrlSessionResult JoinSession(GatewayCtrlControllerSessionListener*listener);

    /**
     * Join session asynchronously with the given gwBusName.
     * @param gwBusName The bus name of the gateway to connect to.
     * @param listener The listener is used to be notified about the session related events
     */
    QStatus JoinSessionAsync(GatewayCtrlControllerSessionListener*listener);

    /**
     * Disconnect the given session
     * @param sessionId The session id to disconnect
     * @return Returns the leave session {@link Status}
     */
    QStatus LeaveSession();


    /**
     * Get the Listener defined for this SessionHandler
     * @return
     */
    GatewayCtrlControllerSessionListener* getListener() const;




    /**
     * @return Status of release
     */
    QStatus Release();

  private:

    void EmptyVector();

    std::vector<GatewayCtrlTPApplication*> m_InstalledApps;

    GatewayCtrlSessionHandler m_SessionHandler;

    GatewayCtrlControllerSessionListener*m_Listener;
};
}
}
#endif /* defined(GatewayCtrlGateway_H) */
