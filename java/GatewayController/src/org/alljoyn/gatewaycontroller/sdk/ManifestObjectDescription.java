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

package org.alljoyn.gatewaycontroller.sdk;

import java.util.HashSet;
import java.util.Set;

import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionInfoAJ.InterfaceInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionInfoAJ.ObjectPathInfoAJ;

/**
 * This class stores object description provided with the manifest of the
 * Gateway Connector Application
 */
public class ManifestObjectDescription {

    /**
     * This class represents an object path received with the
     * {@link ManifestObjectDescription} supported by the Gateway Connector
     * Application
     */
    public static final class ConnAppObjectPath {

        /**
         * AllJoyn object identification
         */
        private final String objectPath;

        /**
         * The friendly name of the object path. This name may be presented to
         * the end user.
         */
        private final String friendlyName;

        /**
         * Gets TRUE if the object path is a prefix for the full object path
         */
        private boolean isPrefix;

        /**
         * Gets TRUE if the object path is allowed by the manifest to be a
         * prefix
         */
        private boolean isPrefixAllowed;

        /**
         * Constructor
         * 
         * @param objectPath
         *            AllJoyn object identification
         * @param friendlyName
         *            The friendly name of the object path. This name may be
         *            presented
         * @param isPrefix
         *            TRUE if the object path is a prefix for the full object
         *            path
         * @param isPrefixAllowed
         *            TRUE if the object path is allowed to be a prefix
         *            according to the {@link ConnectorApplication} manifest
         * @throws IllegalArgumentException
         *             is thrown if bad arguments have been received
         */
        public ConnAppObjectPath(String objectPath, String friendlyName, boolean isPrefix, boolean isPrefixAllowed) {

            if (objectPath == null || objectPath.length() == 0) {
                throw new IllegalArgumentException("objPath is undefined");
            }

            if (friendlyName == null) {
                throw new IllegalArgumentException("friendlyName is undefined");
            }

            this.objectPath      = objectPath;
            this.friendlyName    = friendlyName;
            this.isPrefix        = isPrefix;
            this.isPrefixAllowed = isPrefixAllowed;
        }

        /**
         * Constructor
         * 
         * @param opInfo
         */
        ConnAppObjectPath(ObjectPathInfoAJ opInfo) {

            objectPath   = opInfo.objectPath;
            friendlyName = opInfo.objectPathFriendlyName;
            isPrefix     = opInfo.isPrefix;
        }

        /**
         * AllJoyn object identification
         * 
         * @return The value of the object path
         */
        public String getPath() {
            return objectPath;
        }

        /**
         * Returns the friendly name of the object path. This name may be
         * presented to the end user.
         * 
         * @return Object path friendly name
         */
        public String getFriendlyName() {
            return friendlyName;
        }

        /**
         * @return Returns whether this object path is a prefix
         */
        public boolean isPrefix() {
            return isPrefix;
        }

        /**
         * Set whether this object path is a prefix
         * 
         * @param isPrefix
         */
        public void setPrefix(boolean isPrefix) {
            this.isPrefix = isPrefix;
        }

        /**
         * @return TRUE if the object path is allowed to be a prefix according
         *         to the {@link ConnectorApplication} manifest
         */
        public boolean isPrefixAllowed() {
            return isPrefixAllowed;
        }

        /**
         * @param isPrefixAllowed
         *            Set whether the object path is allowed to be a prefix
         *            according to the {@link ConnectorApplication} manifest
         */
        void setPrefixAllowed(boolean isPrefixAllowed) {
            this.isPrefixAllowed = isPrefixAllowed;
        }

        /**
         * @see java.lang.Object#hashCode()
         */
        @Override
        public int hashCode() {

            final int PRIME  = 31;
            final int BOOL_T = 1231;
            final int BOOL_F = 1237;
            int result       = 1;

            result = PRIME * result + (isPrefix ? BOOL_T : BOOL_F);
            result = PRIME * result + ((objectPath == null) ? 0 : objectPath.hashCode());
            return result;
        }

        /**
         * @see java.lang.Object#equals(java.lang.Object)
         */
        @Override
        public boolean equals(Object obj) {

            if (this == obj) {
                return true;
            }

            if (!(obj instanceof ConnAppObjectPath)) {
                return false;
            }

            ConnAppObjectPath other = (ConnAppObjectPath) obj;

            if (isPrefix != other.isPrefix) {
                return false;
            }
            if (objectPath == null) {

                if (other.objectPath != null) {
                    return false;
                }
            } else if (!objectPath.equals(other.objectPath)) {
                return false;
            }

            return true;
        }

        /**
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {
            return "OP [objectPath='" + objectPath + "', friendlyName='" + friendlyName + "', " + 
                       "isPrefix='" + isPrefix + "', isPrefixAllowed='" + isPrefixAllowed + "']";
        }
    }

    // ======================================//

    /**
     * This class represents an interface received with the
     * {@link ManifestObjectDescription} supported by the Gateway Connector
     * Application
     */
    public static final class ConnAppInterface {

        /**
         * The interface name
         */
        private final String name;

        /**
         * The interface friendly name
         */
        private final String friendlyName;

        /**
         * TRUE of the interface is secured
         */
        private final boolean isSecured;

        /**
         * It's immutable object, so calculate the hash code only once
         */
        private volatile int hashCode;

        /**
         * Constructor
         * 
         * @param name
         *            AllJoyn name of the interface
         * @param friendlyName
         *            The interface friendly name
         * @param isSecured
         *            Whether the interface is secured
         * @throws IllegalArgumentException
         *             is thrown if bad arguments have been received
         */
        public ConnAppInterface(String name, String friendlyName, boolean isSecured) {

            if (name == null || name.length() == 0) {
                throw new IllegalArgumentException("name is undefined");
            }

            if (friendlyName == null) {
                throw new IllegalArgumentException("friendlyName is undefined");
            }

            this.name         = name;
            this.friendlyName = friendlyName;
            this.isSecured    = isSecured;
        }

        /**
         * Constructor Initialize non secure interface
         * 
         * @param name
         *            AllJoyn name of the interface
         * @param friendlyName
         *            The interface friendly name
         * @throws IllegalArgumentException
         *             is thrown if bad arguments have been received
         */
        public ConnAppInterface(String name, String friendlyName) {
            this(name, friendlyName, false);
        }

        /**
         * Constructor
         * 
         * @param interfaceInfo
         */
        ConnAppInterface(InterfaceInfoAJ interfaceInfo) {

            name         = interfaceInfo.interfaceName;
            friendlyName = interfaceInfo.friendlyName;
            isSecured    = interfaceInfo.isSecured;
        }

        /**
         * Returns a name of the interface.
         * 
         * @return interface name
         */
        public String getName() {
            return name;
        }

        /**
         * Returns a friendly name of the interface. The friendly name may be
         * presented to the end user.
         * 
         * @return Interface friendly name
         */
        public String getFriendlyName() {
            return friendlyName;
        }

        /**
         * @return TRUE if the interface is secured
         */
        public boolean isSecured() {
            return isSecured;
        }

        /**
         * @see java.lang.Object#hashCode()
         */
        @Override
        public int hashCode() {

            final int PRIME = 31;

            if (hashCode == 0) {

                hashCode = PRIME + ((name == null) ? 0 : name.hashCode());
            }
            return hashCode;
        }

        /**
         * Two {@link ConnAppInterface} objects are equal if they have the same
         * interface name
         * 
         * @see java.lang.Object#equals(java.lang.Object)
         */
        @Override
        public boolean equals(Object obj) {

            if (this == obj) {                        // Check self reference equity
                return true;
            }

            if (!(obj instanceof ConnAppInterface)) { // Check type correctness
                                                      // and not null
                return false;
            }

            ConnAppInterface other = (ConnAppInterface) obj;

            if (name == null) {

                if (other.name != null) {
                    return false;
                }
            } else if (!name.equals(other.name)) {
                return false;
            }

            return true;
        }

        /**
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {
            return "Iface [name='" + name + "', friendlyName='" + friendlyName + "', isSecured='" 
                        + isSecured + "']";
        }
    }

    // ======================================//

    /**
     * The object path supported by the Service Provider Application manifest
     */
    private final ConnAppObjectPath objectPath;

    /**
     * The interfaces supported by the Service Provider Application manifest
     */
    private final Set<ConnAppInterface> interfaces;

    /**
     * The flag has TRUE if this {@link ManifestObjectDescription} is configured
     * to permit the object path and interfaces to be remoted by the Gateway
     * Connector Application
     */
    private boolean isConfigured;

    /**
     * Constructor
     * 
     * @param objectPath
     *            AllJoyn object identification
     * @param interfaces
     *            Set of the interfaces related to the given object path
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public ManifestObjectDescription(ConnAppObjectPath objectPath, Set<ConnAppInterface> interfaces) {

        if (objectPath == null) {
            throw new IllegalArgumentException("objectPath is undefined");
        }

        if (interfaces == null) {
            throw new IllegalArgumentException("interfaces is undefined");
        }

        this.objectPath = objectPath;
        this.interfaces = interfaces;
    }

    /**
     * Constructor
     * 
     * @param objectPath
     *            AllJoyn object identification
     * @param interfaces
     *            Set of the interfaces related to the given object path
     * @param configured
     *            Set TRUE to permit this {@link ManifestObjectDescription}
     *            object path and interfaces to be remoted by the Gateway
     *            Connector Application
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public ManifestObjectDescription(ConnAppObjectPath objectPath, Set<ConnAppInterface> interfaces, boolean configured) {

        this(objectPath, interfaces);
        this.isConfigured = configured;
    }

    /**
     * Create the object from the {@link ManifestObjectDescriptionInfoAJ}
     * 
     * @param objDescInfo
     */
    ManifestObjectDescription(ManifestObjectDescriptionInfoAJ objDescInfo) {

        objectPath = new ConnAppObjectPath(objDescInfo.objPathInfo);
        interfaces = new HashSet<ManifestObjectDescription.ConnAppInterface>(objDescInfo.interfaces.length);

        for (InterfaceInfoAJ ifaceInfoAJ : objDescInfo.interfaces) {
            interfaces.add(new ConnAppInterface(ifaceInfoAJ));
        }
    }

    /**
     * The object path supported by the Service Provider Application manifest
     * 
     * @return Object path
     */
    public ConnAppObjectPath getObjectPath() {
        return objectPath;
    }

    /**
     * The interfaces supported by the Service Provider Application manifest
     * 
     * @return Set of the {@link ConnAppInterface} objects
     */
    public Set<ConnAppInterface> getInterfaces() {
        return interfaces;
    }

    /**
     * The flag has TRUE if this {@link ManifestObjectDescription} is configured
     * to permit the object path and interfaces to be remoted by the Gateway
     * Connector Application
     * 
     * @return configured state
     */
    public boolean isConfigured() {
        return isConfigured;
    }

    /**
     * Set TRUE to permit this {@link ManifestObjectDescription} object path and
     * interfaces to be remoted by the Gateway Connector Application
     * 
     * @param configured
     */
    public void setConfigured(boolean configured) {
        this.isConfigured = configured;
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "MOD: ('" + objectPath + "', '" + interfaces + "', isConfigured='" + isConfigured + "')";
    }

}
