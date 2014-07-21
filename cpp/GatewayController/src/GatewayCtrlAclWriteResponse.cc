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

#include <alljoyn/gateway/GatewayCtrlAclWriteResponse.h>


namespace ajn {
namespace services {

GatewayCtrlAclWriteResponse::GatewayCtrlAclWriteResponse(qcc::String id, AclResponseCode code, GatewayCtrlAccessRules*invalidRules, qcc::String objPath) : m_AclId(id), m_AclCode(code), m_ObjectPath(objPath), m_InvalidRules(invalidRules)
{
}

GatewayCtrlAclWriteResponse::~GatewayCtrlAclWriteResponse()
{

}
qcc::String GatewayCtrlAclWriteResponse::getAclId() {
    return m_AclId;
}



AclResponseCode GatewayCtrlAclWriteResponse::getResponseCode() {
    return m_AclCode;
}


GatewayCtrlAccessRules* GatewayCtrlAclWriteResponse::getInvalidRules() {
    return m_InvalidRules;
}

qcc::String GatewayCtrlAclWriteResponse::getObjectPath()
{
    return m_ObjectPath;
}

QStatus GatewayCtrlAclWriteResponse::release() {
    if (m_InvalidRules) {
        m_InvalidRules->release();
        delete m_InvalidRules;
        m_InvalidRules = NULL;
    }
    return ER_OK;
}
}
}
