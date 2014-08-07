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
import org.alljoyn.gatewaycontroller.activity.ConnectorAppActivity;
import org.alljoyn.gatewaycontroller.sdk.Acl;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Switch;
import android.widget.TextView;

/**
 * Manages the list of {@link VisualAcl}s
 */
public class ConnectorAppAclsAdapter extends VisualArrayAdapter {
    private static String TAG = "gwcapp" + ConnectorAppAclsAdapter.class.getSimpleName();

    static class AclView {

        TextView aclName;
        Switch isActive;
    }

    // =======================================//

    /**
     * Constructor
     */
    ConnectorAppAclsAdapter(Context context) {

        this(context, -1, null);
    }

    /**
     * Constructor
     * 
     * @param context
     * @param viewItemResId
     * @param itemsList
     */
    public ConnectorAppAclsAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {

        super(context, viewItemResId, itemsList);
    }

    /**
     * @see android.widget.ArrayAdapter#getView(int, android.view.View,
     *      android.view.ViewGroup)
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        View row = convertView;
        AclView aclView;

        if (row == null) {

            row = inflater.inflate(viewItemResId, parent, false);

            aclView          = new AclView();
            aclView.aclName  = (TextView) row.findViewById(R.id.connectorAclName);
            aclView.isActive = (Switch) row.findViewById(R.id.connectorAclActiveSwitch);

            row.setTag(aclView);
        } else {

            aclView = (AclView) row.getTag();
        }

        final VisualAcl visualAcl   = (VisualAcl) getItem(position);
        final Acl acl = visualAcl.getAcl();

        aclView.aclName.setText(acl.getName());

        visualAcl.updateActivityStatus();

        aclView.isActive.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

                if (visualAcl.isActive() == isChecked) {
                    return;
                }

                Log.d(TAG, "The state of the ACL name: '" + acl.getName() + "' changed to isActive: '" + isChecked + "'");
                ((ConnectorAppActivity) context).changeAclActiveStatus(visualAcl, buttonView, isChecked);
            }
        });

        aclView.isActive.setChecked(visualAcl.isActive());

        return row;
    }
}
