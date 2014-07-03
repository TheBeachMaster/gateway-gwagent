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
#include <alljoyn/gateway/GatewayPolicy.h>
#include <alljoyn/gateway/GatewayAcl.h>

namespace ajn {
namespace services {

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
     * @param policy - the policy of the Acl
     * @param metaData - the metaData of the Acl
     * @param customMetaData - the customMetaData of the Acl
     * @return status - success/failure
     */
    static QStatus unmarshalAcl(Message& msg, qcc::String* aclName, GatewayPolicy* policy, std::map<qcc::String, qcc::String>* metaData,
                                std::map<qcc::String, qcc::String>* customMetaData);

    /**
     * Unmarshal an ObjectDescription from the message
     * @param objSpecArgs - the msgArgArray containing the objectDescription
     * @param numObjSpecs - the number of objectDescriptions to unmarshal
     * @param objectDesciptions - the objectDescriptions to fill
     * @return
     */
    static QStatus unmarshalObjectDesciptions(MsgArg* objSpecArgs, size_t numObjSpecs, GatewayObjectDescriptions& objectDesciptions);

    /**
     * Unmarshal the metaData from the message
     * @param metaDataArg - the arg containing the metaData
     * @param metaData - the metaData map to fill
     * @return status
     */
    static QStatus unmarshalMetaData(const MsgArg* metaDataArg, std::map<qcc::String, qcc::String>* metaData);

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
     * MarshalObjectDesciptions - a static function to marshal objectDescriptions
     * @param objects - the objects to marshal
     * @param objectsArray - the array to marshal it into
     * @param objectsIndx - the index in the array to start marshaling into
     * @return status - success/failure
     */
    static QStatus marshalObjectDesciptions(const GatewayObjectDescriptions& objects, MsgArg* objectsArray, size_t* objectsIndx);

    /**
     * marshalMetaData - static function to Marshal the MetaData Array
     * @param metaData - the metaData to marshal
     * @param metaDataArray - the array to marshal it into
     * @param metaDataIndx - the index in the array to start marshaling into
     * @return status - success/failure
     */
    static QStatus marshalMetaData(const std::map<qcc::String, qcc::String>& metaData, MsgArg* metaDataArray, size_t* metaDataIndx);
};

} /* namespace services */
} /* namespace ajn */
#endif /* ACLADAPTER_H_ */

