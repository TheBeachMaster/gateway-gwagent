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

#include <alljoyn/gateway/GatewayAcl.h>
#include <alljoyn/gateway/GatewayConnectorApp.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayMetadataManager.h>
#include "busObjects/AclBusObject.h"
#include "busObjects/AppBusObject.h"
#include "GatewayConstants.h"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <libxml/parser.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace ajn {
namespace gw {
using namespace gwConsts;
using namespace qcc;

GatewayAcl::GatewayAcl(qcc::String const& aclId, GatewayConnectorApp* connectorApp) :
    m_AclId(aclId), m_AclName(""), m_ObjectPath(connectorApp->getObjectPath() + "/" + aclId),
    m_AclStatus(GW_AS_INACTIVE), m_AclBusObject(NULL), m_ConnectorApp(connectorApp)
{
}

GatewayAcl::GatewayAcl(qcc::String const& aclId, qcc::String const& aclName, GatewayConnectorApp* connectorApp,
                       GatewayAclRules const& aclRules, std::map<qcc::String, qcc::String> const& customMetadata, AclStatus aclStatus) :
    m_AclId(aclId), m_AclName(aclName), m_ObjectPath(connectorApp->getObjectPath() + "/" + aclId), m_AclRules(aclRules),
    m_AclStatus(aclStatus), m_CustomMetadata(customMetadata), m_AclBusObject(NULL), m_ConnectorApp(connectorApp)
{
}

GatewayAcl::~GatewayAcl()
{

}

QStatus GatewayAcl::init(BusAttachment* bus)
{
    QStatus status = ER_OK;

    if (!bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (m_AclBusObject) {
        QCC_DbgPrintf(("Objects already registered. Ignoring request"));
        return status;
    }

    m_AclBusObject = new AclBusObject(bus, this, m_ObjectPath, &status);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create AppBusObject"));
        return status;
    }

    status = bus->RegisterBusObject(*m_AclBusObject);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register AppBusObject"));
        return status;
    }
    return status;
}

QStatus GatewayAcl::shutdown(BusAttachment* bus)
{
    QStatus status = ER_OK;
    if (!bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not acccept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (!m_AclBusObject) {
        QCC_DbgPrintf(("Objects not registered. Ignoring request"));
        return status;
    }

    bus->UnregisterBusObject(*m_AclBusObject);
    delete m_AclBusObject;
    m_AclBusObject = NULL;

    return status;
}

const GatewayAclRules& GatewayAcl::getAclRules() const
{
    return m_AclRules;
}

const qcc::String& GatewayAcl::getAclId() const
{
    return m_AclId;
}

const qcc::String& GatewayAcl::getAclName() const
{
    return m_AclName;
}

AclStatus GatewayAcl::getAclStatus() const
{
    return m_AclStatus;
}

const qcc::String& GatewayAcl::getObjectPath() const
{
    return m_ObjectPath;
}

const std::map<qcc::String, qcc::String>& GatewayAcl::getCustomMetadata() const
{
    return m_CustomMetadata;
}

AclResponseCode GatewayAcl::updateAclStatus(AclStatus aclStatus)
{
    bool hasActiveAcl = m_ConnectorApp->hasActiveAcl();
    AclStatus previousStatus = m_AclStatus;
    m_AclStatus = aclStatus;

    QStatus status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist aclStatus - rolling back changes"));
        m_AclStatus = previousStatus;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_ConnectorApp->updatePolicyManager();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update policies successfully"));
        return GW_ACL_RC_POLICYMANAGER_ERROR;
    }

    OperationalStatus operationalStatus = m_ConnectorApp->getOperationalStatus();
    if (operationalStatus != GW_OS_RUNNING && !hasActiveAcl && aclStatus == GW_AS_ACTIVE) {
        bool success = m_ConnectorApp->startConnectorApp();
        if (!success) {
            QCC_DbgHLPrintf(("Could not start the app"));
        }
    } else if (operationalStatus == GW_OS_RUNNING && !m_ConnectorApp->hasActiveAcl()) {
        pthread_t thread;
        bool success = m_ConnectorApp->shutdownConnectorApp(&thread);
        if (!success) {
            QCC_DbgHLPrintf(("Could not stop the app"));
        }
    }

    status = m_ConnectorApp->getAppBusObject()->SendAclUpdatedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Sending AclUpdated Failed"));
    }

    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateAcl(qcc::String const& aclName, GatewayAclRules const& aclRules, std::map<qcc::String, qcc::String> const& metadata,
                                      std::map<qcc::String, qcc::String> const& customMetadata)
{
    GatewayMetadataManager* metadataManager = GatewayMgmt::getInstance()->getMetadataManager();
    if (!metadataManager) {
        QCC_DbgHLPrintf(("metadataManager is NULL"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    QStatus status = metadataManager->updateMetadata(metadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist metadata"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    qcc::String previousName = m_AclName;
    GatewayAclRules previousRules = m_AclRules;
    std::map<qcc::String, qcc::String> previousCustomMetadata = m_CustomMetadata;

    m_AclName = aclName;
    m_AclRules = aclRules;
    m_CustomMetadata = customMetadata;

    status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist acl - rolling back changes"));
        m_AclName = previousName;
        m_AclRules = previousRules;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_ConnectorApp->updatePolicyManager();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update policies successfully"));
        return GW_ACL_RC_POLICYMANAGER_ERROR;
    }

    status = m_ConnectorApp->getAppBusObject()->SendAclUpdatedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Sending AclUpdated Failed"));
    }

    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateMetadata(std::map<qcc::String, qcc::String> const& metadata)
{
    GatewayMetadataManager* metadataManager = GatewayMgmt::getInstance()->getMetadataManager();
    if (!metadataManager) {
        QCC_DbgHLPrintf(("metadataManager is NULL"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    QStatus status = metadataManager->updateMetadata(metadata);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist metadata"));
        return GW_ACL_RC_METADATA_ERROR;
    }
    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateCustomMetadata(std::map<qcc::String, qcc::String> const& customMetadata)
{
    std::map<qcc::String, qcc::String> previousCustomMetadata = m_CustomMetadata;
    m_CustomMetadata = customMetadata;

    QStatus status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist acl - rolling back changes"));
        m_CustomMetadata = previousCustomMetadata;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_ConnectorApp->getAppBusObject()->SendAclUpdatedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Sending AclUpdated Failed"));
    }

    return GW_ACL_RC_SUCCESS;
}

QStatus GatewayAcl::loadFromFile(qcc::String const& fileName)
{
    std::ifstream ifs(fileName.c_str());
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    if (content.empty()) {
        QCC_DbgHLPrintf(("Could not read acl"));
        return ER_READ_ERROR;
    }

    xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        QCC_DbgHLPrintf(("Could not create Parser Context"));
        return ER_OUT_OF_MEMORY;
    }

    xmlDocPtr doc = xmlCtxtReadMemory(ctxt, content.c_str(), content.size(), NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOBLANKS);
    if (doc == NULL) {
        QCC_DbgHLPrintf(("Could not parse XML from file"));
        xmlFreeParserCtxt(ctxt);
        return ER_XML_MALFORMED;
    }

    if (ctxt->valid == 0) {
        QCC_DbgHLPrintf(("Invalid XML - validation failed"));
        xmlFreeParserCtxt(ctxt);
        xmlFreeDoc(doc);
        return ER_BUS_BAD_XML;
    }

    xmlNode* root_element = xmlDocGetRootElement(doc);
    for  (xmlNode* currentKey = root_element->children; currentKey != NULL; currentKey = currentKey->next) {

        if (currentKey->type != XML_ELEMENT_NODE || currentKey->children == NULL) {
            continue;
        }

        const xmlChar* keyName = currentKey->name;
        const xmlChar* value = currentKey->children->content;

        if (xmlStrEqual(keyName, (const xmlChar*)"name")) {
            m_AclName.assign((const char*)value);
        } else if (xmlStrEqual(keyName, (const xmlChar*)"status")) {
            int status = atoi((const char*)value);
            if (status < 0 || status > GW_AS_MAX_ACL_STATUS) {
                QCC_DbgHLPrintf(("AclStatus is not a valid value"));
                xmlFreeParserCtxt(ctxt);
                xmlFreeDoc(doc);
                return ER_INVALID_DATA;
            }
            m_AclStatus = (AclStatus)status;
        } else if (xmlStrEqual(keyName, (const xmlChar*)"exposedServices")) {
            GatewayRuleObjectDescriptions exposedServices;
            parseObjects(currentKey, exposedServices);
            m_AclRules.setExposedServicesRules(exposedServices);
        } else if (xmlStrEqual(keyName, (const xmlChar*)"remotedApps")) {
            GatewayRemoteAppRules remoteAppRules;
            parseRemotedApp(currentKey, remoteAppRules);
            m_AclRules.setRemoteAppRules(remoteAppRules);
        } else if (xmlStrEqual(keyName, (const xmlChar*)"customMetadata")) {
            parseMetadata(currentKey, m_CustomMetadata);
        }
    }

    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
    return ER_OK;
}

void GatewayAcl::parseMetadata(xmlNode* currentKey, std::map<qcc::String, qcc::String>& metadata)
{
    for  (xmlNode* metadataNode = currentKey->children; metadataNode != NULL; metadataNode = metadataNode->next) {

        if (metadataNode->type != XML_ELEMENT_NODE || metadataNode->children == NULL) {
            continue;
        }

        if (!xmlStrEqual(metadataNode->name, (const xmlChar*)"data")) {
            continue;
        }

        qcc::String metadataKey = "";
        qcc::String metadataValue = "";
        for  (xmlNode* dataNode = metadataNode->children; dataNode != NULL; dataNode = dataNode->next) {

            if (dataNode->type != XML_ELEMENT_NODE || dataNode->children == NULL) {
                continue;
            }

            const xmlChar* keyName = dataNode->name;
            const xmlChar* value = dataNode->children->content;

            if (xmlStrEqual(keyName, (const xmlChar*)"key")) {
                metadataKey.assign((const char*)value);
            } else if (xmlStrEqual(keyName, (const xmlChar*)"value")) {
                metadataValue.assign((const char*)value);
            }
        }
        metadata.insert(std::pair<qcc::String, qcc::String>(metadataKey, metadataValue));
    }
}

void GatewayAcl::parseObjects(xmlNode* currentKey, GatewayRuleObjectDescriptions& objects)
{
    for  (xmlNode* objectKey = currentKey->children; objectKey != NULL; objectKey = objectKey->next) {

        if (objectKey->type != XML_ELEMENT_NODE || objectKey->children == NULL) {
            continue;
        }

        qcc::String objectPath = "";
        bool isPrefix = false;
        std::vector<qcc::String> interfaces;

        for  (xmlNode* objectPathKey = objectKey->children; objectPathKey != NULL; objectPathKey = objectPathKey->next) {

            if (objectPathKey->type != XML_ELEMENT_NODE || objectPathKey->children == NULL) {
                continue;
            }

            const xmlChar* objectPathKeyName = objectPathKey->name;

            if (xmlStrEqual(objectPathKeyName, (const xmlChar*)"path")) {
                objectPath.assign((const char*)objectPathKey->children->content);
                continue;
            }

            if (xmlStrEqual(objectPathKeyName, (const xmlChar*)"isPrefix")) {
                if (strcmp((const char*)objectPathKey->children->content, "true") == 0) {
                    isPrefix = true;
                }
                continue;
            }

            if (!xmlStrEqual(objectPathKeyName, (const xmlChar*)"interfaces")) {
                continue;
            }

            for  (xmlNode* interfaceKey = objectPathKey->children; interfaceKey != NULL; interfaceKey = interfaceKey->next) {

                if (interfaceKey->type != XML_ELEMENT_NODE || interfaceKey->children == NULL) {
                    continue;
                }

                qcc::String interfaceName = (const char*)interfaceKey->children->content;
                interfaces.push_back(interfaceName);
            }
        }
        GatewayRuleObjectDescription object(objectPath, isPrefix, interfaces);
        objects.push_back(object);
    }
}

void GatewayAcl::parseRemotedApp(xmlNode* currentKey, GatewayRemoteAppRules& remoteAppRules)
{
    GatewayMetadataManager* metadataManager = GatewayMgmt::getInstance()->getMetadataManager();

    for  (xmlNode* deviceKey = currentKey->children; deviceKey != NULL; deviceKey = deviceKey->next) {

        if (deviceKey->type != XML_ELEMENT_NODE || deviceKey->children == NULL) {
            continue;
        }

        qcc::String deviceId = "";
        qcc::String appId = "";
        GatewayRuleObjectDescriptions objects;

        for  (xmlNode* deviceAppKey = deviceKey->children; deviceAppKey != NULL; deviceAppKey = deviceAppKey->next) {

            if (deviceAppKey->type != XML_ELEMENT_NODE || deviceAppKey->children == NULL) {
                continue;
            }

            const xmlChar* deviceAppKeyName = deviceAppKey->name;

            if (xmlStrEqual(deviceAppKeyName, (const xmlChar*)"deviceId")) {
                deviceId.assign((const char*)deviceAppKey->children->content);
                continue;
            }

            if (xmlStrEqual(deviceAppKeyName, (const xmlChar*)"appId")) {
                appId.assign((const char*)deviceAppKey->children->content);
                continue;
            }

            if (!xmlStrEqual(deviceAppKeyName, (const xmlChar*)"objects")) {
                continue;
            }
            parseObjects(deviceAppKey, objects);
        }
        GatewayAppIdentifier appKey(appId, deviceId);
        GatewayRemoteAppRules::iterator it;
        if ((it = remoteAppRules.find(appKey)) != remoteAppRules.end()) {
            it->second.insert(it->second.end(), objects.begin(), objects.end());
        } else {
            if (metadataManager) {
                metadataManager->incRemoteAppRefCount(appKey);
            }
            remoteAppRules.insert(std::pair<GatewayAppIdentifier, GatewayRuleObjectDescriptions>(appKey, objects));
        }
    }
}

QStatus GatewayAcl::writeToFile()
{
    QStatus status = ER_FAIL;
    std::map<qcc::String, qcc::String>::iterator iter;

    std::stringstream statusStr;
    statusStr << m_AclStatus;

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
    rc = xmlTextWriterWriteComment(writer, (xmlChar*)GATEWAY_XML_COMMENT.c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"Acl");
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteAttribute(writer, (xmlChar*)"xmlns", (xmlChar*)GATEWAY_XML_SCHEMA.c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteElement(writer, (xmlChar*)"name", (xmlChar*)m_AclName.c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterWriteElement(writer, (xmlChar*)"status", (xmlChar*)statusStr.str().c_str());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"exposedServices");
    if (rc < 0) {
        goto exit;
    }
    rc = writeObjectsToFile(writer, m_AclRules.getExposedServicesRules());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndElement(writer); //close exposedServices tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"remotedApps");
    if (rc < 0) {
        goto exit;
    }
    rc = writeRemotedAppsToFile(writer, m_AclRules.getRemoteAppRules());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndElement(writer); //close remotedApps tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"customMetadata");
    if (rc < 0) {
        goto exit;
    }
    for (iter = m_CustomMetadata.begin(); iter != m_CustomMetadata.end(); iter++) {
        rc = xmlTextWriterStartElement(writer, (xmlChar*)"data");
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"key", (xmlChar*)iter->first.c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"value", (xmlChar*)iter->second.c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterEndElement(writer); //close data tag
        if (rc < 0) {
            goto exit;
        }
    }
    rc = xmlTextWriterEndElement(writer); //close customMetadata tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndDocument(writer); //closes all open tags (remotedServices and Acl)
    if (rc < 0) {
        goto exit;
    }
    rc = xmlSaveFormatFile((GATEWAY_APPS_DIRECTORY + "/" + m_ConnectorApp->getConnectorId() + "/acls/" + m_AclId).c_str(), doc, 1);
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

int GatewayAcl::writeObjectsToFile(xmlTextWriterPtr writer, const GatewayRuleObjectDescriptions& objects)
{
    int rc = 0;
    for (size_t objectsIndx = 0; objectsIndx < objects.size(); objectsIndx++) {
        rc = xmlTextWriterStartElement(writer, (xmlChar*)"object");
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"path", (xmlChar*)objects[objectsIndx].getObjectPath().c_str());
        if (rc < 0) {
            return rc;
        }
        qcc::String isPrefix = objects[objectsIndx].getIsPrefix() ? "true" : "false";
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"isPrefix", (xmlChar*)isPrefix.c_str());
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterStartElement(writer, (xmlChar*)"interfaces");
        if (rc < 0) {
            return rc;
        }

        const std::vector<qcc::String>& interfaces = objects[objectsIndx].getInterfaces();
        for (size_t interfacesIndx = 0; interfacesIndx < interfaces.size(); interfacesIndx++) {
            rc = xmlTextWriterWriteElement(writer, (xmlChar*)"interface", (xmlChar*)interfaces[interfacesIndx].c_str());
            if (rc < 0) {
                return rc;
            }
        }

        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            return rc;
        }
    }
    return rc;
}

int GatewayAcl::writeRemotedAppsToFile(xmlTextWriterPtr writer, const GatewayRemoteAppRules& remoteAppRules)
{
    int rc = 0;
    GatewayRemoteAppRules::const_iterator iter;
    for (iter = remoteAppRules.begin(); iter != remoteAppRules.end(); iter++) {

        rc = xmlTextWriterStartElement(writer, (xmlChar*)"device");
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"deviceId", (xmlChar*)iter->first.getDeviceId().c_str());
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"appId", (xmlChar*)iter->first.getAppId().c_str());
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterStartElement(writer, (xmlChar*)"objects");
        if (rc < 0) {
            return rc;
        }
        rc = writeObjectsToFile(writer, iter->second);
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterEndElement(writer); //close objects tag
        if (rc < 0) {
            return rc;
        }
        rc = xmlTextWriterEndElement(writer); //close device tag
        if (rc < 0) {
            return rc;
        }
    }
    return rc;
}

} /* namespace gw */
} /* namespace ajn */

