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
import java.util.UUID;

/**
 * The application that may be reached by the Gateway Connector Application
 * via the configured interfaces and object paths
 */
public class RemotedApp extends DiscoveredApp {
	
	/**
	 * Configuration of the object paths and interfaces that are
	 * used by the Gateway Connector Application to reach this remoted application
	 */
	private final List<ManifestObjectDescription> objDescRules;

	/**
	 * Constructor 
	 * @param appName The name of the application
	 * @param appId The application id
	 * @param deviceName The name of the device
	 * @param deviceId The device id
	 * @param objDescRules object path and interfaces that are used for {@link AccessRules} creation
	 * @throws IllegalArgumentException If bad arguments have been received
	 */
	public RemotedApp(String appName, UUID appId,
			String deviceName, String deviceId, List<ManifestObjectDescription> objDescRules) {
		
		super(null, appName, appId, deviceName, deviceId);
		
		if ( deviceId == null || deviceId.length() == 0) {
			throw new IllegalArgumentException("DeviceId is undefined");
		}
		
		if ( deviceName == null || deviceName.length() == 0 ) {
			throw new IllegalArgumentException("DeviceName is undefined");
		}
		
		if ( appId == null ) {
			throw new IllegalArgumentException("AppId is undefined");
		}
		
		if ( appName == null || appName.length() == 0 ) {
			throw new IllegalArgumentException("AppName is undefined");
		}
		
		if ( objDescRules == null ) {
			throw new IllegalArgumentException("objDescRules is undefined");
		}
		
		this.objDescRules = objDescRules;
	}

	/**
	 * Constructor
	 * @param discoveredApp The {@link DiscoveredApp} to be used to build this {@link RemotedApp}
	 * @param objDescRules object path and interfaces that are used for {@link AccessRules} creation
	 * @throws IllegalArgumentException If bad arguments have been received
	 */
	public RemotedApp(DiscoveredApp discoveredApp, List<ManifestObjectDescription> objDescRules) {
		
		this(discoveredApp.getAppName(), discoveredApp.getAppId(), 
				  discoveredApp.getDeviceName(), discoveredApp.getDeviceId(), objDescRules);
	}

	/**
	 * Configuration of the object paths and interfaces that are
	 * used by the Gateway Connector Application to reach this remoted application
	 * @return List of {@link ManifestObjectDescription} objects
	 */
	public List<ManifestObjectDescription> getObjDescRules() {
		return objDescRules;
	}
	
	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		
		StringBuilder sb = new StringBuilder("RemotedApp [");
		sb.append("appName='").append( getAppName() ).append("',")
		  .append("appId='").append( getAppId() ).append("',")
		  .append("deviceName='").append( getDeviceName() ).append("',")
		  .append("deviceId='").append( getDeviceId() ).append("']");
		
		return sb.toString();
	}
}
