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
import org.alljoyn.gatewaycontroller.activity.ThirdPartyApplicationActivity;
import org.alljoyn.gatewaycontroller.sdk.TPApplication;
import org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus;
import org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus.ConnectionStatus;
import org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus.InstallStatus;
import org.alljoyn.gatewaycontroller.sdk.TPApplicationStatus.OperationalStatus;

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
 * Manages the list of {@link VisualTPApplication}s
 */
public class ThirdPartyAppsAdapter extends VisualArrayAdapter {
	private static final String TAG = "gwcapp" + ThirdPartyAppsAdapter.class.getSimpleName();
	
	static class TPAppView {
		
		Button appName;
		TextView connStatus;
		TextView operStatus;
		TextView installStatus;
	}

	//======================================//
	
	/**
	 * Constructor
	 * @param context
	 */
	ThirdPartyAppsAdapter(Context context) {
		this(context, -1, null);
	}
	
	/**
	 * Constructor
	 * @param context
	 * @param viewItemResId
	 * @param itemsList
	 */
	public ThirdPartyAppsAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {
		
		super(context, viewItemResId, itemsList);
	}

	/**
	 * @see android.widget.ArrayAdapter#getView(int, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		View row  = convertView;
		TPAppView tpAppView;
		
		if ( row == null ) {
			
			row = inflater.inflate(viewItemResId, parent, false);
			
			tpAppView               = new TPAppView();
			tpAppView.appName       = (Button) row.findViewById(R.id.tpAppName);
			tpAppView.connStatus    = (TextView) row.findViewById(R.id.tpAppConnStatus);
			tpAppView.installStatus = (TextView) row.findViewById(R.id.tpAppInstallStatus);
			tpAppView.operStatus    = (TextView) row.findViewById(R.id.tpAppOperStatus);
			
			row.setTag(tpAppView);
		}
		else {
			tpAppView = (TPAppView) row.getTag();
		}
		
		final VisualTPApplication visApp = (VisualTPApplication) getItem(position);
		
		tpAppView.appName.setText(visApp.getApp().getFriendlyName());
		tpAppView.appName.setOnClickListener( new OnClickListener() {
			@Override
			public void onClick(View v) {
				
				TPApplication selectedApp = visApp.getApp();
				Log.d(TAG, "Selected TPApplication, id: '" + selectedApp.getAppId() + "'");
				((GWControllerSampleApplication) context.getApplicationContext()).setSelectedApp(selectedApp);
				
				Intent intent = new Intent(context, ThirdPartyApplicationActivity.class);
				intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
				context.startActivity(intent);
			}
		});
		
		TPApplicationStatus tpAppStatus = visApp.getAppStatus();
		updateStatus(tpAppView.connStatus, tpAppView.operStatus, tpAppView.installStatus, tpAppStatus);
		
		return row;
	}
	
	/**
	 * Updates received {@link TextView} of the statuses with the data received in the 
	 * {@link TPApplicationStatus}
	 * @param connStatusTv
	 * @param operStatusTv
	 * @param installStatusTv
	 * @param status
	 */
	public static void updateStatus(TextView connStatusTv, TextView operStatusTv, TextView installStatusTv,
			                        TPApplicationStatus tpAppStatus){
		
		final String UNKNOWN_STATUS = "unknown";
		
		if ( tpAppStatus == null ) {
			
			connStatusTv.setText(UNKNOWN_STATUS);
			installStatusTv.setText(UNKNOWN_STATUS);
			operStatusTv.setText(UNKNOWN_STATUS);
			
			return;
		}
		
		ConnectionStatus connStatus = tpAppStatus.getConnectionStatus();
		int color = Color.parseColor( VisualTPApplication.getConnStatusColor(connStatus) );
		connStatusTv.setText(connStatus.DESC);
		connStatusTv.setTextColor(color);
		
		InstallStatus instStatus = tpAppStatus.getInstallStatus();
		color = Color.parseColor( VisualTPApplication.getInstallStatusColor(instStatus) );
		installStatusTv.setText( instStatus.DESC );
		installStatusTv.setTextColor(color);
		
		
		OperationalStatus operStatus = tpAppStatus.getOperationalStatus();
		color = Color.parseColor( VisualTPApplication.getOperationalStatusColor(operStatus) );
		operStatusTv.setText(operStatus.DESC);
		operStatusTv.setTextColor(color);
	}
}
