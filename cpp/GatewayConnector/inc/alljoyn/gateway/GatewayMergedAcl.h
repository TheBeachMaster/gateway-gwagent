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

#ifndef GATEWAY_MERGED_ACL_H_
#define GATEWAY_MERGED_ACL_H_

#include <alljoyn/Message.h>

#include <list>

namespace ajn {
namespace gw {

static const uint16_t UUID_LENGTH = 16;

/**
 * Class that contains the MergedAcl information
 */
class GatewayMergedAcl {
  public:

    /**
     * Function to unmarshal a message and retrieve MergedAcl data
     * @param msg - msg to unmarshal
     * @return status - success/failure
     */
    QStatus unmarshal(Message& msg);

    /**
     * ObjectDescription structure
     */
    struct ObjectDescription {

        /**
         * the objectPath of the object
         */
        qcc::String objectPath;

        /**
         * Boolean that dictates whether the ObjectPath is a prefix
         */
        bool isPrefix;

        /**
         * The interfaces of the Object
         */
        std::list<qcc::String> interfaces;
    };

    /**
     * Struct representing a Remoted App
     */
    struct RemotedApp {

        /**
         * The deviceId of the App
         */
        qcc::String deviceId;

        /**
         * The AppId of the App
         */
        uint8_t appId[UUID_LENGTH];

        /**
         * The objectDescriptions of the App
         */
        std::list<ObjectDescription> objectDescs;
    };

    /**
     * The exposed Services of the Acls
     */
    std::list<ObjectDescription> m_ExposedServices;

    /**
     * The remoted Apps of the Acls
     */
    std::list<RemotedApp> m_RemotedApps;

  private:

    /**
     * private function used to unmarshal ObjectDescriptions
     * @param objDescArgs - msgArg to unmarshal
     * @param numObjDescs - number of objectDescriptions
     * @param dest - destination to unmarshal them into
     * @return status - sucess/failure
     */
    QStatus unmarshalObjectDescriptions(MsgArg* objDescArgs, size_t numObjDescs, std::list<ObjectDescription>& dest);
};

} //namespace gw
} //namespace ajn

#endif
