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
import java.util.ArrayList;
import java.util.List;

import org.alljoyn.gatewaycontroller.CallbackMethod;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.adapters.AclMgmtConfigurableItemsAdapter;
import org.alljoyn.gatewaycontroller.adapters.VisualAclConfigurableItem;
import org.alljoyn.gatewaycontroller.adapters.VisualItem;
import org.alljoyn.gatewaycontroller.sdk.Acl;
import org.alljoyn.gatewaycontroller.sdk.Acl.AclResponseCode;
import org.alljoyn.gatewaycontroller.sdk.AclRules;
import org.alljoyn.gatewaycontroller.sdk.AclWriteResponse;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApp;
import org.alljoyn.gatewaycontroller.sdk.ConnectorCapabilities;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;
import org.alljoyn.gatewaycontroller.sdk.RemotedApp;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.ActionBarDrawerToggle;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

/**
 * The activity presents {@link Acl} object and its
 * {@link AclRules}
 */
public class AclManagementActivity extends BaseActivity implements ListView.OnItemClickListener, AclManagementHeaderFragment.AclNameListener, AclManagementButtonFragment.ActionButtonListener {

    private static final String TAG = "gwcapp" + AclManagementActivity.class.getSimpleName();

    /**
     * Activity type
     */
    public static final String ACTIVE_TYPE_KEY = "ACTIVE_TYPE";

    /**
     * Activity type -- ACL create
     */
    public static final int ACTIVE_TYPE_ACL_CREATE  = 1;

    /**
     * Activity type -- ACL update
     */
    public static final int ACTIVE_TYPE_ACL_UPDATE  = 2;

    /**
     * The unique menu item id for "Delete"
     */
    private static final int MENU_DELETE_ITEM_ID     = 7;

    /**
     * The unique menu item id for "ACL ID"
     */
    private static final int MENU_SHOW_ACLID_ITEM_ID = 9;

    /**
     * The type of this activity as it passed with the {@link Intent}
     */
    private int activityType;

    /**
     * Invoke this method when onSessionJoined event is called
     */
    private CallbackMethod invokeOnSessionReady;

    /**
     * Reflection of the retrieveData method
     */
    private static Method retrieveDataMethod;

    /**
     * Reflection of the deleteAcl method
     */
    private static Method deleteAclMethod;

    /**
     * Reflection of the createAcl method
     */
    private static Method createAclMethod;

    /**
     * Reflection of the updateAcl method
     */
    private static Method updateAclMethod;

    /**
     * Configurable items adapter. The configurable items are located in the
     * ItemsDrawer
     */
    private AclMgmtConfigurableItemsAdapter confItemsAdapter;

    /**
     * Asynchronous task to be executed
     */
    private AsyncTask<Void, Void, Void> asyncTask;

    /**
     * ActionBar drawer toggle
     */
    private ActionBarDrawerToggle drawerToggle;

    /**
     * The Activity {@link DrawerLayout}
     */
    private DrawerLayout drawerLayout;

    /**
     * List of the drawer items. It will include the ExposedServices and the
     * remoted apps
     */
    private ListView itemsDrawerList;

    /**
     * Header fragment
     */
    private AclManagementHeaderFragment headerFragment;

    /**
     * Button fragment
     */
    private AclManagementButtonFragment buttonFragment;

    /**
     * Configuration data fragment
     */
    private AclManagementConfDataFragment confDataFragment;

    static {

        try {

            Class<AclManagementActivity> activClass = AclManagementActivity.class;
            retrieveDataMethod = activClass.getDeclaredMethod("retrieveData");
            deleteAclMethod    = activClass.getDeclaredMethod("deleteAcl");
            createAclMethod    = activClass.getDeclaredMethod("createAcl", String.class, AclRules.class);

            updateAclMethod    = activClass.getDeclaredMethod("updateAcl", String.class, AclRules.class);

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
        setContentView(R.layout.activity_acl_management);

        if (retrieveDataMethod == null || deleteAclMethod == null || createAclMethod == null || updateAclMethod == null) {

            Log.e(TAG, "Reflection of the required methods is undefined, can't continue working");
            showOkDialog("Error", "Internal application error./n Can't continue working.", "Ok", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
        }

        activityType = getIntent().getIntExtra(ACTIVE_TYPE_KEY, -1);
        drawerLayout = (DrawerLayout) findViewById(R.id.aclDrawerLayout);
        drawerLayout.setDrawerShadow(R.drawable.drawer_shadow, GravityCompat.START);

        confItemsAdapter = new AclMgmtConfigurableItemsAdapter(this, R.layout.acl_management_drawer_item, new ArrayList<VisualItem>());

        itemsDrawerList  = (ListView) findViewById(R.id.aclMgmtItemsDrawer);
        itemsDrawerList.setAdapter(confItemsAdapter);

        itemsDrawerList.setOnItemClickListener(this);

        getActionBar().setDisplayHomeAsUpEnabled(true);
        getActionBar().setHomeButtonEnabled(true);

        initDrawerListener();
        drawerLayout.setDrawerListener(drawerToggle);
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onStart()
     */
    @Override
    protected void onStart() {

        super.onStart();

        // We don't want at this point to receive status change signals
        app.getSelectedConnectorApp().unsetStatusSignalHandler();
        retrieveData();
    }

    /**
     * @see android.app.Activity#onPostCreate(android.os.Bundle)
     */
    @Override
    protected void onPostCreate(Bundle savedInstanceState) {

        super.onPostCreate(savedInstanceState);
        drawerToggle.syncState();
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onStop()
     */
    @Override
    protected void onStop() {

        super.onStop();

        if (asyncTask != null) {
            asyncTask.cancel(true);
        }
    }

    /**
     * @see android.app.Activity#onConfigurationChanged(android.content.res.Configuration)
     */
    @Override
    public void onConfigurationChanged(Configuration newConfig) {

        super.onConfigurationChanged(newConfig);
        drawerToggle.onConfigurationChanged(newConfig);
    }

    /**
     * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        getMenuInflater().inflate(R.menu.acl_management, menu);

        if (activityType == ACTIVE_TYPE_ACL_UPDATE) {

            menu.add(Menu.NONE, MENU_DELETE_ITEM_ID, 1, R.string.acl_mgmt_menu_delete);
            menu.add(Menu.NONE, MENU_SHOW_ACLID_ITEM_ID, 1, R.string.acl_mgmt_menu_show_aclid);
        }

        return true;
    }

    /**
     * @see android.app.Activity#onOptionsItemSelected(android.view.MenuItem)
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        if (drawerToggle.onOptionsItemSelected(item)) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * Creates {@link ActionBarDrawerToggle}
     *
     * @return {@link ActionBarDrawerToggle}
     */
    private void initDrawerListener() {

        drawerToggle = new ActionBarDrawerToggle(this, drawerLayout, R.drawable.ic_drawer, R.string.acl_mgmt_drawer_open, R.string.acl_mgmt_drawer_close) {

            @Override
            public void onDrawerClosed(View view) {
                invalidateOptionsMenu();
            }

            @Override
            public void onDrawerOpened(View drawerView) {
                invalidateOptionsMenu();
            }
        };
    }

    /**
     * @see android.app.Activity#onMenuItemSelected(int, android.view.MenuItem)
     */
    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {

        switch (item.getItemId()) {

        case R.id.menuRefresh: {

            retrieveData();
            return true;
        }
        case MENU_DELETE_ITEM_ID: {

            verifyDeleteRequest();
            return true;
        }
        case MENU_SHOW_ACLID_ITEM_ID: {

            showAclId();
            return true;
        }
        default: {

            return super.onMenuItemSelected(featureId, item);
        }
        }
    }

    /**
     * @see android.widget.AdapterView.OnItemClickListener#onItemClick(android.widget.AdapterView,
     *      android.view.View, int, long)
     */
    @Override
    public void onItemClick(AdapterView<?> adapter, View clickedView, int position, long rowId) {

        handleItemSelection(position);
    }

    /**
     * Handles selection of the item from the drawer layout
     */
    private void handleItemSelection(int position) {

        itemsDrawerList.setItemChecked(position, true);
        loadConfDataFragment(position);
        drawerLayout.closeDrawer(itemsDrawerList);
    }

    /**
     * The ACL name has been changed. If it's not an empty String enable the
     * actionButton on the {@link AclManagementButtonFragment}
     *
     * @see org.alljoyn.gatewaycontroller.activity.AclManagementHeaderFragment.AclNameListener#onTextChanged(java.lang.String)
     */
    @Override
    public void onTextChanged(String aclName) {

        if (buttonFragment == null) {

            return;
        }

        if (aclName.length() > 0) {

            buttonFragment.enableActionButton(true);
        } else {

            buttonFragment.enableActionButton(false);
        }
    }

    /**
     * Action button was clicked, handle the event according to the activity
     * type
     *
     * @see org.alljoyn.gatewaycontroller.activity.AclManagementButtonFragment.ActionButtonListener#onActionButtonClicked()
     */
    @Override
    public void onActionButtonClicked() {

        String aclName    = headerFragment.getAclName();
        AclRules aclRules = constructAclRules();

        if (activityType == ACTIVE_TYPE_ACL_CREATE) {

            Log.d(TAG, "CREATE button has been clicked");
            createAcl(aclName, aclRules);
        } else {

            Log.d(TAG, "UPDATE button has been clicked");
            updateAcl(aclName, aclRules);
        }
    }

    /**
     * @see org.alljoyn.gatewaycontroller.activity.BaseActivity#onSessionJoined()
     */
    @Override
    protected void onSessionJoined() {

        super.onSessionJoined();

        super.onSessionJoined();

        if (invokeOnSessionReady == null) {
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
     * Retrieves {@link Acl} data
     */
    private void retrieveData() {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't retrieve ACL rules, no session with the GW is established, waiting for" +
                           " the onSessionJoined event");

            invokeOnSessionReady = new CallbackMethod(retrieveDataMethod, new Object[] {});
            return;
        }

        confItemsAdapter.clear();
        showProgressDialog("Retrieving ACL rules");

        Log.d(TAG, "Retrieving ACL rules");

        asyncTask = new AsyncTask<Void, Void, Void>() {

            String errMsg;

            @Override
            protected Void doInBackground(Void... params) {

                errMsg = retrieveRulesAsyncTask(sid);
                return null;
            }

            @Override
            protected void onPostExecute(Void result) {

                hideProgressDialog();

                if (errMsg != null) {

                    showOkDialog("Error", errMsg, "Ok", null);
                }

                confItemsAdapter.notifyDataSetChanged();

                // Load the header and button fragments
                loadHeaderButtonFragments();

                // If there are items in the drawable layout and no item was
                // previously selected, then select the first item
                // otherwise select the selected item
                if (confItemsAdapter.getCount() > 0) {

                    int currSelIdx = itemsDrawerList.getCheckedItemPosition();
                    int selIdx = currSelIdx == AdapterView.INVALID_POSITION ? 0 : currSelIdx;
                    handleItemSelection(selIdx);
                }
            }
        };
        asyncTask.execute();

    }// retrieveData

    /**
     * Retrieve ACL rules. If create ACL call
     * {@link ConnectorApp#retrieveApplicableConnectorCapabilities(int)} if update ACL
     * call
     * {@link Acl#retrieve(int, org.alljoyn.gatewaycontroller.sdk.ConnectorCapabilities)}
     *
     * @param sid
     * @return
     */
    private String retrieveRulesAsyncTask(int sid) {

        AclRules rules;

        try {

            if (activityType == ACTIVE_TYPE_ACL_CREATE) { // Create ACL

                rules = app.getSelectedConnectorApp().retrieveApplicableConnectorCapabilities(sid);
            } else { // Update ACL

                ConnectorCapabilities connectorCapabilities = app.getSelectedConnectorApp().retrieveConnectorCapabilities(sid);
                rules = app.getSelectedAcl().retrieve(sid, connectorCapabilities);
            }
        } catch (GatewayControllerException gce) {

            Log.e(TAG, "Failed to retrieve ACL rules", gce);
            return "Failed to retrieve ACL rules";
        }

        List<RuleObjectDescription> expServices = rules.getExposedServices();
        List<RemotedApp> remApps                = rules.getRemotedApps();

        // Add exposed services
        if (expServices.size() > 0) {

            confItemsAdapter.add(new VisualAclConfigurableItem(this, expServices, getString(R.string.acl_mgmt_exp_services_lbl)));
        }

        if (remApps.size() > 0) {

            for (RemotedApp remApp : remApps) {

                confItemsAdapter.add(new VisualAclConfigurableItem(this, remApp, remApp.getAppName() + "@" + remApp.getDeviceName()));
            }
        }

        return null;
    }

    /**
     * Call {@link ConnectorApp#createAcl(int, String, AclRules)}
     */
    private void createAcl(final String aclName, final AclRules aclRules) {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't create the ACL, no session with the GW is established, waiting for" + " the onSessionJoined event");

            invokeOnSessionReady = new CallbackMethod(createAclMethod, new Object[] { aclName, aclRules });
            return;
        }

        showProgressDialog("Creating ACL, name: '" + aclName + "'");
        Log.d(TAG, "Creating ACL, name: '" + aclName + "' Connapp: '" + app.getSelectedConnectorApp().getObjectPath() + "'");

        asyncTask = new AsyncTask<Void, Void, Void>() {

            private AclWriteResponse aclWriteResponse;
            private String errMsg;

            @Override
            protected Void doInBackground(Void... params) {

                ConnectorApp selConnApp = app.getSelectedConnectorApp();
                try {

                    aclWriteResponse = selConnApp.createAcl(sid, aclName, aclRules);
                } catch (GatewayControllerException gce) {

                    Log.d(TAG, "Failed to create ACL ConnectorApplication: '" + selConnApp.getObjectPath() + "'", gce);
                    errMsg = "ACL creation failed";
                }

                return null;
            }

            @Override
            protected void onPostExecute(Void result) {

                createUpdatePostExecAsyncTask("ACL creation", errMsg, aclWriteResponse);
            }
        };
        asyncTask.execute();
    }

    /**
     * Call {@link Acl#update(int, AclRules, ConnectorCapabilities)}
     *
     * @param aclName
     * @param aclRules
     */
    private void updateAcl(final String aclName, final AclRules aclRules) {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't update the ACL, no session with the GW is established, waiting for" + " the onSessionJoined event");

            invokeOnSessionReady = new CallbackMethod(updateAclMethod, new Object[] { aclName, aclRules });
            return;
        }

        showProgressDialog("Updating ACL, name: '" + aclName + "'");
        Log.d(TAG, "Updating ACL, name: '" + aclName + "' ACL: '" + app.getSelectedAcl().getObjectPath() + "'");

        asyncTask = new AsyncTask<Void, Void, Void>() {

            private AclWriteResponse aclWriteResponse;
            private String errMsg;

            @Override
            protected Void doInBackground(Void... params) {

                Acl selAcl = app.getSelectedAcl();
                try {

                    ConnectorCapabilities connectorCapabilities = app.getSelectedConnectorApp().retrieveConnectorCapabilities(sid);

                    selAcl.setName(aclName);
                    aclWriteResponse = selAcl.update(sid, aclRules, connectorCapabilities);
                } catch (GatewayControllerException gce) {

                    Log.d(TAG, "Failed to update ACL: '" + selAcl.getObjectPath() + "'", gce);
                    errMsg = "ACL updating failed";
                }

                return null;
            }

            @Override
            protected void onPostExecute(Void result) {

                createUpdatePostExecAsyncTask("ACL updating", errMsg, aclWriteResponse);
            }
        };
        asyncTask.execute();
    }

    /**
     * Create/Update ACL post execution process
     *
     * @param taskName
     *            Create or update ACL
     * @param errMsg
     *            Error message on failure
     * @param aclWriteResp
     *            {@link AclWriteResponse}
     */
    private void createUpdatePostExecAsyncTask(String taskName, String errMsg, AclWriteResponse aclWriteResp) {

        hideProgressDialog();
        if (errMsg != null) {
            showOkDialog("Error", errMsg, "Ok", null);
            return;
        }

        AclResponseCode respCode = aclWriteResp.getResponseCode();
        AclRules invRules = aclWriteResp.getInvalidRules();

        if (respCode != AclResponseCode.GW_ACL_RC_SUCCESS) {

            Log.e(TAG, taskName + " failed, '" + respCode + "'");
            showOkDialog("Error", taskName + " failed", "Ok", null);
            return;
        }

        if (invRules.getExposedServices().size() > 0 || invRules.getRemotedApps().size() > 0) {

            String msg = taskName + " succeeded. \n There are some rules not supported by the application which" + " weren't sent to the gateway";
            showOkDialog("Info", msg, "Ok", null);

            Log.w(TAG, taskName + " invalid rules");
            app.printAclRules(invRules);
        } else {
            app.showToast(taskName + " succeeded");
        }

        // Go to the ConnectorApplicationActivity
        Intent intent = new Intent(this, ConnectorAppActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);
    }

    /**
     * Construct {@link AclRules} for ACL creation or update
     *
     * @return {@link AclRules}
     */
    private AclRules constructAclRules() {

        List<RemotedApp> remotedApps = new ArrayList<RemotedApp>();
        List<RuleObjectDescription> exposedServices = new ArrayList<RuleObjectDescription>();

        for (VisualItem visItem : confItemsAdapter.getItemsList()) {

            VisualAclConfigurableItem confItem = (VisualAclConfigurableItem) visItem;
            List<RuleObjectDescription> rules = confItem.getSelectedRules();

            if (confItem.isRemotedApp()) {

                remotedApps.add(new RemotedApp((RemotedApp) confItem.getConfItem(), rules));
            } else { // Exposed service

                exposedServices.addAll(rules);
            }
        }

        return new AclRules(exposedServices, remotedApps);
    }

    /**
     * Load header and button fragments
     */
    private void loadHeaderButtonFragments() {

        // load the header fragment
        headerFragment = new AclManagementHeaderFragment();
        loadFragment(R.id.aclMgmtHeaderFragmentFrame, headerFragment);

        // prepare and load the button fragment
        String buttonLabel;
        boolean buttonEnable = false;

        if (activityType == ACTIVE_TYPE_ACL_CREATE) {

            buttonLabel = getString(R.string.acl_mgmt_create_btn_lbl);
        } else {

            buttonLabel = getString(R.string.acl_mgmt_update_btn_lbl);
            buttonEnable = app.getSelectedAcl().getName().length() > 0 ? true : false;
        }

        Bundle bundle = new Bundle();
        bundle.putString(AclManagementButtonFragment.ACTION_BUTTON_LBL, buttonLabel);
        bundle.putBoolean(AclManagementButtonFragment.ACTION_BUTTON_ENABLE, buttonEnable);

        buttonFragment = new AclManagementButtonFragment();
        buttonFragment.setArguments(bundle);
        loadFragment(R.id.aclMgmtButtonFragmentFrame, buttonFragment);
    }

    /**
     * Load the configuration data fragment with the name of the selected item
     *
     * @param position
     *            Position of the selected item in the drawer
     */
    private void loadConfDataFragment(int position) {

        VisualAclConfigurableItem confItem = (VisualAclConfigurableItem) confItemsAdapter.getItem(position);
        confDataFragment = AclManagementConfDataFragment.createInstance(confItem);
        loadFragment(R.id.aclMgmtConfDataFragmentFrame, confDataFragment);
    }

    /**
     * Show ACLId of the selected ACL, when in the "Update" mode
     */
    private void showAclId() {

        Acl acl = app.getSelectedAcl();
        if (acl == null) {

            Log.wtf(TAG, "This method can't be called when there is no selected ACL !!!");
            return;
        }

        showOkDialog("Info", "Selected ACL Id: '" + acl.getId() + "'", "Ok", null);
    }

    /**
     * Show {@link AlertDialog} with verification whether to delete the ACL If
     * the positive button was pressed, call the deleteAcl method
     */
    private void verifyDeleteRequest() {

        Builder builder = createAlertDialog("Delete ACL", "Are you sure you want delete the ACL: '" + app.getSelectedAcl().getName() + "'");

        final OnClickListener dialogListener = new OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                unlockScreenRotation();
                dialog.dismiss();

                if (which == AlertDialog.BUTTON_POSITIVE) {

                    deleteAcl();
                }
            }
        };

        builder.setPositiveButton("Yes", dialogListener);
        builder.setNegativeButton("No", dialogListener);

        lockScreenRotation();
        builder.show();
    }

    /**
     * Call {@link ConnectorApp#deleteAcl(int, String)}
     */
    private void deleteAcl() {

        final Integer sid = getSession();
        if (sid == null) {

            Log.d(TAG, "Can't delete the ACL, no session with the GW is established, waiting for" + " the onSessionJoined event");

            invokeOnSessionReady = new CallbackMethod(deleteAclMethod, new Object[] {});
            return;
        }

        showProgressDialog("Deleting ACL");
        Log.d(TAG, "Deleting ACL, app: '" + app.getSelectedAcl().getObjectPath());

        asyncTask = new AsyncTask<Void, Void, Void>() {

            private GatewayControllerException gce;
            private AclResponseCode respCode;

            @Override
            protected Void doInBackground(Void... params) {

                try {

                    respCode = app.getSelectedConnectorApp().deleteAcl(sid, app.getSelectedAcl().getId());
                } catch (GatewayControllerException gce) {
                    this.gce = gce;
                }

                return null;
            }

            @Override
            protected void onPostExecute(Void result) {

                hideProgressDialog();

                if (gce != null || respCode != AclResponseCode.GW_ACL_RC_SUCCESS) {

                    Log.w(TAG, "Failed to delete the ACL: '" + app.getSelectedAcl().getObjectPath() + "', " + "ACL response code: '" + respCode + "'");

                    showOkDialog("Error", "Failed to delete the ACL", "Ok", null);
                    return;
                }

                app.showToast("The ACL has been deleted successfully");
                Intent intent = new Intent(AclManagementActivity.this, ConnectorAppActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
            }
        };
        asyncTask.execute();
    }// deleteAcl

    /**
     * Load the given fragment to the activity
     *
     * @param replacedFragmentId
     * @param loagFragment
     */
    private void loadFragment(int replacedFragmentId, Fragment loagFragment) {

        FragmentManager fragmentManager = getFragmentManager();
        fragmentManager.beginTransaction().replace(replacedFragmentId, loagFragment).commit();
    }

}
