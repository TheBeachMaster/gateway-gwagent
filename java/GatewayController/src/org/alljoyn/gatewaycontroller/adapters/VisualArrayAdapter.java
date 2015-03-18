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

import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;

/**
 * All the adapter classes may extend this class
 */
public abstract class VisualArrayAdapter extends ArrayAdapter<VisualItem> {

    /**
     * The context of the activity running the adapter
     */
    protected final Context context;

    /**
     * The resource id of the viewed item
     */
    protected final int viewItemResId;

    /**
     * The list of the {@link VisualItem}s managed by the adapter
     */
    protected final List<VisualItem> itemsList;

    /**
     * Inflater to get {@link View} of the rendered row
     */
    protected final LayoutInflater inflater;

    /**
     * Constructor
     * 
     * @param context
     *            The context of the activity running the adapter
     * @param viewItemResId
     *            The resource id of the viewed item
     * @param itemsList
     *            The list of the {@link VisualItem}s managed by the adapter
     * @throws IllegalArgumentException
     *             if the received itemsList or context are undefined
     */
    public VisualArrayAdapter(Context context, int viewItemResId, List<VisualItem> itemsList) {

        super(context, viewItemResId, itemsList);

        if (context == null) {
            throw new IllegalArgumentException("context is undefined");
        }

        if (itemsList == null) {
            throw new IllegalArgumentException("itemsList is undefined");
        }

        this.context       = context;
        this.viewItemResId = viewItemResId;
        this.itemsList     = itemsList;
        inflater           = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    /**
     * @see android.widget.ArrayAdapter#add(java.lang.Object)
     */
    @Override
    public void add(VisualItem object) {
        itemsList.add(object);
    }

    /**
     * @see android.widget.ArrayAdapter#addAll(java.util.Collection)
     */
    @Override
    public void addAll(Collection<? extends VisualItem> collection) {
        itemsList.addAll(collection);
    }

    /**
     * @see android.widget.ArrayAdapter#addAll(java.lang.Object[])
     */
    @Override
    public void addAll(VisualItem... items) {
        itemsList.addAll(Arrays.asList(items));
    }

    /**
     * @see android.widget.ArrayAdapter#clear()
     */
    @Override
    public void clear() {
        itemsList.clear();
    }

    /**
     * @see android.widget.ArrayAdapter#getCount()
     */
    @Override
    public int getCount() {
        return itemsList.size();
    }

    /**
     * @see android.widget.ArrayAdapter#getItem(int)
     */
    @Override
    public VisualItem getItem(int position) {

        if (position < 0 || position >= getCount()) {
            return null;
        }

        return itemsList.get(position);
    }

    /**
     * @see android.widget.ArrayAdapter#getItemId(int)
     */
    @Override
    public long getItemId(int position) {

        return position;
    }

    /**
     * If the item is not found "-1" is returned
     * 
     * @see android.widget.ArrayAdapter#getPosition(java.lang.Object)
     */
    @Override
    public int getPosition(VisualItem item) {

        int retVal = -1;

        for (int i = 0; i < getCount(); ++i) {

            VisualItem listItem = itemsList.get(i);

            if (listItem.equals(item)) {
                retVal = i;
                break;
            }
        }

        return retVal;
    }

    /**
     * @see android.widget.ArrayAdapter#insert(java.lang.Object, int)
     * @see List#add(int, Object)
     */
    @Override
    public void insert(VisualItem object, int index) {

        if (index < 0 || index > getCount()) {
            return;
        }

        itemsList.add(index, object);
    }

    /**
     * @return List of the visual items
     */
    public List<VisualItem> getItemsList() {

        return itemsList;
    }
}
