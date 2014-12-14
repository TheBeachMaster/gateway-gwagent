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
#include <alljoyn/gateway/Acl.h>
#include <alljoyn/gateway/RuleObjectDescription.h>

#include <alljoyn/Status.h>
#include <qcc/String.h>

namespace ajn {
namespace gwc {

/**
 * PayloadAdapter class. Used to marshal and unmarshal Acls
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
     * Unmarshal an ObjectDescription from the message - input is MsgArg with friendly names
     * @param objSpecArgs - the msgArgArray containing the objectDescription
     * @param objectDescriptions - the objectDescriptions to fill
     * @return
     */
    static RuleObjectDescription*unmarshalObjectDescriptionsWithFriendlyNames(const MsgArg* objSpecArgs, QStatus& status);

    /**
     * Unmarshal MsgArg without friendly names and find them in the manifest, if possible
     * @param manifestObjectDescriptionInfo AllJoyn object
     * @param connectorCapabilities the rules to use to find the friendly names
     * @return pointer to manifest object desc
     */

    static RuleObjectDescription*unmarshalObjectDescriptionsWithoutNames(const ajn::MsgArg*manifestObjectDescriptionInfo, const std::vector<RuleObjectDescription*>& ruleObjDescriptions, QStatus& status);

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
     * @param status - success/failure
     * @return msgArg - the messageArg to put it into
     */
    static MsgArg* marshalAcl(const Acl& acl, QStatus& status);


    /**
     * MarshalObjectDescriptions - a static function to marshal objectDescriptions
     * @param objects - the objects to marshal
     * @param objectsArray - the array to marshal it into
     * @return status - success/failure
     */
    static QStatus marshalObjectDescriptions(const RuleObjectDescription& object, MsgArg* objectsArrayEntry);

    /**
     * marshalMetadata - static function to Marshal the Metadata Array
     * @param metadata - the metadata to marshal
     * @param status - success/failure
     * @return Marshaled metadata
     */
    static MsgArg*MarshalMetadata(const std::map<qcc::String, qcc::String>& metadata, QStatus& status);

    static QStatus FillRuleObjectDescriptionVector(const ajn::MsgArg*inputArray, std::vector<RuleObjectDescription*>& vector);

    /**
     * MarshalAclRules - a static function to marshal objectDescriptions
     * @param aclRules - the objects to marshal
     * @param aclRulesVector - the vector to marshal it into
     * @return status - success/failure
     */
    static QStatus MarshalAclRules(const AclRules& aclRules, std::vector<MsgArg*>& aclRulesVector);






};

}     /* namespace gwc */
} /* namespace ajn */

#endif /* defined(__GATEWAYCONTROLLERCPP__PAYLOADADAPTER__) */
