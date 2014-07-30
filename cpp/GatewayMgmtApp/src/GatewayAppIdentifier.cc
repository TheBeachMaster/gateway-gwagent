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

#include <alljoyn/gateway/GatewayAppIdentifier.h>
#include <qcc/StringUtil.h>

namespace ajn {
namespace gw {

GatewayAppIdentifier::GatewayAppIdentifier(qcc::String const& appId, qcc::String const& deviceId) :
    m_AppId(appId), m_DeviceId(deviceId)
{
    memset(m_AppIdHex, 0, APPID_LENGTH);
    qcc::HexStringToBytes(appId, m_AppIdHex, APPID_LENGTH);
}

GatewayAppIdentifier::GatewayAppIdentifier(uint8_t* appId, size_t appIdLen, qcc::String const& deviceId) :
    m_DeviceId(deviceId)
{
    memset(m_AppIdHex, 0, APPID_LENGTH);
    memcpy(m_AppIdHex, appId, appIdLen > APPID_LENGTH ? APPID_LENGTH : appIdLen);
    m_AppId = qcc::BytesToHexString(appId, APPID_LENGTH);
}

GatewayAppIdentifier::~GatewayAppIdentifier()
{
}

bool GatewayAppIdentifier::operator<(const GatewayAppIdentifier& other) const
{
    if (m_AppId.compare(other.m_AppId) == 0) {
        return (m_DeviceId.compare(other.m_DeviceId) < 0);
    }

    return (m_AppId.compare(other.m_AppId) < 0);
}

bool GatewayAppIdentifier::operator==(const GatewayAppIdentifier& other) const
{
    if (m_AppId.compare(other.m_AppId) != 0) {
        return false;
    }

    return (m_DeviceId.compare(other.m_DeviceId) == 0);
}

const qcc::String& GatewayAppIdentifier::getAppId() const
{
    return m_AppId;
}

const uint8_t* GatewayAppIdentifier::getAppIdHex() const
{
    return m_AppIdHex;
}

const qcc::String& GatewayAppIdentifier::getDeviceId() const
{
    return m_DeviceId;
}

size_t GatewayAppIdentifier::getAppIdHexLength() const
{
    return APPID_LENGTH;
}

} /* namespace gw */
} /* namespace ajn */
