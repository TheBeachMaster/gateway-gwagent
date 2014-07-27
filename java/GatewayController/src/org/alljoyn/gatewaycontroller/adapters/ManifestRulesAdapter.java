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

import java.util.List;

import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.adapters.VisualManifestItem.ItemType;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppObjectPath;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

/**
 * Manages the list of {@link VisualManifestItem}
 */
public class ManifestRulesAdapter extends VisualArrayAdapter {

    static class ManifestItemView {

        TextView friendlyName;
        TextView name;
        TextView boolState;
    }

    // ==========================================//

    /**
     * Constructor
     * 
     * @param context
     */
    ManifestRulesAdapter(Context context) {

        this(context, -1, null);
    }

    /**
     * Constructor
     * 
     * @param context
     * @param viewItemResId
     * @param itemsList
     */
    public ManifestRulesAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {

        super(context, viewItemResId, itemsList);
    }

    /**
     * @see android.widget.ArrayAdapter#getView(int, android.view.View,
     *      android.view.ViewGroup)
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        View row = convertView;
        ManifestItemView manView;

        final VisualManifestItem visItem = (VisualManifestItem) getItem(position);

        manView = new ManifestItemView();

        if (visItem.getType() == ItemType.OBJECT_PATH) {

            row = inflater.inflate(R.layout.connector_manifest_rules_objectpath_item, parent, false);

            manView.friendlyName = (TextView) row.findViewById(R.id.connectorAppManifestRulesObjPathFriendlyName);
            manView.name         = (TextView) row.findViewById(R.id.connectorAppManifestRulesObjPathName);
            manView.boolState    = (TextView) row.findViewById(R.id.connectorAppManifestRulesObjPathIsPrefix);
        } else if (visItem.getType() == ItemType.INTERFACE) {

            row = inflater.inflate(R.layout.connector_manifest_rules_interface_item, parent, false);

            manView.friendlyName = (TextView) row.findViewById(R.id.connectorAppManifestRulesInterfaceFriendlyName);
            manView.name         = (TextView) row.findViewById(R.id.connectorAppManifestRulesInterfaceName);
            manView.boolState    = (TextView) row.findViewById(R.id.connectorAppManifestRulesInterfaceIsSecured);
        }

        populateData(manView, visItem);
        return row;
    }

    /**
     * Populates the {@link ManifestItemView} with the data
     * 
     * @param manView
     * @param visItem
     */
    private void populateData(ManifestItemView manView, VisualManifestItem visItem) {

        if (visItem.getType() == ItemType.OBJECT_PATH) {

            ConnAppObjectPath objPath = (ConnAppObjectPath) visItem.getVisualItem();

            manView.friendlyName.setText(objPath.getFriendlyName());
            manView.name.setText(objPath.getPath());

            String prefix = objPath.isPrefix() ? "Prefix" : "Not Prefix";
            manView.boolState.setText(prefix);
        } else if (visItem.getType() == ItemType.INTERFACE) {

            ConnAppInterface iface = (ConnAppInterface) visItem.getVisualItem();

            manView.friendlyName.setText(iface.getFriendlyName());
            manView.name.setText(iface.getName());

            String secured = iface.isSecured() ? "Secured" : "Not Secured";
            manView.boolState.setText(secured);
        }

    }
}
