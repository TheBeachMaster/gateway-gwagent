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

import org.alljoyn.gatewaycontroller.sdk.AclRules;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleObjectPath;

/**
 * Visualization {@link RuleObjectPath}
 */
public final class VisualObjectPath {

    /**
     * Reference to the {@link RuleObjectPath}
     */
    private RuleObjectPath objPath;

    /**
     * Gets TRUE if the object is selected to be part of the {@link AclRules}
     */
    private boolean isConfigured;

    /**
     * Gets TRUE if the isConfigured checkbox is enabled
     */
    private boolean isConfiguredEnabled;

    /**
     * Constructor
     * 
     * @param objPath
     * @param isConfigured
     */
    public VisualObjectPath(RuleObjectPath objPath, boolean isConfigured) {

        this.objPath             = objPath;
        this.isConfigured        = isConfigured;
        this.isConfiguredEnabled = true;
    }

    /**
     * @return TRUE if the configuration checkbox was selected
     */
    public boolean isConfigured() {
        return isConfigured;
    }

    /**
     * Set the state of the configuration checkbox
     * 
     * @param isConfigured
     */
    public void setConfigured(boolean isConfigured) {
        this.isConfigured = isConfigured;
    }

    /**
     * @return TRUE if the object path configure checkbox is enabled
     */
    public boolean isConfiguredEnabled() {
        return isConfiguredEnabled;
    }

    /**
     * @param isConfiguredEnabled
     *            TRUE if the configure checkbox is enabled
     */
    public void setConfiguredEnabled(boolean isConfiguredEnabled) {
        this.isConfiguredEnabled = isConfiguredEnabled;
    }

    /**
     * @return Returns wrapped {@link RuleObjectPath}
     */
    public RuleObjectPath getObjPath() {
        return objPath;
    }

    /**
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {

        final int PRIME = 31;
        int result      = 1;
        result          = PRIME * result + ((objPath == null) ? 0 : objPath.hashCode());
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

        if (!(obj instanceof VisualObjectPath)) {
            return false;
        }

        VisualObjectPath other = (VisualObjectPath) obj;
        if (objPath == null) {

            if (other.objPath != null) {
                return false;
            }
        } else if (!objPath.equals(other.objPath)) {
            return false;
        }

        return true;
    }

}