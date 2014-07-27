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

package org.alljoyn.gatewaycontroller.sdk.managerinterfaces;

import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.Secure;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;

/**
 * AllJoyn interface for managing Access Control List of a Gateway Connector
 * Application
 */
@BusInterface(name = AclManagement.IFNAME)
@Secure
public interface AclManagement {

    /**
     * AllJoyn name of the interface
     */
    public static final String IFNAME = GatewayController.IFACE_PREFIX + ".AclMgmt";

    /**
     * Creating the ACL
     * 
     * @param aclName
     * @param exposedServices
     * @param remotedApps
     * @param internalMetadata
     * @param customMetadata
     * @return The status of the ACL creation
     * @throws BusException
     */
    @BusMethod(name = "CreateAcl", signature = "sa(obas)a(saya(obas))a{ss}a{ss}", replySignature = "qso")
    public CreateAclStatusAJ createAcl(String aclName, ManifestObjectDescriptionAJ[] exposedServices, RemotedAppAJ[] remotedApps, Map<String, String> internalMetadata,
            Map<String, String> customMetadata) throws BusException;

    /**
     * Delete the ACL
     * 
     * @param aclId
     * @return ACL Response Code
     * @throws BusException
     */
    @BusMethod(name = "DeleteAcl", signature = "s", replySignature = "q")
    public short deleteAcl(String aclId) throws BusException;

    /**
     * Returns a list of the existing ACLs
     * 
     * @return {@link AclInfoAJ} list of the existing ACLs
     * @throws BusException
     */
    @BusMethod(name = "ListAcls", replySignature = "a(ssqo)")
    public AclInfoAJ[] listAcls() throws BusException;

    /**
     * @return The version of this interface
     * @throws BusException
     */
    @BusProperty(signature = "q")
    public short getVersion() throws BusException;
}
