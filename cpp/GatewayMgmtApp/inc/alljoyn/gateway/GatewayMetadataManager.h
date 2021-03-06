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

#include <alljoyn/gateway/GatewayAppIdentifier.h>
#include <alljoyn/Status.h>
#include <map>

#ifndef GATEWAYMETADATAMANAGER_H_
#define GATEWAYMETADATAMANAGER_H_

namespace ajn {
namespace gw {

/**
 * Class that manages the Metadata
 */
class GatewayMetadataManager {

  public:

    /**
     * Constructor for MetadataManager
     */
    GatewayMetadataManager();

    /**
     * Destructor for MetadataManager
     */
    virtual ~GatewayMetadataManager();

    /**
     * Initialize the MetadataManager
     * @return status - success/failure
     */
    QStatus init();

    /**
     * Cleanup the MetadataManager
     * @return status - success/failure
     */
    QStatus cleanup();

    /**
     * Update the metadata
     * @param metadata - metadata to update
     * @return status - success/failure
     */
    QStatus updateMetadata(std::map<qcc::String, qcc::String> const& metadata);

    /**
     * add MetadataValues to metadata map based on key passed in
     * @param key - the key to add metadata for
     * @param metadata - the map to add the Metadata to
     */
    void addMetadataValues(GatewayAppIdentifier const& key, std::map<qcc::String, qcc::String>* metadata);

    /**
     * Increase the Reference Count for a Remote App
     * @param key - key to add Reference Count
     */
    void incRemoteAppRefCount(GatewayAppIdentifier const& key);

  private:

    /**
     * Class that stores MetadataValues.
     * AppName, DeviceName, and RefCount which is used during cleanup
     */
    class MetadataValues {

      public:

        qcc::String appNameKey;
        qcc::String deviceNameKey;
        qcc::String appName;
        qcc::String deviceName;
        int refCount;

        MetadataValues(qcc::String const& appKey, qcc::String const& deviceKey,
                       qcc::String const& app, qcc::String const& device) :
            appNameKey(appKey), deviceNameKey(deviceKey), appName(app), deviceName(device), refCount(0) { }
    };

    /**
     * Metadata being managed
     */
    std::map<GatewayAppIdentifier, MetadataValues> m_Metadata;

    /**
     * Write Metadata to file
     * @return status - success/failure
     */
    QStatus writeToFile();

};

} /* namespace gw */
} /* namespace ajn */

#endif /* GATEWAYMETADATAMANAGER_H_ */
