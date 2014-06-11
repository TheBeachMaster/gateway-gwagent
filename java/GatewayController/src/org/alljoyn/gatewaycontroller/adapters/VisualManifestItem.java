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

import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppInterface;
import org.alljoyn.gatewaycontroller.sdk.ManifestObjectDescription.ConnAppObjectPath;
import org.alljoyn.gatewaycontroller.sdk.ManifestRules;

/**
 * {@link ManifestRules} to be visualized by the {@link ManifestRulesAdapter}
 */
public class VisualManifestItem implements VisualItem {

	public static enum ItemType {
		
		OBJECT_PATH,
		INTERFACE
	}
	
	/**
	 * The visual item of the {@link ItemType}
	 */
	private Object visualItem;
	
	/**
	 * The type of the visual item
	 */
	private ItemType type;
	
	/**
	 * Constructor
	 * @param visualItem must be either {@link ConnAppObjectPath} or {@link ConnAppInterface}
	 * @param type
	 * @throws IllegalArgumentException
	 */
	public VisualManifestItem(Object visualItem, ItemType type) {
		
		if ( !(visualItem instanceof ConnAppObjectPath) && !(visualItem instanceof ConnAppInterface) ) {
			
			throw new IllegalArgumentException("Wrong visualItem type");
		}
		
		if ( type == null ) {
			
			throw new IllegalArgumentException("Undefined type");
		}
		
		this.visualItem = visualItem;
		this.type       = type;
	}
	
	/**
	 * @return visualItem
	 */
	public Object getVisualItem() {
		return visualItem;
	}

	/**
	 * @return VisualItem type
	 */
	public ItemType getType() {
		return type;
	}
}
