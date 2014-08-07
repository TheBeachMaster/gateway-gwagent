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

import org.alljoyn.gatewaycontroller.sdk.Acl;
import org.alljoyn.gatewaycontroller.sdk.Acl.AclStatus;

/**
 * {@link Acl} to be visualized by the
 * {@link ConnectorAppAclsAdapter}
 */
public class VisualAcl implements VisualItem {

    /**
     * {@link Acl}
     */
    private Acl acl;

    /**
     * The acl status
     */
    private boolean isActive;

    /**
     * Constructor
     * 
     * @param acl
     */
    public VisualAcl(Acl acl) {

        this.acl = acl;
        updateActivityStatus();
    }

    /**
     * @return {@link Acl}
     */
    public Acl getAcl() {
        return acl;
    }

    /**
     * @return Current isActive status of the {@link VisualAcl}
     */
    public boolean isActive() {
        return isActive;
    }

    /**
     * Updates current {@link VisualAcl} status with the given {@link AclStatus}
     * 
     * @param aclStatus
     */
    public void updateActivityStatus() {

        switch (acl.getStatus()) {

            case GW_AS_ACTIVE: {
    
                isActive = true;
                break;
            }
            case GW_AS_INACTIVE: {
    
                isActive = false;
                break;
            }
        }
    }
}
