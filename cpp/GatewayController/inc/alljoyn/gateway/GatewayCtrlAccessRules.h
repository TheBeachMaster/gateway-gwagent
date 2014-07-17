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

#ifndef GatewayCtrlAccessRules_H
#define GatewayCtrlAccessRules_H

#include <map>
#include <vector>
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayCtrlRemotedApp.h>
#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>
#include <alljoyn/gateway/GatewayCtrlManifestRules.h>


namespace ajn {
namespace services {
class GatewayCtrlAccessRules {
  public:
    /**
     * Constructor - must call appropriate init
     */
    GatewayCtrlAccessRules();


    /**
     * init
     * @param exposedServicesArrayArg MsgArg containing the exposed services
     * @param remotedAppsArrayArg MsgArg containing the remoted apps
     * @param manifestRules map of manifest rules for this connector app
     * @param internalMetaData internal metadata information from the server
     * @return {@link QStatus}
     */
    QStatus init(const MsgArg*exposedServicesArrayArg, const MsgArg*remotedAppsArrayArg, const GatewayCtrlManifestRules& manifestRules, const std::map<qcc::String, qcc::String>& internalMetaData);


    /**
     * init
     * @param exposedServices The interfaces that Third Party Application exposes to its clients
     * @param remotedApps The applications that may be reached by the Third Party Application
     * via the configured interfaces and object paths
     */
    QStatus init(std::vector<GatewayCtrlManifestObjectDescription*> const& exposedServices, std::vector<GatewayCtrlRemotedApp*> const& remotedApps);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlAccessRules();

    /**
     * The applications that may be reached by the Third Party Application
     * via the configured interfaces and object paths
     * @return List of the remoted applications
     */
    const std::vector<GatewayCtrlRemotedApp*>& getRemotedApps();

    /**
     * The interfaces that Third Party Application exposes to its clients
     * @return List of exposed services
     */
    const std::vector<GatewayCtrlManifestObjectDescription*>& getExposedServices();


    /**
     * Set the given metadata to the given one
     * @param metadata
     */
    void setMetadata(std::map<qcc::String, qcc::String> const& metadata);

    /**
     * Returns metadata value for the given key
     * @param key The metadata key
     * @return Metadata value or NULL if not found
     */
    qcc::String*getMetadata(const qcc::String& key);

    /**
     * Returns current metadata object
     * @return metadata
     */
    const std::map<qcc::String, qcc::String>& getMetadata();


    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return Status of release
     */
    QStatus release();

  private:
    /**
     * This {@link AccessRules} metadata
     */
    std::map<qcc::String, qcc::String> m_Metadata;


    /**
     * The interfaces that the Third Party Application exposes to its clients
     */
    std::vector<GatewayCtrlManifestObjectDescription*> m_ExposedServices;

    /**
     * The applications that may be reached by the Third Party Application
     * via the configured interfaces and object paths
     */
    std::vector<GatewayCtrlRemotedApp*> m_RemotedApps;

    void emptyVectors();
};
}
}
#endif /* defined(GatewayCtrlAccessRules_H) */
