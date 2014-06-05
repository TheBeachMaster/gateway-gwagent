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

package org.alljoyn.gatewaycontroller;

import org.alljoyn.bus.AuthListener;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.Status;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 *  Registers {@link AuthListener}. Answers the 
 *  {@link AuthListener#requested(String, String, int, String, org.alljoyn.bus.AuthListener.AuthRequest[])} 
 *  method call with a default password or with the given one. 
 *  If {@link AuthListener#completed(String, String, boolean)} method call is received with authenticated flag of FALSE, 
 *  then the {@link GWControllerActions#GWC_PASSWORD_REQUIRED} intent is bradcasted.  
 */
public class AuthManager implements AuthListener {
	private static final String TAG = "gwcapp" + AuthManager.class.getSimpleName();
	
	/**
	 * Supported authentication mechanisms
	 */
	public static enum AuthMechanisms {
		ALLJOYN_PIN_KEYX,
		ALLJOYN_SRP_KEYX,
		;
	}
	
	/**
	 * The default pass code which is used to authenticate devices, requesting a pin code
	 */
	public static final char [] DEFAULT_PASSCODE = new char[]{'0','0','0','0','0','0'};
	
	/**
	 * The context object which is used to broadcast {@link Intent}
	 */
	private Context context;
	
	/**
	 * Current pass code of a Gateway Agent
	 */
	private String passCode;
	
	/**
	 * Constructor
	 * @param context The {@link Context} object to be used for {@link Intent} broadcasting 
	 */ 
	public AuthManager(Context context) {
		this.context = context;
	}

	/**
	 * Set current pass code to authenticate a Gateway Agent
	 * @param pinCode
	 */
	public void setPassCode(String pinCode) {
		this.passCode = pinCode;
	}
	
	/**
	 * Register the AuthManager
	 * @param bus {@link BusAttachment} to be used for the registration
	 * @return {@link Status} 
	 */
	public Status register(BusAttachment bus) {
		
		String keyStoreFileName = context.getFileStreamPath("alljoyn_keystore").getAbsolutePath();
		Status status           = bus.registerAuthListener(AuthMechanisms.ALLJOYN_PIN_KEYX + " " + AuthMechanisms.ALLJOYN_SRP_KEYX,
														   this,
														   keyStoreFileName);
		
		Log.d(TAG, "AuthListener has registered, Status: '" + status + "'");
		return status;
	}
	
	/**
	 * @see org.alljoyn.bus.AuthListener#requested(java.lang.String, java.lang.String, int, 
	 * java.lang.String, org.alljoyn.bus.AuthListener.AuthRequest[])
	 */
	@Override
	public boolean requested(String mechanism, String peerName, int count, String userName, AuthRequest[] requests) {
		
		char[] currentPasscode = (passCode == null) ? DEFAULT_PASSCODE : passCode.toCharArray();  
		Log.d(TAG, "Received passcode REQUESTED, mechanism: '" + mechanism + "', peerName: '" + peerName + "'");
		
		if ( !AuthMechanisms.ALLJOYN_PIN_KEYX.name().equals(mechanism) && 
				!AuthMechanisms.ALLJOYN_SRP_KEYX.name().equals(mechanism)) {
			
			Log.d(TAG, "An unsupported mechanism: '" + mechanism + "' has been received, returning FALSE");
			return false;
		}
		
		for ( AuthRequest authRequest : requests ) {
			
			if ( authRequest instanceof PasswordRequest ) {
				Log.d(TAG, "The PasswordRequest interface has been found, giving the passcode: '" +
							String.valueOf(currentPasscode) + "'");
				
				((PasswordRequest) authRequest).setPassword(currentPasscode);
				return true;
			}
		}
		
		Log.d(TAG, "The PasswordRequest interface has NOT been found, returning FALSE");
		return false;
	}
	
	/**
	 * @see org.alljoyn.bus.AuthListener#completed(java.lang.String, java.lang.String, boolean)
	 */
	@Override
	public void completed(String mechanism, String peerName, boolean authenticated) {
		
		if ( authenticated ) {
			Log.d(TAG, "The authentication process has been completed successfully. Mechanism: '" + mechanism +
					   "' ,peerName: '" + peerName + "'");
			return;
		}
		
		Log.d(TAG, "The authentication process has FAILED . Mechanism: '" + mechanism + "' ,peerName: '" +
					peerName + "' broadcasting Intent");
		
		Intent intent = new Intent(GWControllerActions.GWC_PASSWORD_REQUIRED.name());
		context.sendBroadcast(intent);
	}

}
