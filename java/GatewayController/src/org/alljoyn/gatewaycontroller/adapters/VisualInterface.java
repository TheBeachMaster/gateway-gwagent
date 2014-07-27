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

package org.alljoyn.gatewaycontroller.adapters;

import org.alljoyn.gatewaycontroller.sdk.AccessRules;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;

/**
 * Visualization of the {@link ConnAppInterface}
 */
public final class VisualInterface {

    /**
     * Reference to the {@link ConnAppInterface} object
     */
    private ConnAppInterface iface;

    /**
     * Gets TRUE if the interface is selected to be part of the
     * {@link AccessRules}
     */
    private boolean isConfigured;

    /**
     * Constructor
     * 
     * @param iface
     * @param isConfigured
     */
    public VisualInterface(ConnAppInterface iface) {

        this.iface = iface;
    }

    /**
     * @return Returns TRUE if the interface is selected
     */
    public boolean isConfigured() {
        return isConfigured;
    }

    /**
     * @param isConfigured
     *            Set the configuration state of the interface
     */
    public void setConfigured(boolean isConfigured) {
        this.isConfigured = isConfigured;
    }

    /**
     * @return Return the wrapped {@link ConnAppInterface} object
     */
    public ConnAppInterface getIface() {
        return iface;
    }

    /**
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {

        final int PRIME = 31;
        int result      = 1;
        result          = PRIME * result + ((iface == null) ? 0 : iface.hashCode());
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

        if (obj == null) {
            return false;
        }

        if (!(obj instanceof VisualInterface)) {
            return false;
        }

        VisualInterface other = (VisualInterface) obj;
        if (iface == null) {

            if (other.iface != null) {
                return false;
            }
        } else if (!iface.equals(other.iface)) {
            return false;
        }

        return true;
    }

}