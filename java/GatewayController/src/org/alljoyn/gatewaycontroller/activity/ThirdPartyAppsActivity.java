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
import org.alljoyn.gatewaycontroller.adapters.ThirdPartyAppsAdapter;
import org.alljoyn.gatewaycontroller.adapters.VisualItem;
import org.alljoyn.gatewaycontroller.adapters.VisualTPApplication;
import org.alljoyn.gatewaycontroller.sdk.ApplicationStatusSignalHandler;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;
import org.alljoyn.gatewaycontroller.sdk.TPApplication;
import org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus;

import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

/**
 * Third Party Application activity 
 */
public class ThirdPartyAppsActivity extends BaseActivity implements OnClickListener,
                                                                    ApplicationStatusSignalHandler {
	
	private static final String TAG = "gwcapp" + ThirdPartyAppsActivity.class.getSimpleName();

	/**
	 * Gateway name
	 */
	private TextView gwNameTv;
	
	/**
	 * Third Party Applications list
	 */
	private ListView tpListView;
	
	/**
	 * Adapter for the list of third party applications
	 */
	private ThirdPartyAppsAdapter adapter;
	
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
		setContentView(R.layout.activity_third_party_apps);
	}

	/**
	 * @see android.app.Activity#onStart()
	 */
	@Override
	protected void onStart() {
		
		super.onStart();
		
		//Check existence of the selected gateway
		if ( app.getSelectedGateway() == null ) {
			
			Log.w(TAG, "Selected gateway has been lost, handling");
			handleLostOfGateway();
			return;
		}
		
		gwNameTv   = (TextView) findViewById(R.id.gwNameTv);
		gwNameTv.setText( app.getSelectedGateway().getAppName() );
		
		adapter    = new ThirdPartyAppsAdapter(this, R.layout.third_party_app_item, new ArrayList<VisualItem>());
		tpListView = (ListView) findViewById(R.id.tpAppsLv);
		tpListView.setAdapter(adapter);
		tpListView.setEmptyView( findViewById(R.id.tpAppsLvNoApps) );
		tpListView.setSelector(android.R.color.transparent);
		
		refreshBtn = (Button) findViewById(R.id.tpAppsRefresh);
		refreshBtn.setOnClickListener(this);
		
  	    retrieveApps();
	}

	/**
	 * @see android.app.Activity#onStop()
	 */
	@Override
	protected void onStop() {
		
		super.onStop();
		
		if ( asyncTask != null  ) {
			asyncTask.cancel(true);
		}
		
		if ( adapter == null ) {
			return;
		}
		
		Log.d(TAG, "Cleaning applications");
		for ( VisualItem vItem : adapter.getItemsList() ) {
			
			VisualTPApplication vApp = (VisualTPApplication) vItem;
			
			if ( vApp.getApp() != app.getSelectedApp() ) {
				
				vApp .clear();
			}
			
		}
	}
	
	/**
	 * @see android.view.View.OnClickListener#onClick(android.view.View)
	 */
	@Override
	public void onClick(View clickedView) {
		
		switch ( clickedView.getId() ) {
		
			case R.id.tpAppsRefresh: {
				retrieveApps();
				break;
			}
			case R.id.tpAppsDiscoverApps: {
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
	 * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSelectedGatewayLost()
	 */
	@Override
	protected void onSelectedGatewayLost() {
		super.onSelectedGatewayLost();
		handleLostOfGateway();
	}

	/**
	 * Search for the {@link TPApplication} with the given appId and update its status
	 * @see org.alljoyn.gatewaycontroller.sdk.ApplicationStatusSignalHandler#onStatusChanged(java.lang.String, org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus)
	 */
	@Override
	public void onStatusChanged(final String appId, final TPApplicationStatus status) {
		
		asyncTask = new AsyncTask<Void, Void, String>() {
			
			@Override
			protected String doInBackground(Void... params) {
				
				Log.d(TAG, "Searching for the app: '" + appId + "', to update the status to: '" + status + "'");
				
				for ( VisualItem vItem : adapter.getItemsList() ) {
					
					VisualTPApplication vApp = (VisualTPApplication) vItem;
					
					if ( !vApp.getApp().getAppId().equals(appId) ) {
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
	 * Retrieve TP apps and their status
	 */
	private void retrieveApps() {
		
		final Integer sid = getSession();
		if ( sid == null ) {
			
			Log.d(TAG, "Can't retrieve TP applications, no session with the GW is established, waiting for"
					   + " the onSessionJoined event");
			return;
		}
		
		//Check if there is previously selected app, then clean its resources
		TPApplication prevSelApp = app.getSelectedApp();
		if ( prevSelApp != null ) {
			
			prevSelApp.clear();
			prevSelApp = null;
		}
		
		app.setSelectedApp(null);
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
				
				if ( result.length() > 0 ) { //Need to show Error information
					showOkDialog("Error", result, "Ok", null);
				}
				
				adapter.notifyDataSetChanged();
			}
		};
		asyncTask.execute();
	}
	
	/**
	 * The method is executed on the {@link AsyncTask} thread
	 * @return Error string
	 */
	private String retrieveAppsAsyncTask(Integer sid) {
		
		List<TPApplication> tpApps;
		
  	    try {
			tpApps = app.getSelectedGateway().retrieveInstalledApps(sid);
		} catch (GatewayControllerException gce) {
			
			Log.e(TAG, "Failed to retrieve the installed apps", gce);
			return "Failed to retrieve applications.\n Please try again later.";
		}
  	    
  	    boolean sigHandlerErr = false;
  	    boolean retrStatErr   = false;
  	    
  	    for (TPApplication app : tpApps ) {
  	    	
  	    	try {
				app.setStatusChangedHandler(ThirdPartyAppsActivity.this);
			} catch (GatewayControllerException gce) {
				Log.e(TAG, "Failed to register Status Change Handler", gce);
				sigHandlerErr = true;
			}
  	    	
  	    	TPApplicationStatus status = null;
  	    	
  	    	try {
				status = app.retrieveStatus(sid);
			} catch (GatewayControllerException gce) {
				Log.e(TAG, "Failed to retrieve status of the app", gce);
				retrStatErr = true;
			}
  	    	
  	    	Log.d(TAG, "TPApp: '" + app.getObjectPath() + "', Status: '" + status + "'");
  	    	adapter.add( new VisualTPApplication(app, status) );
  	    }
  	    
  	    String retStr = "";
  	    if ( retrStatErr ) {
  	    	
  	    	retStr = "Failed to retrieve status of one of the applications.\n";
  	    }
  	    if ( sigHandlerErr ) {
  	    	retStr += "Failed to register Status Change Handler of one of the applications.";
  	    }
  	    
  	    return retStr;
	}
	
}
