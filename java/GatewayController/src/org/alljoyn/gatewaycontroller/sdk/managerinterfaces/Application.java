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

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;
import org.alljoyn.bus.annotation.BusProperty;
import org.alljoyn.bus.annotation.BusSignal;
import org.alljoyn.bus.annotation.Secure;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;

/**
 * AllJoyn interface for operating the Gateway Connector Application
 */
@BusInterface(name = Application.IFNAME)
@Secure
public interface Application {

    /**
     * AllJoyn name of the interface
     */
    public static final String IFNAME = GatewayController.IFACE_PREFIX + ".App";

    /**
     * The signal about the status change of the Gateway Connector Application
     */
    public static final String APPLICATION_STATUS_CHANGED = "AppStatusChanged";

    /**
     * Returns the state of the application
     * 
     * @return {@link ApplicationStatusAJ}
     * @throws BusException
     */
    @BusMethod(name = "GetAppStatus", replySignature = "qsqq")
    public ApplicationStatusAJ getApplicationStatus() throws BusException;

    /**
     * Returns lists of interfaces from the manifest of the application
     * 
     * @return {@link ManifestRulesAJ}
     * @throws BusException
     */
    @BusMethod(name = "GetManifestInterfaces", replySignature = "a((obs)a(ssb))a((obs)a(ssb))")
    public ManifestRulesAJ getManifestInterfaces() throws BusException;

    /**
     * Restarts the application
     * 
     * @return Application restart status
     * @throws BusException
     */
    @BusMethod(name = "RestartApp", replySignature = "q")
    public short restartApp() throws BusException;

    /**
     * Returns Manifest file of the application
     * 
     * @return The text of the application manifest
     * @throws BusException
     */
    @BusMethod(name = "GetManifestFile", replySignature = "s")
    public String getManifestFile() throws BusException;

    /**
     * The signal describing the change in the status of the application
     * 
     * @param statusCode
     */
    @BusSignal(name = APPLICATION_STATUS_CHANGED, signature = "qsqq")
    public void applicationStatusChanged(short installStatus, String installDescription, short connectionStatus, short operationalStatus) throws BusException;

    /**
     * @return The version of this interface
     * @throws BusException
     */
    @BusProperty(signature = "q")
    public short getVersion() throws BusException;
}
