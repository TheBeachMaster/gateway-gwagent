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

#ifndef GatewayAclRules_H_
#define GatewayAclRules_H_

#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayAppIdentifier.h>
#include <alljoyn/gateway/GatewayRuleObjectDescription.h>

namespace ajn {
namespace gw {

typedef std::vector<GatewayRuleObjectDescription> GatewayRuleObjectDescriptions;
typedef std::map<GatewayAppIdentifier, std::vector<GatewayRuleObjectDescription> > GatewayRemoteAppRules;

/**
 * Class used to define AclRules
 */
class GatewayAclRules {

  public:

    /**
     * Constructor of the GatewayAclRules class
     */
    GatewayAclRules();

    /**
     * Destructor of the GatewayAclRules class
     */
    virtual ~GatewayAclRules();

    /**
     * Get the ExposedServicesRules of the AclRules
     * @return exposedServicesRules
     */
    const GatewayRuleObjectDescriptions& getExposedServicesRules() const;

    /**
     * Set the ExposedServicesRules of the AclRules
     * @param exposedServicesRules
     */
    void setExposedServicesRules(const GatewayRuleObjectDescriptions& exposedServicesRules);

    /**
     * Get the RemoteAppPermissions of the AclRules
     * @return remoteAppPermissions
     */
    const GatewayRemoteAppRules& getRemoteAppRules() const;

    /**
     * Set the RemoteAppRules of the AclRules
     * @param remoteAppRules
     */
    void setRemoteAppRules(const GatewayRemoteAppRules& remoteAppRules);

  private:

    /**
     * Exposed Services Rules
     */
    GatewayRuleObjectDescriptions m_ExposedServicesRules;

    /**
     * RemoteAppRules
     */
    GatewayRemoteAppRules m_RemoteAppRules;

};

} /* namespace gw */
} /* namespace ajn */

#endif /* GatewayAclRules_H_ */
