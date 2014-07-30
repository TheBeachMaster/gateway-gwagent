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
#include <alljoyn/gateway/GatewayAclRules.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/about/AnnounceHandler.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace ajn {
namespace gw {

/**
 * GatewayRouterPolicyManager - Class that manages policies defined and updates the
 * daemon config file accordingly
 */
class GatewayRouterPolicyManager : public services::AnnounceHandler {

  public:

    /**
     * Constructor for the GatewayRouterPolicyManager class
     */
    GatewayRouterPolicyManager();

    /**
     * Destructor for the GatewayRouterPolicyManager class
     */
    virtual ~GatewayRouterPolicyManager();

    /**
     * initialize the GatewayRouterPolicyManager
     * @param bus - bus used to initialize
     * @return status - success/failure
     */
    QStatus init(BusAttachment* bus);

    /**
     * shutdown the GatewayRouterPolicyManager
     * @param bus - bus used for shutdown
     * @return status - success/failure
     */
    QStatus shutdown(BusAttachment* bus);

    /**
     * Add rules for a connector app
     * @param connectorId - the connectorId to add
     * @param rules - the rules for that app
     * @return success/failure
     */
    bool addConnectorAppRules(qcc::String const& connectorId, std::vector<GatewayAclRules> const& rules);

    /**
     * Remove rules for a connector app
     * @param connectorId - connectorId to remove
     * @return success/failure
     */
    bool removeConnectorAppRules(qcc::String const& connectorId);

    /**
     * Commit all Rules as policies in the daemon config file
     * @return success/failure
     */
    QStatus commit();

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
    const std::map<GatewayAppIdentifier, qcc::String>& getAnnouncedDevices() const;

    /**
     * Get the currently defined AclRules for each connector App
     * @return connectorAppAclRules
     */
    const std::map<qcc::String, std::vector<GatewayAclRules> >& getConnectorAppRules() const;

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
     * or only manually via the commit function
     */
    bool m_AutoCommit;

    /**
     * Map of Announced devices, mapped to their busName
     */
    std::map<GatewayAppIdentifier, qcc::String> m_AnnouncedDevices;

    /**
     * AclRules. Map of ConnectorIds to their AclRules
     */
    std::map<qcc::String, std::vector<GatewayAclRules> > m_ConnectorAppRules;

    /**
     * Helper function to write the default policies to a file
     * @return status - success/failure
     */
    QStatus writeDefaultPolicies();

    /**
     * Write Policies for an app to the daemon config file
     * @param iter - iter pointing to connectorId to process
     * @return success/failure
     */
    QStatus writeAppPolicies(std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter);

    /**
     * Commit ies for an app to the daemon config file
     * @param iter - iter pointing to connectorId to process
     * @return success/failure
     */
    QStatus commitAppPolicies(std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter);

    /**
     * Helper function to write the default ies per user to a file
     * @param writer - the writer to use
     * @param userName - user the y should be written for
     * @return rc - success/failure
     */
    int writeDefaultUserPolicies(xmlTextWriterPtr writer, qcc::String const& userName);

    /**
     * Helper function to write the default policies per user to a file
     * @param writer - the writer to use
     * @param policies - the policies that should be written for this User
     * @return rc - success/failure
     */
    int writeAclUserPolicies(xmlTextWriterPtr writer, std::vector<GatewayAclRules> const& rules);

    /**
     * Helper function to write RemotedApps to a file
     * @param writer - the writer to use
     * @param objects - the objects to write
     * @param uniqueName - the uniqueName for the object where applicable
     * @return rc - success/failure
     */
    int writeRemotedApps(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects, qcc::String const& uniqueName);

    /**
     * Helper function to write ExposedServices to a file
     * @param writer - the writer to use
     * @param objects - the objects to write
     * @return rc - success/failure
     */
    int writeExposedServices(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects);

};

} /* namespace gw */
} /* namespace ajn */

#endif /* GATEWAY_POLICYMANAGER_H_ */
