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

#include <alljoyn/AllJoynStd.h>
#include <alljoyn/about/AnnouncementRegistrar.h>
#include <alljoyn/about/AboutPropertyStoreImpl.h>
#include <alljoyn/gateway/GatewayRouterPolicyManager.h>
#include "GatewayConstants.h"
#include <libxml/parser.h>
#include <alljoyn/DBusStd.h>

namespace ajn {
namespace gw {
using namespace services;
using namespace qcc;
using namespace gwConsts;

GatewayRouterPolicyManager::GatewayRouterPolicyManager() : m_AnnounceRegistered(false), m_AutoCommit(false)
{
}

GatewayRouterPolicyManager::~GatewayRouterPolicyManager()
{

}

QStatus GatewayRouterPolicyManager::init(BusAttachment* bus)
{
    QStatus status = ER_OK;

    if (!bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (!m_AnnounceRegistered) {
        status = AnnouncementRegistrar::RegisterAnnounceHandler(*bus, *this, NULL, 0);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register AnnouncementListener. AnnounceHandlerApi not initialized"));
            return status;
        }
        m_AnnounceRegistered = true;
    }
    return status;
}

QStatus GatewayRouterPolicyManager::shutdown(BusAttachment* bus)
{
    QStatus status = ER_OK;
    if (!bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (m_AnnounceRegistered) {
        status = AnnouncementRegistrar::UnRegisterAnnounceHandler(*bus, *this, NULL, 0);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not unregister the AnnounceHandler"));
            return status;
        }
        m_AnnounceRegistered = false;
    }
    return status;
}

const std::map<GatewayAppIdentifier, qcc::String>& GatewayRouterPolicyManager::getAnnouncedDevices() const
{
    return m_AnnouncedDevices;
}

const std::map<qcc::String, std::vector<GatewayAclRules> >& GatewayRouterPolicyManager::getConnectorAppRules() const
{
    return m_ConnectorAppRules;
}

void GatewayRouterPolicyManager::setAutoCommit(bool autoCommit)
{
    m_AutoCommit = autoCommit;
}

bool GatewayRouterPolicyManager::addConnectorAppRules(String const& connectorId, std::vector<GatewayAclRules> const& rules)
{
    std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter;
    if ((iter = m_ConnectorAppRules.find(connectorId)) == m_ConnectorAppRules.end()) {
        m_ConnectorAppRules.insert(std::pair<qcc::String, std::vector<GatewayAclRules> >(connectorId, rules));
    } else {
        iter->second = rules;         //overwrite rules
    }

    if (m_AutoCommit) {
        return (commitAppPolicies(iter) == ER_OK);
    }
    return true;
}

bool GatewayRouterPolicyManager::removeConnectorAppRules(qcc::String const& connectorId)
{
    std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter;
    if ((iter = m_ConnectorAppRules.find(connectorId)) == m_ConnectorAppRules.end()) {
        return false;
    }

    m_ConnectorAppRules.erase(iter);

    int rc = remove((GATEWAY_POLICIES_DIRECTORY + "/apps/" + connectorId + ".xml").c_str());
    if (rc != 0) {
        QCC_DbgHLPrintf(("Could not remove app policy file successfully"));
    }

    if (m_AutoCommit) {
        return (commitAppPolicies(m_ConnectorAppRules.end()) == ER_OK);
    }
    return true;
}

QStatus GatewayRouterPolicyManager::commitAppPolicies(std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter)
{
    BusAttachment* bus = GatewayMgmt::getInstance()->getBusAttachment();
    if (!bus) {
        QCC_LogError(ER_FAIL, ("BusAttachment is null"));
        return ER_FAIL;
    }

    QStatus status = writeDefaultPolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not write the Default Policies"));
        return status;
    }

    if (iter != m_ConnectorAppRules.end()) {
        status = writeAppPolicies(iter);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not write the App Policies"));
            return status;
        }
    }

    bus->EnableConcurrentCallbacks();
    Message reply(*bus);
    const ProxyBusObject& alljoynObj = bus->GetAllJoynProxyObj();
    status = alljoynObj.MethodCall(org::alljoyn::Bus::InterfaceName, "ReloadConfig", NULL, 0, reply);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not reload the config"));
        return status;
    }

    bool success = false;
    reply->GetArgs(AJPARAM_BOOL.c_str(), &success);
    if (!success) {
        QCC_DbgHLPrintf(("Could not reload the config"));
        return ER_INIT_FAILED;
    }

    QCC_DbgPrintf(("Reloaded config successfully"));

    return status;
}

QStatus GatewayRouterPolicyManager::commit()
{
    BusAttachment* bus = GatewayMgmt::getInstance()->getBusAttachment();
    if (!bus) {
        QCC_LogError(ER_FAIL, ("BusAttachment is null"));
        return ER_FAIL;
    }

    QStatus status = writeDefaultPolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not write the Default Policies"));
        return status;
    }

    std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter;
    for (iter = m_ConnectorAppRules.begin(); iter != m_ConnectorAppRules.end(); iter++) {
        status = writeAppPolicies(iter);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not write the App Policies"));
            return status;
        }
    }


    bus->EnableConcurrentCallbacks();
    Message reply(*bus);
    const ProxyBusObject& alljoynObj = bus->GetAllJoynProxyObj();
    status = alljoynObj.MethodCall(org::alljoyn::Bus::InterfaceName, "ReloadConfig", NULL, 0, reply);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not reload the config"));
        return status;
    }

    bool success = false;
    reply->GetArgs(AJPARAM_BOOL.c_str(), &success);
    if (!success) {
        QCC_DbgHLPrintf(("Could not reload the config"));
        return ER_INIT_FAILED;
    }

    QCC_DbgPrintf(("Reloaded config successfully"));

    return status;
}

QStatus GatewayRouterPolicyManager::writeAppPolicies(std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter)
{
    QStatus status = ER_FAIL;
    xmlDocPtr doc = xmlNewDoc((xmlChar*)XML_DEFAULT_VERSION);
    if (doc == NULL) {
        QCC_DbgHLPrintf(("Error creating the xml document tree"));
        return status;
    }

    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        QCC_DbgHLPrintf(("Error creating the xml writer\n"));
        xmlFreeDoc(doc);
        return status;
    }

    int rc = xmlTextWriterStartDocument(writer, "1.0", NULL, NULL);
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"busconfig");
    if (rc < 0) {
        goto exit;
    }

    rc = xmlTextWriterStartElement(writer, (xmlChar*)"policy");
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"user", (xmlChar*)iter->first.c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = writeDefaultUserPolicies(writer, iter->first);
    if (rc < 0) {
        goto exit;
    }
    rc = writeAclUserPolicies(writer, iter->second);
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndDocument(writer); //closes all open tags
    if (rc < 0) {
        goto exit;
    }
    rc = xmlSaveFormatFile((GATEWAY_POLICIES_DIRECTORY + "/apps/" + iter->first + ".xml").c_str(), doc, 1);
    if (rc < 0) {
        status = ER_WRITE_ERROR;
        goto exit;
    }
    status = ER_OK;

exit:

    xmlFreeTextWriter(writer);
    xmlFreeDoc(doc);
    return status;
}

QStatus GatewayRouterPolicyManager::writeDefaultPolicies()
{
    QStatus status = ER_FAIL;

    xmlDocPtr doc = xmlNewDoc((xmlChar*)XML_DEFAULT_VERSION);
    if (doc == NULL) {
        QCC_DbgHLPrintf(("Error creating the xml document tree"));
        return status;
    }
    xmlTextWriterPtr writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        QCC_DbgHLPrintf(("Error creating the xml writer\n"));
        xmlFreeDoc(doc);
        return status;
    }
    std::map<qcc::String, std::vector<GatewayAclRules> >::iterator iter;

    int rc = xmlTextWriterStartDocument(writer, "1.0", NULL, NULL);
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"busconfig");
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteElement(writer, (xmlChar*)"includedir", (xmlChar*)(GATEWAY_POLICIES_DIRECTORY + "/apps").c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"policy");
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"context", (xmlChar*)"default");
    if (rc < 0) {
        goto exit;
    }
    for (iter = m_ConnectorAppRules.begin(); iter != m_ConnectorAppRules.end(); iter++) {

        rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"user", (xmlChar*)iter->first.c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            goto exit;
        }
    }
    rc = xmlTextWriterEndDocument(writer); //closes all open tags
    if (rc < 0) {
        goto exit;
    }
    rc = xmlSaveFormatFile((GATEWAY_POLICIES_DIRECTORY + "/defaultConfig.xml").c_str(), doc, 1);
    if (rc < 0) {
        status = ER_WRITE_ERROR;
        goto exit;
    }
    status = ER_OK;

exit:

    xmlFreeTextWriter(writer);
    xmlFreeDoc(doc);
    return status;
}

int GatewayRouterPolicyManager::writeDefaultUserPolicies(xmlTextWriterPtr writer, qcc::String const& userName)
{
    int rc = 0;

    //deny send_type = *
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"deny");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"*");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    //deny receive_type = *
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"deny");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"*");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    //allow communication with gwMgmtApp
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_destination", (xmlChar*)GW_WELLKNOWN_NAME);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)(AJ_GW_OBJECTPATH + "/" + userName).c_str());
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_call");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }

    //allow default Dbus interface
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_destination", (xmlChar*)org::freedesktop::DBus::WellKnownName);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_sender", (xmlChar*)org::freedesktop::DBus::WellKnownName);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    //allow about communication
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)"/About");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"signal");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }

    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_interface",  (xmlChar*)"org.freedesktop.DBus.Properties");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }

    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_interface",  (xmlChar*)"org.freedesktop.DBus.Properties");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }


    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path",  (xmlChar*)"/org/alljoyn/Bus/Peer");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }

    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path",  (xmlChar*)"/org/alljoyn/Bus/Peer");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }

    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_return");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"error");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)"/About");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_call");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    //allow Device Icon communication
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)"/About/DeviceIcon");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_call");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    return rc;
}

int GatewayRouterPolicyManager::writeAclUserPolicies(xmlTextWriterPtr writer, std::vector<GatewayAclRules> const& rules)
{
    int rc = 0;
    for (size_t policyIndx = 0; policyIndx < rules.size(); policyIndx++) {
        writeExposedServices(writer, rules[policyIndx].getExposedServicesRules());

        const GatewayRemoteAppRules& remoteAppPerms = rules[policyIndx].getRemoteAppRules();
        GatewayRemoteAppRules::const_iterator iter;
        for (iter = remoteAppPerms.begin(); iter != remoteAppPerms.end(); iter++) {

            std::map<GatewayAppIdentifier, qcc::String>::const_iterator announceIter;
            if ((announceIter = m_AnnouncedDevices.find(iter->first)) == m_AnnouncedDevices.end()) {
                continue;
            }
            writeRemotedApps(writer, iter->second, announceIter->second);
        }
    }
    return rc;
}

int GatewayRouterPolicyManager::writeExposedServices(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects)
{
    int rc = 0;
    for (size_t objectsIndx = 0; objectsIndx < objects.size(); objectsIndx++) {
        const qcc::String& objectPath = objects[objectsIndx].getObjectPath();
        bool isPrefix = objects[objectsIndx].getIsPrefix();
        const std::vector<qcc::String>& interfaces = objects[objectsIndx].getInterfaces();
        if (!interfaces.size() && objectPath.compare("*") != 0) {
            //receive_type = method_call
            rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
            if (rc < 0) {
                return rc;
            }
            if (isPrefix) {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path_prefix", (xmlChar*)objectPath.c_str());
            } else {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)objectPath.c_str());
            }
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_call");
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                return rc;
            }
            //send_type=signal
            rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
            if (rc < 0) {
                return rc;
            }
            if (isPrefix) {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path_prefix", (xmlChar*)objectPath.c_str());
            } else {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)objectPath.c_str());
            }
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"signal");
            if (rc < 0) {
                return rc;
            }
        } else {
            for (size_t interfaceIndx = 0; interfaceIndx < interfaces.size(); interfaceIndx++) {
                //receive_type = method_call
                rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
                if (rc < 0) {
                    return rc;
                }
                if (isPrefix) {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path_prefix", (xmlChar*)objectPath.c_str());
                } else {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)objectPath.c_str());
                }
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_interface", (xmlChar*)interfaces[interfaceIndx].c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_call");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
                //send_type=signal
                rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
                if (rc < 0) {
                    return rc;
                }
                if (isPrefix) {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path_prefix", (xmlChar*)objectPath.c_str());
                } else {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)objectPath.c_str());
                }
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_interface", (xmlChar*)interfaces[interfaceIndx].c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"signal");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
            }
        }
    }
    return rc;
}

int GatewayRouterPolicyManager::writeRemotedApps(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects, qcc::String const& uniqueName)
{
    int rc = 0;
    for (size_t objectsIndx = 0; objectsIndx < objects.size(); objectsIndx++) {
        const qcc::String& objectPath = objects[objectsIndx].getObjectPath();
        bool isPrefix = objects[objectsIndx].getIsPrefix();
        const std::vector<qcc::String>& interfaces = objects[objectsIndx].getInterfaces();
        if (!interfaces.size() && objectPath.compare("*") != 0) {
            //send_type = method_call
            rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
            if (rc < 0) {
                return rc;
            }
            if (isPrefix) {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path_prefix", (xmlChar*)objectPath.c_str());
            } else {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)objectPath.c_str());
            }
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_destination", (xmlChar*)uniqueName.c_str());
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_call");
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                return rc;
            }
            //receive_type=signal
            rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
            if (rc < 0) {
                return rc;
            }
            if (isPrefix) {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path_prefix", (xmlChar*)objectPath.c_str());
            } else {
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)objectPath.c_str());
            }
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_sender", (xmlChar*)uniqueName.c_str());
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"signal");
            if (rc < 0) {
                return rc;
            }
        } else {
            for (size_t interfaceIndx = 0; interfaceIndx < interfaces.size(); interfaceIndx++) {
                //receive_type = method_call
                rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
                if (rc < 0) {
                    return rc;
                }
                if (isPrefix) {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path_prefix", (xmlChar*)objectPath.c_str());
                } else {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)objectPath.c_str());
                }
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_interface", (xmlChar*)interfaces[interfaceIndx].c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_destination", (xmlChar*)uniqueName.c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_call");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
                //send_type=signal
                rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
                if (rc < 0) {
                    return rc;
                }
                if (isPrefix) {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path_prefix", (xmlChar*)objectPath.c_str());
                } else {
                    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)objectPath.c_str());
                }
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_interface", (xmlChar*)interfaces[interfaceIndx].c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_sender", (xmlChar*)uniqueName.c_str());
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"signal");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
            }
        }
    }
    return rc;
}

void GatewayRouterPolicyManager::Announce(unsigned short version, unsigned short port, const char* busName, const ObjectDescriptions& objectDescs,
                                          const AboutData& aboutData)
{
    QCC_DbgTrace(("Received Announcement from %s", busName));

    String deviceIdKey = AboutPropertyStoreImpl::getPropertyStoreName(DEVICE_ID);
    String appIdKey = AboutPropertyStoreImpl::getPropertyStoreName(APP_ID);
    String deviceIdValue = "";
    uint8_t* appIdBuffer = NULL;
    size_t numElements = 0;

    for (AboutClient::AboutData::const_iterator it = aboutData.begin(); it != aboutData.end(); ++it) {
        const qcc::String& key = it->first;
        if (key.compare(deviceIdKey) == 0) {
            const ajn::MsgArg& value = it->second;
            char* tmpDeviceId = 0;
            QStatus status = value.Get(AJPARAM_STR.c_str(), &tmpDeviceId);
            if (status == ER_OK) {
                deviceIdValue.assign(tmpDeviceId);
            }
        } else if (key.compare(appIdKey) == 0) {
            const ajn::MsgArg& value = it->second;
            value.Get(AJPARAM_BINARY_ARR.c_str(), &numElements, &appIdBuffer);
        }
    }

    if (!numElements || !deviceIdValue.length()) {
        QCC_DbgHLPrintf(("Announcement missing appId or deviceId - ignoring the announcement"));
        return;
    }

    GatewayAppIdentifier key(appIdBuffer, numElements, deviceIdValue);
    std::map<GatewayAppIdentifier, qcc::String>::iterator iter;
    iter = m_AnnouncedDevices.find(key);
    if (iter == m_AnnouncedDevices.end()) {
        m_AnnouncedDevices.insert(std::pair<GatewayAppIdentifier, qcc::String>(key, busName));
    } else {
        if (iter->second.compare(busName) == 0) {         //busName didn't change in announce
            return;
        }
        iter->second = busName;
    }

    if (m_AutoCommit) {
        commit();         //update config file
    }
}

} /* namespace gw */
} /* namespace ajn */

