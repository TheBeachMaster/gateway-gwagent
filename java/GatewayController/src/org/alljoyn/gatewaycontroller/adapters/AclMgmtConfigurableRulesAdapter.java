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

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.alljoyn.gatewaycontroller.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;

/**
 * Manages the list of {@link VisualReverseObjDesc}
 */
public class AclMgmtConfigurableRulesAdapter extends BaseExpandableListAdapter {

	static class InterfaceView {
		
		TextView interfaceName;
		CheckBox configureInterfaceChkBx;
	}
	
	//=============================================//

	static class ObjectPathView {
		
		TextView objectPathName;
		CheckBox configureObjectPathChkBx;
		CheckBox configureSubObjectsChkBx;
	}
	
	//=============================================//
	
	
	/**
	 * Inflater to get {@link View} of the rendered row
	 */
	private final LayoutInflater inflater;
	
	/**
	 * Name of the interfaces, which are the group markers
	 */
	private final List<VisualInterface> interfaceGroupHeaders;

	/**
	 * Object paths, which are group children
	 */
	private final Map<VisualInterface, List<VisualObjectPath>> objectPathChildren;
	
	/**
	 * Constructor
	 * @param context
	 * @param configRulesData
	 * @throws IllegalArgumentException if undefined arguments have been received
	 */
	public AclMgmtConfigurableRulesAdapter(Context context, Map<VisualInterface, List<VisualObjectPath>> configRulesData) {
		
		if ( context == null || configRulesData == null) {
			throw new IllegalArgumentException("bad arguments");
		}
		
		this.inflater               = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		this.objectPathChildren     = configRulesData;
		this.interfaceGroupHeaders  = new ArrayList<VisualInterface>(configRulesData.keySet());
	}
	
	/**
	 * @see android.widget.ExpandableListAdapter#getChild(int, int)
	 */
	@Override
	public Object getChild(int groupPosition, int childPosition) {
		
		
		return objectPathChildren.get( interfaceGroupHeaders.get(groupPosition) ).get(childPosition);
	}

	/**
	 * @see android.widget.ExpandableListAdapter#getChildId(int, int)
	 */
	@Override
	public long getChildId(int groupPosition, int childPosition) {
		
		return childPosition;
	}

	/**
	 * @see android.widget.ExpandableListAdapter#getChildrenCount(int)
	 */
	@Override
	public int getChildrenCount(int groupPosition) {
		
		return objectPathChildren.get(interfaceGroupHeaders.get(groupPosition)).size();
	}

	/**
	 * @see android.widget.ExpandableListAdapter#getGroup(int)
	 */
	@Override
	public Object getGroup(int groupPosition) {
		
		return interfaceGroupHeaders.get(groupPosition);
	}

	/**
	 * @see android.widget.ExpandableListAdapter#getGroupCount()
	 */
	@Override
	public int getGroupCount() {
		
		return interfaceGroupHeaders.size();
	}

	/**
	 * @see android.widget.ExpandableListAdapter#getGroupId(int)
	 */
	@Override
	public long getGroupId(int groupPosition) {
		
		return groupPosition;
	}
	
	/**
	 * @see android.widget.ExpandableListAdapter#hasStableIds()
	 */
	@Override
	public boolean hasStableIds() {
		
		return false;
	}

	/**
	 * @see android.widget.ExpandableListAdapter#isChildSelectable(int, int)
	 */
	@Override
	public boolean isChildSelectable(int groupPosition, int childPosition) {
		
		return false;
	}

	//======================================================//
	
	/**
	 * @see android.widget.ExpandableListAdapter#getGroupView(int, boolean, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getGroupView(final int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
		
		View row = convertView;
		InterfaceView ifaceView;
		
		if ( row == null ) {
			
			row = inflater.inflate(R.layout.acl_management_interface_item, null);
			
			ifaceView                         = new InterfaceView();
			ifaceView.interfaceName           = (TextView) row.findViewById(R.id.aclMgmtInterface);
			ifaceView.configureInterfaceChkBx = (CheckBox) row.findViewById(R.id.aclMgmtAllowInterfaceChkBx);
			
			row.setTag(ifaceView);
		}
		else {
			
			ifaceView = (InterfaceView) row.getTag();
		}
		
		final VisualInterface vIface = (VisualInterface) interfaceGroupHeaders.get(groupPosition);
		String friendlyName          = vIface.getIface().getFriendlyName();
		String name                  = vIface.getIface().getName();
		
		ifaceView.interfaceName.setText(friendlyName.length() > 0 ? friendlyName : name);
		
		ifaceView.configureInterfaceChkBx.setOnCheckedChangeListener( new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				
				vIface.setConfigured(isChecked);
				updateChildViewConfigureEnableState(!isChecked, vIface);
			}
		} );
		
		boolean vIfaceConf = vIface.isConfigured();
		ifaceView.configureInterfaceChkBx.setChecked(vIfaceConf);
		updateChildViewConfigureEnableState(!vIfaceConf, vIface);
		
		return row;
	}
	
	/**
	 * @see android.widget.ExpandableListAdapter#getChildView(int, int, boolean, android.view.View, android.view.ViewGroup)
	 */
	@Override
	public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {

		View row = convertView;
		ObjectPathView objPathView;
		
		if ( row == null ) {
			
			row = inflater.inflate(R.layout.acl_management_object_item, null);
			
			objPathView                          = new ObjectPathView();
			objPathView.objectPathName           = (TextView) row.findViewById(R.id.aclMgmtObject);
			objPathView.configureObjectPathChkBx = (CheckBox) row.findViewById(R.id.aclMgmtAllowObjectChkBx);
			objPathView.configureSubObjectsChkBx = (CheckBox) row.findViewById(R.id.aclMgmtAllowSubObjectsChkBx);
			
			row.setTag(objPathView);
		}
		else {
			
			objPathView = (ObjectPathView) row.getTag();
		}
		
		VisualInterface vIface          = (VisualInterface) interfaceGroupHeaders.get(groupPosition);
		final VisualObjectPath vObjPath = (VisualObjectPath) objectPathChildren.get(vIface).get(childPosition);
		String friendlyName             = vObjPath.getObjPath().getFriendlyName();
		String name                     = vObjPath.getObjPath().getPath();
		
		objPathView.objectPathName.setText(friendlyName.length() > 0 ? friendlyName : name);
		
		objPathView.configureObjectPathChkBx.setOnCheckedChangeListener( new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				
				vObjPath.setConfigured(isChecked);
			}
		} );
		
		objPathView.configureSubObjectsChkBx.setOnCheckedChangeListener( new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				
				vObjPath.getObjPath().setPrefix(isChecked);
			}
		} );
		
		objPathView.configureObjectPathChkBx.setEnabled( vObjPath.isConfiguredEnabled() );
		objPathView.configureSubObjectsChkBx.setEnabled( vObjPath.getObjPath().isPrefixAllowed() );
		
		objPathView.configureObjectPathChkBx.setChecked( vObjPath.isConfigured());
		objPathView.configureSubObjectsChkBx.setChecked( vObjPath.getObjPath().isPrefix() );
		
		return row;
	}

	/**
	 * @return Configured data
	 */
	public Map<VisualInterface, List<VisualObjectPath>> getConfData() {
		
		return objectPathChildren;
	}
	
	/**
	 * If the interface allowInterfaceChkBx is checked, disable all the child object paths
	 * configure checkbox
	 * @param enable Boolean to enable/disable allow checkbox
	 */
	private void updateChildViewConfigureEnableState(boolean enable, VisualInterface vIface) {
		
		for (VisualObjectPath vObjPath : objectPathChildren.get(vIface) ) {
			
			vObjPath.setConfiguredEnabled(enable);
		}
		
		notifyDataSetChanged();
	}
}
