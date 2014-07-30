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

#ifndef APPBUSOBJECT_H_
#define APPBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/gateway/GatewayConnectorApp.h>
#include <alljoyn/gateway/GatewayConnectorAppManifest.h>

namespace ajn {
namespace gw {

/**
 * AppBusObject - BusObject for ConnectorApp
 */
class AppBusObject : public BusObject  {
  public:

    /**
     * Constructor for AppBusObject class
     * @param bus - the bus to create the interface
     * @param objectPath - objectPath of BusObject
     * @param status - success/failure
     */
    AppBusObject(BusAttachment* bus, GatewayConnectorApp* connectorApp, qcc::String const& objectPath, QStatus* status);

    /**
     * Destructor for the BusObject
     */
    virtual ~AppBusObject();

    /**
     * Callback for the AppStatus method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetAppStatus(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the RestartApp method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void RestartApp(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the GetManifestFile method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetManifestFile(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the GetManifestInterfaces method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetManifestInterfaces(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the GetMergedAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetMergedAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the UpdateConnectionStatus method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void UpdateConnectionStatus(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the CreateAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void CreateAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the DeleteAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void DeleteAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the ListAcls method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void ListAcls(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Send a signal that the Acls were updated
     * @return status - success/failure
     */
    QStatus SendAclUpdatedSignal();

    /**
     * Send a signal to shutdown the App
     * @return status - success/failure
     */
    QStatus SendShutdownAppSignal();

    /**
     * Send a signal that the AppStatus has changed
     * @return status - success/failure
     */
    QStatus SendAppStatusChangedSignal();

    /**
     * Get Property
     * @param interfaceName - name of the interface
     * @param propName - name of the property
     * @param val - msgArg to fill
     * @return status - success/failure
     */
    QStatus Get(const char* interfaceName, const char* propName, MsgArg& val);

    /**
     * Set Property
     * @param interfaceName - name of the interface
     * @param propName - name of the property
     * @param val - msgArg to fill
     * @return status - success/failure
     */
    QStatus Set(const char* interfaceName, const char* propName, MsgArg& val);

  private:

    /**
     * The Connector App that contains this busObject
     */
    GatewayConnectorApp* m_ConnectorApp;

    /**
     * The ObjectPath of this busObject
     */
    qcc::String m_ObjectPath;

    /**
     * Used to send AppStatus Changed signal
     */
    const ajn::InterfaceDescription::Member* m_AppStatusChanged;

    /**
     * Used to send Acl Updated signal
     */
    const ajn::InterfaceDescription::Member* m_AclUpdated;

    /**
     * Used to send the App shutdown signal
     */
    const ajn::InterfaceDescription::Member* m_ShutdownApp;

    /**
     * Private function to create the App Interface
     * @param bus - bus used to create the interface
     * @return status - success/failure
     */
    QStatus createAppInterface(BusAttachment* bus);

    /**
     * Private function to create the App Connector Interface
     * @param bus - bus used to create the interface
     * @return status - success/failure
     */
    QStatus createAppConnectorInterface(BusAttachment* bus);

    /**
     * Private function to create the Acl Mgmt Interface
     * @param bus - bus used to create the interface
     * @return status - success/failure
     */
    QStatus createAclMgmtInterface(BusAttachment* bus);

    /**
     * private function to marshal ManifestCapability
     * @param capabilities - map to marshal
     * @param msgArg - msgArg to marshal it into
     * @return status - success/failure
     */
    QStatus marshalCapabilities(const GatewayConnectorAppManifest::Capabilities& capabilities, MsgArg* msgArg);

};

} /* namespace gw */
} /* namespace ajn */

#endif /* APPBUSOBJECT_H_ */
