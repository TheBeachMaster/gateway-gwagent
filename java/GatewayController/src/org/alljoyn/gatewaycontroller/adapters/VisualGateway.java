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

import org.alljoyn.gatewaycontroller.sdk.GatewayMgmtApp;

/**
 * The {@link GatewayMgmtApp} to be visualized by the {@link DiscoveredGatewaysAdapter}
 */
public class VisualGateway implements VisualItem {

    private GatewayMgmtApp gateway;

    /**
     * Constructor
     * 
     * @param gateway
     *            to be visualized
     */
    public VisualGateway(GatewayMgmtApp gateway) {

        this.gateway = gateway;
    }

    /**
     * @return {@link GatewayMgmtApp}
     */
    public GatewayMgmtApp getGateway() {
        return gateway;
    }

    /**
     * @param gateway
     *            Set the {@link GatewayMgmtApp}
     */
    public void setGateway(GatewayMgmtApp gateway) {
        this.gateway = gateway;
    }
}
