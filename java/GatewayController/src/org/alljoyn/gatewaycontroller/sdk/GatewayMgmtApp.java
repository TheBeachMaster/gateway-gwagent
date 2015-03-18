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
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.Variant;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ApplicationManagement;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.InstalledAppInfoAJ;

import android.util.Log;

/**
 * The Gateway found on the network
 */
public class GatewayMgmtApp extends AnnouncedApp {
    private static final String TAG = "gwc" + GatewayMgmtApp.class.getSimpleName();

    /**
     * Constructor
     *
     * @param gwBusName
     *            The name of the {@link BusAttachment} of the Gateway Management App
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public GatewayMgmtApp(String gwBusName) {

        super(gwBusName, null, null, null, null);

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("Received undefined gwBusName");
        }
    }

    /**
     * Constructor
     *
     * @param gwBusName
     *            The name of the {@link BusAttachment} of the Gateway Management App that sent
     *            the Announcement
     * @param aboutData
     *            The data sent with the Announcement
     * @throws IllegalArgumentException
     *             is thrown if bad gwBusName has been received
     */
    public GatewayMgmtApp(String gwBusName, Map<String, Variant> aboutData) {

        super(gwBusName, aboutData);

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("Received undefined gwBusName");
        }

        String deviceId = getDeviceId();
        UUID appId = getAppId();

        if (deviceId == null || deviceId.length() == 0) {
            throw new IllegalArgumentException("DeviceId is undefined");
        }

        if (appId == null) {
            throw new IllegalArgumentException("AppId is undefined");
        }
    }

    /**
     * Retrieve the list of Connector Applications managed by the Gateway Management App identified by
     * the given gwBusName
     *
     * @param sessionId
     *            The id of the session established with the Gateway Management App
     * @return The {@link ConnectorApp}
     * @throws GatewayControllerException
     */
    public List<ConnectorApp> retrieveConnectorApps(int sessionId) throws GatewayControllerException {

        final String gwBusName = getBusName();
        BusAttachment bus      = GatewayController.getInstance().getBusAttachment();

        ProxyBusObject proxy   = bus.getProxyBusObject(getBusName(), "/gw", sessionId,
                                                       new Class<?>[] { ApplicationManagement.class });

        Log.d(TAG, "Retreiving list of the Connector Applications for the GW: '" + gwBusName + "'");

        ApplicationManagement appMng = proxy.getInterface(ApplicationManagement.class);

        InstalledAppInfoAJ[] appInfoArr;
        try {
            appInfoArr = appMng.getInstalledApps();
        } catch (BusException be) {

            Log.e(TAG, "Failed to retreive list of the Connector Applications for the GW: '" + gwBusName + "'");

            throw new GatewayControllerException("Failed to retreive list of the Connector Applications, Error: '"
                                                     + be.getMessage() + "'", be);
        }

        List<ConnectorApp> connectorApps = new ArrayList<ConnectorApp>(appInfoArr.length);
        for (InstalledAppInfoAJ appInfo : appInfoArr) {

            connectorApps.add(new ConnectorApp(gwBusName, appInfo));
        }

        return connectorApps;
    }
}
