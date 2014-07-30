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

#include <alljoyn/gateway/GatewayAclRules.h>

namespace ajn {
namespace gw {
using namespace qcc;

GatewayAclRules::GatewayAclRules()
{

}

GatewayAclRules::~GatewayAclRules() {

}

const GatewayRuleObjectDescriptions& GatewayAclRules::getExposedServicesRules() const
{
    return m_ExposedServicesRules;
}

void GatewayAclRules::setExposedServicesRules(const GatewayRuleObjectDescriptions& exposedServicesRules)
{
    m_ExposedServicesRules = exposedServicesRules;
}

const GatewayRemoteAppRules& GatewayAclRules::getRemoteAppRules() const
{
    return m_RemoteAppRules;
}

void GatewayAclRules::setRemoteAppRules(const GatewayRemoteAppRules& remoteAppRules)
{
    m_RemoteAppRules = remoteAppRules;
}

} /* namespace gw */
} /* namespace ajn */
