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

import android.app.Fragment;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

/**
 * The class presents the manifest file {@link Fragment} of the
 * {@link ConnectorAppCapabilitiesActivity}
 */
public class ConnectorAppCapabilitiesFileFragment extends Fragment {

    /**
     * Manifest text file {@link TextView}
     */
    private TextView manifestText;

    /**
     * Manifest text file content
     */
    private String manifestTextContent;

    /**
     * Constructor
     */
    public ConnectorAppCapabilitiesFileFragment() {
    }

    /**
     * !!! IMPORTANT !!! Use this method to create the {@link Fragment} object
     * of this class.
     *
     * @param manifestTextContent
     *            Manifest file content
     *
     * @return {@link ConnectorAppCapabilitiesFragment}
     */
    public static ConnectorAppCapabilitiesFileFragment createInstance(String manifestTextContent) {

        ConnectorAppCapabilitiesFileFragment frg = new ConnectorAppCapabilitiesFileFragment();
        frg.manifestTextContent = manifestTextContent;

        return frg;
    }

    /**
     * @see android.app.Fragment#onCreate(android.os.Bundle)
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }

    /**
     * @see android.app.Fragment#onCreateView(android.view.LayoutInflater,
     *      android.view.ViewGroup, android.os.Bundle)
     */
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View frgView = inflater.inflate(R.layout.connector_manifest_file_fragment, container, false);
        manifestText = (TextView) frgView.findViewById(R.id.connectorAppManifestFile);
        manifestText.setMovementMethod(new ScrollingMovementMethod());
        manifestText.setText(manifestTextContent);

        return frgView;
    }
}
