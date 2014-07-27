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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.alljoyn.gatewaycontroller.sdk.AccessControlList;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppObjectPath;
import org.alljoyn.gatewaycontroller.sdk.RemotedApp;

import android.app.Activity;
import android.content.Context;
import android.widget.BaseExpandableListAdapter;

/**
 * {@link AccessControlList} configurable item. Either "Exposed Services" or
 * {@link RemotedApp}
 */
public class VisualAclConfigurableItem implements VisualItem {

    /**
     * The configurable item
     */
    private Object confItem;

    /**
     * The item friendly name
     */
    private String label;

    /**
     * Adapter for the expandable list of interfaces and object paths
     */
    private AclMgmtConfigurableRulesAdapter adapter;

    /**
     * Gets TRUE if all the configurable rules are allowed
     */
    private boolean allowAllSelected;

    /**
     * Constructor
     * 
     * @param activityContext
     *            The activity {@link Context}
     * @param confItem
     *            The configuration item. {@link RemotedApp} or {@link List} of
     *            {@link ManifestObjectDescription} of the "Exposed Services"
     * @param label
     *            The item label
     */
    public VisualAclConfigurableItem(Context activityContext, Object confItem, String label) {

        this.confItem = confItem;
        this.label    = label;
        adapter       = initAdapter(activityContext);
    }

    /**
     * @return TRUE whether this {@link VisualAclConfigurableItem} is a
     *         {@link RemotedApp}. Otherwise returns FALSE, which means that the
     *         {@link VisualAclConfigurableItem} is the exposed service.
     */
    public boolean isRemotedApp() {

        return (confItem instanceof RemotedApp);
    }

    /**
     * @return The configuration item. {@link RemotedApp} or {@link List} of
     *         {@link ManifestObjectDescription} of the "Exposed Services"
     */
    public Object getConfItem() {
        return confItem;
    }

    /**
     * @return The friendly name of the item
     */
    public String getLabel() {
        return label;
    }

    /**
     * @return Configurable items adapter
     */
    public BaseExpandableListAdapter getAdapter() {
        return adapter;
    }

    /**
     * Is allowed all selected for this item
     */
    public boolean isAllowAllSelected() {
        return allowAllSelected;
    }

    /**
     * Set the state of allowedAll
     * 
     * @param allowAllSelected
     */
    public void setAllowAllSelected(boolean allowAllSelected) {
        this.allowAllSelected = allowAllSelected;
    }

    /**
     * Initialize the adapter
     * 
     * @param context
     *            {@link Activity} context
     */
    private AclMgmtConfigurableRulesAdapter initAdapter(Context context) {

        return new AclMgmtConfigurableRulesAdapter(context, convertToVisualRules(extractConfigRules()));
    }

    /**
     * Returns the rules that are selected
     * 
     * @param allowAllChecked
     *            if checked than we relate to all the rules as checked
     * @return Selected rules
     */
    public List<ManifestObjectDescription> getSelectedRules() {

        return convertToManifObjDesc(adapter.getConfData());
    }

    /**
     * Creates Map of the configured rules from the received list of
     * {@link ManifestObjectDescription}
     * 
     * @param rules
     *            List of {@link ManifestObjectDescription}
     * @param interfaceNames
     *            The list of interfaces to be filled by this method with the
     *            names of the interfaces
     * @return Map from {@link VisualInterface} to a {@link List} of
     *         {@link VisualObjectPath}
     */
    private Map<VisualInterface, List<VisualObjectPath>> convertToVisualRules(List<ManifestObjectDescription> rules) {

        Map<VisualInterface, Set<VisualObjectPath>> resRules = new HashMap<VisualInterface, Set<VisualObjectPath>>();

        // The value of this map will be incremented by one each time when a
        // configured object path will be stored in the
        // set of the resRules. So if at the end the Integer value will be equal
        // to the size of resRules.Set, it means that the
        // interface may be marked as configured.
        Map<VisualInterface, Integer> ifaceConfStateInd = new HashMap<VisualInterface, Integer>();

        for (ManifestObjectDescription rule : rules) {

            for (ConnAppInterface iface : rule.getInterfaces()) {

                VisualInterface vIface = new VisualInterface(iface);
                Set<VisualObjectPath> visObjPaths = resRules.get(vIface);

                if (visObjPaths == null) {

                    visObjPaths = new HashSet<VisualObjectPath>();
                    resRules.put(vIface, visObjPaths);
                    ifaceConfStateInd.put(vIface, 0);
                }

                visObjPaths.add(new VisualObjectPath(rule.getObjectPath(), rule.isConfigured()));

                // Increase indicator counter only if the rule is configured,
                // which means that the inserted
                // object path is configured
                if (rule.isConfigured()) {
                    ifaceConfStateInd.put(vIface, ifaceConfStateInd.get(vIface) + 1);
                }

            }// for :: ifaces
        }// for:: rules

        Map<VisualInterface, List<VisualObjectPath>> visObjDescMap = new HashMap<VisualInterface, List<VisualObjectPath>>();
        for (VisualInterface vIface : resRules.keySet()) {

            Set<VisualObjectPath> visObjPaths = resRules.get(vIface);

            // Check whether VisualInterface should be marked as configured
            // The value was incremented by one for each inserted configured
            // object path
            if (ifaceConfStateInd.get(vIface) == visObjPaths.size()) {

                vIface.setConfigured(true);
            }

            visObjDescMap.put(vIface, Arrays.asList(visObjPaths.toArray(new VisualObjectPath[visObjPaths.size()])));
        }

        return visObjDescMap;
    }

    /**
     * Create list of {@link ManifestObjectDescription} from the given
     * {@link Map} of rules.
     * 
     * @param visualRules
     *            Map from {@link VisualInterface} to a {@link List} of
     *            {@link VisualObjectPath}
     * @param allowAllChecked
     *            if TRUE, then all the rules will be considered as configured
     *            (checked)
     * @return List of {@link ManifestObjectDescription}
     */
    private List<ManifestObjectDescription> convertToManifObjDesc(Map<VisualInterface, List<VisualObjectPath>> visualRules) {

        Map<ConnAppObjectPath, Set<ConnAppInterface>> resRules = new HashMap<ConnAppObjectPath, Set<ConnAppInterface>>();

        for (VisualInterface vIface : visualRules.keySet()) {

            List<VisualObjectPath> vObjPaths = visualRules.get(vIface);

            for (VisualObjectPath vop : vObjPaths) {

                ConnAppObjectPath objPath = vop.getObjPath();

                // If allowAllChecked is TRUE, then all the rules are considered
                // as configured.
                // If vIface is configured all its object paths considered as
                // configured.
                // If the vIface is not configured VisualObjectPath is checked.
                if (allowAllSelected || vIface.isConfigured() || vop.isConfigured()) {

                    Set<ConnAppInterface> ifaces = resRules.get(objPath);
                    if (ifaces == null) {

                        ifaces = new HashSet<ConnAppInterface>();
                        resRules.put(objPath, ifaces);
                    }

                    ifaces.add(vIface.getIface());
                }
            }// for :: VisualObjectPath
        }// for :: VisualReverseObjDesc

        List<ManifestObjectDescription> resModList = new ArrayList<ManifestObjectDescription>(resRules.size());
        for (ConnAppObjectPath objPath : resRules.keySet()) {

            resModList.add(new ManifestObjectDescription(objPath, resRules.get(objPath), true));
        }

        return resModList;
    }

    /**
     * @return Extracts rules to be configured in depends on the
     *         {@link VisualAclConfigurableItem#isRemotedApp()}
     */
    @SuppressWarnings("unchecked")
    private List<ManifestObjectDescription> extractConfigRules() {

        List<ManifestObjectDescription> configuredRules;

        if (isRemotedApp()) {

            configuredRules = ((RemotedApp) confItem).getObjDescRules();
        }
        // Exposed services
        else {

            configuredRules = (List<ManifestObjectDescription>) confItem;
        }

        return configuredRules;
    }
}
