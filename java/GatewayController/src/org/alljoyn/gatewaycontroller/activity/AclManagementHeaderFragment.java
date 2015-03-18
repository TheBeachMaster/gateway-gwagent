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

import org.alljoyn.gatewaycontroller.GWControllerSampleApplication;
import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.sdk.Acl;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApp;

import android.app.Activity;
import android.app.Application;
import android.app.Fragment;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

/**
 * This fragment includes the {@link ConnectorApp} name and the the name
 * of the selected {@link Acl}
 */
public class AclManagementHeaderFragment extends Fragment implements TextWatcher {

    /**
     * The interface is used to notify about changes of the ACL name
     * {@link EditText} value.
     */
    public static interface AclNameListener {

        /**
         * The ACL name was changed.
         *
         * @param aclName
         *            The text content of the ACL name {@link EditText}
         */
        void onTextChanged(String aclName);
    }

    // =======================================================//

    /**
     * The {@link Application} object
     */
    private GWControllerSampleApplication app;

    /**
     * The listener of the ACL name change events
     */
    private AclNameListener listener;

    /**
     * The name of the selected {@link ConnectorApp}
     */
    private TextView connAppName;

    /**
     * The name of the {@link Acl} that is created or updated
     */
    private EditText aclName;

    /**
     * Constructor
     */
    public AclManagementHeaderFragment() {
    }

    /**
     * @see android.app.Fragment#onCreateView(android.view.LayoutInflater,
     *      android.view.ViewGroup, android.os.Bundle)
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        Activity act = getActivity();
        app          = (GWControllerSampleApplication) act.getApplicationContext();
        listener     = (AclNameListener) act;

        View frgView = inflater.inflate(R.layout.acl_management_header_fragment, container, false);

        connAppName  = (TextView) frgView.findViewById(R.id.aclMgmtConnAppNameTv);
        aclName      = (EditText) frgView.findViewById(R.id.aclMgmtAclNameEt);

        connAppName.setText(app.getSelectedConnectorApp().getFriendlyName());

        Acl acl = app.getSelectedAcl();
        if (acl != null) {

            aclName.setText(acl.getName());
        }

        aclName.addTextChangedListener(this);

        return frgView;
    }

    /**
     * @see android.text.TextWatcher#afterTextChanged(android.text.Editable)
     */
    @Override
    public void afterTextChanged(Editable s) {

        listener.onTextChanged(s.toString());
    }

    /**
     * @see android.text.TextWatcher#beforeTextChanged(java.lang.CharSequence,
     *      int, int, int)
     */
    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    }

    /**
     * @see android.text.TextWatcher#onTextChanged(java.lang.CharSequence, int,
     *      int, int)
     */
    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
    }

    /**
     * @return Return the {@link Acl} name
     */
    public String getAclName() {

        return aclName.getText().toString();
    }
}
