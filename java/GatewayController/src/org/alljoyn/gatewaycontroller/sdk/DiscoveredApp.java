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

import java.util.Map;
import java.util.UUID;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.services.common.utils.TransportUtil;

import android.util.Log;

/**
 * The application that is discovered by receiving Announcement from devices on the network
 */
public class DiscoveredApp {
	private static final String TAG = "gwc" + DiscoveredApp.class.getSimpleName();
	
	/**
	 * The unique name of the {@link BusAttachment} that sent the Announcement signal
	 */
	private String busName;
	
	/**
	 * The name of the application that sent the announcement
	 */
	private String appName;

	/**
	 * The Id of the application that sent the announcement
	 */
	private UUID appId;
	
	/**
	 * The name of the device that the application belongs to
	 */
	private String deviceName;
	
	/**
	 * The id of the device that the application belongs to
	 */
	private String deviceId;

	/**
	 * Constructor 
	 * @param busName The name of the {@link BusAttachment} of the device that sent
	 * the Announcement
	 * @param appName The name of the application
	 * @param appId The application id
	 * @param deviceName The name of the device
	 * @param deviceId The device id
	 */
	public DiscoveredApp(String busName, String appName, UUID appId,
			String deviceName, String deviceId) {
		
		this.busName       = busName;
		this.appName       = appName;
		this.appId         = appId;
		this.deviceName    = deviceName;
		this.deviceId      = deviceId;
	}

	/**
	 * Constructor
	 * @param busName The name of the {@link BusAttachment} of the device that sent
	 * the Announcement
	 * @param aboutData The data sent with the Announcement
	 */
	public DiscoveredApp (String busName, Map<String, Variant> aboutData) {
		
		if ( aboutData == null ) {
			throw new IllegalArgumentException("Received undefined aboutData");
		}
		
		try {
			
			this.busName = busName;
			
			Variant var  = aboutData.get(AboutKeys.ABOUT_DEVICE_NAME);
			if ( var != null ) {
				deviceName = var.getObject(String.class); 
			}
			
			var = aboutData.get(AboutKeys.ABOUT_APP_NAME);
			if ( var != null ) {
				appName = var.getObject(String.class);
			}
			
			var = aboutData.get(AboutKeys.ABOUT_DEVICE_ID);
			if ( var != null ) {
				deviceId = var.getObject(String.class);
			}
			
			var = aboutData.get(AboutKeys.ABOUT_APP_ID);
			if ( var != null ) {
				byte[] appIdRaw  = var.getObject(byte[].class);
				UUID appId       = TransportUtil.byteArrayToUUID(appIdRaw);
				this.appId       = appId;
			}
		}
		catch (BusException be) {
			Log.e(TAG, "Failed to unmarshal values of the received Announcement from the bus: '" + busName + "'", be);
		}
	}
	
	/**
	 * @return The bus name
	 */
	public String getBusName() {
		return busName;
	}

	/**
	 * @return The name of the application that sent the announcement
	 */
	public String getAppName() {
		return appName;
	}

	/**
	 * @return Id of the application that sent the announcement
	 */
	public UUID getAppId() {
		return appId;
	}

	/**
	 * @return The name of the device that the application belongs to
	 */
	public String getDeviceName() {
		return deviceName;
	}

	/**
	 * @return The id of the device that the application belongs to
	 */
	public String getDeviceId() {
		return deviceId;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		
		StringBuilder sb = new StringBuilder("DiscoveredApp [busUniqueName=");
		sb.append("'").append(busName).append("',")
		.append("appName='").append(appName).append("',")
		.append("appId='").append(appId).append("',")
		.append("deviceName='").append(deviceName).append("',")
		.append("deviceId='").append(deviceId).append("']");
		
		return sb.toString();
	}
	
}