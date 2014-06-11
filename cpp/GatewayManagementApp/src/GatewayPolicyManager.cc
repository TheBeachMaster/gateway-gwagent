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
#include <alljoyn/gateway/GatewayPolicyManager.h>
#include "GatewayConstants.h"
#include <libxml/parser.h>
#include <alljoyn/DBusStd.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace gwConsts;

GatewayPolicyManager::GatewayPolicyManager() : m_AnnounceRegistered(false), m_AutoCommit(false)
{
}

GatewayPolicyManager::~GatewayPolicyManager()
{

}

QStatus GatewayPolicyManager::init(BusAttachment* bus)
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

QStatus GatewayPolicyManager::shutdown(BusAttachment* bus)
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

const std::map<GatewayAppAndDeviceKey, qcc::String>& GatewayPolicyManager::getAnnouncedDevices() const
{
    return m_AnnouncedDevices;
}

const std::map<qcc::String, std::vector<GatewayPolicy> >& GatewayPolicyManager::getPolicyMap() const
{
    return m_PolicyMap;
}

void GatewayPolicyManager::setAutoCommit(bool autoCommit)
{
    m_AutoCommit = autoCommit;
}

bool GatewayPolicyManager::addAppPolicies(String const& appId, std::vector<GatewayPolicy> const& policies)
{
    std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter;
    if ((iter = m_PolicyMap.find(appId)) == m_PolicyMap.end()) {
        m_PolicyMap.insert(std::pair<qcc::String, std::vector<GatewayPolicy> >(appId, policies));
    } else {
        iter->second = policies;         //overwrite policies
    }

    if (m_AutoCommit) {
        return (commitAppPolicies(iter) == ER_OK);
    }
    return true;
}

bool GatewayPolicyManager::removeAppPolicies(qcc::String const& appId)
{
    std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter;
    if ((iter = m_PolicyMap.find(appId)) == m_PolicyMap.end()) {
        return false;
    }

    m_PolicyMap.erase(iter);

    int rc = remove((GATEWAY_POLICIES_DIRECTORY + "/apps/" + appId + ".xml").c_str());
    if (rc != 0) {
        QCC_DbgHLPrintf(("Could not remove app policy file successfully"));
    }

    if (m_AutoCommit) {
        return (commitAppPolicies(m_PolicyMap.end()) == ER_OK);
    }
    return true;
}

QStatus GatewayPolicyManager::commitAppPolicies(std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter)
{
    BusAttachment* bus = GatewayManagement::getInstance()->getBusAttachment();
    if (!bus) {
        QCC_LogError(ER_FAIL, ("BusAttachment is null"));
        return ER_FAIL;
    }

    QStatus status = writeDefaultPolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not write the Default Policies"));
        return status;
    }

    if (iter != m_PolicyMap.end()) {
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

QStatus GatewayPolicyManager::commitPolicies()
{
    BusAttachment* bus = GatewayManagement::getInstance()->getBusAttachment();
    if (!bus) {
        QCC_LogError(ER_FAIL, ("BusAttachment is null"));
        return ER_FAIL;
    }

    QStatus status = writeDefaultPolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not write the Default Policies"));
        return status;
    }

    std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter;
    for (iter = m_PolicyMap.begin(); iter != m_PolicyMap.end(); iter++) {
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

QStatus GatewayPolicyManager::writeAppPolicies(std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter)
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

QStatus GatewayPolicyManager::writeDefaultPolicies()
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
    std::map<qcc::String, std::vector<GatewayPolicy> >::iterator iter;

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
    for (iter = m_PolicyMap.begin(); iter != m_PolicyMap.end(); iter++) {

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

int GatewayPolicyManager::writeDefaultUserPolicies(xmlTextWriterPtr writer, qcc::String const& userName)
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
    //allow communication with gwManagementApp
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
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"allow");
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_sender", (xmlChar*)GW_WELLKNOWN_NAME);
    if (rc < 0) {
        return rc;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_path", (xmlChar*)(AJ_GW_OBJECTPATH + "/" + userName).c_str());
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
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)"/About");
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
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)"/About");
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
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)"/About/DeviceIcon");
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
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_path", (xmlChar*)"/About/DeviceIcon");
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

int GatewayPolicyManager::writeAclUserPolicies(xmlTextWriterPtr writer, std::vector<GatewayPolicy> const& policies)
{
    int rc = 0;
    for (size_t policyIndx = 0; policyIndx < policies.size(); policyIndx++) {
        writeExposedServices(writer, policies[policyIndx].getExposedServices());

        const GatewayRemoteAppPermissions& remoteAppPerms = policies[policyIndx].getRemoteAppPermissions();
        GatewayRemoteAppPermissions::const_iterator iter;
        for (iter = remoteAppPerms.begin(); iter != remoteAppPerms.end(); iter++) {

            std::map<GatewayAppAndDeviceKey, qcc::String>::const_iterator announceIter;
            if ((announceIter = m_AnnouncedDevices.find(iter->first)) == m_AnnouncedDevices.end()) {
                continue;
            }
            writeRemotedApps(writer, iter->second, announceIter->second);
        }
    }
    return rc;
}

int GatewayPolicyManager::writeExposedServices(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects)
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
            //send_type=method_return
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
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_return");
            if (rc < 0) {
                return rc;
            }
            //send_type=error
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
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"error");
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterEndElement(writer);
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
                //send_type=method_return
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
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"method_return");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
                //send_type=error
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
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"send_type", (xmlChar*)"error");
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

int GatewayPolicyManager::writeRemotedApps(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects, qcc::String const& uniqueName)
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
            //send_type=method_return
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
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_return");
            if (rc < 0) {
                return rc;
            }
            //send_type=error
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
            rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"error");
            if (rc < 0) {
                return rc;
            }
            rc = xmlTextWriterEndElement(writer);
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
                //send_type=method_return
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
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"method_return");
                if (rc < 0) {
                    return rc;
                }
                rc = xmlTextWriterEndElement(writer);
                if (rc < 0) {
                    return rc;
                }
                //send_type=error
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
                rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"receive_type", (xmlChar*)"error");
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

void GatewayPolicyManager::Announce(unsigned short version, unsigned short port, const char* busName, const ObjectDescriptions& objectDescs,
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

    GatewayAppAndDeviceKey key(appIdBuffer, numElements, deviceIdValue);
    std::map<GatewayAppAndDeviceKey, qcc::String>::iterator iter;
    iter = m_AnnouncedDevices.find(key);
    if (iter == m_AnnouncedDevices.end()) {
        m_AnnouncedDevices.insert(std::pair<GatewayAppAndDeviceKey, qcc::String>(key, busName));
    } else {
        if (iter->second.compare(busName) == 0) {         //busName didn't change in announce
            return;
        }
        iter->second = busName;
    }

    if (m_AutoCommit) {
        commitPolicies();         //update config file
    }
}

} /* namespace services */
} /* namespace ajn */

