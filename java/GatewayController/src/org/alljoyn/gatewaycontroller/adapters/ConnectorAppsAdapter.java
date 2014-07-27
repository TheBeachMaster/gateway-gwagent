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

import org.alljoyn.gatewaycontroller.GWControllerSampleApplication;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.activity.ConnectorApplicationActivity;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplication;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplicationStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplicationStatus.ConnectionStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplicationStatus.InstallStatus;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplicationStatus.OperationalStatus;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

/**
 * Manages the list of {@link VisualConnectorApplication}s
 */
public class ConnectorAppsAdapter extends VisualArrayAdapter {
    private static final String TAG = "gwcapp" + ConnectorAppsAdapter.class.getSimpleName();

    static class ConnectorAppView {

        Button appName;
        TextView connStatus;
        TextView operStatus;
        TextView installStatus;
    }

    // ======================================//

    /**
     * Constructor
     * 
     * @param context
     */
    ConnectorAppsAdapter(Context context) {
        this(context, -1, null);
    }

    /**
     * Constructor
     * 
     * @param context
     * @param viewItemResId
     * @param itemsList
     */
    public ConnectorAppsAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {

        super(context, viewItemResId, itemsList);
    }

    /**
     * @see android.widget.ArrayAdapter#getView(int, android.view.View,
     *      android.view.ViewGroup)
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        View row = convertView;
        ConnectorAppView connAppView;

        if (row == null) {

            row = inflater.inflate(viewItemResId, parent, false);

            connAppView               = new ConnectorAppView();
            connAppView.appName       = (Button) row.findViewById(R.id.connectorAppName);
            connAppView.connStatus    = (TextView) row.findViewById(R.id.connectorAppConnStatus);
            connAppView.installStatus = (TextView) row.findViewById(R.id.connectorAppInstallStatus);
            connAppView.operStatus    = (TextView) row.findViewById(R.id.connectorAppOperStatus);

            row.setTag(connAppView);
        } else {
            connAppView = (ConnectorAppView) row.getTag();
        }

        final VisualConnectorApplication visApp = (VisualConnectorApplication) getItem(position);

        connAppView.appName.setText(visApp.getApp().getFriendlyName());
        connAppView.appName.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

                ConnectorApplication selectedApp = visApp.getApp();
                Log.d(TAG, "Selected Connector Application, id: '" + selectedApp.getAppId() + "'");
                ((GWControllerSampleApplication) context.getApplicationContext()).setSelectedApp(selectedApp);

                Intent intent = new Intent(context, ConnectorApplicationActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(intent);
            }
        });

        ConnectorApplicationStatus connAppStatus = visApp.getAppStatus();
        updateStatus(connAppView.connStatus, connAppView.operStatus, connAppView.installStatus, connAppStatus);

        return row;
    }

    /**
     * Updates received {@link TextView} of the statuses with the data received
     * in the {@link ConnectorApplicationStatus}
     * 
     * @param connStatusTv
     * @param operStatusTv
     * @param installStatusTv
     * @param status
     */
    public static void updateStatus(TextView connStatusTv, TextView operStatusTv, TextView installStatusTv, ConnectorApplicationStatus connAppStatus) {

        final String UNKNOWN_STATUS = "unknown";

        if (connAppStatus == null) {

            connStatusTv.setText(UNKNOWN_STATUS);
            installStatusTv.setText(UNKNOWN_STATUS);
            operStatusTv.setText(UNKNOWN_STATUS);

            return;
        }

        ConnectionStatus connStatus = connAppStatus.getConnectionStatus();
        int color                   = Color.parseColor(VisualConnectorApplication.getConnStatusColor(connStatus));
        connStatusTv.setText(connStatus.DESC);
        connStatusTv.setTextColor(color);

        InstallStatus instStatus = connAppStatus.getInstallStatus();
        color                    = Color.parseColor(VisualConnectorApplication.getInstallStatusColor(instStatus));
        installStatusTv.setText(instStatus.DESC);
        installStatusTv.setTextColor(color);

        OperationalStatus operStatus = connAppStatus.getOperationalStatus();
        color                        = Color.parseColor(VisualConnectorApplication.getOperationalStatusColor(operStatus));
        operStatusTv.setText(operStatus.DESC);
        operStatusTv.setTextColor(color);
    }
}
