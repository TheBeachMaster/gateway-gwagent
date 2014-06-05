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
 * AllJoyn interface for operating Access Control List of a Third Party Application
 */
@BusInterface(name=AccessControlListIface.IFNAME)
@Secure
public interface AccessControlListIface {
	
	/**
	 * AllJoyn name of the interface
	 */
	public static final String IFNAME = GatewayController.IFACE_PREFIX + ".Acl";

	/**
	 * Activate the ACL
	 * @return ACL Response Code
	 * @throws BusException
	 */
	@BusMethod(name="ActivateAcl", replySignature="q")
	public short activateAcl() throws BusException;
	
	/**
	 * Deactivate the ACL
	 * @return ACL Response Code
	 * @throws BusException
	 */
	@BusMethod(name="DeactivateAcl", replySignature="q")
	public short deactivateAcl() throws BusException;
	
	/**
	 * @return Returns ACL rules 
	 * @throws BusException
	 */
	@BusMethod(name="GetAcl", replySignature="sa(obas)a(saya(obas))a{ss}a{ss}")
	public AccessControlListAJ getAcl() throws BusException;   
	
	/**
	 * @return Return ACL status
	 * @throws BusException
	 */
	@BusMethod(name="GetAclStatus", replySignature="q")
	public short getAclStatus() throws BusException;
	
	/**
	 * Update the ACL
	 * @param aclName
	 * @param exposedServices
	 * @param remotedApps
	 * @param metadata
	 * @return ACL Response Code
	 * @throws BusException
	 */
	@BusMethod(name="UpdateAcl", signature="sa(obas)a(saya(obas))a{ss}a{ss}", replySignature="q")
	public short updateAcl(String aclName, ManifestObjectDescriptionAJ[] exposedServices, 
                           RemotedAppAJ[] remotedApps, Map<String,String> internalMetadata, 
                           Map<String, String> customMetadata) throws BusException;
	
	/**
	 * Update the ACL metadata
	 * @param metadata
	 * @return ACL Response Code
	 * @throws BusException
	 */
	@BusMethod(name="UpdateAclMetaData", signature="a{ss}", replySignature="q")
	public short updateAclMetaData(Map<String,String> metadata) throws BusException;
	
	/**
	 * Update ACL custom metadata
	 * @param metadata internal metadata
	 * @return ACL Response Code
	 * @throws BusException
	 */
	@BusMethod(name="UpdateAclCustomMetaData", signature="a{ss}", replySignature="q")
	public short updateAclCustomMetaData(Map<String,String> internalMetadata) throws BusException;
	
	/**
	 * @return The version of this interface
	 * @throws BusException
	 */
	@BusProperty(signature="q")
	public short getVersion() throws BusException;
}
