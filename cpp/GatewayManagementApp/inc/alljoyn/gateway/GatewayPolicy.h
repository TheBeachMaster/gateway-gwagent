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

#ifndef GATEWAYPOLICY_H_
#define GATEWAYPOLICY_H_

#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/gateway/GatewayAppAndDeviceKey.h>
#include <alljoyn/gateway/GatewayObjectDescription.h>

namespace ajn {
namespace services {

typedef std::vector<GatewayObjectDescription> GatewayObjectDescriptions;
typedef std::map<GatewayAppAndDeviceKey, std::vector<GatewayObjectDescription> > GatewayRemoteAppPermissions;

/**
 * Class used to define a policy
 */
class GatewayPolicy {

  public:

    /**
     * Constructor of the GatewayPolicy class
     */
    GatewayPolicy();

    /**
     * Destructor of the GatewayPolicy class
     */
    virtual ~GatewayPolicy();

    /**
     * Get the ExposedServices of the Policy
     * @return exposedServices
     */
    const GatewayObjectDescriptions& getExposedServices() const;

    /**
     * Set the ExposedServices of the Policy
     * @param exposedServices
     */
    void setExposedServices(const GatewayObjectDescriptions& exposedServices);

    /**
     * Get the RemoteAppPermissions of the Policy
     * @return remoteAppPermissions
     */
    const GatewayRemoteAppPermissions& getRemoteAppPermissions() const;

    /**
     * Set the RemoteAppPErmissions of the Policy
     * @param remoteAppPermissions
     */
    void setRemoteAppPermissions(const GatewayRemoteAppPermissions& remoteAppPermissions);

  private:

    /**
     * Exposed Services
     */
    GatewayObjectDescriptions m_ExposedServices;

    /**
     * RemoteAppPermissions
     */
    GatewayRemoteAppPermissions m_RemoteAppPermissions;

};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYPOLICY_H_ */
