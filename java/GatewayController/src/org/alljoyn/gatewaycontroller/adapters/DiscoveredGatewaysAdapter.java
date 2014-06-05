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

import org.alljoyn.gatewaycontroller.R;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

/**
 * Manages the list of {@link VisualGateway}s
 */
public class DiscoveredGatewaysAdapter extends VisualArrayAdapter {

	/**
	 * Constructor
	 * @param context
	 */
	DiscoveredGatewaysAdapter(Context context) {
		this(context, -1, null);
	}
	
	/**
	 * Constructor
	 * @param context
	 * @param viewItemResId
	 * @param itemsList
	 */
	public DiscoveredGatewaysAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {
		super(context, viewItemResId, itemsList);
	}

	
	/**
	 * @see android.widget.ArrayAdapter#getView(int, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		View row  = convertView;
		TextView gatewayItem;
		
		if ( row == null ) {
			
			row            = inflater.inflate(viewItemResId, parent, false);
			gatewayItem    = (TextView) row.findViewById(R.id.discoveredGatewayItem);
			row.setTag(gatewayItem);
		}
		else {
			
			gatewayItem = (TextView) row.getTag();
		}
		
		final VisualGateway vg = (VisualGateway) getItem(position);
		gatewayItem.setText(vg.getGateway().getAppName());
		
		return row;
	}

}
