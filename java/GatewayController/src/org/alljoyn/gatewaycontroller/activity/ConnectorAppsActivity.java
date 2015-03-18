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
import org.alljoyn.gatewaycontroller.adapters.ConnectorAppsAdapter;
import org.alljoyn.gatewaycontroller.adapters.VisualConnectorApp;
import org.alljoyn.gatewaycontroller.adapters.VisualItem;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatusSignalHandler;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApp;
import org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;

import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

/**
 * Gateway Connector Application activity
 */
public class ConnectorAppsActivity extends BaseActivity implements OnClickListener, ConnectorAppStatusSignalHandler {

    private static final String TAG = "gwcapp" + ConnectorAppsActivity.class.getSimpleName();

    /**
     * Gateway name
     */
    private TextView gwNameTv;

    /**
     * Connector Applications list
     */
    private ListView connListView;

    /**
     * Adapter for the list of connector applications
     */
    private ConnectorAppsAdapter adapter;

    /**
     * Refresh button
     */
    private Button refreshBtn;

    /**
     * Asynchronous task to be executed
     */
    private AsyncTask<Void, Void, String> asyncTask;

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onCreate(android.os.Bundle)
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connector_apps);
    }

    /**
     * @see android.app.Activity#onStart()
     */
    @Override
    protected void onStart() {

        super.onStart();

        // Check existence of the selected gateway app
        if (app.getSelectedGatewayApp() == null) {

            Log.w(TAG, "Selected gateway app has been lost, handling");
            handleLostOfGateway();
            return;
        }

        gwNameTv     = (TextView) findViewById(R.id.gwNameTv);
        gwNameTv.setText(app.getSelectedGatewayApp().getAppName());

        adapter      = new ConnectorAppsAdapter(this, R.layout.connector_app_item, new ArrayList<VisualItem>());
        connListView = (ListView) findViewById(R.id.connectorAppsLv);
        connListView.setAdapter(adapter);
        connListView.setEmptyView(findViewById(R.id.connectorAppsLvNoApps));
        connListView.setSelector(android.R.color.transparent);

        refreshBtn = (Button) findViewById(R.id.connectorAppsRefresh);
        refreshBtn.setOnClickListener(this);

        retrieveApps();
    }

    /**
     * @see android.app.Activity#onStop()
     */
    @Override
    protected void onStop() {

        super.onStop();

        if (asyncTask != null) {
            asyncTask.cancel(true);
        }

        if (adapter == null) {
            return;
        }

        Log.d(TAG, "Cleaning applications");
        for (VisualItem vItem : adapter.getItemsList()) {

            VisualConnectorApp vConnectorApp = (VisualConnectorApp) vItem;

            if (vConnectorApp.getApp() != app.getSelectedConnectorApp()) {

                vConnectorApp.clear();
            }

        }
    }

    /**
     * @see android.view.View.OnClickListener#onClick(android.view.View)
     */
    @Override
    public void onClick(View clickedView) {

        switch (clickedView.getId()) {

            case R.id.connectorAppsRefresh: {
                retrieveApps();
                break;
            }
            case R.id.connectorAppsDiscoverApps: {
                break;
            }
        }
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSessionJoined()
     */
    @Override
    protected void onSessionJoined() {

        super.onSessionJoined();
        retrieveApps();
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onGatewayMgmtAnnounced()
     */
    @Override
    protected void onGatewayMgmtAppAnnounced() {

        super.onGatewayMgmtAppAnnounced();

        // Check that my Gateway Management App wasn't lost because of the GatewayMgmtAppAnnounced
        if (app.getSelectedGatewayApp() == null) {

            return;
        }

        Log.d(TAG, "GatewayListChanged was called, refreshing the activity");
        retrieveApps();
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSelectedGatewayLost()
     */
    @Override
    protected void onSelectedGatewayLost() {
        super.onSelectedGatewayLost();
        handleLostOfGateway();
    }

    /**
     * Search for the {@link ConnectorApp} with the given appId and
     * update its status
     *
     * @see org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatusSignalHandler#onStatusChanged(java.lang.String,
     *      org.alljoyn.gatewaycontroller.sdk.ConnectorAppStatus)
     */
    @Override
    public void onStatusChanged(final String appId, final ConnectorAppStatus status) {

        asyncTask = new AsyncTask<Void, Void, String>() {

            @Override
            protected String doInBackground(Void... params) {

                Log.d(TAG, "Searching for the app: '" + appId + "', to update the status to: '" + status + "'");

                for (VisualItem vItem : adapter.getItemsList()) {

                    VisualConnectorApp vApp = (VisualConnectorApp) vItem;

                    if (!vApp.getApp().getAppId().equals(appId)) {
                        continue;
                    }

                    vApp.setAppStatus(status);
                    break;
                }

                return null;
            }

            @Override
            protected void onPostExecute(String result) {

                adapter.notifyDataSetChanged();
            }
        };
        asyncTask.execute();
    }

    /**
     * Retrieve Connector applications and their status
     */
    private void retrieveApps() {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't retrieve Connector Applications, no session with the GW is established, waiting for" +
                           " the onSessionJoined event");
            return;
        }

        // Check if there is previously selected app, then clean its resources
        ConnectorApp prevSelConnApp = app.getSelectedConnectorApp();
        if (prevSelConnApp != null) {

            prevSelConnApp.clear();
            prevSelConnApp = null;
        }

        app.setSelectedConnectorApp(null);
        adapter.clear();
        showProgressDialog("Retrieving applications");

        Log.d(TAG, "Retrieving applications");

        asyncTask = new AsyncTask<Void, Void, String>() {

            @Override
            protected String doInBackground(Void... params) {
                return retrieveAppsAsyncTask(sid);
            }

            @Override
            protected void onPostExecute(String result) {

                hideProgressDialog();

                if (result.length() > 0) { // Need to show Error information
                    showOkDialog("Error", result, "Ok", null);
                }

                adapter.notifyDataSetChanged();
            }
        };
        asyncTask.execute();
    }

    /**
     * The method is executed on the {@link AsyncTask} thread
     *
     * @return Error string
     */
    private String retrieveAppsAsyncTask(Integer sid) {

        List<ConnectorApp> connApps;

        try {
            connApps = app.getSelectedGatewayApp().retrieveConnectorApps(sid);
        } catch (GatewayControllerException gce) {

            Log.e(TAG, "Failed to retrieve the installed apps", gce);
            return "Failed to retrieve applications.\n Please try again later.";
        }

        boolean sigHandlerErr = false;
        boolean retrStatErr = false;

        for (ConnectorApp app : connApps) {

            try {
                app.setStatusSignalHandler(ConnectorAppsActivity.this);
            } catch (GatewayControllerException gce) {
                Log.e(TAG, "Failed to register Status Change Handler", gce);
                sigHandlerErr = true;
            }

            ConnectorAppStatus status = null;

            try {
                status = app.retrieveStatus(sid);
            } catch (GatewayControllerException gce) {
                Log.e(TAG, "Failed to retrieve status of the app", gce);
                retrStatErr = true;
            }

            Log.d(TAG, "Connector App: '" + app.getObjectPath() + "', Status: '" + status + "'");
            adapter.add(new VisualConnectorApp(app, status));
        }

        String retStr = "";
        if (retrStatErr) {

            retStr = "Failed to retrieve status of one of the applications.\n";
        }
        if (sigHandlerErr) {
            retStr += "Failed to register Status Change Handler of one of the applications.";
        }

        return retStr;
    }

}
