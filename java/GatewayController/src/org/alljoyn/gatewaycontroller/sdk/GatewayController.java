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

import java.util.List;

import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil.SessionResult;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.GatewayControllerSessionListener;
import org.alljoyn.gatewaycontroller.sdk.announcement.AnnouncementManager;

import android.util.Log;

/**
 * This class includes the main functionality for the Gateway Controller
 * Application
 */
public class GatewayController {
    private static final String TAG = "gwc" + GatewayController.class.getSimpleName();

    /**
     * Self reference for the {@link GatewayController} singleton
     */
    private static final GatewayController SELF  = new GatewayController();

    /**
     * The prefix for all the gateway interface names
     */
    public static final String IFACE_PREFIX      = "org.alljoyn.gwagent.ctrl";

    /**
     * Port number to connect to a Gateway Management App
     */
    public static final short PORT_NUM           = 1020;

    /**
     * The BusAttachment to be used
     */
    private BusAttachment bus;

    /**
     * Receives and manages incoming announcements
     */
    private AnnouncementManager announceManager;

    /**
     * Constructor
     */
    private GatewayController() {
    }

    /**
     * @return The {@link GatewayController} object
     */
    public static GatewayController getInstance() {
        return SELF;
    }

    /**
     * Initialize the gateway controller
     *
     * @param bus
     *            {@link BusAttachment} to use
     * @throws IllegalStateException
     *             If illegal {@link BusAttachment} has been received
     */
    public void init(BusAttachment bus) {

        checkBusValidity(bus);
        Log.i(TAG, "Initializing the GatewayController");

        this.bus = bus;

        if (!AboutServiceImpl.getInstance().isClientRunning()) {
            throw new IllegalStateException("AboutClient is not running, can't initialize AnnouncementManager");
        }

        announceManager = new AnnouncementManager();
    }

    /**
     * Shutdown the gateway controller
     */
    public void shutdown() {

        Log.i(TAG, "Shutting down the GatewayController");
        bus = null;

        if (announceManager != null) {
            announceManager.clear();
            announceManager = null;
        }
    }

    /**
     * @return {@link BusAttachment} that is used by the
     *         {@link GatewayController}
     */
    public BusAttachment getBusAttachment() {
        return bus;
    }

    /**
     * @return {@link AnnouncementManager} component
     */
    AnnouncementManager getAnnouncementManager() {
        return announceManager;
    }

    /**
     * Join session synchronously with the given Gateway Management App identified by the
     * gwBusName. This method doesn't require
     * {@link GatewayControllerSessionListener}. Use this method when there is
     * no need to receive any session related event.
     *
     * @param gwBusName
     *            The bus name of the Gateway Management App to connect to.
     * @return {@link SessionResult}
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public SessionResult joinSession(String gwBusName) {

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("The given gwBusName is undefined");
        }

        Log.d(TAG, "Join session synchronously with the Gateway Management App: '" + gwBusName + "'");
        return joinSession(gwBusName, new GatewayControllerSessionListener());
    }

    /**
     * Join session synchronously with the given gateway identified by the
     * gwBusName. The session related events will be sent to the given listener.
     *
     * @param gwBusName
     *            The bus name of the Gateway Management App to connect to.
     * @param listener
     *            The listener is used to be notified about the session related
     *            events
     * @return {@link SessionResult}
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public SessionResult joinSession(String gwBusName, GatewayControllerSessionListener listener) {

        checkSessionValidity(gwBusName, listener);
        return CommunicationUtil.joinSession(bus, gwBusName, listener);
    }

    /**
     * Join session asynchronously with the given gwBusName.
     *
     * @param gwBusName
     *            The bus name of the Gateway Management App to connect to.
     * @param listener
     *            The listener is used to be notified about the session related
     *            events
     * @throws IllegalArgumentException
     *             is thrown if bad arguments have been received
     */
    public void joinSessionAsync(String gwBusName, GatewayControllerSessionListener listener) {

        checkSessionValidity(gwBusName, listener);

        Log.d(TAG, "Join session asynchronously with the Gateway Management App: '" + gwBusName + "'");
        CommunicationUtil.joinSessionAsync(bus, gwBusName, listener);
    }

    /**
     * Disconnect the given session
     *
     * @param sessionId
     *            The session id to disconnect
     * @return Returns the leave session {@link Status}
     */
    public Status leaveSession(int sessionId) {

        Log.d(TAG, "Leave the session: '" + sessionId + "'");
        return CommunicationUtil.leaveSession(bus, sessionId);
    }

    /**
     * Set the {@link GatewayMgmtAppListener} to be notified about the Announcement signals
     * received from a {@link GatewayMgmtApp}
     *
     * @param handler
     *            {@link GatewayMgmtAppListener}
     * @throws IllegalArgumentException
     *             If the received handler is NULL
     * @throws IllegalStateException
     *             If the {@link GatewayController} hasn't been initialized
     */
    public void setAnnounceListener(GatewayMgmtAppListener handler) {

        if (handler == null) {

            throw new IllegalArgumentException("Received an undefined handler");
        }

        if (announceManager == null) {

            throw new IllegalStateException("GatewayController hasn't been initialized");
        }

        announceManager.setGatewayMgmtAppListener(handler);
    }

    /**
     * @return Returns list of {@link GatewayMgmtApp}s found on the network
     * @throws IllegalStateException
     *          If the {@link GatewayController} hasn't been initialized
     */
    public List<GatewayMgmtApp> getGatewayMgmtApps() {

        if (announceManager == null) {

            throw new IllegalStateException("GatewayController hasn't been initialized");
        }

        return announceManager.getGateways();
    }

    /**
     * Checks {@link BusAttachment} validity. It should be not null and
     * connected
     *
     * @throws IllegalStateException
     */
    private void checkBusValidity(BusAttachment bus) {

        if (this.bus != null) {
            throw new IllegalStateException("The service has been already initialized");
        }

        if (bus == null) {
            throw new IllegalStateException("Not initialized BusAttachment has been received");
        }

        if (!bus.isConnected()) {
            throw new IllegalStateException("The received BusAttachment is not connected to the AllJoyn daemon");
        }
    }

    /**
     * Check validity of the received arguments
     *
     * @param gwBusName
     *            The bus name of the Gateway Management App to connect to.
     * @param listener
     *            Session events listener
     * @throws IllegalArgumentException
     *             If bad arguments have been received
     */
    private void checkSessionValidity(String gwBusName, GatewayControllerSessionListener listener) {

        if (gwBusName == null || gwBusName.length() == 0) {
            throw new IllegalArgumentException("The given gwBusName is undefined");
        }

        if (listener == null) {
            throw new IllegalArgumentException("The given listener is undefined");
        }
    }
}
