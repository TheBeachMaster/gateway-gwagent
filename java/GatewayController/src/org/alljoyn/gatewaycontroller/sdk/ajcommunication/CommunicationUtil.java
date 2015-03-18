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

package org.alljoyn.gatewaycontroller.sdk.ajcommunication;

import java.util.Locale;
import java.util.UUID;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Mutable;
import org.alljoyn.bus.Mutable.IntegerValue;
import org.alljoyn.bus.OnJoinSessionListener;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;

import android.util.Log;

/**
 * Utility class for AllJoyn communication
 */
public class CommunicationUtil {
    private static final String TAG = "gwc" + CommunicationUtil.class.getSimpleName();

    /**
     * Session join response object
     */
    public static class SessionResult {

        /**
         * Status of joining session
         */
        private final Status status;

        /**
         * Session id
         */
        private final int sid;

        /**
         * Constructor
         * 
         * @param status
         *            Status of joining the session
         * @param sid
         *            Session id
         */
        SessionResult(Status status, int sid) {
            this.status = status;
            this.sid = sid;
        }

        /**
         * @return Status of joining the session
         */
        public Status getStatus() {
            return status;
        }

        /**
         * @return Session id after joining the session. Join session status
         *         should be checked before using the session id
         */
        public int getSid() {
            return sid;
        }

        /**
         * @see java.lang.Object#toString()
         */
        @Override
        public String toString() {
            return "SessionResult [status='" + status + "', sid='" + sid + "']";
        }
    }

    // ==========================================//

    static class ControllerOnJoinSessionListener extends OnJoinSessionListener {

        /**
         * @see org.alljoyn.bus.OnJoinSessionListener#onJoinSession(org.alljoyn.bus.Status,
         *      int, org.alljoyn.bus.SessionOpts, java.lang.Object)
         */
        @Override
        public void onJoinSession(Status status, int sessionId, SessionOpts opts, Object context) {

            GatewayController.getInstance().getBusAttachment().enableConcurrentCallbacks();

            if (!(context instanceof GatewayControllerSessionListener)) {
                Log.wtf(TAG, "Received OnJoinSession with a wrong Context object");
                return;
            }

            GatewayControllerSessionListener listener = (GatewayControllerSessionListener) context;
            SessionResult result = new SessionResult(status, sessionId);
            listener.sessionJoined(result);
        }
    }

    // ==========================================//

    private CommunicationUtil() {
    }

    /**
     * Synchronously join session with the given busName. Use the given listener
     * to notify about session related events
     * 
     * @param bus
     *            {@link BusAttachment} to use to join session
     * @param busName
     *            Connect to this bus name
     * @param listener
     * @return {@link SessionResult}
     */
    public static SessionResult joinSession(BusAttachment bus, String busName, GatewayControllerSessionListener listener) {

        Mutable.IntegerValue sid = new IntegerValue();
        Status status = bus.joinSession(busName, GatewayController.PORT_NUM, sid, getSessionOpts(),
                                           listener);

        SessionResult result = new SessionResult(status, sid.value);
        return result;
    }

    /**
     * Asynchronously join session with the given busName. Use the given
     * listener to notify about session related events
     * 
     * @param bus
     *            {@link BusAttachment} to use to join session
     * @param busName
     *            Connect to this bus name
     * @param listener
     */
    public static void joinSessionAsync(BusAttachment bus, String busName, GatewayControllerSessionListener listener) {

        Status status = bus.joinSession(busName, GatewayController.PORT_NUM, getSessionOpts(), listener, new ControllerOnJoinSessionListener(), listener);

        if (status != Status.OK) {
            Log.d(TAG, "Async join session failed, SessionResult with the status: '" + status + "'");
            SessionResult result = new SessionResult(status, 0);
            listener.sessionJoined(result);
        }
    }

    /**
     * Leave the session with the given sid
     * 
     * @param bus
     * @param sid
     * @return {@link Status}
     */
    public static Status leaveSession(BusAttachment bus, int sid) {
        return bus.leaveSession(sid);
    }

    /**
     * Returns enum constant of the given enumToSearch class, if this
     * enumToSearch has a field of CODE that its value equals to the
     * checkedValue.
     * 
     * @param enumToSearch
     *            The class object of the enum to search the checkedValue.
     * @param checkedValue
     *            The value to be searched
     * @return The enum constant or NULL if the checkedValue wasn't found or the
     *         received enum doesn't have the CODE field
     */
    public static <T> T shortToEnum(Class<T> enumToSearch, short checkedValue) {

        if (!enumToSearch.isEnum()) {

            return null;
        }

        T[] enumValues = enumToSearch.getEnumConstants();

        for (T value : enumValues) {

            short code;

            try {
                code = value.getClass().getField("CODE").getShort(value);
            } catch (Exception e) {

                Log.d(TAG, "Failed to retrieve CODE field from the given enum");
                return null;
            }

            if (code == checkedValue) {
                return value;
            }
        }

        return null;
    }

    /**
     * Constructs a key string from the received deviceId and the appId
     * 
     * @param deviceId
     * @param appId
     * @return key
     */
    public static String getKey(String deviceId, UUID appId) {

        String appIdStr = appId.toString().replace("-", "");
        return deviceId + "_" + appIdStr.toUpperCase(Locale.ENGLISH);
    }

    /**
     * @return {@link SessionOpts}
     */
    private static SessionOpts getSessionOpts() {
        
        SessionOpts sessionOpts  = new SessionOpts();
        sessionOpts.traffic      = SessionOpts.TRAFFIC_MESSAGES; // Use reliable message-based communication to move data between session endpoints
        sessionOpts.isMultipoint = false;                   // A session is multi-point if it can  be joined multiple times
        sessionOpts.proximity    = SessionOpts.PROXIMITY_ANY;  // Holds the proximity for this SessionOpt
        sessionOpts.transports   = SessionOpts.TRANSPORT_ANY; // Holds the allowed transports for this SessionOpt
        return sessionOpts;
    }// getSessionOpts
}
