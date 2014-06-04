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

#ifndef ACLBUSOBJECT_H_
#define ACLBUSOBJECT_H_

#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/gateway/GatewayAcl.h>

namespace ajn {
namespace services {

/**
 * AclBusObject - BusObject for Acls
 */
class AclBusObject : public BusObject  {
  public:

    /**
     * Constructor for GatewayAclBusObject class
     * @param bus - the bus to create the interface
     * @param objectPath - objectPath of BusObject
     * @param status - success/failure
     */
    AclBusObject(BusAttachment* bus, GatewayAcl* acl, qcc::String const& objectPath, QStatus* status);

    /**
     * Destructor for the BusObject
     */
    virtual ~AclBusObject();

    /**
     * Callback for the ActivateAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void ActivateAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the GetAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the GetAclStatus method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void GetAclStatus(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the UpdateAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void UpdateAcl(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the UpdateMetaData method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void UpdateMetaData(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the UpdateCustomMetaData method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void UpdateCustomMetaData(const InterfaceDescription::Member* member, Message& msg);

    /**
     * Callback for the DeactivateAcl method
     * @param member - the member called
     * @param msg - the message of the method
     */
    void DeactivateAcl(const InterfaceDescription::Member* member, Message& msg);

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
     * The Acl that contains this BusObject
     */
    GatewayAcl* m_Acl;

    /**
     * The ObjectPath of this ObjectPath
     */
    qcc::String m_ObjectPath;

};

} /* namespace services */
} /* namespace ajn */

#endif /* ACLBUSOBJECT_H_ */
