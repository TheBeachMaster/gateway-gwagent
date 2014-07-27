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

import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ApplicationStatusAJ;

/**
 * The information about the state of the Gateway Connector Application
 */
public class ConnectorApplicationStatus {

    /**
     * Connection status of the Gateway Connector Application to the cloud
     * service
     */
    public static enum ConnectionStatus {

        GW_CS_NOT_INITIALIZED("Not initialized", (short) 0), 
        GW_CS_IN_PROGRESS("In progress", (short) 1),
        GW_CS_CONNECTED("Connected", (short) 2),
        GW_CS_NOT_CONNECTED("Not connected", (short) 3),
        GW_CS_ERROR("Error", (short) 4),
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
        private ConnectionStatus(String desc, short code) {

            DESC = desc;
            CODE = code;
        }
    }

    // =============================================//

    /**
     * The installation status of the Gateway Connector Application
     */
    public static enum InstallStatus {

        GW_IS_INSTALLED("Installed", (short) 0),
        GW_IS_INSTALL_IN_PROGRESS("Installing", (short) 1), 
        GW_IS_UPGRADE_IN_PROGRESS("Upgrading", (short) 2),
        GW_IS_UNINSTALL_IN_PROGRESS("Uninstalling", (short) 3),
        GW_IS_INSTALL_FAILED("Failed", (short) 4);

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
        private InstallStatus(String desc, short code) {

            DESC = desc;
            CODE = code;
        }

    }

    // =============================================//

    /**
     * The operational status of the Gateway Connector Application
     */
    public static enum OperationalStatus {

        GW_OS_RUNNING("Running", (short) 0),
        GW_OS_STOPPED("Stopped", (short) 1),
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
         *            The status description
         * @param code
         *            The status code
         */
        private OperationalStatus(String desc, short code) {

            DESC = desc;
            CODE = code;
        }

    }

    // =============================================//

    /**
     * The restart status of the Gateway Connector Application
     */
    public static enum RestartStatus {

        GW_RESTART_APP_RC_SUCCESS("Restarted", (short) 0),
        GW_RESTART_APP_RC_INVALID("Failed", (short) 1),
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
         *            The status description
         * @param code
         *            The status code
         */
        private RestartStatus(String desc, short code) {

            DESC = desc;
            CODE = code;
        }

    }

    // =============================================//

    /**
     * Installation status
     */
    private final InstallStatus installStatus;

    /**
     * Installation description
     */
    private final String installDescription;

    /**
     * Connection status of the Gateway Connector Application to its cloud
     * service
     */
    private final ConnectionStatus connectionStatus;

    /**
     * The state whether the Gateway Connector Application is working
     */
    private final OperationalStatus operationalStatus;

    /**
     * Constructor
     * 
     * @throws GatewayControllerException
     *             If failed to unmarshal the status
     */
    ConnectorApplicationStatus(ApplicationStatusAJ appStatusAJ) throws GatewayControllerException {

        this(appStatusAJ.installStatus, appStatusAJ.installDesc, appStatusAJ.connectionStatus,
                appStatusAJ.operationalStatus);
    }

    /**
     * Constructor
     * 
     * @param installStatus
     * @param installDescription
     * @param connectionStatus
     * @param operationalStatus
     * @throws GatewayControllerException
     */
    ConnectorApplicationStatus(short installStatus, String installDescription, short connectionStatus, 
                                   short operationalStatus) throws GatewayControllerException {

        this.installDescription = installDescription;
        this.installStatus      = CommunicationUtil.shortToEnum(InstallStatus.class, installStatus);

        if (this.installStatus == null) {
            throw new GatewayControllerException("The unknown install status has been received: '" + installStatus + "'");
        }

        this.connectionStatus = CommunicationUtil.shortToEnum(ConnectionStatus.class, connectionStatus);

        if (this.connectionStatus == null) {
            throw new GatewayControllerException("The unknown connection status has been received: '" + connectionStatus + "'");
        }

        this.operationalStatus = CommunicationUtil.shortToEnum(OperationalStatus.class, operationalStatus);

        if (this.operationalStatus == null) {
            throw new GatewayControllerException("The unknown operational status has been received: '" + operationalStatus + "'");
        }
    }

    /**
     * @return The installation status of the Gateway Connector Application
     */
    public InstallStatus getInstallStatus() {
        return installStatus;
    }

    /**
     * @return The installation description of the Gateway Connector Application
     */
    public String getInstallDescriptions() {
        return installDescription;
    }

    /**
     * @return Connection status of the Gateway Connector Application to its
     *         cloud service
     */
    public ConnectionStatus getConnectionStatus() {
        return connectionStatus;
    }

    /**
     * @return The state whether the Gateway Connector Application is running
     */
    public OperationalStatus getOperationalStatus() {
        return operationalStatus;
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        return "ConnectorApplicationStatus [installStatus='" + installStatus + "', " + "installDescription='" +
                installDescription + "', " + "connectionStatus='" + connectionStatus + "', " + 
                "operationalStatus='" + operationalStatus + "']";
    }
}