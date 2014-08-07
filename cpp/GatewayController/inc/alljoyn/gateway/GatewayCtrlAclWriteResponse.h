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

#ifndef GatewayCtrlAclWriteResponse_H
#define GatewayCtrlAclWriteResponse_H
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayCtrlEnums.h>
#include <alljoyn/gateway/GatewayCtrlAclRules.h>

namespace ajn {
namespace gwcontroller {
class GatewayCtrlAclWriteResponse {

  public:
    /**
     * Constructor
     * @param id ACL id
     * @param code {@link GatewayCtrlAclResponseCode}
     * @param invalidRules {@link GatewayCtrlAclRules}
     * @param objPath Object path
     */
    GatewayCtrlAclWriteResponse(const qcc::String& id, AclResponseCode code, GatewayCtrlAclRules*invalidRules, const qcc::String& objPath);


    /**
     * Destructor
     */
    virtual ~GatewayCtrlAclWriteResponse();

    /**
     * @return The id of the ACL that the write operation was referred to
     */
    const qcc::String& getAclId();


    /**
     * @return {@link AclResponseCode} of the ACL write action
     */
    AclResponseCode getResponseCode();

    /**
     * @return {@link GatewayCtrlAclRules} with the rules that don't comply with the {@link GatewayCtrlConnectorCapabilities}
     */
    GatewayCtrlAclRules* getInvalidRules();

    /**
     * @return {@link Acl} object path
     */
    const qcc::String& getObjectPath();

    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return {@link QStatus}
     */
    QStatus release();
  private:
    /**
     * Acl id
     */
    qcc::String m_AclId;

    /**
     * Response code
     */
    AclResponseCode m_AclCode;

    /**
     * ACL object path
     */
    qcc::String m_ObjectPath;

    /**
     * The rules that don't comply with the {@link GatewayCtrlConnectorCapabilities}
     */
    GatewayCtrlAclRules*m_InvalidRules;

};
}
}
#endif /* defined(GatewayCtrlAclWriteResponse_H) */
