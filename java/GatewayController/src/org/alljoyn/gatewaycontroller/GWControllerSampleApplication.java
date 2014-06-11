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

import org.alljoyn.about.AboutServiceImpl;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.Status;
import org.alljoyn.bus.alljoyn.DaemonInit;
import org.alljoyn.gatewaycontroller.sdk.AccessControlList;
import org.alljoyn.gatewaycontroller.sdk.AccessRules;
import org.alljoyn.gatewaycontroller.sdk.Gateway;
import org.alljoyn.gatewaycontroller.sdk.GatewayController;
import org.alljoyn.gatewaycontroller.sdk.GatewayControllerException;
import org.alljoyn.gatewaycontroller.sdk.GatewayListChangedHandler;
import org.alljoyn.gatewaycontroller.sdk.RemotedApp;
import org.alljoyn.gatewaycontroller.sdk.ConnectorApplication;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil.SessionResult;
import org.alljoyn.gatewaycontroller.sdk.ajcommunication.GatewayControllerSessionListener;

import android.app.Application;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

/**
 * The {@link Application} main class
 */
public class GWControllerSampleApplication extends Application implements GatewayListChangedHandler {
    private static final String TAG = "gwcapp" + GWControllerSampleApplication.class.getSimpleName();

	static {
		System.loadLibrary("alljoyn_java");
	}   
	
	/**
	 * Session events listener
	 */
	private class SessionListener extends GatewayControllerSessionListener {
		
		/**
		 * @see org.alljoyn.gatewaycontroller.sdk.ajcommunication.GatewayControllerSessionListener#sessionLost(int, int)
		 */
		@Override
		public void sessionLost(int sessionId, int reason) {
			
			super.sessionLost(sessionId, reason);
			GWControllerSampleApplication.this.sessionId = null;
		}

		/**
		 * @see org.alljoyn.gatewaycontroller.sdk.ajcommunication.GatewayControllerSessionListener#sessionJoined(org.alljoyn.gatewaycontroller.sdk.ajcommunication.CommunicationUtil.SessionResult)
		 */
		@Override
		public void sessionJoined(SessionResult result) {
			
			super.sessionJoined(result);
			
			GWControllerSampleApplication app = GWControllerSampleApplication.this;
			Status status               = result.getStatus();
			
			if ( status == Status.OK  || status == Status.ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED ) {
				
				sessionId = result.getSid();
				app.sendBroadcast(new Intent(GWControllerActions.GWC_SESSION_JOINED.name()));
			}
			else {
				
				GWControllerSampleApplication.this.sessionId = null;
				app.sendBroadcast(new Intent(GWControllerActions.GWC_SESSION_JOIN_FAILED.name()));
			}
		}
	}
	
	//==============================================//
	
	
    /** 
     * The daemon should advertise itself "quietly" (directly to the calling port)
     * This is to reply directly to a TC looking for a daemon 
     */  
    private static final String DAEMON_NAME_PREFIX          = "org.alljoyn.BusNode.IoeService";
    
    /** 
     * The daemon should advertise itself "quietly" (directly to the calling port)
     * This is to reply directly to a TC looking for a daemon 
     */  
    private static final String DAEMON_QUIET_PREFIX         = "quiet@";

    /** 
     * The {@link BusAttachment} to be used by the {@link NotificationService}
     */  
    private BusAttachment bus;

    /**
     * Authentication manager. Responsible for providing password for secured interfaces
     */
    private AuthManager authManager;
    
    /**
     * {@link GatewayController} object
     */
    private GatewayController gwController;
    
    /**
     * Session id
     */
    private volatile Integer sessionId;
    
    /**
     * Currently selected gateway
     */
    private volatile Gateway selectedGateway;
    
    /**
     * Currently selected application
     */
    private volatile ConnectorApplication selectedApp;
    
    /**
     * Currently selected ACL
     */
    private volatile AccessControlList selectedAcl;
    
    /**
     * @see android.app.Application#onCreate()
     */
	@Override
    public void onCreate() {
		
		super.onCreate();
		Log.i(TAG, "Starting the Gateway Controller application");
		
		//Initialize the AJ daemon
		DaemonInit.PrepareDaemon(this);
		gwController = GatewayController.getInstance();
		
		try {
			
			prepareAJ();
			authManager = new AuthManager(this);
			authManager.register(bus);
			
			AboutServiceImpl.getInstance().startAboutClient(bus);
			Log.i(TAG, "The Gateway Controller application has been started, Bus unique name: '" + bus.getUniqueName() + "'");
			
			gwController.init(bus);
			
			//Register to receive events about changes in the gateway list
			gwController.setGatewayListChangedHandler(this);
			
		} catch (GatewayControllerException gce) {
			Log.e(TAG, "Failed to connect a BusAttachment to the daemon, Error: '" + gce.getMessage() + "'");
			showToast("Failed to connect to AllJoyn daemon");
		}
		catch (Exception e) {
			Log.e(TAG, "General failure has occurred, Error: '" + e.getMessage() + "'");
			showToast("General failure has occurred");
		}
	}

	/**
	 * Provide the {@link AuthManager} with gateway passcode
	 * @param passcode
	 */
	public void setGatewayPasscode(String passcode) {
		authManager.setPassCode(passcode);
	}
	
	/**
	 * @return Current sessionId
	 */
	public Integer getSessionId() {
		
		return sessionId;
	}
	
	/**
	 * Join the session with the Gateway
	 */
	public void joinSession() {
		
		if ( selectedGateway == null ) {
			
			Log.e(TAG, "Can't join session, no gateway is selected");
			return;
		}
		
		gwController.joinSessionAsync(selectedGateway.getBusName(), new SessionListener());
	}
	
	/**
	 * Leave session
	 */
	public void leaveSession() {

		Log.d(TAG, "Leave session is called");
		
		if ( sessionId == null ) {
			
			return;
		}
		
	    Status status = gwController.leaveSession(sessionId);
			
  	    if ( status == Status.OK ) {
  	    	
  	    	sessionId = null;
		}
	}
	
	/**
	 * @return Currently selected {@link Gateway}
	 */
	public Gateway getSelectedGateway() {
		
		return selectedGateway;
	}
	
	/**
	 * @param selectedGateway Set the selected gateway
	 */
	public void setSelectedGateway(Gateway selectedGateway) {
		this.selectedGateway = selectedGateway;
	}
	
	/**
	 * @return Currently selected {@link ConnectorApplication}
	 */
	public ConnectorApplication getSelectedApp() {
		return selectedApp;
	}

	/**
	 * Set selected {@link ConnectorApplication}
	 * @param selectedApp
	 */
	public void setSelectedApp(ConnectorApplication selectedApp) {
		this.selectedApp = selectedApp;
	}

	/**
	 * @return Currently selected ACL
	 */
	public AccessControlList getSelectedAcl() {
		return selectedAcl;
	}

	/**
	 * @param selectedAcl Set selected ACL
	 */
	public void setSelectedAcl(AccessControlList selectedAcl) {
		this.selectedAcl = selectedAcl;
	}

	/**
	 * @see org.alljoyn.gatewaycontroller.sdk.GatewayListChangedHandler#gatewayChanged()
	 */
	@Override
	public void gatewayChanged() {
		
		Log.d(TAG, "Received gatewayChanged event, sending the Intent");
		sendBroadcast( new Intent(GWControllerActions.GWC_GATEWAY_LIST_CHANGED.name()) );
	}
	
    
    /**
     * For debug print {@link AccessRules} to log
     * @param rules
     */
    public void printAccessRules(AccessRules rules) {
    	
    	Log.v(TAG, "Exposed Services: " + rules.getExposedServices());
		
		Log.d(TAG, "====== REMOTED APPS === ");
		
		for (RemotedApp remApp : rules.getRemotedApps() ) {
			
			Log.v(TAG, "App: '" + remApp + "', ObjDesc: '" + remApp.getObjDescRules() + "'");
			Log.v(TAG, "------------------------------------------------");
		}//for :: remotedApp
		
		Log.d(TAG, "========================= ");
    }
    
	/**
     * Show the Android toast message
     * @param msg
     */
    public void showToast(final String msg) {
        Toast toast = Toast.makeText(this, msg, Toast.LENGTH_LONG);
        toast.show();
    }

	/**
     * Performs all the preparation before starting the service
	 * @throws GatewayControllerException 
     */
    private void prepareAJ() throws GatewayControllerException {

        Log.d(TAG, "Create the BusAttachment");
        bus = new BusAttachment("GatewayController", BusAttachment.RemoteMessage.Receive);

        //For verbose AJ logging use the following lines
        //bus.setDaemonDebug("ALL", 7);
        //bus.setLogLevels("ALLJOYN=7");
        //bus.setLogLevels("ALL=7");
        //bus.useOSLogging(true);

        Status conStatus = bus.connect();
        if ( conStatus != Status.OK ) {
            Log.e(TAG, "Failed connect to bus, Error: '" + conStatus + "'");
            throw new GatewayControllerException("Failed connect to bus, Error: '" + conStatus + "'");
        }

        //Advertise the daemon so that the thin client can find it
        advertiseDaemon();
    }//prepareAJ
    
    /**
     * Advertise the daemon so that the thin client can find it
     * @param logger
     */
   private void advertiseDaemon() throws GatewayControllerException {
       int flag = BusAttachment.ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;

       String daemonName = DAEMON_NAME_PREFIX + ".G" + bus.getGlobalGUIDString();

       //request the name   
       Status reqStatus = bus.requestName(daemonName, flag);
        if (reqStatus == Status.OK) {

            //advertise the name with a quite prefix for TC to find it
            Status adStatus = bus.advertiseName(DAEMON_QUIET_PREFIX + daemonName, SessionOpts.TRANSPORT_ANY );

            if (adStatus != Status.OK){

                bus.releaseName(daemonName);
                Log.e(TAG, "Failed to advertise daemon name " + daemonName + ", Error: '" + adStatus + "'") ;
                throw new GatewayControllerException("Failed to advertise daemon name '" + daemonName + " ', Error: '" + adStatus + "'");
            }
            else{
                Log.d(TAG, "Succefully advertised daemon name: '" + daemonName + "'");
            }
        }
        else {
            Log.d(TAG, "Failed to request the daemon name: '" + daemonName + "', Error: '" + reqStatus + "' ");
            throw new GatewayControllerException("Failed to request the DaemonName: '" + daemonName + "',  Error: '" + reqStatus + "'");
        }

    }//advertiseDaemon



}
