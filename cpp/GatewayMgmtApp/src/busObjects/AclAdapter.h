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

#ifndef ACLADAPTER_H_
#define ACLADAPTER_H_

#include <alljoyn/about/AnnounceHandler.h>
#include <alljoyn/gateway/GatewayAclRules.h>
#include <alljoyn/gateway/GatewayAcl.h>

namespace ajn {
namespace gw {

/**
 * AclAdapter class. Used to marshal and unmarshal Acls
 */
class AclAdapter {

  public:

    /**
     * Constructor for AclAdapter
     */
    AclAdapter();

    /**
     * Destructor for AclAdapter
     */
    virtual ~AclAdapter();

    /**
     * UnmarshalAcl - static function to unmarshal an acl
     * @param msg - message to unmarshal
     * @param aclName - the aclName of the Acl
     * @param aclRules - the rules of the Acl
     * @param metadata - the metadata of the Acl
     * @param customMetadata - the customMetadata of the Acl
     * @return status - success/failure
     */
    static QStatus unmarshalAcl(Message& msg, qcc::String* aclName, GatewayAclRules* aclRules, std::map<qcc::String, qcc::String>* metadata,
                                std::map<qcc::String, qcc::String>* customMetadata);

    /**
     * Unmarshal an ObjectDescription from the message
     * @param objDescArgs - the msgArgArray containing the objectDescription
     * @param numObjDescs - the number of objectDescriptions to unmarshal
     * @param objectDesciptions - the objectDescriptions to fill
     * @return
     */
    static QStatus unmarshalObjectDesciptions(MsgArg* objDescArgs, size_t numObjDescs, GatewayRuleObjectDescriptions& objectDesciptions);

    /**
     * Unmarshal the metadata from the message
     * @param metadataArg - the arg containing the metadata
     * @param metadata - the metadata map to fill
     * @return status
     */
    static QStatus unmarshalMetadata(const MsgArg* metadataArg, std::map<qcc::String, qcc::String>* metadata);

    /**
     * MarshalAcl - static function to marshal an acl
     * @param acl - the acl to marshal
     * @param msgArg - the messageArg to put it into
     * @return status - success/failure
     */
    static QStatus marshalAcl(GatewayAcl* acl, ajn::MsgArg* msgArg);

    /**
     * MarshalMergedAcl - marshal a combination of all the active acls
     * @param acls - array of acls to possibly marshal
     * @param msgArg - msgArg to fill
     * @return status - success/failure
     */
    static QStatus marshalMergedAcl(std::map<qcc::String, GatewayAcl*> const& acls, ajn::MsgArg* msgArg);

    /**
     * MarshalObjectDescriptions - a static function to marshal objectDescriptions
     * @param objects - the objects to marshal
     * @param objectsArray - the array to marshal it into
     * @param objectsIndx - the index in the array to start marshaling into
     * @return status - success/failure
     */
    static QStatus marshalObjectDesciptions(const GatewayRuleObjectDescriptions& objects, MsgArg* objectsArray, size_t* objectsIndx);

    /**
     * marshalMetadata - static function to Marshal the Metadata Array
     * @param metadata - the metadata to marshal
     * @param metadataArray - the array to marshal it into
     * @param metadataIndx - the index in the array to start marshaling into
     * @return status - success/failure
     */
    static QStatus marshalMetadata(const std::map<qcc::String, qcc::String>& metadata, MsgArg* metadataArray, size_t* metadataIndx);
};

} /* namespace gw */
} /* namespace ajn */
#endif /* ACLADAPTER_H_ */

