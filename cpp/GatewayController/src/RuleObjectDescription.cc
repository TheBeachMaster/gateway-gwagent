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

#include <alljoyn/gateway/RuleObjectDescription.h>
#include <alljoyn/gateway/LogModule.h>


namespace ajn {
namespace gwc {

RuleInterface::RuleInterface(const qcc::String& name, const qcc::String& friendlyName, bool isSecured) : m_Name(name), m_FriendlyName(friendlyName), m_IsSecured(isSecured)
{
}

const qcc::String& RuleInterface::getName() const
{
    return m_Name;
}

const qcc::String& RuleInterface::getFriendlyName() const
{
    return m_FriendlyName;
}

bool RuleInterface::isSecured() const
{
    return m_IsSecured;
}

bool RuleInterface::operator<(const RuleInterface to) const
{
    const bool ALessThanB = getName().operator<(to.getName());

    return ALessThanB;
}


RuleObjectPath::RuleObjectPath(const qcc::String& objectPath, const qcc::String& friendlyName, bool isPrefix, bool isPrefixAllowed) : m_ObjectPath(objectPath), m_FriendlyName(friendlyName), m_IsPrefix(isPrefix), m_isPrefixAllowed(isPrefixAllowed)
{
}

RuleObjectPath::~RuleObjectPath()
{
}

const qcc::String& RuleObjectPath::getPath() const
{
    return m_ObjectPath;
}

const qcc::String& RuleObjectPath::getFriendlyName()
{
    return m_FriendlyName;
}

bool RuleObjectPath::isPrefix() const
{
    return m_IsPrefix;
}

void RuleObjectPath::setPrefix(bool isPrefix)
{
    m_IsPrefix = isPrefix;
}

bool RuleObjectPath::isPrefixAllowed() const {
    return m_isPrefixAllowed;
}

void RuleObjectPath::setPrefixAllowed(bool isPrefixAllowed) {
    m_isPrefixAllowed = isPrefixAllowed;
}

RuleObjectDescription::RuleObjectDescription(const RuleObjectPath& objectPath, std::set<RuleInterface> interfaces) : m_IsConfigured(false), m_Interfaces(interfaces)
{
    m_ObjectPath = new RuleObjectPath(objectPath);
}

RuleObjectDescription::RuleObjectDescription(const RuleObjectPath& objectPath, std::set<RuleInterface> interfaces, bool isConfigured) : m_IsConfigured(isConfigured), m_Interfaces(interfaces)
{
    m_ObjectPath = new RuleObjectPath(objectPath);
}

RuleObjectDescription::~RuleObjectDescription()
{

}

RuleObjectPath* RuleObjectDescription::getObjectPath() const
{
    return m_ObjectPath;
}

const std::set<RuleInterface>* RuleObjectDescription::getInterfaces() const
{
    return &m_Interfaces;
}

bool RuleObjectDescription::isConfigured() const
{
    return m_IsConfigured;

}

void RuleObjectDescription::setConfigured(bool configured)
{
    m_IsConfigured = configured;

}



QStatus RuleObjectDescription::release()
{
    if (m_ObjectPath) {
        delete m_ObjectPath;
        m_ObjectPath = NULL;
    }
    return ER_OK;
}
}
}
