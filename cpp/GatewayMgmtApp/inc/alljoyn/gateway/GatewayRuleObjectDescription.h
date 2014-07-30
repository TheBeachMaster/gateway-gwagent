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

#ifndef GatewayRuleObjectDescription_H_
#define GatewayRuleObjectDescription_H_

#include <qcc/String.h>
#include <vector>

namespace ajn {
namespace gw {

/**
 * Class used to define an ObjectDescription
 */
class GatewayRuleObjectDescription {

  public:

    /**
     * Constructor for the GatewayRuleObjectDescription class
     */
    GatewayRuleObjectDescription();

    /**
     * Constructor for the GatewayRuleObjectDescription class
     * @param objectPath - objectPath of the ObjectDescription
     * @param isPrefix - isPrefix of the ObjectDescription
     * @param interfaces - interfaces of the ObjectDescription
     */
    GatewayRuleObjectDescription(qcc::String const& objectPath, bool isPrefix, std::vector<qcc::String> const& interfaces);

    /**
     * Destructor of the GatewayRuleObjectDescription class
     */
    virtual ~GatewayRuleObjectDescription();

    /**
     * Get the interfaces of the ObjectDescription
     * @return interfaces vector
     */
    const std::vector<qcc::String>& getInterfaces() const;

    /**
     * Set the interfaces of the ObjectDescription
     * @param interfaces
     */
    void setInterfaces(const std::vector<qcc::String>& interfaces);

    /**
     * Get the ObjectPath of the ObjectDescription
     * @return objectPath
     */
    const qcc::String& getObjectPath() const;

    /**
     * Set the ObjectPath of the ObjectDescription
     * @param objectPath
     */
    void setObjectPath(const qcc::String& objectPath);

    /**
     * Get the isPrefix boolean of the ObjectDescription
     * @return isPrefix
     */
    bool getIsPrefix() const;

    /**
     * Set the isPrefix boolean of the ObjectDescriptions
     * @param isPrefix
     */
    void setIsPrefix(bool isPrefix);

  private:

    /**
     * The ObjectPath of the ObjectDescription
     */
    qcc::String m_ObjectPath;

    /**
     * Is the ObjectPath a Prefix
     */
    bool m_IsPrefix;

    /**
     * The Interfaces of the ObjectDescription
     */
    std::vector<qcc::String> m_Interfaces;
};

} /* namespace gw */
} /* namespace ajn */

#endif /* GatewayRuleObjectDescription_H_ */
