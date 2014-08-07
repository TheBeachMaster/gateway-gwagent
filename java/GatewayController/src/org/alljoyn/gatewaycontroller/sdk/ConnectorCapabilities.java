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

import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleObjectPath;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestObjectDescriptionInfoAJ;
import org.alljoyn.gatewaycontroller.sdk.managerinterfaces.ManifestRulesAJ;

/**
 * The manifest rules of the Gateway Connector Application
 */
public class ConnectorCapabilities {

    /**
     * The interfaces that the Gateway Connector Application exposes to its
     * clients
     */
    private final List<RuleObjectDescription> exposedServices;

    /**
     * The interfaces that the Gateway Connector Application allows to remote
     */
    private final List<RuleObjectDescription> remotedServices;

    /**
     * Constructor
     * 
     * @param manifRulesAJ
     */
    ConnectorCapabilities(ManifestRulesAJ manifRulesAJ) {
        exposedServices = new ArrayList<RuleObjectDescription>(manifRulesAJ.exposedServices.length);
        remotedServices = new ArrayList<RuleObjectDescription>(manifRulesAJ.remotedServices.length);

        convertList(manifRulesAJ.exposedServices, exposedServices);
        convertList(manifRulesAJ.remotedServices, remotedServices);
    }

    /**
     * The {@link RuleObjectDescription} objects that the Gateway Connector
     * Application exposes to its clients
     * 
     * @return List of exposed services
     */
    public List<RuleObjectDescription> getExposedServices() {
        return exposedServices;
    }

    /**
     * The {@link RuleObjectDescription} objects that the Gateway Connector
     * Application supports for being remoted
     * 
     * @return List of remoted interfaces
     */
    public List<RuleObjectDescription> getRemotedServices() {
        return remotedServices;
    }

    /**
     * Converts {@link ManifestObjectDescriptionAJ} into
     * {@link RuleObjectDescription} and fills with it the given list
     */
    private void convertList(ManifestObjectDescriptionInfoAJ[] from, List<RuleObjectDescription> to) {

        for (ManifestObjectDescriptionInfoAJ objDescInfoAJ : from) {

            RuleObjectDescription mod = new RuleObjectDescription(objDescInfoAJ);
            RuleObjectPath objPath = mod.getObjectPath();

            // If manifest object path isPrefix true, then isAllowedObjectPath is TRUE
            objPath.setPrefixAllowed(objPath.isPrefix());
            to.add(mod);
        }
    }

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "ManifestRules [exposedServices='" + exposedServices + "', remotedServices='" + 
                    remotedServices + "']";
    }
}