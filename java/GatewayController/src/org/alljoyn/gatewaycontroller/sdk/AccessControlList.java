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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppObjectPath;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil;
import org.alljoyn.gatewaycontroller.sdk.announcement.AnnouncementData;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.AccessControlListAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.AccessControlListIface;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.AclInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.RemotedAppAJ;
import org.alljoyn.services.common.utils.TransportUtil;

import android.util.Log;


/**
 * The object contains access control configuration rules for a Gateway Connector Application.
 */
public class AccessControlList {
	private static final String TAG = "gwc" + AccessControlList.class.getSimpleName();
	
	/**
	 * Access Control List response code. 
	 * This status is returned as a result of actions applied on the Access Control List 
	 */
	public static enum AclResponseCode {
		
		GW_ACL_RC_SUCCESS("Success", (short) 0),
		GW_ACL_RC_INVALID("Invalid", (short) 1),
		GW_ACL_RC_REGISTER_ERROR("Register error", (short) 2),
		GW_ACL_RC_ACL_NOT_FOUND("ACL not found",(short) 3),
		GW_ACL_RC_PERSISTENCE_ERROR("ACL persistence error", (short) 4),
		GW_ACL_RC_POLICYMANAGER_ERROR("ACL policy manager error", (short)5),
		GW_ACL_RC_METADATA_ERROR("Metadata error", (short)6) 
		;
		
		/**
		 * Status description
		 */
		public final String DESC;

		/**
		 * The status code
		 */
		public final short CODE;
		
		/**
		 * Constructor  
		 * @param desc Status description 
		 * @param code Status code
		 */
		private AclResponseCode(String desc, short code) {
			
			DESC = desc;
			CODE = code;
		}
	}
	
	//===========================================//
	
	/**
	 * The current status of the Access Control List
	 */
	public static enum AclStatus {
		
		GW_AS_INACTIVE("Inactive", (short) 0),
		GW_AS_ACTIVE("Active", (short) 1),
		;
		
		/**
		 * Status description
		 */
		public final String DESC;

		/**
		 * The status code
		 */
		public final short CODE;
		
		/**
		 * Constructor  
		 * @param desc Status description 
		 * @param code Status code
		 */
		private AclStatus (String desc, short code) {
			
			DESC = desc;
			CODE = code;
		}
	}
	
	//===========================================//
	
	/**
	 * ACL id
	 */
	private final String id;
	
	/**
	 * ACL name
	 * The ACL name may be updated
	 */
	private String aclName;
	
	/**
	 * Currect ACL status
	 */
	private AclStatus status;
	
	/**
	 * ACL objPath
	 */
	private final String objectPath;

	/**
	 * The name of the gateway {@link BusAttachment} a Gateway Connector Application which is related to this ACL
	 * is installed on
	 */
	private final String gwBusName;

	/**
	 * The metadata for the SDK internal usage. The metadata is set by the call to
	 * {@link AccessControlList#retrieveAcl(int, ManifestRules)}
	 */
	private Map<String, String> aclMetadata;
	
	/**
	 * Suffix of the device name that is sent with the ACL metadata
	 */
	private static final String DEVICE_NAME_SUFFIX = "_DEVICE_NAME";
	
	/**
	 * Suffix of the application name that is sent with the ACL metadata
	 */
	private static final String APP_NAME_SUFFIX    = "_APP_NAME";
	
	/**
	 * Constructor
	 * @param gwBusName The name of the gateway {@link BusAttachment} hosting a Gateway Connector Application 
	 * that is related to this ACL
	 * @param id Id of the Access Control List
	 * @param objectPath The object path of the Access Control List
	 * @throws IllegalArgumentException is thrown if bad arguments have been received 
	 */
	public AccessControlList( String gwBusName, String id, String objectPath) {
		
		if ( gwBusName == null || gwBusName.length() == 0 ) {
			throw new IllegalArgumentException("gwBusName is undefined");
		}
		
		if ( id == null || id.length() == 0 ) {
			throw new IllegalArgumentException("id is undefined");
		}
		
		if ( objectPath == null || objectPath.length() == 0 ) {
			throw new IllegalArgumentException("objPath is undefined");
		}
		
		this.id         = id;
		this.objectPath = objectPath;
		this.gwBusName  = gwBusName;
	}
	
	/**
	 * Constructor
	 * @param gwBusName The name of the gateway {@link BusAttachment} hosting a Gateway Connector Application 
	 * that is related to this ACL 
	 * @param aclInfoAJ The ACL information
	 * @throws GatewayControllerException if failed to initialize the object
	 */
	AccessControlList(String gwBusName, AclInfoAJ aclInfoAJ) throws GatewayControllerException {
		
		this.gwBusName      = gwBusName;
		this.aclName        = aclInfoAJ.aclName;
		this.id             = aclInfoAJ.aclId;
		this.objectPath     = aclInfoAJ.objectPath;
		
		AclStatus aclStatus = CommunicationUtil.shortToEnum(AclStatus.class, aclInfoAJ.aclStatus);
		if ( aclStatus == null ) {
			
			Log.e(TAG, "Received unrecognized ACL status: '" + aclInfoAJ.aclStatus + "', objPath: '" +
		                this.objectPath + "'");
			
			throw new GatewayControllerException("Received unrecognized ACL status: '" +
			          aclInfoAJ.aclStatus + "'");
		}
		
		this.status = aclStatus;
	}

	/**
	 * @return The name of the Access Control List
	 */
	public String getName() {
		return aclName;
	}

	/**
	 * Set the name of the Access Control List 
	 * @param name The ACL name
	 * @throws IllegalArgumentException is thrown if undefined name has been received 
	 */
	public void setName(String name) {
		
		if ( name == null ) {
			throw new IllegalArgumentException("name is undefined");
		}
		
		this.aclName = name;
	}

	/**
	 * @return Id of the Access Control List
	 */
	public String getId() {
		return id;
	}

	/**
	 * @return Object path of the Access Control List
	 */
	public String getObjectPath() {
		return objectPath;
	}

	/**
	 * @return The name of the gateway {@link BusAttachment} hosting a Gateway Connector Application 
	 * that is related to this Access Control List 
	 */
	public String getGwBusName() {
		return gwBusName;
	}
	
	/**
	 * Return the current state of the {@link AccessControlList}
	 * @return {@link AclStatus}
	 */
	public AclStatus getStatus() {
		return status;
	}
	
	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		
		StringBuilder sb = new StringBuilder("AccessControlList [");
		sb.append("id='").append(id).append("', ")
		  .append("name='").append(aclName).append("', ")
		  .append("objPath='").append(objectPath).append("', ")
		  .append("gwBusName='").append(gwBusName).append("', ")
		  .append("status='").append(status).append("'");
		
		return sb.toString();
	}
	
	
	//===================================================//
	
	/**
	 * Activate the Access Control List
	 * @param sessionId The id of the session established with the gateway
	 * @return {@link AclResponseCode}
	 * @throws GatewayControllerException if failed to activate the ACL
	 */
	public AclResponseCode activate(int sessionId) throws GatewayControllerException {
		
		AccessControlListIface acl = getAclProxy(sessionId);
		
		Log.d(TAG, "Activate ACL, objPath: '" + objectPath + "'");
		
		short respCode;
		try {
			respCode = acl.activateAcl();
		}
		catch (BusException be) {
			
			Log.e(TAG, "Failed to activate the ACL, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("Failed to activate the ACL", be);
		}
		
		AclResponseCode aclRespCode = getAclResponseCode(respCode);
		Log.d(TAG, "ACL activation response code: '" + aclRespCode + "', objPath: '" + objectPath + "'");
		
		if ( aclRespCode == AclResponseCode.GW_ACL_RC_SUCCESS ) {
			
			status = AclStatus.GW_AS_ACTIVE;
		}
		
		return aclRespCode;
	}
	
	/**
	 * Deactivate the Access Control List
	 * @param sessionId The id of the session established with the gateway
	 * @return {@link AclResponseCode}
	 * @throws GatewayControllerException if failed to deactivate the ACL
	 */
	public AclResponseCode deactivate(int sessionId) throws GatewayControllerException {
		
		AccessControlListIface acl = getAclProxy(sessionId);
		
		Log.d(TAG, "Deactivate ACL, objPath: '" + objectPath + "'");
		
		short respCode;
		try {
			respCode = acl.deactivateAcl();
		}
		catch (BusException be) {
			
			Log.e(TAG, "Failed to deactivate the ACL, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("Failed to deactivate the ACL", be);
		}
		
		AclResponseCode aclRespCode = getAclResponseCode(respCode);
		Log.d(TAG, "ACL deactivation response code: '" + aclRespCode + "', objPath: '" + objectPath + "'");
		
		if ( aclRespCode == AclResponseCode.GW_ACL_RC_SUCCESS ) {
			
			status = AclStatus.GW_AS_INACTIVE;
		}
		
		return aclRespCode;
	}
	
	/**
	 * Sends request to update Access Control List with the received {@link AccessRules}. 
	 * The {@link AccessRules} are validated against the provided {@link ManifestRules}.
	 * Only valid rules will be sent to update the ACL. The invalid rules are returned in the 
	 * {@link AclWriteResponse} object. <br>
	 * Before calling this method call 
	 * {@link AccessControlList#retrieveAcl(int, ManifestRules)} to create {@link AccessRules} that will
	 * update the ACL.
	 * @param sessionId The id of the session established with the gateway
	 * @param accessRules The ACL access rules
	 * @param manifestRules {@link ManifestRules} for validation of the received accessRules.
	 * The {@link ManifestRules} to be retrieved by the call to{@link ConnectorApplication#retrieveManifestRules(int)}
	 * @return {@link AclWriteResponse}
	 * @throws GatewayControllerException if failed to send request to update the ACL
	 * @throws IllegalArgumentException is thrown if bad arguments have been received
	 */
	public AclWriteResponse updateAcl(int sessionId, AccessRules accessRules, ManifestRules manifestRules) 
	                                 throws GatewayControllerException {
		
		if ( accessRules == null ) {
			throw new IllegalArgumentException("accessRules is undefined");
		}
		
		if ( manifestRules == null ) {
			throw new IllegalArgumentException("manifestRules is undefined");
		}
		
		Log.d(TAG, "Updating ACL name: '" + aclName + "', objPath: '" + objectPath + "'");
		
		//Check the AclMetadata
		if ( aclMetadata == null ) {
			
			Log.w(TAG, "The ACL internal metadata is undefined, looks like the retrieveAcl() method hasn't been called");
			aclMetadata = new HashMap<String, String>();
		}
		
		List<ManifestObjectDescriptionAJ> exposedServices = new ArrayList<ManifestObjectDescriptionAJ>();
		List<RemotedAppAJ> remotedApps                    = new ArrayList<RemotedAppAJ>();
		
		AccessRules invalidRules = AccessControlList.marshalAccessRules(accessRules,
				                                                        manifestRules,
				                                                        exposedServices, 
				                                                        remotedApps,
				                                                        aclMetadata);
		
		ManifestObjectDescriptionAJ[] exposedServicesArr = new ManifestObjectDescriptionAJ[exposedServices.size()];
		RemotedAppAJ[] remotedAppsArr                    = new RemotedAppAJ[remotedApps.size()];
		
		exposedServices.toArray(exposedServicesArr);
		remotedApps.toArray(remotedAppsArr);
		

		AccessControlListIface acl = getAclProxy(sessionId);
		short updateStatus;
		
		
		Map<String, String> customMetadata = accessRules.getMetadata();
		if ( customMetadata == null ) {
			customMetadata = new HashMap<String, String>();
		}
		
		
		try {
			updateStatus = acl.updateAcl(aclName, exposedServicesArr, remotedAppsArr, aclMetadata, customMetadata);
		}
		catch (BusException be) {
			Log.e(TAG, "Failed to update the ACL, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("ACL update failed", be);
		}
		
		AclResponseCode aclRespCode = getAclResponseCode(updateStatus);
		
		return new AclWriteResponse(id, aclRespCode, invalidRules, objectPath);
	}
	
	/**
	 * Updates metadata of the Access Control List. The ACL metadata is rewritten following the
	 * request.
	 * @param sessionId The id of the session established with the gateway
	 * @param metadata The metadata to update the ACL
	 * @return {@link AclResponseCode}
	 * @throws GatewayControllerException if failed to update metadata
	 * @throws IllegalArgumentException if undefined metadata has been received
	 */
	public AclResponseCode updateMetadata(int sessionId, Map<String, String> metadata) throws GatewayControllerException {
			
		if ( metadata == null ) {
			throw new IllegalArgumentException("metadata is undefined");
		}
		
		AccessControlListIface acl = getAclProxy(sessionId);
		
		Log.d(TAG, "Updata ACL custom, objPath: '" + objectPath + "'");
		
		short respCode;
		try {
			respCode = acl.updateAclCustomMetaData(metadata);
		}
		catch (BusException be) {
			
			Log.e(TAG, "Failed to update ACL custom metadata, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("Failed to update ACL custom metadata", be);
		}
		
		return getAclResponseCode(respCode);
	
	}
	
	/**
	 * Refreshes the {@link AccessControlList} object and returns its {@link AccessRules} 
	 * @param sessionId The id of the session established with the gateway
	 * @param manifestRules {@link ManifestRules} that is used for the {@link AccessRules} creation.
	 * The content of the {@link ManifestRules} object changes while executing this method. 
	 * It's strongly recommended to retrieve a fresh copy of the {@link ManifestRules} by invoking the
	 * {@link ConnectorApplication#retrieveManifestRules(int)} for a further usage.
	 * @return {@link AccessRules}.
	 * execution of this method
	 * @throws GatewayControllerException if failed to retrieve the rules
	 * @throws IllegalArgumentException if undefined manifestRules has been received
	 */
	public AccessRules retrieveAcl(int sessionId, ManifestRules manifestRules) throws GatewayControllerException {
		
		if ( manifestRules == null ) {
			throw new IllegalArgumentException("manifestRules is undefined");
		}
		
		Log.d(TAG, "Retrieve ACL, first retrieve the ACL status, then the AccessRules, objectPath: '" + objectPath + "'");
		
		//Update the ACL status
		retrieveStatus(sessionId);
		
		AccessControlListIface acl = getAclProxy(sessionId);
		AccessControlListAJ aclData;
		
		try {
			aclData = acl.getAcl();
		}
		catch (BusException be) {
			Log.e(TAG, "Failed to retrieve the ACL, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("Failed to retrieve the ACL", be);
		}
		
		//Update the ACL name
		aclName      = aclData.aclName;
		aclMetadata  = aclData.internalMetadata;
		
		List<ManifestObjectDescription> expServices     = manifestRules.getExposedServices();
		List<ManifestObjectDescription> remServices     = manifestRules.getRemotedServices();
		ManifObjDescComparator comparator               = new ManifObjDescComparator();
		Collections.sort(expServices, comparator);
		Collections.sort(remServices, comparator);
		
		List<ManifestObjectDescription> exposedServices = convertExposedServices(aclData.exposedServices,
				                                                                 expServices);
		
		List<RemotedApp> remotedApps = new ArrayList<RemotedApp>();
		boolean updateMetadata       =  convertRemotedApps(aclData.remotedApps,
				                                           remotedApps,
				                                           remServices);
		
		if ( updateMetadata ) {
			
			try {
				acl.updateAclMetaData(aclMetadata);
			}
			catch(BusException be) {
				Log.e(TAG, "Failed to update the ACL metadata, objPath: '" + objectPath + "'", be);
			}
		}
		
		AccessRules retRule = new AccessRules(exposedServices, remotedApps);
		retRule.setMetadata(aclData.customMetadata);
		return retRule;
	}
	
	/**
	 * Retrieve from the gateway status of the Access Control List
	 * @param sessionId The id of the session established with the gateway
	 * @return {@link AclStatus}
	 * @throws GatewayControllerException if failed to retrieve the ACL status
	 */
	public AclStatus retrieveStatus(int sessionId) throws GatewayControllerException {
	
		AccessControlListIface acl = getAclProxy(sessionId);
		
		Log.d(TAG, "Retrieve ACL status, objPath: '" + objectPath + "'");
		
		short respCode;
		try {
			respCode = acl.getAclStatus();
		}
		catch (BusException be) {
			
			Log.e(TAG, "Failed to retrieve the ACL status, objPath: '" + objectPath + "'");
			throw new GatewayControllerException("Failed to retrieve the ACL status", be);
		}
		
		AclStatus aclStatus = CommunicationUtil.shortToEnum(AclStatus.class, respCode);
		
		if ( aclStatus == null ) {
			
			Log.e(TAG, "Received unrecognized ACL status: '" + respCode + "'");
			throw new GatewayControllerException("Received unrecognized ACL status: '" + respCode + "'");
		}
		
		this.status = aclStatus;
		
		return aclStatus;
	}
	
	//===================================================//
	
	/**
	 * Returns {@link ProxyBusObject} of the {@link AccessControlListIface} interface
	 * @param sid The id of the session established with the gateway
	 * @return {@link AccessControlListIface} 
	 */
	private AccessControlListIface getAclProxy(int sid) {
		
		BusAttachment bus    = GatewayController.getInstance().getBusAttachment();
		
		ProxyBusObject proxy = bus.getProxyBusObject(gwBusName, objectPath, sid, 
				                         new Class<?>[]{AccessControlListIface.class});
		
		return proxy.getInterface(AccessControlListIface.class);
	}
	
	/**
	 * Converts from received code the {@link AclResponseCode}
	 * @param code get {@link AclResponseCode} code value from this code
	 * @return {@link AclResponseCode}
	 * @throws GatewayControllerException If received an unrecognized {@link AclResponseCode}
	 */
	private AclResponseCode getAclResponseCode(short code) throws GatewayControllerException {
		
		AclResponseCode respCode = CommunicationUtil.shortToEnum(AclResponseCode.class, code);
		
		if ( respCode == null ) {
			Log.e(TAG, "Received unrecognized ACL Response Code: '" + code + "'");
			throw new GatewayControllerException("Received unrecognized ACL Response Code: '" + code + "'");
		}
		
		return respCode;
	}
	
	/**
	 * Gets exposed services of the ACL, intersects it with the manifest exposed services in order to create
	 * the exposed services of the {@link AccessRules}. In addition adds the exposed services rules 
	 * that haven't configured yet.
	 * @param aclExpServicesAJ Exposed services retrieved from the ACL
	 * @param manifExpServices Manifest exposed services
	 * @return List of {@link ManifestObjectDescription} of the exposed services
	 */
	private List<ManifestObjectDescription> convertExposedServices(ManifestObjectDescriptionAJ[] aclExpServicesAJ, 
			                                            List<ManifestObjectDescription> manifExpServices) {
	
		Map<ConnAppObjectPath, Set<ConnAppInterface>> usedManRules = new HashMap<ConnAppObjectPath, Set<ConnAppInterface>>();
		List<ManifestObjectDescription> aclExpServices   = convertObjectDescription(aclExpServicesAJ, 
				                                                                    manifExpServices, 
				                                                                    usedManRules);
		
		//Find out the manifest exposed services rules that weren't used
		for ( ManifestObjectDescription manifExpSrvc : manifExpServices ) {
			
			ConnAppObjectPath manop          = manifExpSrvc.getObjectPath();
			Set<ConnAppInterface> manifs     = manifExpSrvc.getInterfaces();
			
			Set<ConnAppInterface> usedIfaces = usedManRules.get(manop);
			ConnAppObjectPath storeOp        = new ConnAppObjectPath(manop.getPath(), manop.getFriendlyName(), false,
                                                           manop.isPrefixAllowed());
			
			//Check if this rule was NOT used then add it to the resExpServices
			if ( usedIfaces == null ) {
				
				aclExpServices.add( new ManifestObjectDescription(storeOp, manifs, false) );
				continue;
			}
			
			//Remove from the manifest interfaces the interfaces that have been used
			manifs.removeAll(usedIfaces);
			
			//Add to the resExpServices the object path and the interfaces that weren't used
			if ( manifs.size() > 0 ) {
				aclExpServices.add( new ManifestObjectDescription(storeOp, manifs, false) );
			}
		}
		
		return aclExpServices;
	}//convertExposedServices

	/**
	 * Fills the received list of {@link RemotedApp}s from the array of {@link RemotedAppAJ}.
	 * The appName and deviceName that are required for creating the {@link RemotedApp} object are taken
	 * from the internal metadata or the {@link AnnouncementData}. If the {@link AnnouncementData} has appName or deviceName
	 * that are different from the metadata values, the metadata is updated. The object description rules of the 
	 * created {@link RemotedApp} are completed from the rules which are returned by the
	 * {@link ConnectorApplication#extractRemotedApp(List, AnnouncementData)}
	 * @param remotedAppsAJ The source for filling the remotedApps list
	 * @param remotedApps The list to be filled
	 * @param remotedServices The manifest data that is required for creation the {@link RemotedApp}
	 * @return TRUE if the received metadata was updated
	 */
	private boolean convertRemotedApps(RemotedAppAJ[] remotedAppsAJ, List<RemotedApp> remotedApps,
			                              List<ManifestObjectDescription> remotedServices) {
		
		//Gets TRUE if the metadata needs to be updated
		boolean updatedMeta               = false;
		List<RemotedApp> configurableApps = ConnectorApplication.extractRemotedApps(remotedServices);
		
		//Iterate over the remoted apps 
		for (RemotedAppAJ remAppAJ : remotedAppsAJ ) {
			
			//Retrieve announcement data to check whether the aclRemApps should be completed 
			UUID appId = TransportUtil.byteArrayToUUID(remAppAJ.appId);
			if ( appId == null ) {
				Log.e(TAG, "retrieveRemotedApps - remotedApp with a bad appId has been received, objPath: '" + objectPath + "'");
				continue;
			}
			
			//Convert the acl remoted app object descriptions to the list of ManifestObjectDescriptions
			//by intersecting with the manifest data.
			List<ManifestObjectDescription> configuredRules = convertObjectDescription(remAppAJ.objDescs,
					                                                                   remotedServices,
					                                                                   new HashMap<ConnAppObjectPath, Set<ConnAppInterface>>());
			
			//Construct the standard deviceId_appId key
			String key = CommunicationUtil.getKey(remAppAJ.deviceId, appId);
			
			int confRulesSize = configuredRules.size();
			Log.v(TAG, "retrieveRemotedApps - Created ObjDesc rules of the remoted app: '" + key + "' rules size: '"
			             + confRulesSize + "', objPath: '" + objectPath  + "'");
			
			//Retrieve appName and deviceName from the metadata
			boolean findMeta      = true;  
			String deviceNameMeta = aclMetadata.get(key + DEVICE_NAME_SUFFIX);
			String appNameMeta    = aclMetadata.get(key + APP_NAME_SUFFIX);
			
			if ( deviceNameMeta == null || deviceNameMeta.length() == 0 || 
					appNameMeta == null || appNameMeta.length() == 0 ) {
				
				Log.e(TAG, "retrieveRemotedApps - metadata is corrupted!!!. deviceName or appName weren't found, "
						   + "objPath: '" + objectPath + "'");
				
				findMeta = false;
			}

			//Look for the configurable RemotedApp from intersection of the manifest 
			//with announcement data
			RemotedApp configurableApp = getRemotedApp(configurableApps, remAppAJ.deviceId, appId);
			
			//If there is no configurableApp, but aclMetadata has appName and deviceName to construct the RemotedApp object 
			//and the acl configuredRules were created successfully, then create the RemotedApp object
			if ( configurableApp == null ) {
				
				Log.v(TAG, "retrieveRemotedApps - not found any ConfigurableApp for the remoted app: '" + key + "', objPath: '" + 
				              objectPath + "'");
				
				 if  ( findMeta && confRulesSize > 0 ) {
					 
					 Log.v(TAG, "retrieveRemotedApps - metadata has the required values, creating the remoted app");
					 //Create RemotedApp
					 remotedApps.add( new RemotedApp(appNameMeta, appId, deviceNameMeta, remAppAJ.deviceId, configuredRules) );
				 }
			}
			else {   //There is configurableApp
				
				Log.v(TAG, "retrieveRemotedApps - found announcement for the remoted app: '" + key + "', objPath: '" + objectPath + "'");
				
				if ( metadataUpdated(deviceNameMeta, appNameMeta, configurableApp, key) ) {
					updatedMeta = true;
				}
				
				//Completes already configured rules with rules that haven't configured yet
				addUnconfiguredRemotedAppRules(configurableApp.getObjDescRules(), configuredRules);
				
				if ( configuredRules.size() > 0 ) {
					remotedApps.add( new RemotedApp(configurableApp, configuredRules) );
				}
				
			}//if :: annData != null 
			
		}//for :: remotedApp
		
		//Add to the configured remotedApps the unconfigured remoted apps.
		//These apps remained in the configurableApps after working the algorithm above
		remotedApps.addAll(configurableApps);
		
		return updatedMeta;
	}

	/**
	 * Search for the {@link RemotedApp} in the given list of the remotedApps with the given
	 * deviceId and appId. If the {@link RemotedApp} is found it's removed from the remotedApps
	 * and is returned  
	 * @param remotedApps To look for the {@link RemotedApp}
	 * @param deviceId
	 * @param appId
	 * @return {@link RemotedApp} if found or NULL if NOT
	 */
	private RemotedApp getRemotedApp(List<RemotedApp> remotedApps, String deviceId, UUID appId) {
		
		RemotedApp retApp         = null;
		Iterator<RemotedApp> iter = remotedApps.iterator();
		
		while ( iter.hasNext() ) {
			
			RemotedApp currApp = iter.next();
			if ( currApp.getDeviceId().equals(deviceId) && currApp.getAppId().equals(appId) ) {
				
				iter.remove();
				retApp = currApp;
				break;
			}
		}
		
		return retApp;
	}
	

	/**
	 * Check whether the deviceNameMeta and appNameMeta are equal to the annApp,
	 * if not return TRUE
	 * @param deviceNameMeta
	 * @param appNameMeta
	 * @param annApp
	 * @param key metadata prefix key
	 * @return TRUE if the metadata needs to be updated
	 */
	private boolean metadataUpdated(String deviceNameMeta, String appNameMeta, RemotedApp annApp, String key) {
		
		boolean updatedMeta      = false;
		
		String annAppName        = annApp.getAppName();
		String annDeviceName     = annApp.getDeviceName();
		
		String appNameMetaKey    = key + APP_NAME_SUFFIX;
		String deviceNameMetaKey = key + DEVICE_NAME_SUFFIX;
		
		//Check appName, deviceName correctness vs. announcements
		if ( !annAppName.equals(appNameMeta) ) {
			
			Log.v(TAG, "retrieveRemotedApps - metaAppName is differ from the announcement app name, update " +
					     " the metadata with the app name: '" + annAppName + "', objPath: '" + objectPath + "'");
			
			aclMetadata.put(appNameMetaKey, annAppName);
			updatedMeta = true;
		}
		
		if ( !annDeviceName.equals(deviceNameMeta) ) {
			
			Log.v(TAG, "retrieveRemotedApps - metaDeviceName is differ from the announcement device name, update " +
					    " the metadata with the device name: '" + annDeviceName + "', objPath: '" + objectPath + "'");
			
			aclMetadata.put(deviceNameMetaKey, annDeviceName);
			updatedMeta = true;
		}
		
		return updatedMeta;
	}
	
	
	/**
	 * Compares configured rules of the remoted apps with the unconfigured rules.
	 * Completes the configured rules with the rules that haven't configured yet.
	 * @param unconfRules
	 * @param confRules
	 */
	private void addUnconfiguredRemotedAppRules(List<ManifestObjectDescription> unconfRules, List<ManifestObjectDescription> confRules) {
			
		for (ManifestObjectDescription unconfRule : unconfRules ) {
			
			ConnAppObjectPath unconfOP          = unconfRule.getObjectPath();
			Set<ConnAppInterface> unconfIfaces  = unconfRule.getInterfaces();
			
			//Gets TRUE if unconfOP was found among the confRules
			boolean unconfOpInConf = false;
						
			for (ManifestObjectDescription confRule : confRules) {
				
				ConnAppObjectPath confOP         = confRule.getObjectPath();
				Set<ConnAppInterface> confIfaces = confRule.getInterfaces();
				
				//Check if the unconfOP NOT equals confOP 
				if ( !unconfOP.getPath().equals(confOP.getPath()) ) {
					continue;
				}
				
				unconfOpInConf = true;
				unconfIfaces.removeAll(confIfaces);
				break;
			}
			
			if ( !unconfOpInConf || unconfIfaces.size() > 0 ) {
				confRules.add( new ManifestObjectDescription(unconfOP, unconfIfaces, false) );
			}
			
		}//for::unconfRule
	}
	
		
	/**
	 * Converts {@link ManifestObjectDescriptionAJ} array in to {@link ManifestObjectDescription} list. The converted rules
	 * are validated against the received manifest rules. Manifest rules that were used for the validation and 
	 * the {@link ManifestObjectDescription}  construction are stored in the given usedManRules map. 
	 * Created {@link ManifestObjectDescription} rules are marked as configured. <br>
	 * Important, for the correct work of this algorithm the manifest list must be sorted with the
	 * {@link ManifObjDescComparator}.
	 * @param objDescsAJ to be converted
	 * @param manifest manifest rules that are used for the validation and {@link ManifestObjectDescription} construction
	 * @param usedManRules manifest rules that were used for validation and the {@link ManifestObjectDescription} construction
	 * @return {@link ManifestObjectDescription} list converted from the {@link ManifestObjectDescriptionAJ} array
	 */
	private List<ManifestObjectDescription> convertObjectDescription(ManifestObjectDescriptionAJ[] objDescsAJ,
			                      List<ManifestObjectDescription> manifest, Map<ConnAppObjectPath, Set<ConnAppInterface>> usedManRules ) {
		
		Map<ConnAppObjectPath, Set<ConnAppInterface>> resRules = new HashMap<ConnAppObjectPath, Set<ConnAppInterface>>();
 		
		for (ManifestObjectDescriptionAJ objDescAJ : objDescsAJ) {
			
			List<String> ifacesToConvert = new ArrayList<String>( Arrays.asList(objDescAJ.interfaces) );
			
			for (ManifestObjectDescription manifRule : manifest ) {
				
				ConnAppObjectPath manop      = manifRule.getObjectPath();
				Set<ConnAppInterface> manifs = manifRule.getInterfaces();
				int manifsSize          = manifs.size();
				
				if ( !isValidObjPath(manop, objDescAJ.objectPath, objDescAJ.isPrefix) ) {
					continue;
				}
				
				ConnAppObjectPath resObjPath;
				
				if ( manop.getPath().equals(objDescAJ.objectPath) ) {
					resObjPath = new ConnAppObjectPath(objDescAJ.objectPath, manop.getFriendlyName(), objDescAJ.isPrefix, 
							                      manop.isPrefixAllowed());
				}
				else {
					resObjPath = new ConnAppObjectPath(objDescAJ.objectPath, "", objDescAJ.isPrefix, 
							                      manop.isPrefixAllowed());
				}
				
				//Add used manifest rules for the empty manifest interfaces array
				if ( manifsSize == 0 ) {
					
					Set<ConnAppInterface> usedIfaces = usedManRules.get(manop);
					if ( usedIfaces == null ) {
						usedManRules.put(manop, new HashSet<ConnAppInterface>());
					}
				}
				
				Iterator<String> ifacesToConvertIter = ifacesToConvert.iterator();
				Set<ConnAppInterface> resInterfaces       = new HashSet<ConnAppInterface>();
				
				//Validate interfaces
				while ( ifacesToConvertIter.hasNext() ) {
					
					String ajIface = ifacesToConvertIter.next();
					
					//If there are not interfaces in the manifest, it means that all the interfaces are supported
					//add them without display names
					if ( manifsSize == 0 ) {
						resInterfaces.add( new ConnAppInterface(ajIface, "") );
						ifacesToConvertIter.remove();
						continue;
					}
					
					for (ConnAppInterface manIface : manifs ) {
						
						//aclInterface is found in manifest
						if ( ajIface.equals(manIface.getName()) ) {
							
							resInterfaces.add( new ConnAppInterface(ajIface, manIface.getFriendlyName(), manIface.isSecured()) );
							ifacesToConvertIter.remove();
							break;
						}
					}//for :: manifest interfaces
				}//while :: aclInterfaces
				
				//Not found any matched interfaces, continue to the next manifest rule
				if ( resInterfaces.size() == 0 ) {
					continue;
				}
				
				//Add the interfaces to the resObjPath
				Set<ConnAppInterface> ifaces = resRules.get(resObjPath);
				if ( ifaces == null ) {
					resRules.put(resObjPath, resInterfaces);
				}
				else {
					
					//Merge interfaces
					ifaces.addAll(resInterfaces);
				}
				
				//Add used manifest rules
				if ( manifsSize > 0 ) {
					
					Set<ConnAppInterface> usedIfaces = usedManRules.get(manop);
					if ( usedIfaces == null ) {
						usedManRules.put(manop, new HashSet<ConnAppInterface>(resInterfaces));
					}
					else {
						usedIfaces.addAll(resInterfaces);
					}
				}
				
				//If all the objDescAJ interfaces have been handled, no need to continue iterating 
				//over the manifest rules
				if ( ifacesToConvert.size() == 0 ) {
					break;
				}
				
			}//for :: manifest
			
		}//for :: objDescsAJ
		
		//Create final list of the configured rules
		List<ManifestObjectDescription> rules = new ArrayList<ManifestObjectDescription>(resRules.size()); 
		for ( ConnAppObjectPath op : resRules.keySet() ) {
			rules.add( new ManifestObjectDescription(op,  resRules.get(op), true) );
		}
		
		return rules;
	}//convertObjectDescription
	
	/**
	 * Converts received {@link AccessRules} to the exposedServicesAJ and the remotedAppsAJ 
	 * in order to be sent to a Gateway Connector Application as a part of an acl creation or an update.
	 * Validates received {@link AccessRules} against the {@link ManifestRules}.
	 * Fills the exposedServicesAJ and the remotedAppsAJ with the valid rules.
	 * All the invalid rules are stored in the returned {@link AccessRules} object.
	 * @param accessRules The object to be validated and marshaled
	 * @param manifestRules Check the {@link AccessRules} validity against this {@link ManifestRules}
	 * @param exposedServicesAJ An empty list that will be populated with exposedServices rules 
	 * converted to the {@link ManifestObjectDescriptionAJ}
	 * @param remotedAppsAJ An empty list that will be populated with the remoted applications
	 * converted to the {@link RemotedAppsAJ}
	 * @param metadata The internal ACL metadata to be populated for the ACL creation or an update 
	 * @return {@link AccessRules} The rules that marked as invalid during testing against the {@link ManifestRules}
	 */
	static AccessRules marshalAccessRules(AccessRules accessRules, ManifestRules manifestRules, 
			        List<ManifestObjectDescriptionAJ> exposedServicesAJ, List<RemotedAppAJ> remotedAppsAJ,
			        Map<String, String> metadata) {
		
		//Marshal Exposed Services
		List<ManifestObjectDescription> invalidExpServices = marshalManifObjDescs(accessRules.getExposedServices(),
				                                                                  exposedServicesAJ, 
				                                                                  manifestRules.getExposedServices());
		
		//Marshal Remoted Apps
		List<RemotedApp> invalidRemotedApps = new ArrayList<RemotedApp>();
		
		for (RemotedApp rmApp : accessRules.getRemotedApps() ) {
			
			//This will be populated with the valid rules marshaled in to the ManifestObjectDescriptionAJ 
			List<ManifestObjectDescriptionAJ> marshalledRules  = new ArrayList<ManifestObjectDescriptionAJ>();
			List<ManifestObjectDescription> invalidRemAppRules = marshalManifObjDescs(rmApp.getObjDescRules(), 
					                                                                  marshalledRules,
					                                                                  manifestRules.getRemotedServices());
			
			//If there are invalid rules store it
			if ( invalidRemAppRules.size() > 0 ) {
				invalidRemotedApps.add( new RemotedApp(rmApp, invalidRemAppRules) ); 
			}
			
			int marshaledRulesSize = marshalledRules.size();
			
			//If there is no any marshaled rule, no valid rule was found -> continue 
			if ( marshaledRulesSize == 0 ) {
				continue;
			}
			
			//Populate the RemotedAppAJ
			RemotedAppAJ remotedAppAJ = new RemotedAppAJ();
			remotedAppAJ.deviceId     = rmApp.getDeviceId(); 
			remotedAppAJ.appId        = TransportUtil.uuidToByteArray(rmApp.getAppId());
			remotedAppAJ.objDescs     = marshalledRules.toArray( new ManifestObjectDescriptionAJ[marshaledRulesSize] );
			
			remotedAppsAJ.add(remotedAppAJ);
			
			//Store this application data in the metadata
			String key = CommunicationUtil.getKey(rmApp.getDeviceId(), rmApp.getAppId());
			metadata.put(key + DEVICE_NAME_SUFFIX, rmApp.getDeviceName());
			metadata.put(key + APP_NAME_SUFFIX, rmApp.getAppName());
		}
		
		//Return the AccessRules object with invalid rules
		return new AccessRules(invalidExpServices, invalidRemotedApps);
	}//marshalAccessRules
	
	/**
	 * Marshals received {@link ManifestObjectDescription} list to the list of {@link ManifestObjectDescriptionAJ} 
	 * which is used for ACL creation or an update. Rules validity is checked before being marshaled.
	 * The list of the invalid rules is returned by this method.
	 * @param toMarshal The list of {@link ManifestObjectDescription} rules to be marshaled
	 * @param target The list of {@link ManifestObjectDescriptionAJ} that is populated with the valid rules
	 * from the "toMarshal" list
	 * @param manifestRules Check validation of the "toMarshal" rules against the list of this manifest rules
	 * @return List of an invalid rules. The rules that weren't found in the manifest rules
	 */
	private static List<ManifestObjectDescription> marshalManifObjDescs(List<ManifestObjectDescription> toMarshal, 
			       List<ManifestObjectDescriptionAJ> target, List<ManifestObjectDescription> manifestRules) {
		
		List<ManifestObjectDescription> invalidRules = new ArrayList<ManifestObjectDescription>();
		
		for (ManifestObjectDescription mod : toMarshal ) {
			
			Set<ConnAppInterface> invInterfaces = new HashSet<ConnAppInterface>();
			boolean isValid                = isValidRule(mod, invInterfaces, manifestRules);
			
			//If current ManifestObjectDescription is NOT valid it need to be added to the invalid rules
			//OR the ManifestObjectDescription could be valid but some of its interfaces are not
			if ( !isValid || invInterfaces.size() > 0 ) {
				invalidRules.add( new ManifestObjectDescription(mod.getObjectPath(), invInterfaces) );
			}
			
			if ( !isValid ) {
				continue;
			}
			
			//Marshal and add the valid rules to the target
			target.add( new ManifestObjectDescriptionAJ(mod) );
		}
		
		return invalidRules;
	}
	
	/**
	 * Checks that received toValidate rule complies with received manifestRules.
	 * The method removes from toValidate interfaces the interfaces that are not valid (not in the
	 * manifestRules interfaces). Not valid interfaces are added to the received notValid set. 
	 * @param toValidate {@link ManifestObjectDescription} rules to be validated
	 * @param notValid An empty Set that will be populated with  interfaces that don't comply 
	 * with the manifest interfaces
	 * @param manifestRules validate received toValidate rules against this list of the manifest rules
	 * @return Returns TRUE if there is at least one rule (interface) that complies with the manifest rules.
	 * FALSE means that toValidate rule doesn't comply with the manifest rules not by the object path
	 * and not by the interfaces. As a result of this method execution, toValidate will contain 
	 * only valid interfaces all the invalid interfaces will be moved to the notValid set.
	 */
	private static boolean isValidRule(ManifestObjectDescription toValidate, Set<ConnAppInterface> notValid,
			                   List<ManifestObjectDescription> manifestRules) {
		
		boolean validRuleFound       = false;
		Set<ConnAppInterface> validIfaces = toValidate.getInterfaces();
		
		notValid.addAll(validIfaces);
		validIfaces.clear();
		
		//If toValidate is not configured it considered as a not valid rule, that won't be sent to the gateway
		if ( !toValidate.isConfigured() ) {
			return false;
		}
		
		for ( ManifestObjectDescription mRule : manifestRules ) {
			
			ConnAppObjectPath manop      = mRule.getObjectPath();
			Set<ConnAppInterface> manifs = mRule.getInterfaces();
			
			//Check object path validity
			if ( isValidObjPath(manop, toValidate.getObjectPath().getPath(), toValidate.getObjectPath().isPrefix()) ) {
				
				//If the the list of the manifest interfaces is empty, it means that all the interfaces
				//of the toValidate object path are supported, so toValidate object is fully valid ==> return true
				if ( manifs.size() == 0 ) {
					
					validIfaces.addAll(notValid);
					notValid.clear();
					return true; 
				}
				
				//Validate interfaces
				Iterator<ConnAppInterface> notValidIter = notValid.iterator();
				while( notValidIter.hasNext() ) {
					
					ConnAppInterface ifaceToTest = notValidIter.next();
					
					if ( manifs.contains(ifaceToTest) ) { // Check if the interface is valid
						validRuleFound = true;
						notValidIter.remove();            // Remove the interface from notValid group
						validIfaces.add(ifaceToTest);     // Add the interface to the valid group
					}
				}
					
				//All the interfaces toValidate are valid
				if ( validRuleFound && notValid.size() == 0 ) {
					return true;
				}
				
			}//if :: objPath
			
		}//for :: manifestRules
		
		return validRuleFound;
	}//isValidRule

	/**
	 * Checks the object path validity against the manifest object path.
	 * @param manifOp Manifest object path
	 * @param toValidOP Object path to be validated
	 * @param isPrefix If toValidOP is prefix
	 * @return TRUE if toValidOP is valid
	 */
	private static boolean isValidObjPath(ConnAppObjectPath manifOp, String toValidOP, boolean isPrefix) {
		
		if ( manifOp.isPrefix() && toValidOP.startsWith(manifOp.getPath()) || 
				( !manifOp.isPrefix() && !isPrefix && manifOp.getPath().equals(toValidOP) ) ) {
			
			return true;
		}
		
		return false;
	}
}
