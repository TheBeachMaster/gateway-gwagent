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

#include <alljoyn/gateway/GatewayCtrlRuleObjectDescription.h>
#include <alljoyn/gateway/LogModule.h>


namespace ajn {
namespace gwcontroller {

GatewayCtrlRuleInterface::GatewayCtrlRuleInterface(const qcc::String& name, const qcc::String& friendlyName, bool isSecured) : m_Name(name), m_FriendlyName(friendlyName), m_IsSecured(isSecured)
{
}

const qcc::String& GatewayCtrlRuleInterface::getName() const
{
    return m_Name;
}

const qcc::String& GatewayCtrlRuleInterface::getFriendlyName() const
{
    return m_FriendlyName;
}

bool GatewayCtrlRuleInterface::isSecured() const
{
    return m_IsSecured;
}

bool GatewayCtrlRuleInterface::operator<(const GatewayCtrlRuleInterface to) const
{
    const bool ALessThanB = getName().operator<(to.getName());

    return ALessThanB;
}


GatewayCtrlRuleObjectPath::GatewayCtrlRuleObjectPath(const qcc::String& objectPath, const qcc::String& friendlyName, bool isPrefix, bool isPrefixAllowed) : m_ObjectPath(objectPath), m_FriendlyName(friendlyName), m_IsPrefix(isPrefix), m_isPrefixAllowed(isPrefixAllowed)
{
}

GatewayCtrlRuleObjectPath::~GatewayCtrlRuleObjectPath()
{
}

const qcc::String& GatewayCtrlRuleObjectPath::getPath() const
{
    return m_ObjectPath;
}

const qcc::String& GatewayCtrlRuleObjectPath::getFriendlyName()
{
    return m_FriendlyName;
}

bool GatewayCtrlRuleObjectPath::isPrefix() const
{
    return m_IsPrefix;
}

void GatewayCtrlRuleObjectPath::setPrefix(bool isPrefix)
{
    m_IsPrefix = isPrefix;
}

bool GatewayCtrlRuleObjectPath::isPrefixAllowed() const {
    return m_isPrefixAllowed;
}

void GatewayCtrlRuleObjectPath::setPrefixAllowed(bool isPrefixAllowed) {
    m_isPrefixAllowed = isPrefixAllowed;
}

GatewayCtrlRuleObjectDescription::GatewayCtrlRuleObjectDescription(const GatewayCtrlRuleObjectPath& objectPath, std::set<GatewayCtrlRuleInterface> interfaces) : m_Interfaces(interfaces), m_IsConfigured(false)
{
    m_ObjectPath = new GatewayCtrlRuleObjectPath(objectPath);
}

GatewayCtrlRuleObjectDescription::GatewayCtrlRuleObjectDescription(const GatewayCtrlRuleObjectPath& objectPath, std::set<GatewayCtrlRuleInterface> interfaces, bool isConfigured) : m_Interfaces(interfaces), m_IsConfigured(isConfigured)
{
    m_ObjectPath = new GatewayCtrlRuleObjectPath(objectPath);
}

GatewayCtrlRuleObjectDescription::~GatewayCtrlRuleObjectDescription()
{

}

GatewayCtrlRuleObjectPath* GatewayCtrlRuleObjectDescription::getObjectPath() const
{
    return m_ObjectPath;
}

const std::set<GatewayCtrlRuleInterface>* GatewayCtrlRuleObjectDescription::getInterfaces() const
{
    return &m_Interfaces;
}

bool GatewayCtrlRuleObjectDescription::isConfigured() const
{
    return m_IsConfigured;

}

void GatewayCtrlRuleObjectDescription::setConfigured(bool configured)
{
    m_IsConfigured = configured;

}



QStatus GatewayCtrlRuleObjectDescription::release()
{
    if (m_ObjectPath) {
        delete m_ObjectPath;
        m_ObjectPath = NULL;
    }
    return ER_OK;
}
}
}