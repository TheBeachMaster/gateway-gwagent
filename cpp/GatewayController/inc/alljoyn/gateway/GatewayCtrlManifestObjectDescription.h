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

#ifndef GatewayCtrlManifestObjectDescription_H
#define GatewayCtrlManifestObjectDescription_H

#include <set>
#include <vector>
#include <qcc/String.h>
#include <alljoyn/Status.h>
#include <alljoyn/MsgArg.h>

namespace ajn {
namespace services {

class GatewayCtrlManifestRules;

class GatewayCtrlConnAppInterface {
  public:

    /**
     * Constructor
     * @param name AllJoyn name of the interface
     * @param friendlyName The interface friendly name
     * @param isSecured Whether the interface is secured
     */
    GatewayCtrlConnAppInterface(qcc::String name, qcc::String friendlyName, bool isSecured);

    /**
     * Copy Constructor
     * @param other Object to copy from
     */
    GatewayCtrlConnAppInterface(const GatewayCtrlConnAppInterface& other) { m_Name = other.m_Name; m_FriendlyName = other.m_FriendlyName; m_IsSecured = other.m_IsSecured; }

    /**
     * Destructor
     */
    virtual ~GatewayCtrlConnAppInterface() { }

    /**
     * Returns a name of the interface.
     * @return interface name
     */
    qcc::String GetName() const;

    /**
     * Returns a friendly name of the interface.
     * The friendly name may be presented to the end user.
     * @return Interface friendly name
     */
    qcc::String GetFriendlyName() const;

    /**
     * @return TRUE if the interface is secured
     */
    bool IsSecured() const;

    /**
     * operator less than
     * @param to check if the object is less than this param
     * @return TRUE if the object is less than 'to'
     */
    bool operator<(const GatewayCtrlConnAppInterface to) const;


  private:
    /**
     * The interface name
     */
    qcc::String m_Name;

    /**
     * The interface friendly name
     */
    qcc::String m_FriendlyName;

    /**
     * TRUE of the interface is secured
     */
    bool m_IsSecured;

};

class GatewayCtrlConnAppObjectPath {
  public:

    /**
     * Constructor
     * @param objectPath AllJoyn object identification
     * @param friendlyName The friendly name of the object path. This name may be presented
     * @param isPrefix TRUE if the object path is a prefix for the full object path
     * @param isPrefixAllowed TRUE if the object path is allowed to be a prefix according
     * to the end user.
     */
    GatewayCtrlConnAppObjectPath(qcc::String objectPath, qcc::String friendlyName, bool isPrefix, bool isPrefixAllowed);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlConnAppObjectPath();

    /**
     * AllJoyn object identification
     * @return The value of the object path
     */
    qcc::String GetPath() const;

    /**
     * Returns the friendly name  of the object path.
     * This name may be presented to the end user.
     * @return Object path friendly name
     */
    qcc::String GetFriendlyName();

    /**
     * @return Returns whether this object path is a prefix
     */
    bool IsPrefix() const;

    /**
     * @param isPrefix set the prefix flag to isPrefix
     */
    void SetPrefix(bool isPrefix);

    /**
     * @return TRUE if the object path is allowed to be a prefix according
     * to the {@link ConnectorApplication} manifest
     */
    bool isPrefixAllowed() const;

    /**
     * @param isPrefixAllowed Set whether the object path is allowed to be a prefix according
     * to the {@link ConnectorApplication} manifest
     */
    void setPrefixAllowed(bool isPrefixAllowed);

    /**
     * operator less than
     * @param other check if the object is less than this param
     * @return TRUE if the object is less than 'other'
     */

    bool operator<(const GatewayCtrlConnAppObjectPath& other) const {
        if (m_IsPrefix != other.m_IsPrefix) {
            return true;
        }


        return m_ObjectPath < other.m_ObjectPath;
    }

  private:
    /**
     * AllJoyn object identification
     */
    qcc::String m_ObjectPath;

    /**
     * The friendly name of the object path. This name may be presented
     * to the end user.
     */
    qcc::String m_FriendlyName;

    /**
     * Gets TRUE if the object path is a prefix for the full object path
     */
    bool m_IsPrefix;

    /**
     * Gets TRUE if the object path is allowed by the manifest to be a prefix
     */
    bool m_isPrefixAllowed;
};

class GatewayCtrlManifestObjectDescription {

  public:


    /**
     * Constructor
     * @param objectPath AllJoyn object identification
     * @param interfaces Set of the interfaces related to the given object path
     */
    GatewayCtrlManifestObjectDescription(const GatewayCtrlConnAppObjectPath& objectPath, std::set<GatewayCtrlConnAppInterface> interfaces);

    /**
     * Constructor
     * @param objectPath AllJoyn object identification
     * @param interfaces Set of the interfaces related to the given object path
     * @param configured  Set TRUE to permit this {@link GatewayCtrlManifestObjectDescription} object path and interfaces
     * to be remoted by the Third Party Application
     */
    GatewayCtrlManifestObjectDescription(const GatewayCtrlConnAppObjectPath& objectPath, std::set<GatewayCtrlConnAppInterface> interfaces, bool isConfigured);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlManifestObjectDescription();

    /**
     * The object path supported by the Service Provider Application manifest
     * @return Object path
     */
    GatewayCtrlConnAppObjectPath* GetObjectPath() const;

    /**
     * The interfaces supported by the Service Provider Application manifest
     * @return Set of the {@link ConnAppInterface} objects
     */
    const std::set<GatewayCtrlConnAppInterface>*GetInterfaces() const;

    /**
     * The flag has TRUE if this {@link ManifestObjectDescription} is configured
     * to permit the object path and interfaces to be remoted by the Third Party Application
     * @return configured state
     */
    bool IsConfigured() const;

    /**
     * Set TRUE to permit this {@link ManifestObjectDescription} object path and interfaces
     * to be remoted by the Third Party Application
     * @param configured
     */
    void SetConfigured(bool configured);

    /**
     * @return Status of release
     */
    QStatus Release();

  private:


    /**
     * The flag has TRUE if this {@link ManifestObjectDescription} is configured
     * to permit the object path and interfaces to be remoted by the Third Party Application
     */
    bool m_IsConfigured;

    /**
     * The object path supported by the Service Provider Application manifest
     */
    GatewayCtrlConnAppObjectPath*m_ObjectPath;          //TODO: why is this a pointer?

    /**
     * The interfaces supported by the Service Provider Application manifest
     */
    std::set<GatewayCtrlConnAppInterface> m_Interfaces;
};
}
}
#endif /* defined(GatewayCtrlManifestObjectDescription_H) */
