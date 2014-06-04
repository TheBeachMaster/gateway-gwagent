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

#include <alljoyn/gateway/GatewayMetaDataManager.h>
#include "GatewayConstants.h"
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <fstream>

namespace ajn {
namespace services {
using namespace gwConsts;

GatewayMetaDataManager::GatewayMetaDataManager()
{
}

GatewayMetaDataManager::~GatewayMetaDataManager()
{
}

QStatus GatewayMetaDataManager::init()
{
    std::ifstream ifs((GATEWAY_APPS_DIRECTORY + "/MetaData.xml").c_str());
    if (ifs.fail()) {
        QCC_DbgHLPrintf(("MetaData File doesn't exist"));
        return ER_OK;                 //this is not a failure
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    if (content.empty()) {
        QCC_DbgHLPrintf(("MetaData File is empty"));
        return ER_OK;                 //this is not a failure
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

        if (!xmlStrEqual(currentKey->name, (const xmlChar*)"remotedApp")) {
            continue;
        }

        qcc::String appId = "";
        qcc::String appName = "";
        qcc::String deviceId = "";
        qcc::String deviceName = "";

        for  (xmlNode* appKey = currentKey->children; appKey != NULL; appKey = appKey->next) {

            if (appKey->type != XML_ELEMENT_NODE || appKey->children == NULL) {
                continue;
            }

            const xmlChar* keyName = appKey->name;
            const xmlChar* value = appKey->children->content;

            if (xmlStrEqual(keyName, (const xmlChar*)"appId")) {
                appId.assign((char*)value);
            } else if (xmlStrEqual(keyName, (const xmlChar*)"appName")) {
                appName.assign((char*)value);
            } else if (xmlStrEqual(keyName, (const xmlChar*)"deviceId")) {
                deviceId.assign((char*)value);
            } else if (xmlStrEqual(keyName, (const xmlChar*)"deviceName")) {
                deviceName.assign((char*)value);
            }
        }
        GatewayAppAndDeviceKey key(appId, deviceId);
        qcc::String appNameKey = deviceId + "_" + appId + "_APP_NAME";
        qcc::String deviceNameKey = deviceId + "_" + appId + "_DEVICE_NAME";
        MetaDataValues value(appNameKey, deviceNameKey, appName, deviceName);
        m_MetaData.insert(std::pair<GatewayAppAndDeviceKey, MetaDataValues>(key, value));
    }

    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
    return ER_OK;
}

QStatus GatewayMetaDataManager::cleanup()
{
    bool metaDataUpdated = false;
    std::map<GatewayAppAndDeviceKey, MetaDataValues>::iterator iter;
    for (iter = m_MetaData.begin(); iter != m_MetaData.end();) {
        if (!iter->second.refCount) {
            m_MetaData.erase(iter++);
            metaDataUpdated = true;
        } else {
            iter++;
        }
    }
    if (metaDataUpdated) {
        return writeToFile();
    }

    //nothing was updated - just return ER_OK
    return ER_OK;
}

QStatus GatewayMetaDataManager::updateMetaData(std::map<qcc::String, qcc::String> const& metaData)
{
    bool metaDataUpdated = false;

    std::map<qcc::String, qcc::String>::const_iterator iter;
    for (iter = metaData.begin(); iter != metaData.end(); iter++) {

        qcc::String const& key = iter->first;
        size_t typePos = key.find_last_of('_');
        if (typePos == qcc::String::npos) {
            QCC_DbgHLPrintf(("Could not find an '_' where expected"));
            return ER_FAIL;
        }

        typePos = key.find_last_of('_', typePos);
        if (typePos == qcc::String::npos) {
            QCC_DbgHLPrintf(("Could not find an '_' where expected"));
            return ER_FAIL;
        }

        size_t appPos = key.find_last_of('_', typePos);
        if (appPos == qcc::String::npos) {
            QCC_DbgHLPrintf(("Could not find an '_' where expected"));
            return ER_FAIL;
        }

        qcc::String deviceId = key.substr(0, appPos);
        qcc::String appId = key.substr(appPos + 1, (typePos - appPos - 1));
        qcc::String type = key.substr(typePos + 1);

        GatewayAppAndDeviceKey appDeviceKey(appId, deviceId);
        std::map<GatewayAppAndDeviceKey, MetaDataValues>::iterator it;
        if ((it = m_MetaData.find(appDeviceKey)) != m_MetaData.end()) {
            if (type.compare("APP_NAME") == 0) {
                if (!metaDataUpdated && it->second.appName.compare(iter->second) != 0) {
                    metaDataUpdated = true;
                }
                it->second.appName = iter->second;
            } else if (type.compare("DEVICE_NAME") == 0) {
                if (!metaDataUpdated && it->second.deviceName.compare(iter->second) != 0) {
                    metaDataUpdated = true;
                }
                it->second.deviceName = iter->second;
            } else {
                QCC_DbgHLPrintf(("Failure. type is %s", type.c_str()));
                return ER_FAIL;
            }
        } else {
            qcc::String appName = "";
            qcc::String deviceName = "";
            if (type.compare("APP_NAME") == 0) {
                appName = iter->second;
            } else if (type.compare("DEVICE_NAME") == 0) {
                deviceName = iter->second;
            } else {
                QCC_DbgHLPrintf(("Failure. type is %s", type.c_str()));
                return ER_FAIL;
            }
            qcc::String appNameKey = deviceId + "_" + appId + "_APP_NAME";
            qcc::String deviceNameKey = deviceId + "_" + appId + "_DEVICE_NAME";
            MetaDataValues values(appNameKey, deviceNameKey, appName, deviceName);
            m_MetaData.insert(std::pair<GatewayAppAndDeviceKey, MetaDataValues>(appDeviceKey, values));
            metaDataUpdated = true;
        }
    }
    if (metaDataUpdated) {
        QStatus status = writeToFile();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not write to MetaData File"));
        }
        return status;
    }

    //nothing was updated - just return ER_OK
    return ER_OK;
}

void GatewayMetaDataManager::addMetaDataValues(GatewayAppAndDeviceKey const& key, std::map<qcc::String, qcc::String>* metaData)
{
    std::map<GatewayAppAndDeviceKey, MetaDataValues>::iterator iter;
    if ((iter = m_MetaData.find(key)) != m_MetaData.end()) {
        metaData->insert(std::pair<qcc::String, qcc::String>(iter->second.appNameKey, iter->second.appName));
        metaData->insert(std::pair<qcc::String, qcc::String>(iter->second.deviceNameKey, iter->second.deviceName));
    }
}

void GatewayMetaDataManager::incRemoteAppRefCount(GatewayAppAndDeviceKey const& key)
{
    std::map<GatewayAppAndDeviceKey, MetaDataValues>::iterator iter;
    if ((iter = m_MetaData.find(key)) != m_MetaData.end()) {
        iter->second.refCount++;
    }
}

QStatus GatewayMetaDataManager::writeToFile()
{
    QStatus status = ER_FAIL;
    std::map<GatewayAppAndDeviceKey, MetaDataValues>::iterator iter;

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
    rc = xmlTextWriterStartElement(writer, (xmlChar*)"MetaData");
    if (rc < 0) {
        goto exit;
    }
    for (iter = m_MetaData.begin(); iter != m_MetaData.end(); iter++) {
        rc = xmlTextWriterStartElement(writer, (xmlChar*)"remotedApp");
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"appId", (xmlChar*)iter->first.getAppId().c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"deviceId", (xmlChar*)iter->first.getDeviceId().c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"appName", (xmlChar*)iter->second.appName.c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterWriteElement(writer, (xmlChar*)"deviceName", (xmlChar*)iter->second.deviceName.c_str());
        if (rc < 0) {
            goto exit;
        }
        rc = xmlTextWriterEndElement(writer);         //close remoteApp tag
        if (rc < 0) {
            goto exit;
        }
    }
    rc = xmlTextWriterEndDocument(writer);     //closes all open tags (remotedServices and Acl)
    if (rc < 0) {
        goto exit;
    }
    rc = xmlSaveFormatFile((GATEWAY_APPS_DIRECTORY + "/MetaData.xml").c_str(), doc, 1);
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

} /* namespace services */
} /* namespace ajn */
