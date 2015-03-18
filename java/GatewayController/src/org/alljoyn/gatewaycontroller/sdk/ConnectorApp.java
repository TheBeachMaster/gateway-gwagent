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

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Status;
import org.alljoyn.gatewaycontroller.sdk.Acl.AclResponseCode;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus.RestartStatus;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleInterface;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleObjectPath;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil;
import org.alljoyn.gatewaycontroller.sdk.announcement.AnnouncementData;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.AclInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.AclManagement;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.Application;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ApplicationStatusAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.CreateAclStatusAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.InstalledAppInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestRulesAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.RemotedAppAJ;
import org.alljoyn.services.common.BusObjectDescription;

import android.util.Log;

/**
 * The Connector Application managed by the {@link GatewayMgmtApp}
 */
public class ConnectorApp {
    private static final String TAG = "gwc" + ConnectorApp.class.getSimpleName();

    /**
     * This class receives Connector Application related signals
     */
    private class SignalHandler implements BusObject, Application {

        /**
         * The Method object receiving the status changed signals
         */
        private Method signalMethod;

        /**
         * Receives AllJoyn signal when the state of the Connector Application has been
         * changed
         *
         * @param appStatusAJ
         *            AllJoyn structure of the application state
         */
        @Override
        public void applicationStatusChanged(short installStatus, String installDescription, short connectionStatus,
                                                 short operationalStatus) {

            BusAttachment bus = GatewayController.getInstance().getBusAttachment();
            bus.enableConcurrentCallbacks();

            ConnectorAppStatus appStatus;

            try {
                appStatus = new ConnectorAppStatus(installStatus, installDescription, connectionStatus, operationalStatus);
            } catch (GatewayControllerException gce) {

                Log.e(TAG, "Failed to read data of the connector application status changed signal, objPath: '" + objectPath + "'", gce);
                return;
            }

            // Lock to prevent race with the unsetStatusChangedHandler method
            synchronized (ConnectorApp.this) {

                if (appSignalHandler == null) {
                    Log.w(TAG, "appSignalHandler is NULL can't deliver StatusChanged signal, objPath: '" + objectPath + "'");
                    return;
                }

                Log.d(TAG, "Received StatusChanged signal for the connector application id: '" + appId + "', Status: '" + appStatus + "'");
                appSignalHandler.onStatusChanged(appId, appStatus);
            }
        }

        /**
         * @return Status changed signal handler method or NULL if failed to get it
         */
        public Method getStatusChangedMethod() {

            if (signalMethod == null) {
                try {
                    signalMethod = getClass().getDeclaredMethod("applicationStatusChanged", short.class, String.class,
                                                                    short.class, short.class);

                } catch (NoSuchMethodException nsme) {
                    Log.e(TAG, "Failed to get a reflection of the appStatusChanged method", nsme);
                }
            }

            return signalMethod;
        }

        @Override
        public ApplicationStatusAJ getApplicationStatus() throws BusException {
            return null;
        }

        @Override
        public ManifestRulesAJ getManifestInterfaces() throws BusException {
            return null;
        }

        @Override
        public short restartApp() throws BusException {
            return 0;
        }

        @Override
        public String getManifestFile() throws BusException {
            return null;
        }

        @Override
        public short getVersion() throws BusException {
            return 0;
        }
    }

    // ===================================================//

    /**
     * The name of the gateway management app {@link BusAttachment} that manages
     * this {@link ConnectorApp}
     */
    private final String gwBusName;

    /**
     * Application id
     */
    private final String appId;

    /**
     * The connector connector application friendly name or description
     */
    private final String friendlyName;

    /**
     * The identification of the connector application object
     */
    private final String objectPath;

    /**
     * The connector application version
     */
    private final String appVersion;

    /**
     * This is immutable object. Cache the toString value
     */
    private String toStrVal;

    /**
     * The local object receiving status change signals
     */
    private volatile SignalHandler sigHandler;

    /**
     * Client's object to be notified about the status changed signal
     */
    private volatile ConnectorAppStatusSignalHandler appSignalHandler;

    /**
     * Constructor
     *
     * @param gwBusName
     *            The name of the gateway {@link BusAttachment} managing this {@link ConnectorApp}
     * @param appObjPath
     *            The object path to reach the Gateway Connector Application on
     *            the gateway
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public ConnectorApp(String gwBusName, String appObjPath) {

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("gwBusName is undefined");
        }

        if (appObjPath == null || appObjPath.length() == 0) {
            throw new IllegalArgumentException("appObjPath is undefined");
        }

        this.gwBusName = gwBusName;
        objectPath     = appObjPath;
        appId          = null;
        friendlyName   = null;
        appVersion     = null;
    }

    /**
     * Constructor
     *
     * @param appInfo
     */
    ConnectorApp(String gwBusName, InstalledAppInfoAJ appInfo) {

        this.gwBusName = gwBusName;
        objectPath     = appInfo.objectPath;
        appId          = appInfo.appId;
        friendlyName   = appInfo.friendlyName;
        appVersion     = appInfo.appVersion;
    }

    /**
     * @return gwBusName the {@link ConnectorApp} is installed on
     */
    public String getGwBusName() {
        return gwBusName;
    }

    /**
     * @return The id of the {@link ConnectorApp}
     */
    public String getAppId() {
        return appId;
    }

    /**
     * @return The name of the {@link ConnectorApp}.
     */
    public String getFriendlyName() {
        return friendlyName;
    }

    /**
     * @return The object path to reach the {@link ConnectorApp} on the gateway
     */
    public String getObjectPath() {
        return objectPath;
    }

    /**
     * @return The connector application version
     */
    public String getAppVersion() {
        return appVersion;
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        if (toStrVal == null) {

            StringBuilder sb = new StringBuilder("ConnectorApplication [");
            sb.append("gwName='").append(gwBusName).append("', ")
            .append("appId='").append(appId).append("', ")
            .append("friendlyName='").append(friendlyName).append("', ")
            .append("objectPath='").append(objectPath).append("', ")
            .append("appVersion='").append(appVersion).append("']");

            toStrVal = sb.toString();
        }

        return toStrVal;
    }

    // ===================================================//

    /**
     * Clears the object resources
     */
    public void clear() {

        unsetStatusSignalHandler();
    }

    /**
     * Returns string representation of the Manifest file of the Connector Application.
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return String representation of the Manifest file in XML format.
     * @throws GatewayControllerException
     *             If failed to retrieve the Manifest file
     */
    public String retrieveManifestFile(int sessionId) throws GatewayControllerException {

        Application app = getApplicationProxy(sessionId);

        Log.d(TAG, "Retrieving application manifest, objPath: '" + objectPath + "'");

        try {

            return app.getManifestFile();
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve the manifest file, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the manifest file", be);
        }
    }

    /**
     * Returns the {@link ConnectorCapabilities} of the {@link ConnectorApp}
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link ConnectorCapabilities}
     * @throws GatewayControllerException
     *             If failed to retrieve the {@link ConnectorCapabilities}
     */
    public ConnectorCapabilities retrieveConnectorCapabilities(int sessionId) throws GatewayControllerException {

        Application app = getApplicationProxy(sessionId);

        Log.d(TAG, "Retrieving Connector Capabilities, objPath: '" + objectPath + "'");

        try {

            ManifestRulesAJ manifestRulesAJ = app.getManifestInterfaces();
            return new ConnectorCapabilities(manifestRulesAJ);
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve the ConnectorCapabilities, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the Connector Capabilities", be);
        }
    }

    /**
     * Use data of the returned {@link AclRules} object for creation the Acl
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link AclRules} to be used for Acl creation
     * @throws GatewayControllerException
     *             failed to create the {@link AclRules}
     */
    public AclRules retrieveApplicableConnectorCapabilities(int sessionId) throws GatewayControllerException {

        Log.d(TAG, "Retrieving applicable connector capabilities, objPath: '" + objectPath + "'");

        ConnectorCapabilities manifest = retrieveConnectorCapabilities(sessionId);

        List<RuleObjectDescription> remotedServices = manifest.getRemotedServices();
        Collections.sort(remotedServices, new RuleObjectDescriptionComparator());

        return new AclRules(manifest.getExposedServices(), extractRemotedApps(remotedServices));
    }

    /**
     * Returns the state of the Connector Application
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link ConnectorAppStatus}
     * @throws GatewayControllerException
     *             If failed to retrieve the connector application status
     */
    public ConnectorAppStatus retrieveStatus(int sessionId) throws GatewayControllerException {

        Application app = getApplicationProxy(sessionId);

        Log.d(TAG, "Retrieving connector application state, objPath: '" + objectPath + "'");

        try {

            ApplicationStatusAJ appStatusAJ = app.getApplicationStatus();
            return new ConnectorAppStatus(appStatusAJ);
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve the connector application status, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the connector application status", be);
        }
    }

    /**
     * Restarts the connector application
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return {@link RestartStatus}
     * @throws GatewayControllerException
     *             If failed to restart the connector application
     */
    public RestartStatus restart(int sessionId) throws GatewayControllerException {

        Application app = getApplicationProxy(sessionId);

        Log.d(TAG, "Restarting the connector application, objPath: '" + objectPath + "'");

        try {

            short returnCode     = app.restartApp();
            RestartStatus status = CommunicationUtil.shortToEnum(RestartStatus.class, returnCode);

            if (status == null) {
                throw new GatewayControllerException("Unknown restart response status has been received: '" + status + "'");
            }

            return status;
        } catch (BusException be) {

            Log.e(TAG, "Failed to restart the connector application, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to restart the connector application", be);
        }

    }

    /**
     * Set an {@link ConnectorAppStatusSignalHandler} to receive connector application
     * related events. In order to receive the events, in addition to calling
     * this method, a session should be successfully established with the
     * Gateway Management App hosting the Connector Application. Use
     * {@link ConnectorApp#unsetStatusSignalHandler()} to stop
     * receiving the events.
     *
     * @param handler
     *            Signal handler
     * @throws GatewayControllerException
     *             If failed to register the signal handler
     * @throws IllegalArgumentException
     *             If the received handler is NULL
     */
    public void setStatusSignalHandler(ConnectorAppStatusSignalHandler handler) throws GatewayControllerException {

        if (handler == null) {
            throw new IllegalArgumentException("The received signal handler is NULL");
        }

        Log.d(TAG, "Registering connector application status signal handler, objPath: '" + objectPath + "'");
        appSignalHandler = handler;

        if (sigHandler != null) {
            return;
        }

        // Need to register a local signal handler
        sigHandler        = new SignalHandler();
        BusAttachment bus = GatewayController.getInstance().getBusAttachment();

        Method sigMethod  = sigHandler.getStatusChangedMethod();

        if (sigMethod == null) {
            throw new GatewayControllerException("The signal handler method wasn't found");
        }

        Status status = bus.registerBusObject(sigHandler, objectPath);

        if (status != Status.OK) {
            throw new GatewayControllerException("Failed to register BusObject, Status: '" + status + "'");
        }

        status = bus.registerSignalHandler(Application.IFNAME, Application.APPLICATION_STATUS_CHANGED, sigHandler, sigMethod,
                                               objectPath);

        if (status != Status.OK) {

            unsetStatusSignalHandler();
            throw new GatewayControllerException("Failed to register signal handler, Status: '" + status + "'");
        }

        String matchRule = "interface='" + Application.IFNAME + "',type='signal'";

        status = bus.addMatch(matchRule);
        if (status != Status.OK) {
            Log.e(TAG, "Failed to add the rule: '" + matchRule + "' for receiving status change signals, Status: '" + status +
                           "', objPath: '" + objectPath + "'");

            unsetStatusSignalHandler();
            return;
        }

        Log.d(TAG, "Added the rule to receive status change signals, rule: '" + matchRule + "', objPath: '" + objectPath + "'");
    }

    /**
     * Stop receiving Connector Application related signals
     */
    public void unsetStatusSignalHandler() {

        Log.d(TAG, "Unsetting status signal handler, objPath: '" + objectPath + "'");

        synchronized (this) {
            appSignalHandler = null;
        }

        BusAttachment bus = GatewayController.getInstance().getBusAttachment();

        if (sigHandler != null && bus != null) {

            Method sigMethod = sigHandler.getStatusChangedMethod();

            if (sigMethod != null) {
                bus.unregisterSignalHandler(sigHandler, sigMethod);
            }

            bus.unregisterBusObject(sigHandler);

            Status status = bus.removeMatch("interface='" + Application.IFNAME + "',type='signal'");
            Log.d(TAG, "Unregistered signal handler; Removed match rule status: '" + status + "', ConnectorApp objPath: '" +
                            objectPath + "'");

            sigHandler = null;
        }
    }

    /**
     * Sends request to create {@link Acl} object with the
     * received name and the {@link AclRules}. The {@link AclRules} are
     * validated against the {@link ConnectorCapabilities}. Only valid rules will be
     * sent for the ACL creation. The invalid rules could be received from the
     * returned {@link AclWriteResponse} object.
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @param name
     *            The Acl name
     * @param aclRules
     *            The Acl rules
     * @return {@link AclWriteResponse}
     * @throws GatewayControllerException
     *             if failed to send request to create the ACL
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public AclWriteResponse createAcl(int sessionId, String name, AclRules aclRules) throws GatewayControllerException {

        if (name == null || name.length() == 0) {
            throw new IllegalArgumentException("ACL name is undefined");
        }

        if (aclRules == null) {
            throw new IllegalArgumentException("aclRules is undefined");
        }

        Log.d(TAG, "Creating ACL with the name: '" + name + "', objPath: '" + objectPath + "'");

        ConnectorCapabilities connectorCapabilities       = retrieveConnectorCapabilities(sessionId);
        List<ManifestObjectDescriptionAJ> exposedServices = new ArrayList<ManifestObjectDescriptionAJ>();
        List<RemotedAppAJ> remotedApps                    = new ArrayList<RemotedAppAJ>();
        Map<String, String> internalMetadata              = new HashMap<String, String>();

        AclRules invalidRules = Acl.marshalAclRules(aclRules, connectorCapabilities, exposedServices, remotedApps,
                                                        internalMetadata);

        ManifestObjectDescriptionAJ[] exposedServicesArr = new ManifestObjectDescriptionAJ[exposedServices.size()];
        RemotedAppAJ[] remotedAppsArr                    = new RemotedAppAJ[remotedApps.size()];

        exposedServices.toArray(exposedServicesArr);
        remotedApps.toArray(remotedAppsArr);

        CreateAclStatusAJ createStatus;

        AclManagement aclMngr = getAclProxy(sessionId);

        Map<String, String> customMetadata = aclRules.getMetadata();
        if (customMetadata == null) {
            customMetadata = new HashMap<String, String>();
        }

        try {
            createStatus = aclMngr.createAcl(name, exposedServicesArr, remotedAppsArr, internalMetadata, customMetadata);
        } catch (BusException be) {
            Log.e(TAG, "Failed to create the ACL, name: '" + name + "', objPath: '" + objectPath + "'");
            throw new GatewayControllerException("ACL creation failed", be);
        }

        AclResponseCode aclRespCode = CommunicationUtil.shortToEnum(AclResponseCode.class, createStatus.aclResponseCode);
        if (aclRespCode == null) {
            throw new GatewayControllerException("Unknown AclResponseCode has been received, code: '" + aclRespCode +
                                                     "', objPath: '" + objectPath + "'");
        }

        return new AclWriteResponse(createStatus.aclId, aclRespCode, invalidRules, createStatus.objPath);
    }

    /**
     * Return a list of the ACLs installed on the Connector Application
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return List of the {@link Acl}
     * @throws GatewayControllerException
     *             If failed to retrieve the ACL list
     */
    public List<Acl> retrieveAcls(int sessionId) throws GatewayControllerException {

        AclManagement aclMngr = getAclProxy(sessionId);

        Log.d(TAG, "Retrieving the list of installed ACLs, objPath: '" + objectPath + "'");

        AclInfoAJ[] aclInfoArr;

        try {
            aclInfoArr = aclMngr.listAcls();
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve the list of installed ACLs, objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to retrieve the list of installed ACLs", be);
        }

        List<Acl> aclList = new ArrayList<Acl>(aclInfoArr.length);

        for (AclInfoAJ aclInfoAJ : aclInfoArr) {

            try {
                aclList.add(new Acl(gwBusName, aclInfoAJ));
            } catch (GatewayControllerException gce) {
                Log.d(TAG, "Failed to initialize received ACL, Error: '" + gce.getMessage() + "'");
            }
        }

        return aclList;
    }

    /**
     * Delete the Access Control List of this Connector Application
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @param aclId
     *            The id of the ACL to be deleted
     * @return {@link AclResponseCode}
     * @throws GatewayControllerException
     *             If failed to delete the ACL
     * @throws IllegalArgumentException
     *             is thrown if bad aclId has been received
     */
    public AclResponseCode deleteAcl(int sessionId, String aclId) throws GatewayControllerException {

        if (aclId == null || aclId.length() == 0) {
            throw new IllegalArgumentException("aclId is undefined");
        }

        AclManagement aclMngr = getAclProxy(sessionId);

        Log.d(TAG, "Delete the ACL, objPath: '" + objectPath + "', alcId: '" + aclId + "'");

        short aclRespRes;

        try {
            aclRespRes = aclMngr.deleteAcl(aclId);
        } catch (BusException be) {

            Log.e(TAG, "Failed to delete the ACL, id: '" + aclId + "', objPath: '" + objectPath + "'");
            throw new GatewayControllerException("Failed to delete the ACL", be);
        }

        AclResponseCode aclRespCode = CommunicationUtil.shortToEnum(AclResponseCode.class, aclRespRes);

        if (aclRespCode == null) {
            throw new GatewayControllerException("Unknown AclResponseCode has been received, code: '" + aclRespRes +
                                                     "', objPath: '" + objectPath + "'");
        }

        return aclRespCode;
    }

    // ===================================================//

    /**
     * Intersects {@link AnnouncementData} with the received remotedServices,
     * creates a list of {@link RemotedApp}
     *
     * @param remotedServices
     *            The remotedServices from the application manifest
     * @return List of {@link RemotedApp}
     */
    static List<RemotedApp> extractRemotedApps(List<RuleObjectDescription> remotedServices) {

        List<AnnouncementData> announcements = GatewayController.getInstance().getAnnouncementManager().getAnnouncementData();

        List<RemotedApp> remotedApps = new ArrayList<RemotedApp>();

        for (AnnouncementData ann : announcements) {

            RemotedApp remotedApp = extractRemotedApp(remotedServices, ann);
            if (remotedApp != null) {
                remotedApps.add(remotedApp);
            }
        }

        return remotedApps;
    }

    /**
     * Intersects received {@link AnnouncementData} with the received
     * remotedServices, creates a {@link RemotedApp}. <br>
     * Important, for the correct work of this algorithm the list of the remoted
     * services must be sorted with the {@link RuleObjectDescriptionComparator}.
     *
     * @param remotedServices
     *            The remotedServices from the Connector App capabilities
     * @param ann
     *            {@link AnnouncementData} to be intersected with the remotedServices
     * @return {@link RemotedApp} or NULL if the {@link BusObjectDescription}s
     *         of the received {@link AnnouncementData} do not have any object
     *         path or interfaces that match the remotedServices. Additionally
     *         NULL is returned if the {@link AnnouncementData} doesn't have
     *         mandatory values for {@link RemotedApp} creation.
     */
    static RemotedApp extractRemotedApp(List<RuleObjectDescription> remotedServices, AnnouncementData ann) {

        Map<RuleObjectPath, Set<RuleInterface>> remotedRules = new HashMap<RuleObjectPath, Set<RuleInterface>>();

        for (BusObjectDescription bod : ann.getObjDescArr()) {

            List<String> ifacesToMatch = new ArrayList<String>(Arrays.asList(bod.getInterfaces()));

            for (RuleObjectDescription connectorRule : remotedServices) {

                RuleObjectPath connectorObjPath    = connectorRule.getObjectPath();
                Set<RuleInterface> connectorIfaces = connectorRule.getInterfaces();
                int connectorIfacesSize               = connectorIfaces.size();

                // Check object path suitability: if connector app objPath is a  prefix of BusObjDesc objPath
                // or both object paths are equal
                if ((connectorObjPath.isPrefix() && bod.getPath().startsWith(connectorObjPath.getPath())) ||
                        connectorObjPath.getPath().equals(bod.getPath())) {

                    Set<RuleInterface> resIfaces    = new HashSet<RuleInterface>();
                    Iterator<String> ifacesToMatchIter = ifacesToMatch.iterator();

                    // Search for the interfaces that comply with the connector app interfaces
                    while (ifacesToMatchIter.hasNext()) {

                        String iface = ifacesToMatchIter.next();

                        // If there are not interfaces in the connector app manifest,
                        // it means that all the interfaces are supported add them without display names
                        if (connectorIfacesSize == 0) {
                            resIfaces.add(new RuleInterface(iface, ""));
                            ifacesToMatchIter.remove();
                            continue;
                        }

                        for (RuleInterface conAppIface : connectorIfaces) {

                            if (conAppIface.getName().equals(iface)) {

                                resIfaces.add(conAppIface); // found interface -> add it to the results
                                ifacesToMatchIter.remove();
                                break;
                            }
                        }
                    }// while :: ifacesToMatch

                    // not found any matched interfaces, continue to the next Capability rule
                    if (resIfaces.size() == 0) {
                        continue;
                    }

                    // We add the capability rule, if the connector app OP is the prefix of the BOD.objPath
                    // or both object paths are equal
                    RuleObjectPath storeOp = new RuleObjectPath(connectorObjPath.getPath(), connectorObjPath.getFriendlyName(),
                                                                          false, connectorObjPath.isPrefixAllowed());

                    Set<RuleInterface> remotedIfaces = remotedRules.get(storeOp);
                    if (remotedIfaces == null) {
                        remotedRules.put(storeOp, resIfaces);
                    } else {
                        remotedIfaces.addAll(resIfaces);
                    }

                    // We add the BOD.objPath if the connector app OP is not equal to
                    // the BOD.objPath
                    if (!connectorObjPath.getPath().equals(bod.getPath())) {

                        // bodOp starts with the manOp but itself it's not a  prefix
                        RuleObjectPath bodOp = new RuleObjectPath(bod.getPath(), "", false, connectorObjPath.isPrefixAllowed());
                        remotedIfaces           = remotedRules.get(bodOp);

                        if (remotedIfaces == null) {
                            remotedRules.put(bodOp, new HashSet<RuleInterface>(resIfaces));
                        } else {
                            remotedIfaces.addAll(resIfaces);
                        }
                    }

                }// if :: objPath

                // If all the BusObjectDescription interfaces have been handled, no need to continue iterating
                // over the connector app rules
                if (ifacesToMatch.size() == 0) {
                    break;
                }

            }// for :: remotedServices

        }// for :: BusObjectDesc

        int rulesSize = remotedRules.size();

        // Check if this announcement complies with the ConnectorApp capabilities
        if (rulesSize == 0) {
            return null;
        }

        // Create Remoted rules list
        List<RuleObjectDescription> rules = new ArrayList<RuleObjectDescription>(rulesSize);
        for (RuleObjectPath op : remotedRules.keySet()) {
            rules.add(new RuleObjectDescription(op, remotedRules.get(op)));
        }

        RemotedApp remotedApp = null;
        try {
            remotedApp = new RemotedApp(ann.getApplicationData(), rules);
        } catch (IllegalArgumentException ilae) {
            Log.e(TAG, "Failed to create a RemotedApp ", ilae);
        }

        return remotedApp;
    }

    /**
     * Returns {@link ProxyBusObject} of the {@link Application} interface
     *
     * @param sid
     *            The id of the session established with the Gateway Management App
     * @return {@link Application}
     */
    private Application getApplicationProxy(int sid) {

        ProxyBusObject proxy = getProxyBusObject(sid, new Class<?>[] { Application.class });
        return proxy.getInterface(Application.class);
    }

    /**
     * Returns {@link ProxyBusObject} of the {@link AclManagement} interface
     *
     * @param sid
     *            The id of the session established with the Gateway Management App
     * @return {@link AclManagement}
     */
    private AclManagement getAclProxy(int sid) {

        ProxyBusObject proxy = getProxyBusObject(sid, new Class<?>[] { AclManagement.class });
        return proxy.getInterface(AclManagement.class);
    }

    /**
     * Create {@link ProxyBusObject}
     *
     * @param sid
     *            Session id
     * @param ifaces
     *            interfaces
     * @return {@link ProxyBusObject}
     */
    private ProxyBusObject getProxyBusObject(int sid, Class<?>[] ifaces) {

        BusAttachment bus = GatewayController.getInstance().getBusAttachment();
        return bus.getProxyBusObject(gwBusName, objectPath, sid, ifaces);
    }

}
