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
#include <alljoyn/gateway/GatewayApp.h>
#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/gateway/GatewayMetaDataManager.h>
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
namespace services {
using namespace gwConsts;
using namespace qcc;

GatewayAcl::GatewayAcl(qcc::String const& aclId, GatewayApp* app) :
    m_AclId(aclId), m_AclName(""), m_ObjectPath(app->getObjectPath() + "/" + aclId),
    m_AclStatus(GW_AS_INACTIVE), m_AclBusObject(NULL), m_App(app)
{
}

GatewayAcl::GatewayAcl(qcc::String const& aclId, qcc::String const& aclName, GatewayApp* app,
                       GatewayPolicy const& policy, std::map<qcc::String, qcc::String> const& customMetaData, AclStatus aclStatus) :
    m_AclId(aclId), m_AclName(aclName), m_ObjectPath(app->getObjectPath() + "/" + aclId), m_Policy(policy), m_AclStatus(aclStatus),
    m_CustomMetaData(customMetaData), m_AclBusObject(NULL), m_App(app)
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

const GatewayPolicy& GatewayAcl::getPolicy() const
{
    return m_Policy;
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

AclResponseCode GatewayAcl::updateAclStatus(AclStatus aclStatus)
{
    bool hasActiveAcl = m_App->hasActiveAcl();
    AclStatus previousStatus = m_AclStatus;
    m_AclStatus = aclStatus;

    QStatus status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist aclStatus - rolling back changes"));
        m_AclStatus = previousStatus;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_App->updatePolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update policies successfully"));
        return GW_ACL_RC_POLICYMANAGER_ERROR;
    }

    OperationalStatus operationalStatus = m_App->getOperationalStatus();
    if (operationalStatus != GW_OS_RUNNING && !hasActiveAcl && aclStatus == GW_AS_ACTIVE) {
        bool success = m_App->startApp();
        if (!success) {
            QCC_DbgHLPrintf(("Could not start the app"));
        }
    } else if (operationalStatus == GW_OS_RUNNING && !m_App->hasActiveAcl()) {
        pthread_t thread;
        bool success = m_App->shutdownApp(&thread);
        if (!success) {
            QCC_DbgHLPrintf(("Could not stop the app"));
        }
    }

    status = m_App->getAppBusObject()->SendAclUpdatedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Sending AclUpdated Failed"));
    }

    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateAcl(qcc::String const& aclName, GatewayPolicy const& policy, std::map<qcc::String, qcc::String> const& metaData,
                                      std::map<qcc::String, qcc::String> const& customMetaData)
{
    GatewayMetaDataManager* metaDataManager = GatewayManagement::getInstance()->getMetaDataManager();
    if (!metaDataManager) {
        QCC_DbgHLPrintf(("metaDataManager is NULL"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    QStatus status = metaDataManager->updateMetaData(metaData);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist metaData"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    qcc::String previousName = m_AclName;
    GatewayPolicy previousPolicy = m_Policy;
    std::map<qcc::String, qcc::String> previousCustomMetaData = m_CustomMetaData;

    m_AclName = aclName;
    m_Policy = policy;
    m_CustomMetaData = customMetaData;

    status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist acl - rolling back changes"));
        m_AclName = previousName;
        m_Policy = previousPolicy;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_App->updatePolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update policies successfully"));
        return GW_ACL_RC_POLICYMANAGER_ERROR;
    }

    status = m_App->getAppBusObject()->SendAclUpdatedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Sending AclUpdated Failed"));
    }

    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateMetaData(std::map<qcc::String, qcc::String> const& metaData)
{
    GatewayMetaDataManager* metaDataManager = GatewayManagement::getInstance()->getMetaDataManager();
    if (!metaDataManager) {
        QCC_DbgHLPrintf(("metaDataManager is NULL"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    QStatus status = metaDataManager->updateMetaData(metaData);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist metaData"));
        return GW_ACL_RC_METADATA_ERROR;
    }
    return GW_ACL_RC_SUCCESS;
}

AclResponseCode GatewayAcl::updateCustomMetaData(std::map<qcc::String, qcc::String> const& customMetaData)
{
    std::map<qcc::String, qcc::String> previousCustomMetaData = m_CustomMetaData;
    m_CustomMetaData = customMetaData;

    QStatus status = writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist acl - rolling back changes"));
        m_CustomMetaData = previousCustomMetaData;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    status = m_App->getAppBusObject()->SendAclUpdatedSignal();
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
            GatewayObjectDescriptions exposedServices;
            parseObjects(currentKey, exposedServices);
            m_Policy.setExposedServices(exposedServices);
        } else if (xmlStrEqual(keyName, (const xmlChar*)"remotedServices")) {
            GatewayRemoteAppPermissions remotePermissions;
            parseRemotedServices(currentKey, remotePermissions);
            m_Policy.setRemoteAppPermissions(remotePermissions);
        } else if (xmlStrEqual(keyName, (const xmlChar*)"customMetaData")) {
            parseMetaData(currentKey, m_CustomMetaData);
        }
    }

    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
    return ER_OK;
}

void GatewayAcl::parseMetaData(xmlNode* currentKey, std::map<qcc::String, qcc::String>& metaData)
{
    for  (xmlNode* metaDataNode = currentKey->children; metaDataNode != NULL; metaDataNode = metaDataNode->next) {

        if (metaDataNode->type != XML_ELEMENT_NODE || metaDataNode->children == NULL) {
            continue;
        }

        if (!xmlStrEqual(metaDataNode->name, (const xmlChar*)"data")) {
            continue;
        }

        qcc::String metaDataKey = "";
        qcc::String metaDataValue = "";
        for  (xmlNode* dataNode = metaDataNode->children; dataNode != NULL; dataNode = dataNode->next) {

            if (dataNode->type != XML_ELEMENT_NODE || dataNode->children == NULL) {
                continue;
            }

            const xmlChar* keyName = dataNode->name;
            const xmlChar* value = dataNode->children->content;

            if (xmlStrEqual(keyName, (const xmlChar*)"key")) {
                metaDataKey.assign((const char*)value);
            } else if (xmlStrEqual(keyName, (const xmlChar*)"value")) {
                metaDataValue.assign((const char*)value);
            }
        }
        metaData.insert(std::pair<qcc::String, qcc::String>(metaDataKey, metaDataValue));
    }
}

void GatewayAcl::parseObjects(xmlNode* currentKey, GatewayObjectDescriptions& objects)
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
        GatewayObjectDescription object(objectPath, isPrefix, interfaces);
        objects.push_back(object);
    }
}

void GatewayAcl::parseRemotedServices(xmlNode* currentKey, GatewayRemoteAppPermissions& remotePermissions)
{
    GatewayMetaDataManager* metaDataManager = GatewayManagement::getInstance()->getMetaDataManager();

    for  (xmlNode* deviceKey = currentKey->children; deviceKey != NULL; deviceKey = deviceKey->next) {

        if (deviceKey->type != XML_ELEMENT_NODE || deviceKey->children == NULL) {
            continue;
        }

        qcc::String deviceId = "";
        qcc::String appId = "";
        GatewayObjectDescriptions objects;

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
        GatewayAppAndDeviceKey appIdKey(appId, deviceId);
        GatewayRemoteAppPermissions::iterator it;
        if ((it = remotePermissions.find(appIdKey)) != remotePermissions.end()) {
            it->second.insert(it->second.end(), objects.begin(), objects.end());
        } else {
            if (metaDataManager) {
                metaDataManager->incRemoteAppRefCount(appIdKey);
            }
            remotePermissions.insert(std::pair<GatewayAppAndDeviceKey, GatewayObjectDescriptions>(appIdKey, objects));
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
    rc = writeObjectsToFile(writer, m_Policy.getExposedServices());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndElement(writer); //close exposedServices tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"remotedServices");
    if (rc < 0) {
        goto exit;
    }
    rc = writeRemotedServicesToFile(writer, m_Policy.getRemoteAppPermissions());
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndElement(writer); //close remotedServices tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"customMetaData");
    if (rc < 0) {
        goto exit;
    }
    for (iter = m_CustomMetaData.begin(); iter != m_CustomMetaData.end(); iter++) {
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
    rc = xmlTextWriterEndElement(writer); //close customMetaData tag
    if (rc < 0) {
        goto exit;
    }
    rc = xmlTextWriterEndDocument(writer); //closes all open tags (remotedServices and Acl)
    if (rc < 0) {
        goto exit;
    }
    rc = xmlSaveFormatFile((GATEWAY_APPS_DIRECTORY + "/" + m_App->getAppId() + "/acls/" + m_AclId).c_str(), doc, 1);
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

int GatewayAcl::writeObjectsToFile(xmlTextWriterPtr writer, const GatewayObjectDescriptions& objects)
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

const std::map<qcc::String, qcc::String>& GatewayAcl::getCustomMetaData() const
{
    return m_CustomMetaData;
}

int GatewayAcl::writeRemotedServicesToFile(xmlTextWriterPtr writer, const GatewayRemoteAppPermissions& remotePermissions)
{
    int rc = 0;
    GatewayRemoteAppPermissions::const_iterator iter;
    for (iter = remotePermissions.begin(); iter != remotePermissions.end(); iter++) {

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

} /* namespace services */
} /* namespace ajn */

