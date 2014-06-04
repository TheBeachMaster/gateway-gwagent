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

#ifndef GATEWAYMANIFESTOBJECT_H_
#define GATEWAYMANIFESTOBJECT_H_

#include <qcc/String.h>
#include <vector>

namespace ajn {
namespace services {

/**
 * Class used to define an ObjectDescription
 */
class GatewayManifestObject {

  public:

    /**
     * typedef to define a Name and FriendlyName pair
     */
    typedef struct {
        qcc::String interfaceName;
        qcc::String interfaceFriendlyName;
        bool isSecured;
    } InterfaceDesc;

    /**
     * Constructor for the GatewayManifestObject class
     */
    GatewayManifestObject();

    /**
     * Constructor for the GatewayManifestObject class
     * @param objectPath - objectPath of the ObjectDescription
     * @param interfaces - interfaces of the ObjectDescription
     */
    GatewayManifestObject(qcc::String const& objectPath, qcc::String const& objectPathFriendly, bool isPrefix,
                          std::vector<InterfaceDesc> const& interfaces);

    /**
     * Destructor of the GatewayManifestObject class
     */
    virtual ~GatewayManifestObject();

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
     * Get the IsObjectPathPrefix flag of the ObjectDescription
     * @return
     */
    bool getIsObjectPathPrefix() const;

    /**
     * Set the IsObjectPathPrefix flag of the ObjectDescription
     * @param objectPath
     */
    void setIsObjectPathPrefix(bool isObjectPathPrefix);

    /**
     * Get the interfaces of the ObjectDescription
     * @return interfaces vector
     */
    const std::vector<InterfaceDesc>& getInterfaces() const;

    /**
     * Set the interfaces of the ObjectDescription
     * @param interfaces
     */
    void setInterfaces(const std::vector<InterfaceDesc>& interfaces);

    /**
     * Get the ObjectPath FriendlyName of the ObjectDescription
     * @return objectPath
     */
    const qcc::String& getObjectPathFriendlyName() const;

    /**
     * Set the ObjectPath Friendly Name of the ObjectDescription
     * @param objectPath
     */
    void setObjectPathFriendlyName(const qcc::String& objectPathFriendlyName);

  private:

    /**
     * The ObjectPath of the ObjectDescription
     */
    qcc::String m_ObjectPath;

    /**
     * The ObjectPath FriendlyName of the ObjectDescription
     */
    qcc::String m_ObjectPathFriendlyName;

    /**
     * Is the ObjectPath a Prefix
     */
    bool m_IsObjectPathPrefix;

    /**
     * The Interfaces of the ObjectDescription
     */
    std::vector<InterfaceDesc> m_Interfaces;
};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYMANIFESTOBJECT_H_ */
