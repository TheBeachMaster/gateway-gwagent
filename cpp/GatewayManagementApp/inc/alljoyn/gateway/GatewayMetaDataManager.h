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

#include <alljoyn/gateway/GatewayAppAndDeviceKey.h>
#include <alljoyn/Status.h>
#include <map>

#ifndef GATEWAYMETADATAMANAGER_H_
#define GATEWAYMETADATAMANAGER_H_

namespace ajn {
namespace services {

/**
 * Class that manages the MetaData
 */
class GatewayMetaDataManager {

  public:

    /**
     * Constructor for MetaDataManager
     */
    GatewayMetaDataManager();

    /**
     * Destructor for MetaDataManager
     */
    virtual ~GatewayMetaDataManager();

    /**
     * Initialize the MetaDataManager
     * @return status - success/failure
     */
    QStatus init();

    /**
     * Cleanup the MetaDataManager
     * @return status - success/failure
     */
    QStatus cleanup();

    /**
     * Update the metaData
     * @param metaData - metaData to update
     * @return status - success/failure
     */
    QStatus updateMetaData(std::map<qcc::String, qcc::String> const& metaData);

    /**
     * add MetaDataValues to metaData map based on appandDeviceKey passed in
     * @param key - the key to add metaData for
     * @param metaData - the map to add the MetaData to
     */
    void addMetaDataValues(GatewayAppAndDeviceKey const& key, std::map<qcc::String, qcc::String>* metaData);

    /**
     * Increase the Reference Count for a Remote App
     * @param key - key to add Reference Count
     */
    void incRemoteAppRefCount(GatewayAppAndDeviceKey const& key);

  private:

    /**
     * Class that stores MetaDataValues.
     * AppName, DeviceName, and RefCount which is used during cleanup
     */
    class MetaDataValues {

      public:

        qcc::String appNameKey;
        qcc::String deviceNameKey;
        qcc::String appName;
        qcc::String deviceName;
        int refCount;

        MetaDataValues(qcc::String const& appKey, qcc::String const& deviceKey,
                       qcc::String const& app, qcc::String const& device) :
            appNameKey(appKey), deviceNameKey(deviceKey), appName(app), deviceName(device), refCount(0) { }
    };

    /**
     * MetaData being managed
     */
    std::map<GatewayAppAndDeviceKey, MetaDataValues> m_MetaData;

    /**
     * Write MetaData to file
     * @return status - success/failure
     */
    QStatus writeToFile();

};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAYMETADATAMANAGER_H_ */
