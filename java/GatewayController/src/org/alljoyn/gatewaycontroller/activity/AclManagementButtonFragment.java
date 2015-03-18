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

import org.alljoyn.gatewaycontroller.R;
import org.alljoyn.gatewaycontroller.sdk.Acl;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;

/**
 * This fragment includes the button to create or update the
 * {@link Acl}
 */
public class AclManagementButtonFragment extends Fragment implements OnClickListener {

    /**
     * The interface is used to notify when the action button has been clicked.
     */
    public static interface ActionButtonListener {

        void onActionButtonClicked();
    }

    // =======================================================//

    /**
     * Fragment arguments key to pass the button label
     */
    public static final String ACTION_BUTTON_LBL = "ACTION_BUTTON_LABEL";

    /**
     * Fragment arguments key to pass the button enable state
     */
    public static final String ACTION_BUTTON_ENABLE = "ACTION_BUTTON_ENABLE";

    /**
     * The button to create or update the {@link Acl}
     */
    private Button actionButton;

    /**
     * Listener to the action button onClick events
     */
    private ActionButtonListener listener;

    /**
     * Constructor
     */
    public AclManagementButtonFragment() {
    }

    /**
     * @see android.app.Fragment#onCreateView(android.view.LayoutInflater,
     *      android.view.ViewGroup, android.os.Bundle)
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View frgView = inflater.inflate(R.layout.acl_management_button_fragment, container, false);

        listener      = (ActionButtonListener) getActivity();
        actionButton  = (Button) frgView.findViewById(R.id.aclMgmtActionButton);

        Bundle args   = getArguments();
        actionButton.setText(args.getString(ACTION_BUTTON_LBL));
        actionButton.setEnabled(args.getBoolean(ACTION_BUTTON_ENABLE));
        actionButton.setOnClickListener(this);

        return frgView;
    }

    /**
     * Set state of the action button
     */
    public void enableActionButton(boolean enabled) {

        actionButton.setEnabled(enabled);
    }

    /**
     * @see android.view.View.OnClickListener#onClick(android.view.View)
     */
    @Override
    public void onClick(View v) {

        listener.onActionButtonClicked();
    }
}
