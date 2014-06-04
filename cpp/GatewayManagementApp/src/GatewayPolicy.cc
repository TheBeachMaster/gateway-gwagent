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

#include <alljoyn/gateway/GatewayPolicy.h>

namespace ajn {
namespace services {
using namespace qcc;

GatewayPolicy::GatewayPolicy()
{

}

GatewayPolicy::~GatewayPolicy() {

}

const GatewayObjectDescriptions& GatewayPolicy::getExposedServices() const
{
    return m_ExposedServices;
}

void GatewayPolicy::setExposedServices(const GatewayObjectDescriptions& exposedServices)
{
    m_ExposedServices = exposedServices;
}

const GatewayRemoteAppPermissions& GatewayPolicy::getRemoteAppPermissions() const
{
    return m_RemoteAppPermissions;
}

void GatewayPolicy::setRemoteAppPermissions(const GatewayRemoteAppPermissions& remoteAppPermissions)
{
    m_RemoteAppPermissions = remoteAppPermissions;
}

} /* namespace services */
} /* namespace ajn */
