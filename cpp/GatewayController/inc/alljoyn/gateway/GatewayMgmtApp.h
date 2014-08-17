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

#ifndef GatewayMgmtApp_H
#define GatewayMgmtApp_H

#include <map>
#include <vector>
#include <qcc/String.h>
#include <alljoyn/about/AboutClient.h>
#include <alljoyn/gateway/AnnouncedApp.h>
#include <alljoyn/gateway/ConnectorApp.h>
#include <alljoyn/gateway/SessionListener.h>
#include <alljoyn/gateway/SessionHandler.h>
#include <alljoyn/about/AboutClient.h>


namespace ajn {
namespace gwc {
typedef  struct  {
    /**
     * Status of joining session
     */
    QStatus m_status;

    /**
     * Session id
     */
    int m_sid;

} SessionResult;

class GatewayMgmtApp : public AnnouncedApp {
  public:

    /**
     * Constructor
     */
    GatewayMgmtApp() : AnnouncedApp(), m_SessionHandler(NULL) { }

    /**
     * init
     * @param busName The name of the {@link BusAttachment} of the gateway management app that sent
     * the Announcement
     * @param aboutData The data sent with the Announcement
     * @return {@link QStatus}
     */
    QStatus init(const qcc::String& gwBusName, ajn::services::AboutClient::AboutData const& aboutData);

    /**
     * Destructor
     */
    virtual ~GatewayMgmtApp();

    /**
     * Retrieve the list of applications installed on the gateway identified by the given gwBusName
     * @param sessionId The id of the session established with the gateway
     * @param connectorApps A reference to a vector of {@link ConnectorApp} objects
     * @return {@link QStatus}
     */
    QStatus retrieveConnectorApps(SessionId sessionId, std::vector<ConnectorApp*>& connectorApps);


    /**
     * Join session synchronously with the given gateway identified by the gwBusName.
     * This method doesn't require {@link SessionListener}. Use this method
     * when there is no need to receive any session related event.
     * @param gwBusName The bus name of the gateway to connect to.
     * @return {@link SessionResult}
     */
    SessionResult joinSession();

    /**
     * Join session synchronously with the given gateway identified by the gwBusName.
     * The session related events will be sent to the given listener.
     * @param gwBusName The bus name of the gateway to connect to.
     * @param listener The listener is used to be notified about the session related events
     * @return {@link SessionResult}
     */
    SessionResult joinSession(SessionListener*listener);

    /**
     * Join session asynchronously with the given gwBusName.
     * @param listener The listener is used to be notified about the session related events
     * @return {@link QStatus}
     */
    QStatus joinSessionAsync(SessionListener*listener);

    /**
     * Disconnect the given session
     * @return Returns the leave session {@link QStatus}
     */
    QStatus leaveSession();


    /**
     * Get the Listener defined for this SessionHandler
     * @return {@link SessionListener}
     */
    SessionListener* getListener() const;




    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return {@link QStatus}
     */
    QStatus release();

  private:

    void emptyVector();

    std::vector<ConnectorApp*> m_InstalledApps;

    SessionHandler m_SessionHandler;

    SessionListener*m_Listener;
};
}
}
#endif /* defined(GatewayMgmtApp_H) */
