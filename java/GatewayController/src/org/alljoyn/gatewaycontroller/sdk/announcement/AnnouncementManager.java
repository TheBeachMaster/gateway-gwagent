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

package org.alljoyn.gatewaycontroller.sdk.announcement;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.alljoyn.about.AboutService;
import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.Variant;
import org.alljoyn.gatewaycontroller.sdk.AnnouncedApp;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;
import org.alljoyn.gatewaycontroller.sdk.GatewayMgmtApp;
import org.alljoyn.gatewaycontroller.sdk.GatewayMgmtAppListener;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil;
import org.alljoyn.services.common.AnnouncementHandler;
import org.alljoyn.services.common.BusObjectDescription;

import android.util.Log;

/**
 * The {@link GatewayController} component that is responsible to receive and
 * manage the {@link AboutService} announcements
 */
public class AnnouncementManager implements AnnouncementHandler {
    private static final String TAG = "gwc" + AnnouncementManager.class.getSimpleName();

    /**
     * Handles Announcement tasks sequentially
     */
    private ExecutorService announceTaskHandler;

    /**
     * Received announcements of the devices in proximity The key is built by:
     * {@link AnnouncedApp#getKey()}
     */
    private Map<String, AnnouncementData> appAnnouncements;

    /**
     * Received announcements of the gateway management apps in proximity.
     * The key is created by: {@link AnnouncedApp#getKey()}
     */
    private Map<String, GatewayMgmtApp> gatewayApps;

    /**
     * Listeners for the announcements from the Gateway Management apps
     */
    private GatewayMgmtAppListener gwMgtmAppListener;

    /**
     * Constructor
     */
    public AnnouncementManager() {

        appAnnouncements    = new HashMap<String, AnnouncementData>();
        gatewayApps         = new HashMap<String, GatewayMgmtApp>();
        announceTaskHandler = Executors.newSingleThreadExecutor();

        // Gateway Controller needs to receive Announcement signals with all
        // type of the interfaces
        AboutServiceImpl.getInstance().addAnnouncementHandler(this, null);
    }

    /**
     * Clear the {@link AnnouncementManager} object resources
     */
    public void clear() {

        Log.d(TAG, "Clearing the object resources");
        AboutServiceImpl.getInstance().removeAnnouncementHandler(this, null);

        if (appAnnouncements != null) {
            appAnnouncements.clear();
            appAnnouncements = null;
        }

        if (gatewayApps != null) {
            gatewayApps.clear();
            gatewayApps = null;
        }

        if (announceTaskHandler != null) {
            announceTaskHandler.shutdownNow();
            announceTaskHandler = null;
        }
    }

    /**
     * Provide {@link GatewayMgmtAppListener} to be notified about Announcement signals
     * received from a {@link GatewayMgmtApp}.
     *
     * @param listener
     *            {@link GatewayMgmtAppListener}
     */
    public void setGatewayMgmtAppListener(GatewayMgmtAppListener listener) {
        this.gwMgtmAppListener = listener;
    }

    /**
     * @return List of the Gateway Management Apps found on the network
     */
    public List<GatewayMgmtApp> getGateways() {

        return new ArrayList<GatewayMgmtApp>(gatewayApps.values());
    }

    /**
     * @return Returns {@link AnnouncementData} objects that have been received
     */
    public List<AnnouncementData> getAnnouncementData() {

        return new ArrayList<AnnouncementData>(appAnnouncements.values());
    }

    /**
     * Returns {@link AnnouncementData} of the given device and the application
     * that has sent the Announcement
     *
     * @param deviceId
     *            The id of the device that sent the Announcement
     * @param appId
     *            The id of the application that sent the Announcement
     * @return {@link AnnouncementData} or NULL if the data wasn't found
     */
    public AnnouncementData getAnnouncementData(String deviceId, UUID appId) {
        return appAnnouncements.get(CommunicationUtil.getKey(deviceId, appId));
    }

    // ============== AnnouncementHandler ==============//

    /**
     * @see org.alljoyn.services.common.AnnouncementHandler#onAnnouncement(java.lang.String,
     *      short, org.alljoyn.services.common.BusObjectDescription[],
     *      java.util.Map)
     */
    @Override
    public void onAnnouncement(final String busName, final short port,
                                   final BusObjectDescription[] objectDescriptions,
                                       final Map<String, Variant> aboutData) {

        Log.d(TAG, "Received Announcement from: '" + busName + "' enqueueing");
        announceTaskHandler.execute(new Runnable() {
            @Override
            public void run() {
                handleAnnouncement(busName, port, objectDescriptions, aboutData);
            }
        });
    }

    /**
     * @see org.alljoyn.services.common.AnnouncementHandler#onDeviceLost(java.lang.String)
     */
    @Override
    public void onDeviceLost(final String busName) {

        Log.d(TAG, "Received onDeviceLost event of: '" + busName + "' enqueueing");
        announceTaskHandler.execute(new Runnable() {
            @Override
            public void run() {
                handleDeviceLost(busName);
            }
        });
    }

    /**
     * Handles asynchronously the received Announcement
     *
     * @param busName
     * @param port
     * @param objectDescriptions
     * @param aboutData
     */
    private void handleAnnouncement(String busName, short port, BusObjectDescription[] objectDescriptions, Map<String, Variant> aboutData) {

        Log.d(TAG, "Received announcement from: '" + busName + "', handling");

        // Received announcement from a Gateway Management App
        if (isFromGW(objectDescriptions)) {

            Log.d(TAG, "Received Announcement from Gateway Management App, bus: '" + busName + "', storing");

            GatewayMgmtApp gw;

            try {
                gw = new GatewayMgmtApp(busName, aboutData);
            } catch (IllegalArgumentException ilae) {

                Log.e(TAG, "Received announcement from Gateway Management App, but failed to create the GatewayMgmt object", ilae);
                return;
            }

            String key = CommunicationUtil.getKey(gw.getDeviceId(), gw.getAppId());

            gatewayApps.put(key, gw);

            if (gwMgtmAppListener != null) {
                gwMgtmAppListener.gatewayMgmtAppAnnounced(); // Received Announcement from Gateway -> Notify
            }

            return;
        }

        AnnouncedApp app  = new AnnouncedApp(busName, aboutData);

        UUID appId        = app.getAppId();
        String deviceId   = app.getDeviceId();

        if (appId == null || deviceId == null || deviceId.length() == 0) {

            Log.e(TAG, "Received Announcement from the application: '" + app + "', but deviceId or appId are not defined");

            return;
        }

        Log.d(TAG, "Received Announcement from the application: '" + app + "' storing");

        AnnouncementData annData = new AnnouncementData(port, objectDescriptions, aboutData, app);
        String key               = CommunicationUtil.getKey(deviceId, appId);

        // Store the AnnouncementData object
        appAnnouncements.put(key, annData);

    }// handleAnnouncement

    /**
     * Handles asynchronously received lostAdvertisedName event
     *
     * @param busName
     */
    private void handleDeviceLost(String busName) {

        handleDeviceLostApps(busName);
        boolean gwRemoved = handleDeviceLostGateway(busName);

        if (gwMgtmAppListener != null && gwRemoved) {
            gwMgtmAppListener.gatewayMgmtAppAnnounced();
        }
    }

    /**
     * Search the application by the given busName to be removed from the
     * appAnnouncements
     *
     * @param busName
     * @return TRUE if an application was removed
     */
    private void handleDeviceLostApps(String busName) {

        Iterator<AnnouncementData> iterator = appAnnouncements.values().iterator();

        while (iterator.hasNext()) {

            AnnouncementData anData = iterator.next();
            if (anData.getApplicationData().getBusName().equals(busName)) {

                Log.d(TAG, "lostAdvertisedName for Applications, removed: '" + busName + "'");
                iterator.remove();
            }
        }
    }

    /**
     * Search the Gateway App by the given busName to be removed from the
     * gatewayApps
     *
     * @param busName
     * @return TRUE if a gateway was removed
     */
    private boolean handleDeviceLostGateway(String busName) {

        boolean gwRemoved = false;
        Iterator<GatewayMgmtApp> iterator = gatewayApps.values().iterator();

        while (iterator.hasNext()) {

            GatewayMgmtApp gw = iterator.next();
            if (gw.getBusName().equals(busName)) {

                Log.d(TAG, "lostAdvertisedName for GW, removed: '" + busName + "'");
                iterator.remove();
                gwRemoved = true;
            }
        }

        return gwRemoved;
    }

    /**
     * @param objectDescriptions
     * @return Return TRUE the announcement was sent from GW, otherwise FALSE
     */
    private boolean isFromGW(BusObjectDescription[] objectDescriptions) {

        // Check whether the announcement was sent from a Gateway Management App
        for (BusObjectDescription objDesc : objectDescriptions) {

            for (String iface : objDesc.getInterfaces()) {

                if (iface.startsWith(GatewayController.IFACE_PREFIX)) {
                    return true;
                }
            }
        }

        return false;
    }

}
