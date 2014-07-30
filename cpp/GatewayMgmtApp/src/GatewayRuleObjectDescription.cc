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

#include <alljoyn/gateway/GatewayRuleObjectDescription.h>

namespace ajn {
namespace gw {

GatewayRuleObjectDescription::GatewayRuleObjectDescription() : m_ObjectPath(""), m_IsPrefix(false)
{

}

GatewayRuleObjectDescription::GatewayRuleObjectDescription(qcc::String const& objectPath, bool isPrefix, std::vector<qcc::String> const& interfaces) :
    m_ObjectPath(objectPath), m_IsPrefix(isPrefix), m_Interfaces(interfaces)
{

}

GatewayRuleObjectDescription::~GatewayRuleObjectDescription()
{

}

const std::vector<qcc::String>& GatewayRuleObjectDescription::getInterfaces() const
{
    return m_Interfaces;
}

void GatewayRuleObjectDescription::setInterfaces(const std::vector<qcc::String>& interfaces)
{
    m_Interfaces = interfaces;
}

const qcc::String& GatewayRuleObjectDescription::getObjectPath() const
{
    return m_ObjectPath;
}

void GatewayRuleObjectDescription::setObjectPath(const qcc::String& objectPath)
{
    m_ObjectPath = objectPath;
}

bool GatewayRuleObjectDescription::getIsPrefix() const
{
    return m_IsPrefix;
}

void GatewayRuleObjectDescription::setIsPrefix(bool isPrefix)
{
    m_IsPrefix = isPrefix;
}

} /* namespace gw */
} /* namespace ajn */
