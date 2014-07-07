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

#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>
#include <alljoyn/gateway/LogModule.h>


namespace ajn {
namespace services {

GatewayCtrlConnAppInterface::GatewayCtrlConnAppInterface(qcc::String name, qcc::String friendlyName, bool isSecured) : m_Name(name), m_FriendlyName(friendlyName), m_IsSecured(isSecured)
{
}

qcc::String GatewayCtrlConnAppInterface::GetName() const
{
    return m_Name;
}

qcc::String GatewayCtrlConnAppInterface::GetFriendlyName() const
{
    return m_FriendlyName;
}

bool GatewayCtrlConnAppInterface::IsSecured() const
{
    return m_IsSecured;
}

bool GatewayCtrlConnAppInterface::operator<(const GatewayCtrlConnAppInterface to) const
{
    const bool ALessThanB = GetName().operator<(to.GetName());

    return ALessThanB;
}


GatewayCtrlConnAppObjectPath::GatewayCtrlConnAppObjectPath(qcc::String objectPath, qcc::String friendlyName, bool isPrefix, bool isPrefixAllowed) : m_ObjectPath(objectPath), m_FriendlyName(friendlyName), m_IsPrefix(isPrefix), m_isPrefixAllowed(isPrefixAllowed)
{
}

GatewayCtrlConnAppObjectPath::~GatewayCtrlConnAppObjectPath()
{
}

qcc::String GatewayCtrlConnAppObjectPath::GetPath() const
{
    return m_ObjectPath;
}

qcc::String GatewayCtrlConnAppObjectPath::GetFriendlyName()
{
    return m_FriendlyName;
}

bool GatewayCtrlConnAppObjectPath::IsPrefix() const
{
    return m_IsPrefix;
}

void GatewayCtrlConnAppObjectPath::SetPrefix(bool isPrefix)
{
    m_IsPrefix = isPrefix;
}

bool GatewayCtrlConnAppObjectPath::isPrefixAllowed() const {
    return m_isPrefixAllowed;
}

void GatewayCtrlConnAppObjectPath::setPrefixAllowed(bool isPrefixAllowed) {
    m_isPrefixAllowed = isPrefixAllowed;
}

GatewayCtrlManifestObjectDescription::GatewayCtrlManifestObjectDescription(const GatewayCtrlConnAppObjectPath& objectPath, std::set<GatewayCtrlConnAppInterface> interfaces) : m_Interfaces(interfaces), m_IsConfigured(false)
{
    m_ObjectPath = new GatewayCtrlConnAppObjectPath(objectPath);
}

GatewayCtrlManifestObjectDescription::GatewayCtrlManifestObjectDescription(const GatewayCtrlConnAppObjectPath& objectPath, std::set<GatewayCtrlConnAppInterface> interfaces, bool isConfigured) : m_Interfaces(interfaces), m_IsConfigured(isConfigured)
{
    m_ObjectPath = new GatewayCtrlConnAppObjectPath(objectPath);
}

GatewayCtrlManifestObjectDescription::~GatewayCtrlManifestObjectDescription()
{

}

GatewayCtrlConnAppObjectPath* GatewayCtrlManifestObjectDescription::GetObjectPath() const
{
    return m_ObjectPath;
}

const std::set<GatewayCtrlConnAppInterface>* GatewayCtrlManifestObjectDescription::GetInterfaces() const
{
    return &m_Interfaces;
}

bool GatewayCtrlManifestObjectDescription::IsConfigured() const
{
    return m_IsConfigured;

}

void GatewayCtrlManifestObjectDescription::SetConfigured(bool configured)
{
    m_IsConfigured = configured;

}



QStatus GatewayCtrlManifestObjectDescription::Release()
{
    if (m_ObjectPath) {
        delete m_ObjectPath;
        m_ObjectPath = NULL;
    }
    return ER_OK;
}
}
}