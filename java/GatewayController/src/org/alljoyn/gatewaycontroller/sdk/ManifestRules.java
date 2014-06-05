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

import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.TPObjectPath;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestRulesAJ;

/**
 * The manifest rules of the Third Party Application
 */
public class ManifestRules {
	
	/**
	 * The interfaces that the Third Party Application exposes to its clients 
	 */
	private final List<ManifestObjectDescription> exposedServices; 
	
	/**
	 * The interfaces that the Third Party Application allows to remote
	 */
	private final List<ManifestObjectDescription> remotedServices;
	
	/**
	 * Constructor
	 * @param manifRulesAJ
	 */
	ManifestRules(ManifestRulesAJ manifRulesAJ) {
		exposedServices = new ArrayList<ManifestObjectDescription>(manifRulesAJ.exposedServices.length);
		remotedServices = new ArrayList<ManifestObjectDescription>(manifRulesAJ.remotedServices.length);
		
		convertList(manifRulesAJ.exposedServices, exposedServices);
		convertList(manifRulesAJ.remotedServices, remotedServices);
	}
	
	/**
	 * The {@link ManifestObjectDescription} objects that the Third Party Application 
	 * exposes to its clients
	 * @return List of exposed services
	 */
	public List<ManifestObjectDescription> getExposedServices() {
		return exposedServices;
	}

	/**
     * The {@link ManifestObjectDescription} objects that the Third Party Application
     * supports for being remoted
	 * @return List of remoted interfaces
	 */
	public List<ManifestObjectDescription> getRemotedServices() {
		return remotedServices;
	}

	/**
	 * Converts {@link ManifestObjectDescriptionAJ} into {@link ManifestObjectDescription}
	 * and fills with it the given list
	 */
	private void convertList(ManifestObjectDescriptionInfoAJ[] from, List<ManifestObjectDescription> to) {
		
		for ( ManifestObjectDescriptionInfoAJ objDescInfoAJ : from ) {
			
			ManifestObjectDescription mod = new ManifestObjectDescription(objDescInfoAJ);
			TPObjectPath objPath          = mod.getObjectPath();
			
			//If manifest object path isPrefix true, then isAllowedObjectPath is TRUE 
			objPath.setPrefixAllowed( objPath.isPrefix() );
			to.add(mod);
		}
	}

	
	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ManifestRules [exposedServices='" + exposedServices + "', remotedServices='" 
	                          + remotedServices + "']";
	}
}