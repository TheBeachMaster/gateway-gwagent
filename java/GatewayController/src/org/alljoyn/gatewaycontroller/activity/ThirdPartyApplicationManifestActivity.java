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

import java.lang.reflect.Method;

import org.alljoyn.gatewaycontroller.CallbackMethod;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;
import org.alljoyn.gatewaycontroller.sdk.ManifestRules;
import org.alljoyn.gatewaycontroller.sdk.TPApplication;

import android.app.ActionBar;
import android.app.ActionBar.Tab;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v13.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;

/**
 * The activity presents the {@link ManifestRules} and the manifest file 
 * of a {@link TPApplication}.
 */
public class ThirdPartyApplicationManifestActivity extends BaseActivity implements ActionBar.TabListener  {
	public static final String TAG = "gwcapp" + ThirdPartyApplicationManifestActivity.class.getSimpleName();
	
	/**
	 * Loads the fragment of the selected tab
	 */
	public class SelectedPageAdapter extends FragmentPagerAdapter {

		/**
		 * Constructor
		 * @param fragmentMgr
		 */
		public SelectedPageAdapter(FragmentManager fragmentMgr) {
			
			super(fragmentMgr);
		}
		
		/**
		 * @see android.support.v13.app.FragmentPagerAdapter#getItem(int)
		 */
		@Override
		public Fragment getItem(int position) {
			
			Fragment frg = null;
			
			switch(position) {
				
				case RULES_TAB_INDEX: {
					
					frg = ThirdPartyApplicationManifestRulesFragment.createInstance(manifestRules);
					break;
				}
				case FILE_TAB_INDEX: {
					
					frg = ThirdPartyApplicationManifestFileFragment.createInstance(manifestFile);
					break;
				}
			}
			
			return frg;
		}
		
		/**
		 * @see android.support.v4.view.PagerAdapter#getItemPosition(java.lang.Object)
		 */
		@Override
		public int getItemPosition(Object object) {
			
			return POSITION_NONE;
		}
		
		/**
		 * @see android.support.v4.view.PagerAdapter#getCount()
		 */
		@Override
		public int getCount() {
			
			return manifestTabs.length;
		}
	}
	
	//=========================================================//
	
	private static int[] manifestTabs            = new int[] {R.string.tp_app_manifest_rules,
	     	                                                  R.string.tp_app_manifest_file};
	
	private static final int RULES_TAB_INDEX     = 0;
	private static final int FILE_TAB_INDEX      = 1;
	
	/**
	 * Used for the page swiping
	 */
	private ViewPager viewPager;
	
	/**
	 * Loads the fragment of the selected page
	 */
	private SelectedPageAdapter pageAdapter;

    /**
     * Asynchronous task to be executed
     */
    private AsyncTask<Void, Void, Void> asyncTask;
	
	/**
	 * Reflection of the retrieveData method
	 */
	private static Method retrieveDataMethod;
	
    /**
     * Invoke this method when onSessionJoined event is called
     */
    private CallbackMethod invokeOnSessionReady;
	
    /**
     * The manifest file
     */
    private String manifestFile;

    /**
     * The {@link ManifestRules}
     */
    private ManifestRules manifestRules;
    
    static {
    	
        try {
        	
            Class<ThirdPartyApplicationManifestActivity> activClass = ThirdPartyApplicationManifestActivity.class;
            retrieveDataMethod = activClass.getDeclaredMethod("retrieveData");
        } catch (NoSuchMethodException nsme) {
            Log.wtf(TAG, "NoSuchMethodException", nsme);
        }
    }

    
	/**
	 * @see android.app.Activity#onCreate(android.os.Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_third_party_application_manifest);
		
        if ( retrieveDataMethod == null ) {
        	
            Log.e(TAG, "Reflection of the required methods is undefined, can't continue working");
            showOkDialog("Error", "Internal application error./n Can't continue working.", "Ok",
                                   new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
        }
		
		
		//Check existence of the selected gateway
        if ( app.getSelectedGateway() == null ) {

             Log.w(TAG, "Selected gateway has been lost, handling");
             handleLostOfGateway();
             return;
        }
        
        setTitle( getString(R.string.title_activity_tp_manifest) + ": " + app.getSelectedApp().getFriendlyName());
        
        retrieveData();
	}
	
	/**
	 * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onDestroy()
	 */
	@Override
	protected void onDestroy() {
		
		super.onDestroy();
		
	    if ( asyncTask != null ) {
            asyncTask.cancel(true);
        }
	}
	
	/**
	 * @see android.app.ActionBar.TabListener#onTabSelected(android.app.ActionBar.Tab, android.app.FragmentTransaction)
	 */
	@Override
	public void onTabSelected(Tab tab, FragmentTransaction ft) {
		
		viewPager.setCurrentItem(tab.getPosition());
	}
	
	/**
	 * @see android.app.ActionBar.TabListener#onTabReselected(android.app.ActionBar.Tab, android.app.FragmentTransaction)
	 */
	@Override
	public void onTabReselected(Tab tab, FragmentTransaction ft) {}

	/**
	 * @see android.app.ActionBar.TabListener#onTabUnselected(android.app.ActionBar.Tab, android.app.FragmentTransaction)
	 */
	@Override
	public void onTabUnselected(Tab tab, FragmentTransaction ft) {}
	
    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSessionJoined()
     */
    @Override
    protected void onSessionJoined() {

        super.onSessionJoined();
        
        if ( invokeOnSessionReady == null ) {
            Log.w(TAG, "onSessionJoined is called, but invokeOnSessionReady is undefiend, returning");
            return;
        }

        try {
            invokeOnSessionReady.getMethod().invoke(this, invokeOnSessionReady.getArgs());
        } catch (Exception e) {
            Log.e(TAG, "Failed to invoke the method: '" + invokeOnSessionReady.getMethod().getName() + "'");
            showOkDialog("Error", "Failed to execute an operation", "Ok", null);
        }

        invokeOnSessionReady = null;
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
     * Loads Tab navigation
     */
    private void loadTabNavigation() {
    	
    	final ActionBar actionBar = getActionBar();
		actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_TABS);
		
		actionBar.setHomeButtonEnabled(false);
		actionBar.setDisplayHomeAsUpEnabled(false);
		
		pageAdapter = new SelectedPageAdapter(getFragmentManager());
		viewPager   = (ViewPager) findViewById(R.id.tpAppManifestPager);
		viewPager.setAdapter(pageAdapter);
		
		viewPager.setOnPageChangeListener( new ViewPager.SimpleOnPageChangeListener() {
			@Override
			public void onPageSelected(int position) {
				actionBar.setSelectedNavigationItem(position);
			}
		});
		
		//Fill the ActionBar with tabs
		for ( int nameRes : manifestTabs ) {
		
			actionBar.addTab(actionBar.newTab()
					                  .setText(getString(nameRes))
					                  .setTabListener(this));
		}
    }
    
    /**
     * Retrieves the {@link ManifestRules} and the manifest file of the selected
     * {@link TPApplication}
     */
    private void retrieveData() {
    	
        final Integer sid = getSession();
        if ( sid == null ) {

             Log.d(TAG, "Can't retrieve TP application manifest, no session with the GW is established, waiting for"
                        + " the onSessionJoined event");

             invokeOnSessionReady = new CallbackMethod(retrieveDataMethod, new Object[]{});
             return;
        }

    	showProgressDialog("Retrieving manifest data");
    	
    	Log.d(TAG, "Retrieving manifest data");
    	
    	asyncTask = new AsyncTask<Void, Void, Void> () {
    		
    		private boolean isOk = true; 
    		
			@Override
			protected Void doInBackground(Void... params) {

				try {
					
					TPApplication selApp = app.getSelectedApp(); 
					manifestFile         = selApp.retrieveManifestFile(sid);
					manifestRules        = selApp.retrieveManifestRules(sid);
				} catch (GatewayControllerException gce) {
					
					isOk = false;
					Log.e(TAG, "Failed to retrieve the manifest", gce);
				}
				return null;
			}

			@Override
			protected void onPostExecute(Void result) {
				
				hideProgressDialog();
				
				if ( !isOk ) {
					
					showOkDialog("Error", "Failed to retrieve the manifest data", "Ok", null);
					return;
				}
				
				loadTabNavigation();
			}//onPostExecute
    	};
    	asyncTask.execute();
    	
    }//retrieveData

}
