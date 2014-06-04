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

#ifndef GATEWAYAPPIDDEVICEIDKEY_H_
#define GATEWAYAPPIDDEVICEIDKEY_H_

#include <qcc/String.h>

namespace ajn {
namespace services {

#define APPID_LENGTH 16

/**
 * Class used to define a key combining appId and DeviceId
 */
class GatewayAppAndDeviceKey {
  public:

    /**
     * Constructor for GatewayAppAndDeviceKey
     * @param appId as a qcc::String
     * @param deviceId
     */
    GatewayAppAndDeviceKey(qcc::String const& appId, qcc::String const& deviceId);

    /**
     * Constructor for GatewayAppAndDeviceKey
     * @param appId as array of uint8_t
     * @param appIdLen len of array
     * @param deviceId
     */
    GatewayAppAndDeviceKey(uint8_t* appId, size_t appIdLen, qcc::String const& deviceId);

    /**
     * Destructor for GatewayAppAndDeviceKey
     */
    virtual ~GatewayAppAndDeviceKey();

    /**
     * operator < used for key comparisons
     * @param other
     * @return boolean smaller or not
     */
    bool operator<(const GatewayAppAndDeviceKey& other) const;

    /**
     * operator == used for map finds
     * @param other
     * @return boolean equal or not
     */
    bool operator==(const GatewayAppAndDeviceKey& other) const;

    /**
     * get the AppId as string
     * @return appID
     */
    const qcc::String& getAppId() const;

    /**
     * get the AppIdHex
     * @return appID
     */
    const uint8_t* getAppIdHex() const;

    /**
     * Get the length of the AppId hex value
     * @return appIdHexLength
     */
    size_t getAppIdHexLength() const;

    /**
     * get the DeviceId
     * @return DeviceId
     */
    const qcc::String& getDeviceId() const;

  private:

    /**
     * the AppId of the Key
     */
    qcc::String m_AppId;

    /**
     * The AppId in Hex form of the Key
     */
    uint8_t m_AppIdHex[APPID_LENGTH];

    /**
     * The DeviceId of the Key
     */
    qcc::String m_DeviceId;

};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYAPPIDDEVICEIDKEY_H_ */
