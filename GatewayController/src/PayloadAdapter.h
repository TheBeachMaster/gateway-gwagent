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

#ifndef __GATEWAYCONTROLLERCPP__PAYLOADADAPTER__
#define __GATEWAYCONTROLLERCPP__PAYLOADADAPTER__

#include <alljoyn/about/AnnounceHandler.h>
#include <alljoyn/gateway/GatewayCtrlAccessControlList.h>
#include <alljoyn/gateway/GatewayCtrlManifestObjectDescription.h>

#include <alljoyn/Status.h>
#include <qcc/String.h>

namespace ajn {
namespace services {

/**
 * AclAdapter class. Used to marshal and unmarshal Acls
 */
class PayloadAdapter {

  public:

    /**
     * Constructor for PayloadAdapter
     */
    PayloadAdapter();

    /**
     * Destructor for PayloadAdapter
     */
    virtual ~PayloadAdapter();

    /**
     * UnmarshalAcl - static function to unmarshal an acl
     * @param msg - message to unmarshal
     * @param aclName - the aclName of the Acl
     * @param metaData - the metaData of the Acl
     * @param customMetaData - the customMetaData of the Acl
     * @param status - success/failure
     * @return ACL object
     */
//            static GatewayCtrlAccessControlList *unmarshalAcl(const Message& msg, const qcc::String &aclName, const std::map<qcc::String, qcc::String>& metaData,
//                                        const std::map<qcc::String, qcc::String>& customMetaData, QStatus &status); TODO: do we need this?

    /**
     * Unmarshal an ObjectDescription from the message - input is MsgArg with friendly names
     * @param objSpecArgs - the msgArgArray containing the objectDescription
     * @param objectDescriptions - the objectDescriptions to fill
     * @return
     */
    static GatewayCtrlManifestObjectDescription*unmarshalObjectDescriptionsWithFriendlyNames(const MsgArg* objSpecArgs, QStatus& status);

    /**
     * Unmarshal MsgArg without friendly names and find them in the manifest, if possible
     * @param manifestObjectDescriptionInfo AllJoyn object
     * @param manifestRules the rules to use to find the friendly names
     * @return pointer to manifest object desc
     */

    static GatewayCtrlManifestObjectDescription*unmarshalObjectDescriptionsWithoutNames(const ajn::MsgArg*manifestObjectDescriptionInfo, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules, QStatus& status);

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
     * @param status - success/failure
     * @return msgArg - the messageArg to put it into
     */
    static MsgArg* marshalAcl(const GatewayCtrlAccessControlList& acl, QStatus& status);


    /**
     * MarshalObjectDescriptions - a static function to marshal objectDescriptions
     * @param objects - the objects to marshal
     * @param objectsArray - the array to marshal it into
     * @return status - success/failure
     */
    static QStatus marshalObjectDescriptions(const GatewayCtrlManifestObjectDescription& object, MsgArg* objectsArrayEntry);

    /**
     * marshalMetaData - static function to Marshal the MetaData Array
     * @param metaData - the metaData to marshal
     * @param status - success/failure
     * @return Marshaled metadata
     */
    static MsgArg*MarshalMetaData(const std::map<qcc::String, qcc::String>& metadata, QStatus& status);

    static QStatus FillManifestObjectDescriptionVector(const ajn::MsgArg*inputArray, std::vector<GatewayCtrlManifestObjectDescription*>& vector);

    /**
     * MarshalAccessRules - a static function to marshal objectDescriptions
     * @param accessRules - the objects to marshal
     * @param accessRulesVector - the vector to marshal it into
     * @return status - success/failure
     */
    static QStatus MarshalAccessRules(const GatewayCtrlAccessRules& accessRules, std::vector<MsgArg*>& accessRulesVector);






};

}     /* namespace services */
} /* namespace ajn */

#endif /* defined(__GATEWAYCONTROLLERCPP__PAYLOADADAPTER__) */
