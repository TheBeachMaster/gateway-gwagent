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

package org.alljoyn.gatewaycontroller.activity;

import java.util.ArrayList;
import java.util.List;

import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.adapters.ManifestRulesAdapter;
import org.alljoyn.gatewaycontroller.adapters.VisualItem;
import org.alljoyn.gatewaycontroller.adapters.VisualManifestItem;
import org.alljoyn.gatewaycontroller.adapters.VisualManifestItem.ItemType;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;
import org.alljoyn.gatewaycontroller.sdk.ManifestRules;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

/**
 * The class presents the manifest interfaces {@link Fragment} of the
 * {@link ConnectorApplicationManifestActivity}
 */
public class ConnectorApplicationManifestRulesFragment extends Fragment {

    /**
     * Exposed services to be presented
     */
    private ListView exposedServicesListView;

    /**
     * Remoted services to presented
     */
    private ListView remotedServicesListView;

    /**
     * Exposed services adapter
     */
    private ManifestRulesAdapter exposedServicesAdapter;

    /**
     * Remoted services adapter
     */
    private ManifestRulesAdapter remotedServicesAdapter;

    /**
     * {@link ManifestRules} to be presented
     */
    private ManifestRules rules;

    /**
     * Constructor
     */
    public ConnectorApplicationManifestRulesFragment() {
    }

    /**
     * !!! IMPORTANT !!! Use this method to create the {@link Fragment} object
     * of this class.
     * 
     * @param rules
     *            {@link ManifestRules}
     * @return {@link ConnectorApplicationManifestRulesFragment}
     */
    public static ConnectorApplicationManifestRulesFragment createInstance(ManifestRules rules) {

        ConnectorApplicationManifestRulesFragment frg = new ConnectorApplicationManifestRulesFragment();
        frg.rules = rules;

        return frg;
    }

    /**
     * @see android.app.Fragment#onCreate(android.os.Bundle)
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        this.setRetainInstance(true);
    }

    /**
     * @see android.app.Fragment#onCreateView(android.view.LayoutInflater,
     *      android.view.ViewGroup, android.os.Bundle)
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View frgView = inflater.inflate(R.layout.connector_manifest_rules_fragment, container, false);

        exposedServicesListView = (ListView) frgView.findViewById(R.id.connectorAppManifestRulesExpServicesRules);
        remotedServicesListView = (ListView) frgView.findViewById(R.id.connectorAppManifestRulesRemServicesRules);

        exposedServicesListView.setEmptyView(frgView.findViewById(R.id.connectorAppManifestRulesExpServicesRulesNotFound));

        remotedServicesListView.setEmptyView(frgView.findViewById(R.id.connectorAppManifestRulesRemServicesRulesNotFound));

        exposedServicesAdapter = new ManifestRulesAdapter(getActivity(), R.layout.connector_manifest_rules_objectpath_item, new ArrayList<VisualItem>());

        exposedServicesListView.setAdapter(exposedServicesAdapter);

        remotedServicesAdapter = new ManifestRulesAdapter(getActivity(), R.layout.connector_manifest_rules_objectpath_item, new ArrayList<VisualItem>());

        remotedServicesListView.setAdapter(remotedServicesAdapter);

        populateVisualData();

        return frgView;
    }

    /**
     * Fills the adapters
     */
    private void populateVisualData() {

        fillAdapter(exposedServicesAdapter, rules.getExposedServices());
        exposedServicesAdapter.notifyDataSetChanged();

        fillAdapter(remotedServicesAdapter, rules.getRemotedServices());
        remotedServicesAdapter.notifyDataSetChanged();
    }

    /**
     * Fills the given adapter with the given rules
     * 
     * @param adapter
     * @param manRules
     */
    private void fillAdapter(ManifestRulesAdapter adapter, List<ManifestObjectDescription> manRules) {

        for (ManifestObjectDescription rule : manRules) {

            adapter.add(new VisualManifestItem(rule.getObjectPath(), ItemType.OBJECT_PATH));

            for (ConnAppInterface iface : rule.getInterfaces()) {

                adapter.add(new VisualManifestItem(iface, ItemType.INTERFACE));
            }
        }
    }// fillAdapter

}
