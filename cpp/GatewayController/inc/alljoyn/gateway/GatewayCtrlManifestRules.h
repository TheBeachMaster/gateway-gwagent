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

#ifndef GatewayCtrlManifestRules_H
#define GatewayCtrlManifestRules_H

#include <vector>
#include <alljoyn/Status.h>
#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>

namespace ajn {
namespace services {
/**
 * The manifest rules of the Third Party Application
 */
class GatewayCtrlManifestRules {
  public:

    /**
     * Constructor
     */
    GatewayCtrlManifestRules() { }

    /**
     * Constructor
     * @param manifRulesAJ
     */
    GatewayCtrlManifestRules(const ajn::MsgArg*manifRulesAJ);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlManifestRules();

    /**
     * The {@link GatewayCtrlManifestObjectDescription} objects that the Third Party Application
     * exposes to its clients
     * @return List of exposed services
     */
    const std::vector<GatewayCtrlManifestObjectDescription*>& GetExposedServices() const;

    /**
     * The {@link GatewayCtrlManifestObjectDescription} objects that the Third Party Application
     * supports for being remoted
     * @return List of remoted interfaces
     */
    const std::vector<GatewayCtrlManifestObjectDescription*>& GetRemotedServices() const;

    /**
     * @return Status of release
     */
    QStatus Release();

  private:
    /**
     * The interfaces that the Third Party Application exposes to its clients
     */
    std::vector<GatewayCtrlManifestObjectDescription*> m_ExposedServices;

    /**
     * The interfaces that the Third Party Application allows to remote
     */
    std::vector<GatewayCtrlManifestObjectDescription*> m_RemotedServices;

    void EmptyVectors();

};
}
}
#endif /* defined(GatewayCtrlManifestRules_H) */
