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

#ifndef GATEWAYAPPMANIFEST_H_
#define GATEWAYAPPMANIFEST_H_

#include <qcc/String.h>
#include <alljoyn/Status.h>
#include <alljoyn/gateway/GatewayConnectorAppCapability.h>
#include <libxml/tree.h>
#include <map>
#include <vector>

namespace ajn {
namespace gw {

/**
 * Class used to parse a Manifest file for an App and store its data
 */
class GatewayConnectorAppManifest {

  public:

    /**
     * typedef to define a Manifest Permissions Vector.
     */
    typedef std::vector<GatewayConnectorAppCapability> Capabilities;

    /**
     * Constructor for GatewayConnectorAppManifest
     */
    GatewayConnectorAppManifest();

    /**
     * Destructor for GatewayConnectorAppManifest
     */
    virtual ~GatewayConnectorAppManifest();

    /**
     * Parse the ManifestFile and use it to fill this class
     * @param manifestFileName - name of file
     * @return success/failure
     */
    QStatus parseManifestFile(qcc::String const& manifestFileName);

    /**
     * Get the FriendlyName
     * @return friendlyName
     */
    const qcc::String& getFriendlyName() const;

    /**
     * Get the Minimum AJ-Sdk version
     * @return minAjSdkVersion
     */
    const qcc::String& getMinAjSdkVersion() const;

    /**
     * Get the packageName
     * @return packageName
     */
    const qcc::String& getPackageName() const;

    /**
     * Get the Version
     * @return version
     */
    const qcc::String& getVersion() const;

    /**
     * Get the ManifestData
     * @return manifestData
     */
    const qcc::String& getManifestData() const;

    /**
     * Get the ExposedServices
     * @return exposedServices
     */
    const Capabilities& getExposedServices() const;

    /**
     * Get the RemotedServices
     * @return remotedServices
     */
    const Capabilities& getRemotedServices() const;

    /**
     * Get the Environment Variables of the App
     * @return environment variables
     */
    const std::vector<qcc::String>& getEnvironmentVariables() const;

    /**
     * Get the Executable Name of the App
     * @return executable Name
     */
    const qcc::String& getExecutableName() const;

    /**
     * Get the Applications Arguments
     * @return arguments
     */
    const std::vector<qcc::String>& getAppArguments() const;

  private:

    /**
     * ManifestData of the App
     */
    qcc::String m_ManifestData;

    /**
     * PackageNAme of the App
     */
    qcc::String m_PackageName;

    /**
     * FriendlyName of the App
     */
    qcc::String m_FriendlyName;

    /**
     * ExecutableName of the App
     */
    qcc::String m_ExecutableName;

    /**
     * Version of the App
     */
    qcc::String m_Version;

    /**
     * MinAjSdkVersion of the App
     */
    qcc::String m_MinAjSdkVersion;

    /**
     * Environment Variables of the App
     */
    std::vector<qcc::String> m_EnvironmentVariables;

    /**
     * Application's arguments
     */
    std::vector<qcc::String> m_AppArguments;

    /**
     * ExposedServices of the App
     */
    Capabilities m_ExposedServices;

    /**
     * RemotedServices of the App
     */
    Capabilities m_RemotedServices;

    /**
     * parseObjects - internal function to help parse the objects
     * @param currentKey - current key in parser
     * @param permissionsMap - map to fill
     */
    void parseObjects(xmlNode* currentKey, Capabilities& capabilitiesMap);

    /**
     * parseExecutionInfo - internal function to help parse the parseExecutionInfo
     * @param currentKey - current key in parser
     */
    void parseExecutionInfo(xmlNode* currentKey);

};

} /* namespace gw */
} /* namespace ajn */
#endif /* GATEWAYAPPMANIFEST_H_ */
