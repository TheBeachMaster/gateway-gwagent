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

import org.alljoyn.gatewaycontroller.sdk.Gateway;

/**
 * The {@link Gateway} to be visualized by the {@link DiscoveredGatewaysAdapter}
 */
public class VisualGateway implements VisualItem {

	private Gateway gateway;

	/**
	 * Constructor
	 * @param gateway to be visualized
	 */
	public VisualGateway(Gateway gateway) {
		
		this.gateway = gateway;
	}
	
	/**
	 * @return {@link Gateway}
	 */
	public Gateway getGateway() {
		return gateway;
	}

	/**
	 * @param gateway Set the {@link Gateway}
	 */
	public void setGateway(Gateway gateway) {
		this.gateway = gateway;
	}
}
