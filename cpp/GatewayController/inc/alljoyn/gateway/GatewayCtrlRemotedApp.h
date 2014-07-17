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

#ifndef GatewayCtrlRemotedApp_H
#define GatewayCtrlRemotedApp_H

#include <vector>
#include <alljoyn/gateway/GatewayCtrlDiscoveredApp.h>
#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>

namespace ajn {
namespace services {
class GatewayCtrlRemotedApp : public GatewayCtrlDiscoveredApp {
  public:

    /**
     * Constructor - must call appropriate init
     */
    GatewayCtrlRemotedApp() { }

    /**
     * init
     * @param busUniqueName The name of the {@link BusAttachment} of the remoted application
     * @param appName The name of the application
     * @param appId The application id
     * @param deviceName The name of the device
     * @param deviceId The device id
     * @param objDescRules Configuration of the object paths and interfaces that are
     * used by the Third Party Application to reach this remoted application
     * @return {@link QStatus}
     */
    QStatus init(const qcc::String& busUniqueName, const qcc::String& appName, uint8_t*appId, const qcc::String& deviceName, const qcc::String& deviceId, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules);

    /**
     * init
     * @param remotedAppInfo MsgArg with the information for this remoted app
     * @param objDescRules vector with object description rules for the remoted app
     * @param internalMetaData map of the internal metadata
     * @return {@link QStatus}
     */
    QStatus init(const ajn::MsgArg*remotedAppInfo, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules, const std::map<qcc::String, qcc::String>& internalMetaData);

    /**
     * init
     * @param aboutData Announcement's about data object
     * @param objDescRules vector with object description rules for the remoted app
     * @return {@link QStatus}
     */
    QStatus init(AboutClient::AboutData const& aboutData, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules);


    /**
     * init
     * @param discoveredApp The {@link DiscoveredApp} to be used to build this {@link RemotedApp}
     * @param objDescRules Configuration of the object paths and interfaces that are
     * used by the Third Party Application to reach this remoted application
     */
    QStatus init(GatewayCtrlDiscoveredApp* discoveredApp, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlRemotedApp();

    /**
     * Configuration of the object paths and interfaces that are
     * used by the Third Party Application to reach this remoted application
     * @return List of {@link ManifestObjectDescription} objects
     */
    const std::vector<GatewayCtrlManifestObjectDescription*>& getObjDescRules();

    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return Status of release
     */
    QStatus release();

  private:
    /**
     * Configuration of the object paths and interfaces that are
     * used by the Third Party Application to reach this remoted application
     */
    std::vector<GatewayCtrlManifestObjectDescription*> m_ObjDescRules;

    void emptyVector();
};
}
}

#endif /* defined(GatewayCtrlRemotedApp_H) */
