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

#include <alljoyn/gateway/GatewayManifestObject.h>

namespace ajn {
namespace services {

GatewayManifestObject::GatewayManifestObject() : m_ObjectPath(""), m_ObjectPathFriendlyName(""), m_IsObjectPathPrefix(false)
{

}

GatewayManifestObject::GatewayManifestObject(qcc::String const& objectPath, qcc::String const& objectPathFriendly, bool isPrefix,
                                             std::vector<InterfaceDesc> const& interfaces) : m_ObjectPath(objectPath),
    m_ObjectPathFriendlyName(objectPathFriendly), m_IsObjectPathPrefix(isPrefix), m_Interfaces(interfaces)
{

}

GatewayManifestObject::~GatewayManifestObject()
{
}

const qcc::String& GatewayManifestObject::getObjectPath() const
{
    return m_ObjectPath;
}

const qcc::String& GatewayManifestObject::getObjectPathFriendlyName() const
{
    return m_ObjectPathFriendlyName;
}

bool GatewayManifestObject::getIsObjectPathPrefix() const {
    return m_IsObjectPathPrefix;
}

const std::vector<GatewayManifestObject::InterfaceDesc>& GatewayManifestObject::getInterfaces() const
{
    return m_Interfaces;
}

void GatewayManifestObject::setIsObjectPathPrefix(bool isObjectPathPrefix)
{
    m_IsObjectPathPrefix = isObjectPathPrefix;
}

void GatewayManifestObject::setInterfaces(const std::vector<InterfaceDesc>& interfaces)
{
    m_Interfaces = interfaces;
}

void GatewayManifestObject::setObjectPathFriendlyName(const qcc::String& objectPathFriendlyName)
{
    m_ObjectPathFriendlyName = objectPathFriendlyName;
}

void GatewayManifestObject::setObjectPath(const qcc::String& objectPath)
{
    m_ObjectPath = objectPath;
}

} /* namespace services */
} /* namespace ajn */
