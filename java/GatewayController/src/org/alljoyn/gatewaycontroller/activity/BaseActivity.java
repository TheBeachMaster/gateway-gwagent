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

import java.util.List;

import org.alljoyn.gatewaycontroller.GWControllerActions;
import org.alljoyn.gatewaycontroller.GWControllerSampleApplication;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;
import org.alljoyn.gatewaycontroller.sdk.GatewayMgmtApp;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Application;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.text.Editable;
import android.text.method.PasswordTransformationMethod;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

/**
 * The parent class for other activities
 */
public abstract class BaseActivity extends Activity {
    private static final String TAG = "gwcapp" + BaseActivity.class.getSimpleName();

    class BaseBroadcastReceiver extends BroadcastReceiver {

        /**
         * @see android.content.BroadcastReceiver#onReceive(android.content.Context,
         *      android.content.Intent)
         */
        @Override
        public void onReceive(Context context, Intent intent) {

            String actionStr = intent.getAction();
            Log.d(TAG, "Received intent of: '" + actionStr + "'");

            GWControllerActions action;

            try {
                action = GWControllerActions.valueOf(actionStr);
            } catch (IllegalArgumentException ilae) {
                Log.w(TAG, "Received an unsupported Action: '" + actionStr + "'", ilae);
                return;
            }

            switch (action) {

                case GWC_PASSWORD_REQUIRED: {
                    showPasswordDialog();
                    break;
                }
                case GWC_SESSION_JOINED: {
                    onSessionJoined();
                    break;
                }
                case GWC_SESSION_JOIN_FAILED: {
                    onSessionJoinFailed();
                    break;
                }
                case GWC_GATEWAY_ANNOUNCE_RECEIVED: {
                    onGatewayMgmtAppAnnounced();
                    break;
                }
                default: {
                    Log.w(TAG, "Received an Action: '" + action + "' unsupported by the BaseBroadcastReceiver");
                }
            }

        }// onReceive
    }

    // ===========================================//

    /**
     * Reference to the {@link Application} object
     */
    protected GWControllerSampleApplication app;

    /**
     * {@link BaseBroadcastReceiver} receiver
     */
    private BaseBroadcastReceiver broadcastReceiver;

    /**
     * Password alert dialog
     */
    private AlertDialog passwordDialog;

    /**
     * Progress dialog
     */
    private ProgressDialog progressDialog;

    /**
     * @see android.app.Activity#onStart()
     */
    @Override
    protected void onStart() {

        super.onStart();
        app = (GWControllerSampleApplication) getApplicationContext();
        registerBaseIntentReceiver();
    }

    /**
     * @see android.app.Activity#onStop()
     */
    @Override
    protected void onStop() {

        super.onStop();
        unregisterReceiver(broadcastReceiver);
    }

    /**
     * Returns sessionId from the application. If sessionId is null calls join
     * session
     *
     * @return Currently established session or NULL session is not established.
     *         Join session is called, need to wait for receiving
     *         {@link BaseActivity#onSessionJoined()} or
     *         {@link BaseActivity#onSessionJoinFailed()} events
     */
    protected Integer getSession() {

        Integer sid = app.getSessionId();

        if (sid != null) {
            return sid;
        }

        showProgressDialog("Connecting to the gateway");

        Log.d(TAG, "No session is established, calling to join session");
        app.joinSession();

        return null;
    }

    /**
     * Override this method to be notified when the session is joined
     */
    protected void onSessionJoined() {

        Log.d(TAG, "onSessionJoined is called");
        hideProgressDialog();
    }

    /**
     * Override this method to be notified if session joining has failed
     */
    protected void onSessionJoinFailed() {

        hideProgressDialog();
        app.showToast("Failed to connect to the gateway");
    }

    /**
     * Override this method to be notified when the GW is lost
     */
    protected void onSelectedGatewayLost() {

        Log.d(TAG, "Selected Gateway Management App has been lost");
        app.setSelectedGatewayApp(null);
    }

    /**
     * Override this method to be notified when an Announcement from a {@link GatewayMgmtApp} is received. <br>
     *
     * <br>
     * By default this method retrieves the list of the found {@link GatewayMgmtApp}s.
     * If currently selected gateway app is not in the list of the retrieved gateways,
     * the method {@link BaseActivity#onSelectedGatewayLost()} is called.
     */
    protected void onGatewayMgmtAppAnnounced() {

        Log.d(TAG, "GatewayListChanged was called");

        GatewayMgmtApp selGw = app.getSelectedGatewayApp();
        if (selGw == null) {
            onSelectedGatewayLost();
            return;
        }

        boolean gwFound = false;

        List<GatewayMgmtApp> gatewayApps = GatewayController.getInstance().getGatewayMgmtApps();
        for (GatewayMgmtApp gw : gatewayApps) {

            if (gw.getBusName().equals(selGw.getBusName())) {
                gwFound = true;
                break;
            }
        }

        if (!gwFound) {
            Log.w(TAG, "Selected GW busName: '" + selGw.getBusName() + "' is not found, call onSelectedGatewayLost()");
            onSelectedGatewayLost();
        }
    }

    /**
     * If the {@link ProgressDialog} is not initialized it's created and is
     * presented. If the {@link ProgressDialog} is already presented it's msg is
     * updated.
     *
     * @param msg
     *            The message to show with the {@link ProgressDialog}
     */
    protected void showProgressDialog(String msg) {

        if (progressDialog == null || !progressDialog.isShowing()) {

            progressDialog = ProgressDialog.show(this, "", msg, true);
            progressDialog.setCancelable(false);
        } else if (progressDialog.isShowing()) {
            progressDialog.setMessage(msg);
        }
    }

    /**
     * Create {@link AlertDialog.Builder} with the given title and message
     *
     * @param title
     *            Dialog title
     * @param message
     *            Dialog message
     * @return {@link AlertDialog.Builder}
     */
    protected AlertDialog.Builder createAlertDialog(String title, String message) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title);
        builder.setMessage(message);
        builder.setCancelable(false);

        return builder;
    }

    /**
     * Show {@link AlertDialog} with the positive button and the given title and
     * message.
     *
     * @param title
     *            {@link AlertDialog} title
     * @param message
     *            {@link AlertDialog} message
     * @param clickListener
     *            The listener to be called
     */
    protected void showOkDialog(String title, String message, String btnLabel, final DialogInterface.OnClickListener clickListener) {

        final AlertDialog.Builder builder = createAlertDialog(title, message);
        builder.setPositiveButton(btnLabel, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                unlockScreenRotation();
                if (clickListener != null) {
                    clickListener.onClick(dialog, which);
                }
            }
        });

        lockScreenRotation();
        builder.show();
    }

    /**
     * Handle loose of the selected {@link GatewayMgmtApp}
     */
    protected void handleLostOfGateway() {

        showOkDialog("Message", "Selected gateway lost", "Discover gateways", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                dialog.dismiss();
                Intent intent = new Intent(app, DiscoveredGatewaysActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(intent);
            }
        });
    }

    /**
     * Hide {@link ProgressDialog}
     */
    protected void hideProgressDialog() {

        if (progressDialog != null) {
            progressDialog.dismiss();
        }
    }

    /**
     * Lock screen rotation
     */
    protected void lockScreenRotation() {

        int currentRotation = getResources().getConfiguration().orientation;

        switch (currentRotation) {

            case Configuration.ORIENTATION_PORTRAIT: {

                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                break;
            }
            case Configuration.ORIENTATION_LANDSCAPE: {

                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                break;
            }
        }
    }

    /**
     * Resume screen rotation
     */
    protected void unlockScreenRotation() {

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
    }

    /**
     * Register the {@link BaseBroadcastReceiver}
     */
    private void registerBaseIntentReceiver() {

        broadcastReceiver    = new BaseBroadcastReceiver();

        IntentFilter filters = new IntentFilter();
        for (GWControllerActions action : GWControllerActions.values()) {
            filters.addAction(action.name());
        }

        registerReceiver(broadcastReceiver, filters);
    }

    /**
     * Create and show password {@link AlertDialog}
     */
    private void showPasswordDialog() {

        if (passwordDialog == null) {

            // Create password dialog
            final View confirmPasswordLayout = getLayoutInflater().inflate(R.layout.alert_confirm_password, null);
            final TextView titleView         = (TextView) confirmPasswordLayout.findViewById(R.id.alert_pwd_title_text_view);
            final TextView messageTxt        = (TextView) confirmPasswordLayout.findViewById(R.id.alert_pwd_msg_text_view);
            final Button posBtn              = (Button) confirmPasswordLayout.findViewById(R.id.alert_pwd_button_pos);
            final EditText passwordEdit      = (EditText) confirmPasswordLayout.findViewById(R.id.alert_pwd_edit);

            // Create Builder
            final AlertDialog.Builder builder = new AlertDialog.Builder(this);

            builder.setView(confirmPasswordLayout);
            builder.setCancelable(false);

            // Create title and message
            titleView.setText(R.string.alert_pwd_title_label);
            messageTxt.setText(R.string.alert_pwd_msg_label);

            // Define button label. The button is disabled when no text is
            // entered
            posBtn.setText(R.string.alert_pwd_pos_btn_label);
            posBtn.setEnabled(false);
            posBtn.setOnClickListener(new OnClickListener() {

                @Override
                public void onClick(View v) {

                    String passcode = passwordEdit.getText().toString();
                    Log.d(TAG, "Password Dialog, received passcode: '" + passcode + "'");
                    app.setGatewayPasscode(passcode);

                    if (passwordDialog != null) {
                        passwordDialog.dismiss();
                        // After dismissing the dialog resume screen orientation
                        unlockScreenRotation();
                    }
                }// onClick
            });// onClickListener

            // Define passwordEdit
            passwordEdit.addTextChangedListener(new PasswordTransformationMethod() {

                @Override
                public void afterTextChanged(Editable s) {
                    super.afterTextChanged(s);
                    final String txt = s.toString();
                    if (txt != null && txt.length() != 0) {
                        posBtn.setEnabled(true);
                    } else {
                        posBtn.setEnabled(false);
                    }
                }
            });// addTextChangedListener

            passwordDialog = builder.create();
            passwordDialog.setCanceledOnTouchOutside(false);
        }// if :: passwordDialog == null

        if (!passwordDialog.isShowing()) {

            Log.d(TAG, "Show Password Dialog");
            passwordDialog.show();

            // lock screen orientation to prevent leak of resource
            lockScreenRotation();
        } else {
            Log.d(TAG, "The Password Dialog already showing");
        }
    }// showPasswordDialog

}
