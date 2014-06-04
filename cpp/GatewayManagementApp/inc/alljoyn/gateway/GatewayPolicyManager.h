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

#ifndef GATEWAY_POLICYMANAGER_H_
#define GATEWAY_POLICYMANAGER_H_

#include <map>
#include <vector>
#include <string>
#include <qcc/String.h>
#include <alljoyn/gateway/GatewayPolicy.h>
#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/about/AnnounceHandler.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace ajn {
namespace services {

/**
 * GatewayPolicyManager - Class that manages policies defined and updates the
 * daemon config file accordingly
 */
class GatewayPolicyManager : public AnnounceHandler {

  public:

    /**
     * Constructor for the GatewayPolicyManager class
     */
    GatewayPolicyManager();

    /**
     * Destructor for the GatewayPolicyManager class
     */
    virtual ~GatewayPolicyManager();

    /**
     * initialize the GatewayPolicyManager
     * @param bus - bus used to initialize
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * shutdown the GatewayPolicyManager
     * @param bus - bus used for shutdown
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * Add policies for an app
     * @param appId - the appId to add
     * @param policies - the policies for that app
     * @return success/failure
     */
    bool addAppPolicies(qcc::String const& appId, std::vector<GatewayPolicy> const& policies);

    /**
     * Remove policies for an app
     * @param appId - appId to remove
     * @return success/failure
     */
    bool removeAppPolicies(qcc::String const& appId);

    /**
     * Commit all Policies to the daemon config file
     * @return success/failure
     */
    QStatus commitPolicies();

    /**
     * Callback when an Announce signal arrives
     * @param version
     * @param port
     * @param busName
     * @param objectDescs
     * @param aboutData
     */
    void Announce(unsigned short version, unsigned short port, const char* busName, const ObjectDescriptions& objectDescs,
                  const AboutData& aboutData);

    /**
     * Get the map of announced devices
     * @return announced devices map
     */
    const std::map<GatewayAppAndDeviceKey, qcc::String>& getAnnouncedDevices() const;

    /**
     * Get the currently defined Policy Map
     * @return policy Map
     */
    const std::map<qcc::String, std::vector<GatewayPolicy> >& getPolicyMap() const;

    /**
     * Set the AutoCommit flag. When autocommit is on every change automatically
     * updates the daemon config file. If autocommit is off the daemon config file
     * is only updated when commitPolicies is called
     * @param autoCommit
     */
    void setAutoCommit(bool autoCommit);

  private:

    /**
     * Boolean to track whether the AnnounceHandler was already registered
     */
    bool m_AnnounceRegistered;

    /**
     * Boolean to dictate whether we will commit automatically after each change
     * or only manually via the commitPolicies function
     */
    bool m_AutoCommit;

    /**
     * Map of Announced devices, mapped to their busName
     */
    std::map<GatewayAppAndDeviceKey, qcc::String> m_AnnouncedDevices;

    /**
     * PolicyMap. Map of AppIds to their policies
     */
    std::map<qcc::String, std::vector<GatewayPolicy> > m_PolicyMap;

    /**
     * Helper function to write the default policies to a file
     * @return status - success/failure
     */
    QStatus writeDefaultPolicies();

    /**
     * Write Policies for an app to the daemon config file
     * @param iter - iter pointing to appId to process
     * @return success/failure
     */
    QStatus writeAppPolicies(std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter);

    /**
     * Commit Policies for an app to the daemon config file
     * @param iter - iter pointing to appId to process
     * @return success/failure
     */
    QStatus commitAppPolicies(std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter);

    /**
     * Helper function to write the default policies per user to a file
     * @param writer - the writer to use
     * @param userName - user the policy should be written for
     * @return rc - success/failure
     */
    int writeDefaultUserPolicies(xmlTextWriterPtr writer, qcc::String const& userName);

    /**
     * Helper function to write the default policies per user to a file
     * @param writer - the writer to use
     * @param policies - the policies that should be written for this User
     * @return rc - success/failure
     */
    int writeAclUserPolicies(xmlTextWriterPtr writer, std::vector<GatewayPolicy> const& policies);

    /**
     * Helper function to write RemotedApps to a file
     * @param writer - the writer to use
     * @param objects - the objects to write
     * @param uniqueName - the uniqueName for the object where applicable
     * @return rc - success/failure
     */
    int writeRemotedApps(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects, qcc::String const& uniqueName);

    /**
     * Helper function to write ExposedServices to a file
     * @param writer - the writer to use
     * @param objects - the objects to write
     * @return rc - success/failure
     */
    int writeExposedServices(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects);

};

} /* namespace services */
} /* namespace ajn */

#endif /* GATEWAY_POLICYMANAGER_H_ */
