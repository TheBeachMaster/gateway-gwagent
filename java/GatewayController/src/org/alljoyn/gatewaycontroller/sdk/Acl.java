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
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleInterface;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleObjectPath;
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
 * The object contains access control configuration rules for a Gateway
 * Connector Application.
 */
public class Acl {
    private static final String TAG = "gwc" + Acl.class.getSimpleName();

    /**
     * Acl response code. This status is returned as a result of
     * actions applied on the Acl
     */
    public static enum AclResponseCode {

        GW_ACL_RC_SUCCESS("Success", (short) 0),
        GW_ACL_RC_INVALID("Invalid", (short) 1),
        GW_ACL_RC_REGISTER_ERROR("Register error", (short) 2),
        GW_ACL_RC_ACL_NOT_FOUND("ACL not found", (short) 3),
        GW_ACL_RC_PERSISTENCE_ERROR("ACL persistence error", (short) 4),
        GW_ACL_RC_POLICYMANAGER_ERROR("ACL policy manager error", (short) 5),
        GW_ACL_RC_METADATA_ERROR("Metadata error", (short) 6);

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
         *
         * @param desc
         *            Status description
         * @param code
         *            Status code
         */
        private AclResponseCode(String desc, short code) {

            DESC = desc;
            CODE = code;
        }
    }

    // ===========================================//

    /**
     * The current status of the Acl
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
         *
         * @param desc
         *            Status description
         * @param code
         *            Status code
         */
        private AclStatus(String desc, short code) {

            DESC = desc;
            CODE = code;
        }
    }

    // ===========================================//

    /**
     * ACL id
     */
    private final String id;

    /**
     * ACL name The ACL name may be updated
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
     * The name of the gateway {@link BusAttachment} a Gateway Connector
     * Application which is related to this ACL is installed on
     */
    private final String gwBusName;

    /**
     * The metadata for the SDK internal usage. The metadata is set by the call
     * to {@link Acl#retrieve(int, ConnectorCapabilities)}
     */
    private Map<String, String> aclMetadata;

    /**
     * Suffix of the device name that is sent with the ACL metadata
     */
    private static final String DEVICE_NAME_SUFFIX = "_DEVICE_NAME";

    /**
     * Suffix of the application name that is sent with the ACL metadata
     */
    private static final String APP_NAME_SUFFIX = "_APP_NAME";

    /**
     * Constructor
     *
     * @param gwBusName
     *            The name of the Gateway Management App {@link BusAttachment} hosting a
     *            Gateway Connector Application that is related to this ACL
     * @param id
     *            Id of the Acl
     * @param objectPath
     *            The object path of the Acl
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public Acl(String gwBusName, String id, String objectPath) {

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("gwBusName is undefined");
        }

        if (id == null || id.length() == 0) {
            throw new IllegalArgumentException("id is undefined");
        }

        if (objectPath == null || objectPath.length() == 0) {
            throw new IllegalArgumentException("objPath is undefined");
        }

        this.id         = id;
        this.objectPath = objectPath;
        this.gwBusName  = gwBusName;
    }

    /**
     * Constructor
     *
     * @param gwBusName
     *            The name of the Gateway Management App {@link BusAttachment} hosting a
     *            Gateway Connector Application that is related to this ACL
     * @param aclInfoAJ
     *            The ACL information
     * @throws GatewayControllerException
     *             if failed to initialize the object
     */
    Acl(String gwBusName, AclInfoAJ aclInfoAJ) throws GatewayControllerException {

        this.gwBusName      = gwBusName;
        this.aclName        = aclInfoAJ.aclName;
        this.id             = aclInfoAJ.aclId;
        this.objectPath     = aclInfoAJ.objectPath;

        AclStatus aclStatus = CommunicationUtil.shortToEnum(AclStatus.class, aclInfoAJ.aclStatus);
        if (aclStatus == null) {

            Log.e(TAG, "Received unrecognized ACL status: '" + aclInfoAJ.aclStatus + "', objPath: '" + this.objectPath + "'");

            throw new GatewayControllerException("Received unrecognized ACL status: '" + aclInfoAJ.aclStatus + "'");
        }

        this.status = aclStatus;
    }

    /**
     * @return The name of the Acl
     */
    public String getName() {
        return aclName;
    }

    /**
     * Set the name of the Acl
     *
     * @param name
     *            The Acl name
     * @throws IllegalArgumentException
     *             is thrown if undefined name has been received
     */
    public void setName(String name) {

        if (name == null) {
            throw new IllegalArgumentException("name is undefined");
        }

        this.aclName = name;
    }

    /**
     * @return Id of the Acl
     */
    public String getId() {
        return id;
    }

    /**
     * @return Object path of the Acl
     */
    public String getObjectPath() {
        return objectPath;
    }

    /**
     * @return The name of the Gateway Management App {@link BusAttachment} hosting a Gateway
     *         Connector Application that is related to this Acl
     */
    public String getGwBusName() {
        return gwBusName;
    }

    /**
     * Return the current state of the {@link Acl}
     *
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
          .append("gwBusName='").append(gwBusName)
          .append("', ").append("status='").append(status).append("'");

        return sb.toString();
    }

    // ===================================================//

    /**
     * Activate the Acl
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link AclResponseCode}
     * @throws GatewayControllerException
     *             if failed to activate the ACL
     */
    public AclResponseCode activate(int sessionId) throws GatewayControllerException {

        AccessControlListIface acl = getAclProxy(sessionId);

        Log.d(TAG, "Activate ACL, objPath: '" + objectPath + "'");

        short respCode;
        try {
            respCode = acl.activateAcl();
        } catch (BusException be) {

            Log.e(TAG, "Failed to activate the ACL, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to activate the ACL", be);
        }

        AclResponseCode aclRespCode = getAclResponseCode(respCode);
        Log.d(TAG, "ACL activation response code: '" + aclRespCode + "', objPath: '" + objectPath + "'");

        if (aclRespCode == AclResponseCode.GW_ACL_RC_SUCCESS) {

            status = AclStatus.GW_AS_ACTIVE;
        }

        return aclRespCode;
    }

    /**
     * Deactivate the Acl
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link AclResponseCode}
     * @throws GatewayControllerException
     *             if failed to deactivate the ACL
     */
    public AclResponseCode deactivate(int sessionId) throws GatewayControllerException {

        AccessControlListIface acl = getAclProxy(sessionId);

        Log.d(TAG, "Deactivate ACL, objPath: '" + objectPath + "'");

        short respCode;
        try {
            respCode = acl.deactivateAcl();
        } catch (BusException be) {

            Log.e(TAG, "Failed to deactivate the ACL, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to deactivate the ACL", be);
        }

        AclResponseCode aclRespCode = getAclResponseCode(respCode);
        Log.d(TAG, "ACL deactivation response code: '" + aclRespCode + "', objPath: '" + objectPath + "'");

        if (aclRespCode == AclResponseCode.GW_ACL_RC_SUCCESS) {

            status = AclStatus.GW_AS_INACTIVE;
        }

        return aclRespCode;
    }

    /**
     * Sends request to update Acl with the received
     * {@link AclRules}. The {@link AclRules} are validated against the
     * provided {@link ConnectorCapabilities}. Only valid rules will be sent to update
     * the ACL. The invalid rules are returned in the {@link AclWriteResponse}
     * object. <br>
     * Before calling this method call
     * {@link Acl#retrieve(int, ConnectorCapabilities)} to create
     * {@link AclRules} that will update the ACL.
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @param aclRules
     *            The ACL access rules
     * @param connectorCapabilities
     *            {@link ConnectorCapabilities} for validation of the received
     *            aclRules. The {@link ConnectorCapabilities} to be retrieved by the
     *            call to{@link ConnectorApp#retrieveConnectorCapabilities(int)}
     * @return {@link AclWriteResponse}
     * @throws GatewayControllerException
     *             if failed to send request to update the ACL
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public AclWriteResponse update(int sessionId, AclRules aclRules, ConnectorCapabilities connectorCapabilities)
                                          throws GatewayControllerException {

        if (aclRules == null) {
            throw new IllegalArgumentException("aclRules is undefined");
        }

        if (connectorCapabilities == null) {
            throw new IllegalArgumentException("connectorCapabilities is undefined");
        }

        Log.d(TAG, "Updating ACL name: '" + aclName + "', objPath: '" + objectPath + "'");

        // Check the AclMetadata
        if (aclMetadata == null) {

            Log.w(TAG, "The ACL internal metadata is undefined, looks like the retrieveAcl() method hasn't been called");
            aclMetadata = new HashMap<String, String>();
        }

        List<ManifestObjectDescriptionAJ> exposedServices = new ArrayList<ManifestObjectDescriptionAJ>();
        List<RemotedAppAJ> remotedApps                    = new ArrayList<RemotedAppAJ>();

        AclRules invalidRules = Acl.marshalAclRules(aclRules, connectorCapabilities, exposedServices, remotedApps,
                                                           aclMetadata);

        ManifestObjectDescriptionAJ[] exposedServicesArr = new ManifestObjectDescriptionAJ[exposedServices.size()];
        RemotedAppAJ[] remotedAppsArr                    = new RemotedAppAJ[remotedApps.size()];

        exposedServices.toArray(exposedServicesArr);
        remotedApps.toArray(remotedAppsArr);

        AccessControlListIface acl = getAclProxy(sessionId);
        short updateStatus;

        Map<String, String> customMetadata = aclRules.getMetadata();
        if (customMetadata == null) {
            customMetadata = new HashMap<String, String>();
        }

        try {
            updateStatus = acl.updateAcl(aclName, exposedServicesArr, remotedAppsArr, aclMetadata, customMetadata);
        } catch (BusException be) {
            Log.e(TAG, "Failed to update the ACL, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("ACL update failed", be);
        }

        AclResponseCode aclRespCode = getAclResponseCode(updateStatus);

        return new AclWriteResponse(id, aclRespCode, invalidRules, objectPath);
    }

    /**
     * Updates metadata of the Acl. The ACL metadata is
     * rewritten following the request.
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @param metadata
     *            The metadata to update the ACL
     * @return {@link AclResponseCode}
     * @throws GatewayControllerException
     *             if failed to update metadata
     * @throws IllegalArgumentException
     *             if undefined metadata has been received
     */
    public AclResponseCode updateMetadata(int sessionId, Map<String, String> metadata) throws GatewayControllerException {

        if (metadata == null) {
            throw new IllegalArgumentException("metadata is undefined");
        }

        AccessControlListIface acl = getAclProxy(sessionId);

        Log.d(TAG, "Updata ACL custom, objPath: '" + objectPath + "'");

        short respCode;
        try {
            respCode = acl.updateAclCustomMetadata(metadata);
        } catch (BusException be) {

            Log.e(TAG, "Failed to update ACL custom metadata, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to update ACL custom metadata", be);
        }

        return getAclResponseCode(respCode);

    }

    /**
     * Refreshes the {@link Acl} object and returns its
     * {@link AclRules}
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @param connectorCapabilities
     *            {@link ConnectorCapabilities} that is used for the {@link AclRules}
     *            creation. The content of the {@link ConnectorCapabilities} object
     *            changes while executing this method. It's strongly recommended
     *            to retrieve a fresh copy of the {@link ConnectorCapabilities} by
     *            invoking the
     *            {@link ConnectorApp#retrieveConnectorCapabilities(int)} for a
     *            further usage.
     * @return {@link AclRules}.
     * @throws GatewayControllerException
     *             if failed to retrieve the rules
     * @throws IllegalArgumentException
     *             if undefined connectorCapabilities has been received
     */
    public AclRules retrieve(int sessionId, ConnectorCapabilities connectorCapabilities) throws GatewayControllerException {

        if (connectorCapabilities == null) {
            throw new IllegalArgumentException("connectorCapabilities is undefined");
        }

        Log.d(TAG, "Retrieve ACL, first retrieve the ACL status, then the AclRules, objectPath: '" + objectPath + "'");

        // Update the ACL status
        retrieveStatus(sessionId);

        AccessControlListIface acl = getAclProxy(sessionId);
        AccessControlListAJ aclData;

        try {
            aclData = acl.getAcl();
        } catch (BusException be) {
            Log.e(TAG, "Failed to retrieve the ACL, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the ACL", be);
        }

        // Update the ACL name
        aclName     = aclData.aclName;
        aclMetadata = aclData.internalMetadata;

        List<RuleObjectDescription> expServices = connectorCapabilities.getExposedServices();
        List<RuleObjectDescription> remServices = connectorCapabilities.getRemotedServices();
        RuleObjectDescriptionComparator comparator = new RuleObjectDescriptionComparator();
        Collections.sort(expServices, comparator);
        Collections.sort(remServices, comparator);

        List<RuleObjectDescription> exposedServices = convertExposedServices(aclData.exposedServices, expServices);

        List<RemotedApp> remotedApps = new ArrayList<RemotedApp>();
        boolean updateMetadata = convertRemotedApps(aclData.remotedApps, remotedApps, remServices);

        if (updateMetadata) {

            try {
                acl.updateAclMetadata(aclMetadata);
            } catch (BusException be) {
                Log.e(TAG, "Failed to update the ACL metadata, objPath: '" + objectPath + "'", be);
            }
        }

        AclRules retRule = new AclRules(exposedServices, remotedApps);
        retRule.setMetadata(aclData.customMetadata);
        return retRule;
    }

    /**
     * Retrieve from the gateway status of the Acl
     *
     * @param sessionId
     *            The id of the session established with the gateway
     * @return {@link AclStatus}
     * @throws GatewayControllerException
     *             if failed to retrieve the ACL status
     */
    public AclStatus retrieveStatus(int sessionId) throws GatewayControllerException {

        AccessControlListIface acl = getAclProxy(sessionId);

        Log.d(TAG, "Retrieve ACL status, objPath: '" + objectPath + "'");

        short respCode;
        try {
            respCode = acl.getAclStatus();
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve the ACL status, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the ACL status", be);
        }

        AclStatus aclStatus = CommunicationUtil.shortToEnum(AclStatus.class, respCode);

        if (aclStatus == null) {

            Log.e(TAG, "Received unrecognized ACL status: '" + respCode + "'");
            throw new GatewayControllerException("Received unrecognized ACL status: '" + respCode + "'");
        }

        this.status = aclStatus;

        return aclStatus;
    }

    // ===================================================//

    /**
     * Returns {@link ProxyBusObject} of the {@link AccessControlListIface} interface
     *
     * @param sid
     *           The id of the session established with the Gateway Management App
     * @return {@link AccessControlListIface}
     */
    private AccessControlListIface getAclProxy(int sid) {

        BusAttachment bus    = GatewayController.getInstance().getBusAttachment();
        ProxyBusObject proxy = bus.getProxyBusObject(gwBusName, objectPath, sid, new Class<?>[] { AccessControlListIface.class });

        return proxy.getInterface(AccessControlListIface.class);
    }

    /**
     * Converts from received code the {@link AclResponseCode}
     *
     * @param code
     *            get {@link AclResponseCode} code value from this code
     * @return {@link AclResponseCode}
     * @throws GatewayControllerException
     *             If received an unrecognized {@link AclResponseCode}
     */
    private AclResponseCode getAclResponseCode(short code) throws GatewayControllerException {

        AclResponseCode respCode = CommunicationUtil.shortToEnum(AclResponseCode.class, code);

        if (respCode == null) {
            Log.e(TAG, "Received unrecognized ACL Response Code: '" + code + "'");
            throw new GatewayControllerException("Received unrecognized ACL Response Code: '" + code + "'");
        }

        return respCode;
    }

    /**
     * Gets exposed services of the ACL, intersects it with the manifest exposed
     * services in order to create the exposed services of the
     * {@link AclRules}. In addition adds the exposed services rules that
     * haven't configured yet.
     *
     * @param aclExpServicesAJ
     *            Exposed services retrieved from the ACL
     * @param manifExpServices
     *            Manifest exposed services
     * @return List of {@link RuleObjectDescription} of the exposed services
     */
    private List<RuleObjectDescription> convertExposedServices(ManifestObjectDescriptionAJ[] aclExpServicesAJ,
                                                                   List<RuleObjectDescription> manifExpServices) {

        Map<RuleObjectPath, Set<RuleInterface>> usedManRules = new HashMap<RuleObjectPath, Set<RuleInterface>>();
        List<RuleObjectDescription> aclExpServices = convertObjectDescription(aclExpServicesAJ, manifExpServices, usedManRules);

        // Find out the manifest exposed services rules that weren't used
        for (RuleObjectDescription manifExpSrvc : manifExpServices) {

            RuleObjectPath manop = manifExpSrvc.getObjectPath();
            Set<RuleInterface> manifs = manifExpSrvc.getInterfaces();

            Set<RuleInterface> usedIfaces = usedManRules.get(manop);
            RuleObjectPath storeOp = new RuleObjectPath(manop.getPath(), manop.getFriendlyName(), false, manop.isPrefixAllowed());

            // Check if this rule was NOT used then add it to the resExpServices
            if (usedIfaces == null) {

                aclExpServices.add(new RuleObjectDescription(storeOp, manifs, false));
                continue;
            }

            // Remove from the manifest interfaces the interfaces that have been used
            manifs.removeAll(usedIfaces);

            // Add to the resExpServices the object path and the interfaces that weren't used
            if (manifs.size() > 0) {
                aclExpServices.add(new RuleObjectDescription(storeOp, manifs, false));
            }
        }

        return aclExpServices;
    }// convertExposedServices

    /**
     * Fills the received list of {@link RemotedApp}s from the array of
     * {@link RemotedAppAJ}. The appName and deviceName that are required for
     * creating the {@link RemotedApp} object are taken from the internal
     * metadata or the {@link AnnouncementData}. If the {@link AnnouncementData}
     * has appName or deviceName that are different from the metadata values,
     * the metadata is updated. The object description rules of the created
     * {@link RemotedApp} are completed from the rules which are returned by the
     * {@link ConnectorApp#extractRemotedApp(List, AnnouncementData)}
     *
     * @param remotedAppsAJ
     *            The source for filling the remotedApps list
     * @param remotedApps
     *            The list to be filled
     * @param remotedServices
     *            The manifest data that is required for creation the
     *            {@link RemotedApp}
     * @return TRUE if the received metadata was updated
     */
    private boolean convertRemotedApps(RemotedAppAJ[] remotedAppsAJ, List<RemotedApp> remotedApps,
                                           List<RuleObjectDescription> remotedServices) {

        // Gets TRUE if the metadata needs to be updated
        boolean updatedMeta               = false;
        List<RemotedApp> configurableApps = ConnectorApp.extractRemotedApps(remotedServices);

        // Iterate over the remoted apps
        for (RemotedAppAJ remAppAJ : remotedAppsAJ) {

            // Retrieve announcement data to check whether the aclRemApps should be completed
            UUID appId = TransportUtil.byteArrayToUUID(remAppAJ.appId);
            if (appId == null) {
                Log.e(TAG, "retrieveRemotedApps - remotedApp with a bad appId has been received, objPath: '" + objectPath + "'");
                continue;
            }

            // Convert the acl remoted app object descriptions to the list of RuleObjectDescriptions
            // by intersecting with the ConnectorCapabilities.
            List<RuleObjectDescription> configuredRules = convertObjectDescription(remAppAJ.objDescs, remotedServices,
                                                                        new HashMap<RuleObjectPath, Set<RuleInterface>>());

            // Construct the standard deviceId_appId key
            String key = CommunicationUtil.getKey(remAppAJ.deviceId, appId);

            int confRulesSize = configuredRules.size();
            Log.v(TAG, "retrieveRemotedApps - Created ObjDesc rules of the remoted app: '" + key + "' rules size: '" + confRulesSize +
                            "', objPath: '" + objectPath + "'");

            // Retrieve appName and deviceName from the metadata
            boolean findMeta      = true;
            String deviceNameMeta = aclMetadata.get(key + DEVICE_NAME_SUFFIX);
            String appNameMeta    = aclMetadata.get(key + APP_NAME_SUFFIX);

            if (deviceNameMeta == null || deviceNameMeta.length() == 0 || appNameMeta == null || appNameMeta.length() == 0) {

                Log.e(TAG, "retrieveRemotedApps - metadata is corrupted!!!. deviceName or appName weren't found, " + "objPath: '" +
                               objectPath + "'");

                findMeta = false;
            }

            // Look for the configurable RemotedApp from intersection of the manifest with announcement data
            RemotedApp configurableApp = getRemotedApp(configurableApps, remAppAJ.deviceId, appId);

            // If there is no configurableApp, but aclMetadata has appName and deviceName to construct the RemotedApp object
            // and the acl configuredRules were created successfully, then create the RemotedApp object
            if (configurableApp == null) {

                Log.v(TAG, "retrieveRemotedApps - not found any ConfigurableApp for the remoted app: '" + key + "', objPath: '" +
                               objectPath + "'");

                if (findMeta && confRulesSize > 0) {

                    Log.v(TAG, "retrieveRemotedApps - metadata has the required values, creating the remoted app");
                    // Create RemotedApp
                    remotedApps.add(new RemotedApp(appNameMeta, appId, deviceNameMeta, remAppAJ.deviceId, configuredRules));
                }
            } else { // There is configurableApp

                Log.v(TAG, "retrieveRemotedApps - found announcement for the remoted app: '" + key + "', objPath: '" + objectPath + "'");

                if (metadataUpdated(deviceNameMeta, appNameMeta, configurableApp, key)) {
                    updatedMeta = true;
                }

                // Completes already configured rules with rules that haven't configured yet
                addUnconfiguredRemotedAppRules(configurableApp.getRuleObjectDescriptions(), configuredRules);

                if (configuredRules.size() > 0) {
                    remotedApps.add(new RemotedApp(configurableApp, configuredRules));
                }

            }// if :: annData != null

        }// for :: remotedApp

        // Add to the configured remotedApps the unconfigured remoted apps.
        // These apps remained in the configurableApps after working of the algorithm above
        remotedApps.addAll(configurableApps);

        return updatedMeta;
    }

    /**
     * Search for the {@link RemotedApp} in the given list of the remotedApps
     * with the given deviceId and appId. If the {@link RemotedApp} is found
     * it's removed from the remotedApps and is returned
     *
     * @param remotedApps
     *            To look for the {@link RemotedApp}
     * @param deviceId
     * @param appId
     * @return {@link RemotedApp} if found or NULL if NOT
     */
    private RemotedApp getRemotedApp(List<RemotedApp> remotedApps, String deviceId, UUID appId) {

        RemotedApp retApp         = null;
        Iterator<RemotedApp> iter = remotedApps.iterator();

        while (iter.hasNext()) {

            RemotedApp currApp = iter.next();
            if (currApp.getDeviceId().equals(deviceId) && currApp.getAppId().equals(appId)) {

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
     *
     * @param deviceNameMeta
     * @param appNameMeta
     * @param annApp
     * @param key
     *            metadata prefix key
     * @return TRUE if the metadata needs to be updated
     */
    private boolean metadataUpdated(String deviceNameMeta, String appNameMeta, RemotedApp annApp, String key) {

        boolean updatedMeta      = false;

        String annAppName        = annApp.getAppName();
        String annDeviceName     = annApp.getDeviceName();

        String appNameMetaKey    = key + APP_NAME_SUFFIX;
        String deviceNameMetaKey = key + DEVICE_NAME_SUFFIX;

        // Check appName, deviceName correctness vs. announcements
        if (!annAppName.equals(appNameMeta)) {

            Log.v(TAG, "retrieveRemotedApps - metaAppName is differ from the announcement app name, update " +
                           " the metadata with the app name: '" + annAppName + "', objPath: '" + objectPath + "'");

            aclMetadata.put(appNameMetaKey, annAppName);
            updatedMeta = true;
        }

        if (!annDeviceName.equals(deviceNameMeta)) {

            Log.v(TAG, "retrieveRemotedApps - metaDeviceName is differ from the announcement device name, update " +
                           " the metadata with the device name: '" + annDeviceName + "', objPath: '" + objectPath + "'");

            aclMetadata.put(deviceNameMetaKey, annDeviceName);
            updatedMeta = true;
        }

        return updatedMeta;
    }

    /**
     * Compares configured rules of the remoted apps with the unconfigured
     * rules. Completes the configured rules with the rules that haven't
     * configured yet.
     *
     * @param unconfRules
     * @param confRules
     */
    private void addUnconfiguredRemotedAppRules(List<RuleObjectDescription> unconfRules, List<RuleObjectDescription> confRules) {

        for (RuleObjectDescription unconfRule : unconfRules) {

            RuleObjectPath unconfOP         = unconfRule.getObjectPath();
            Set<RuleInterface> unconfIfaces = unconfRule.getInterfaces();

            // Gets TRUE if unconfOP was found among the confRules
            boolean unconfOpInConf = false;

            for (RuleObjectDescription confRule : confRules) {

                RuleObjectPath confOP = confRule.getObjectPath();
                Set<RuleInterface> confIfaces = confRule.getInterfaces();

                // Check if the unconfOP NOT equals confOP
                if (!unconfOP.getPath().equals(confOP.getPath())) {
                    continue;
                }

                unconfOpInConf = true;
                unconfIfaces.removeAll(confIfaces);
                break;
            }

            if (!unconfOpInConf || unconfIfaces.size() > 0) {
                confRules.add(new RuleObjectDescription(unconfOP, unconfIfaces, false));
            }

        }// for::unconfRule
    }

    /**
     * Converts {@link ManifestObjectDescriptionAJ} array in to
     * {@link RuleObjectDescription} list. The converted rules are validated
     * against the received {@link ConnectorCapabilities}. {@link ConnectorCapabilities} that were used for
     * the validation and the {@link RuleObjectDescription} construction are
     * stored in the given usedManRules map. Created
     * {@link RuleObjectDescription} rules are marked as configured. <br>
     * Important, for the correct work of this algorithm the manifest list must
     * be sorted with the {@link RuleObjectDescriptionComparator}.
     *
     * @param objDescsAJ
     *            to be converted
     * @param capabilities
     *            {@link ConnectorCapabilities} that are used for the validation and {@link RuleObjectDescription} construction
     * @param usedManRules
     *            manifest rules that were used for validation and the {@link RuleObjectDescription} construction
     * @return {@link RuleObjectDescription} list converted from the {@link ManifestObjectDescriptionAJ} array
     */
    private List<RuleObjectDescription> convertObjectDescription(ManifestObjectDescriptionAJ[] objDescsAJ,
                                   List<RuleObjectDescription> capabilities, Map<RuleObjectPath, Set<RuleInterface>> usedManRules) {

        Map<RuleObjectPath, Set<RuleInterface>> resRules = new HashMap<RuleObjectPath, Set<RuleInterface>>();

        for (ManifestObjectDescriptionAJ objDescAJ : objDescsAJ) {

            List<String> ifacesToConvert = new ArrayList<String>(Arrays.asList(objDescAJ.interfaces));

            for (RuleObjectDescription capRule : capabilities) {

                RuleObjectPath capObjPath      = capRule.getObjectPath();
                Set<RuleInterface> capIfaces   = capRule.getInterfaces();
                int capIfacesSize                 = capIfaces.size();

                if (!isValidObjPath(capObjPath, objDescAJ.objectPath, objDescAJ.isPrefix)) {
                    continue;
                }

                RuleObjectPath resObjPath;

                if (capObjPath.getPath().equals(objDescAJ.objectPath)) {
                    resObjPath = new RuleObjectPath(objDescAJ.objectPath, capObjPath.getFriendlyName(), objDescAJ.isPrefix, capObjPath.isPrefixAllowed());
                } else {
                    resObjPath = new RuleObjectPath(objDescAJ.objectPath, "", objDescAJ.isPrefix, capObjPath.isPrefixAllowed());
                }

                // Add used capabilities rules with the empty capability interfaces array
                if (capIfacesSize == 0) {

                    Set<RuleInterface> usedIfaces = usedManRules.get(capObjPath);
                    if (usedIfaces == null) {
                        usedManRules.put(capObjPath, new HashSet<RuleInterface>());
                    }
                }

                Iterator<String> ifacesToConvertIter = ifacesToConvert.iterator();
                Set<RuleInterface> resInterfaces  = new HashSet<RuleInterface>();

                // Validate interfaces
                while (ifacesToConvertIter.hasNext()) {

                    String ajIface = ifacesToConvertIter.next();

                    // If there are not interfaces in the capability rule, it means that all the interfaces are supported,
                    // add them without display names
                    if (capIfacesSize == 0) {
                        resInterfaces.add(new RuleInterface(ajIface, ""));
                        ifacesToConvertIter.remove();
                        continue;
                    }

                    for (RuleInterface connAppIface : capIfaces) {

                        // ajIface is found in the capable interfaces
                        if (ajIface.equals(connAppIface.getName())) {

                            resInterfaces.add(new RuleInterface(ajIface, connAppIface.getFriendlyName(), connAppIface.isSecured()));
                            ifacesToConvertIter.remove();
                            break;
                        }
                    }// for :: connector app interfaces
                }// while :: ifacesToConvertIter

                // Not found any matched interfaces, continue to the next
                // capability rule
                if (resInterfaces.size() == 0) {
                    continue;
                }

                // Add the interfaces to the resObjPath
                Set<RuleInterface> ifaces = resRules.get(resObjPath);
                if (ifaces == null) {
                    resRules.put(resObjPath, resInterfaces);
                } else {

                    // Merge interfaces
                    ifaces.addAll(resInterfaces);
                }

                // Add used capability rules
                if (capIfacesSize > 0) {

                    Set<RuleInterface> usedIfaces = usedManRules.get(capObjPath);
                    if (usedIfaces == null) {
                        usedManRules.put(capObjPath, new HashSet<RuleInterface>(resInterfaces));
                    } else {
                        usedIfaces.addAll(resInterfaces);
                    }
                }

                // If all the objDescAJ interfaces have been handled, no need to continue iterating over the manifest rules
                if (ifacesToConvert.size() == 0) {
                    break;
                }

            }// for :: manifest

        }// for :: objDescsAJ

        // Create final list of the configured rules
        List<RuleObjectDescription> rules = new ArrayList<RuleObjectDescription>(resRules.size());
        for (RuleObjectPath op : resRules.keySet()) {
            rules.add(new RuleObjectDescription(op, resRules.get(op), true));
        }

        return rules;
    }// convertObjectDescription

    /**
     * Converts received {@link AclRules} to the exposedServicesAJ and the
     * remotedAppsAJ in order to be sent to a Gateway Connector Application as a
     * part of an acl creation or an update. Validates received
     * {@link AclRules} against the {@link ConnectorCapabilities}. Fills the
     * exposedServicesAJ and the remotedAppsAJ with the valid rules. All the
     * invalid rules are stored in the returned {@link AclRules} object.
     *
     * @param aclRules
     *            The object to be validated and marshaled
     * @param connectorCapabilities
     *            Check the {@link AclRules} validity against this
     *            {@link ConnectorCapabilities}
     * @param exposedServicesAJ
     *            An empty list that will be populated with exposedServices
     *            rules converted to the {@link ManifestObjectDescriptionAJ}
     * @param remotedAppsAJ
     *            An empty list that will be populated with the remoted
     *            applications converted to the {@link RemotedAppsAJ}
     * @param metadata
     *            The internal ACL metadata to be populated for the ACL creation
     *            or an update
     * @return {@link AclRules} The rules that marked as invalid during
     *         testing against the {@link ConnectorCapabilities}
     */
    static AclRules marshalAclRules(AclRules aclRules, ConnectorCapabilities connectorCapabilities,
                                              List<ManifestObjectDescriptionAJ> exposedServicesAJ, List<RemotedAppAJ> remotedAppsAJ,
                                                  Map<String, String> metadata) {

        // Marshal Exposed Services
        List<RuleObjectDescription> invalidExpServices = marshalManifObjDescs(aclRules.getExposedServices(), exposedServicesAJ,
                                                                                      connectorCapabilities.getExposedServices());

        // Marshal Remoted Apps
        List<RemotedApp> invalidRemotedApps = new ArrayList<RemotedApp>();

        for (RemotedApp rmApp : aclRules.getRemotedApps()) {

            // This will be populated with the valid rules marshaled in to the
            // ManifestObjectDescriptionAJ
            List<ManifestObjectDescriptionAJ> marshalledRules  = new ArrayList<ManifestObjectDescriptionAJ>();
            List<RuleObjectDescription> invalidRemAppRules = marshalManifObjDescs(rmApp.getRuleObjectDescriptions(), marshalledRules,
                                                                                          connectorCapabilities.getRemotedServices());

            // If there are invalid rules store it
            if (invalidRemAppRules.size() > 0) {
                invalidRemotedApps.add(new RemotedApp(rmApp, invalidRemAppRules));
            }

            int marshaledRulesSize = marshalledRules.size();

            // If there is no any marshaled rule, no valid rule was found ->
            // continue
            if (marshaledRulesSize == 0) {
                continue;
            }

            // Populate the RemotedAppAJ
            RemotedAppAJ remotedAppAJ   = new RemotedAppAJ();
            remotedAppAJ.deviceId       = rmApp.getDeviceId();
            remotedAppAJ.appId          = TransportUtil.uuidToByteArray(rmApp.getAppId());
            remotedAppAJ.objDescs       = marshalledRules.toArray(new ManifestObjectDescriptionAJ[marshaledRulesSize]);

            remotedAppsAJ.add(remotedAppAJ);

            // Store this application data in the metadata
            String key = CommunicationUtil.getKey(rmApp.getDeviceId(), rmApp.getAppId());
            metadata.put(key + DEVICE_NAME_SUFFIX, rmApp.getDeviceName());
            metadata.put(key + APP_NAME_SUFFIX, rmApp.getAppName());
        }

        // Return the AclRules object with invalid rules
        return new AclRules(invalidExpServices, invalidRemotedApps);
    }// marshalAclRules

    /**
     * Marshals received {@link RuleObjectDescription} list to the list of
     * {@link ManifestObjectDescriptionAJ} which is used for ACL creation or an
     * update. Rules validity is checked before being marshaled. The list of the
     * invalid rules is returned by this method.
     *
     * @param toMarshal
     *            The list of {@link RuleObjectDescription} rules to be
     *            marshaled
     * @param target
     *            The list of {@link ManifestObjectDescriptionAJ} that is
     *            populated with the valid rules from the "toMarshal" list
     * @param connectorCapabilities
     *            Check validation of the "toMarshal" rules against the list of
     *            this manifest rules
     * @return List of an invalid rules. The rules that weren't found in the
     *         manifest rules
     */
    private static List<RuleObjectDescription> marshalManifObjDescs(List<RuleObjectDescription> toMarshal,
                                                                         List<ManifestObjectDescriptionAJ> target,
                                                                            List<RuleObjectDescription> connectorCapabilities) {

        List<RuleObjectDescription> invalidRules = new ArrayList<RuleObjectDescription>();

        for (RuleObjectDescription ruleObjDesc : toMarshal) {

            Set<RuleInterface> invInterfaces = new HashSet<RuleInterface>();
            boolean isValid                     = isValidRule(ruleObjDesc, invInterfaces, connectorCapabilities);

            // If current RuleObjectDescription is NOT valid it need to be added to the invalid rules
            // OR the RuleObjectDescription could be valid but some of its interfaces are not
            if (!isValid || invInterfaces.size() > 0) {
                invalidRules.add(new RuleObjectDescription(ruleObjDesc.getObjectPath(), invInterfaces));
            }

            if (!isValid) {
                continue;
            }

            // Marshal and add the valid rules to the target
            target.add(new ManifestObjectDescriptionAJ(ruleObjDesc));
        }

        return invalidRules;
    }

    /**
     * Checks that received toValidate rule complies with received
     * connectorCapabilities. The method removes from toValidate interfaces the
     * interfaces that are not valid (not in the connectorCapabilities interfaces). Not
     * valid interfaces are added to the received notValid set.
     *
     * @param toValidate
     *            {@link RuleObjectDescription} rules to be validated
     * @param notValid
     *            An empty Set that will be populated with interfaces that don't
     *            comply with the manifest interfaces
     * @param connectorCapabilities
     *            validate received toValidate rules against this list of the
     *            Connector Capabilities
     * @return Returns TRUE if there is at least one rule (interface) that
     *         complies with the manifest rules. FALSE means that toValidate
     *         rule doesn't comply with the manifest rules not by the object
     *         path and not by the interfaces. As a result of this method
     *         execution, toValidate will contain only valid interfaces all the
     *         invalid interfaces will be moved to the notValid set.
     */
    private static boolean isValidRule(RuleObjectDescription toValidate, Set<RuleInterface> notValid,
                                        List<RuleObjectDescription> connectorCapabilities) {

        boolean validRuleFound            = false;
        Set<RuleInterface> validIfaces = toValidate.getInterfaces();

        notValid.addAll(validIfaces);
        validIfaces.clear();

        // If toValidate is not configured it considered as a not valid rule, that won't be sent to the gateway
        if (!toValidate.isConfigured()) {
            return false;
        }

        for (RuleObjectDescription connCapRule : connectorCapabilities) {

            RuleObjectPath connCapOp         = connCapRule.getObjectPath();
            Set<RuleInterface> connCapIfaces = connCapRule.getInterfaces();

            // Check object path validity
            if (isValidObjPath(connCapOp, toValidate.getObjectPath().getPath(), toValidate.getObjectPath().isPrefix())) {

                // If the the list of the manifest interfaces is empty, it means that all the interfaces
                // of the toValidate object path are supported, so toValidate object is fully valid ==> return true
                if (connCapIfaces.size() == 0) {

                    validIfaces.addAll(notValid);
                    notValid.clear();
                    return true;
                }

                // Validate interfaces
                Iterator<RuleInterface> notValidIter = notValid.iterator();
                while (notValidIter.hasNext()) {

                    RuleInterface ifaceToTest = notValidIter.next();

                    if (connCapIfaces.contains(ifaceToTest)) { // Check if the interface is valid
                        validRuleFound = true;
                        notValidIter.remove();          // Remove the interface from notValid group
                        validIfaces.add(ifaceToTest);   // Add the interface to the valid group
                    }
                }

                // All the interfaces toValidate are valid
                if (validRuleFound && notValid.size() == 0) {
                    return true;
                }

            }// if :: objPath

        }// for :: connectorCapabilities

        return validRuleFound;
    }// isValidRule

    /**
     * Checks the object path validity against the manifest object path.
     *
     * @param connAppObjPath
     *
     * @param toValidOP
     *            Object path to be validated
     * @param isPrefix
     *            If toValidOP is prefix
     * @return TRUE if toValidOP is valid
     */
    private static boolean isValidObjPath(RuleObjectPath connAppObjPath, String toValidOP, boolean isPrefix) {

        if (connAppObjPath.isPrefix() && toValidOP.startsWith(connAppObjPath.getPath()) ||
                (!connAppObjPath.isPrefix() && !isPrefix && connAppObjPath.getPath().equals(toValidOP))) {

            return true;
        }

        return false;
    }
}
