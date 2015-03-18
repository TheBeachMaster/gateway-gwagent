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

package org.alljoyn.gatewaycontroller.adapters;

import java.util.EnumMap;
import java.util.Map;

import org.alljoyn.gatewaycontroller.sdk.ConnectorApp;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus.ConnectionStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus.InstallStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus.OperationalStatus;

/**
 * {@link ConnectorApp} to be visualized by the
 * {@link ConnectorAppsAdapter}
 */
public class VisualConnectorApp implements VisualItem {

    /**
     * {@link ConnectionStatus} color
     */
    private static Map<ConnectionStatus, String> connStatusColor = new EnumMap<ConnectionStatus, String>(ConnectionStatus.class);

    /**
     * {@link InstallStatus} color
     */
    private static Map<InstallStatus, String> installStatusColor = new EnumMap<InstallStatus, String>(InstallStatus.class);

    /**
     * {@link OperationalStatus} color
     */
    private static Map<OperationalStatus, String> operationalStatusColor = new EnumMap<OperationalStatus, String>(OperationalStatus.class);

    static {

        connStatusColor.put(ConnectionStatus.GW_CS_NOT_INITIALIZED, "#61210B");
        connStatusColor.put(ConnectionStatus.GW_CS_IN_PROGRESS, "#0404B4");
        connStatusColor.put(ConnectionStatus.GW_CS_CONNECTED, "#088A08");
        connStatusColor.put(ConnectionStatus.GW_CS_NOT_CONNECTED, "#F7750C");
        connStatusColor.put(ConnectionStatus.GW_CS_ERROR, "#DF0101");

        installStatusColor.put(InstallStatus.GW_IS_INSTALLED, "#088A08");
        installStatusColor.put(InstallStatus.GW_IS_INSTALL_IN_PROGRESS, "#0404B4");
        installStatusColor.put(InstallStatus.GW_IS_UPGRADE_IN_PROGRESS, "#0404B4");
        installStatusColor.put(InstallStatus.GW_IS_UNINSTALL_IN_PROGRESS, "#0404B4");
        installStatusColor.put(InstallStatus.GW_IS_INSTALL_FAILED, "#DF0101");

        operationalStatusColor.put(OperationalStatus.GW_OS_RUNNING, "#088A08");
        operationalStatusColor.put(OperationalStatus.GW_OS_STOPPED, "#F7750C");
    }

    // =========================================//

    /**
     * {@link ConnectorApp}
     */
    private ConnectorApp app;

    /**
     * {@link ConnectorAppStatus}
     */
    private ConnectorAppStatus appStatus;

    /**
     * Constructor
     * 
     * @param app
     * @param appStatus
     */
    public VisualConnectorApp(ConnectorApp app, ConnectorAppStatus appStatus) {

        this.app       = app;
        this.appStatus = appStatus;
    }

    /**
     * Get connection status color
     * 
     * @param status
     * @return
     */
    public static String getConnStatusColor(ConnectionStatus status) {
        return connStatusColor.get(status);
    }

    /**
     * @return {@link ConnectorAppStatus}
     */
    public ConnectorAppStatus getAppStatus() {
        return appStatus;
    }

    /**
     * @param appStatus
     *            {@link ConnectorAppStatus}
     */
    public void setAppStatus(ConnectorAppStatus appStatus) {
        this.appStatus = appStatus;
    }

    /**
     * Get intallation status color
     * 
     * @param status
     * @return
     */
    public static String getInstallStatusColor(InstallStatus status) {
        return installStatusColor.get(status);
    }

    /**
     * Get operational status color
     * 
     * @param status
     * @return
     */
    public static String getOperationalStatusColor(OperationalStatus status) {
        return operationalStatusColor.get(status);
    }

    /**
     * @return {@link ConnectorApp}
     */
    public ConnectorApp getApp() {
        return app;
    }

    /**
     * Call clean on {@link ConnectorApp}
     */
    public void clear() {
        app.clear();
    }
}
