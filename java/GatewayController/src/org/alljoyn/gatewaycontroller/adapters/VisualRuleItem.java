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

import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleInterface;
import org.alljoyn.gatewaycontroller.sdk.RuleObjectDescription.RuleObjectPath;
import org.alljoyn.gatewaycontroller.sdk.ConnectorCapabilities;

/**
 * {@link ConnectorCapabilities} to be visualized by the {@link ConnectorCapabilitiesAdapter}
 */
public class VisualRuleItem implements VisualItem {

    public static enum ItemType {

        OBJECT_PATH, INTERFACE
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
     * 
     * @param visualItem
     *            must be either {@link RuleObjectPath} or
     *            {@link RuleInterface}
     * @param type
     * @throws IllegalArgumentException
     */
    public VisualRuleItem(Object visualItem, ItemType type) {

        if (!(visualItem instanceof RuleObjectPath) && !(visualItem instanceof RuleInterface)) {

            throw new IllegalArgumentException("Wrong visualItem type");
        }

        if (type == null) {

            throw new IllegalArgumentException("Undefined type");
        }

        this.visualItem = visualItem;
        this.type = type;
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
