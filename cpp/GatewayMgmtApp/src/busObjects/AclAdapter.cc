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

#include "AclAdapter.h"
#include "../GatewayConstants.h"
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayMetadataManager.h>

namespace ajn {
namespace gw {
using namespace gwConsts;

AclAdapter::AclAdapter() {

}

AclAdapter::~AclAdapter() {

}

QStatus AclAdapter::unmarshalObjectDesciptions(MsgArg* objDescArgs, size_t numObjDescs, GatewayRuleObjectDescriptions& objectDesciptions)
{
    QStatus status = ER_OK;
    for (size_t i = 0; i < numObjDescs; ++i) {

        char* objectPath;
        bool isPrefix;
        std::vector<qcc::String> interfaces;
        MsgArg* interfacesArray;
        size_t interfacesSize;
        status = objDescArgs[i].Get(AJPARAM_INTERFACE_INFO.c_str(), &objectPath, &isPrefix, &interfacesSize, &interfacesArray);
        if (status != ER_OK) {
            return status;
        }

        for (size_t j = 0; j < interfacesSize; ++j) {
            char* interfaceName;
            status = interfacesArray[j].Get(AJPARAM_STR.c_str(), &interfaceName);
            if (status != ER_OK) {
                return status;
            }
            interfaces.push_back(interfaceName);
        }
        GatewayRuleObjectDescription object(objectPath, isPrefix, interfaces);
        objectDesciptions.push_back(object);
    }

    return ER_OK;
}

QStatus AclAdapter::unmarshalMetadata(const MsgArg* metadataArg, std::map<qcc::String, qcc::String>* metadata)
{
    MsgArg* metadataArray;
    size_t metadataSize;
    QStatus status = metadataArg->Get(AJPARAM_ACL_METADATA_ARRAY.c_str(), &metadataSize, &metadataArray);
    if (status != ER_OK) {
        return status;
    }
    for (size_t i = 0; i < metadataSize; ++i) {
        char* key;
        char* value;
        status = metadataArray[i].Get(AJPARAM_ACL_METADATA.c_str(), &key, &value);
        if (status != ER_OK) {
            return status;
        }
        metadata->insert(std::pair<qcc::String, qcc::String>(key, value));
    }
    return status;
}

QStatus AclAdapter::unmarshalAcl(Message& msg, qcc::String* aclName, GatewayAclRules* aclRules, std::map<qcc::String, qcc::String>* metadata,
                                 std::map<qcc::String, qcc::String>* customMetadata)
{
    if (aclName == 0 || aclRules == 0) {
        return ER_INVALID_DATA;
    }

    const ajn::MsgArg* args = 0;
    size_t numArgs = 0;
    msg->GetArgs(numArgs, args);
    if (numArgs != 5) {
        return ER_INVALID_DATA;
    }

    size_t argsIndx = 0;

    char* aclNameChar = NULL;
    QStatus status = args[argsIndx++].Get(AJPARAM_STR.c_str(), &aclNameChar);
    if (status != ER_OK) {
        return status;
    }
    aclName->assign(aclNameChar);

    GatewayRuleObjectDescriptions exposedServices;
    MsgArg* exposedServicesArray;
    size_t exposedServicesSize;
    status = args[argsIndx++].Get(AJPARAM_INTERFACE_INFO_ARRAY.c_str(), &exposedServicesSize, &exposedServicesArray);
    if (status != ER_OK) {
        return status;
    }

    status = unmarshalObjectDesciptions(exposedServicesArray, exposedServicesSize, exposedServices);
    if (status != ER_OK) {
        return status;
    }
    aclRules->setExposedServicesRules(exposedServices);

    GatewayRemoteAppRules remoteAppRules;
    MsgArg* remotedAppsArray;
    size_t remotedAppsSize;
    status = args[argsIndx++].Get(AJPARAM_REMOTED_APPS_ARRAY.c_str(), &remotedAppsSize, &remotedAppsArray);
    if (status != ER_OK) {
        return status;
    }

    for (size_t i = 0; i < remotedAppsSize; ++i) {
        char* deviceId;
        uint8_t* appId;
        size_t appIdLen;
        MsgArg* objectsArray;
        size_t objectsSize;
        status = remotedAppsArray[i].Get(AJPARAM_REMOTED_APPS.c_str(), &deviceId, &appIdLen, &appId, &objectsSize, &objectsArray);
        if (status != ER_OK) {
            return status;
        }

        GatewayRuleObjectDescriptions remotedServices;
        status = unmarshalObjectDesciptions(objectsArray, objectsSize, remotedServices);
        if (status != ER_OK) {
            return status;
        }

        GatewayAppIdentifier appkey(appId, appIdLen, deviceId);
        GatewayRemoteAppRules::iterator it;

        if ((it = remoteAppRules.find(appkey)) != remoteAppRules.end()) {
            it->second.insert(it->second.end(), remotedServices.begin(), remotedServices.end());
        } else {
            remoteAppRules.insert(std::pair<GatewayAppIdentifier, GatewayRuleObjectDescriptions>(appkey, remotedServices));
        }
    }
    aclRules->setRemoteAppRules(remoteAppRules);

    status = unmarshalMetadata(&args[argsIndx++], metadata);
    if (status != ER_OK) {
        return status;
    }

    status = unmarshalMetadata(&args[argsIndx++], customMetadata);
    return status;
}

QStatus AclAdapter::marshalObjectDesciptions(const GatewayRuleObjectDescriptions& objects, MsgArg* objectsArray, size_t* objectsIndx)
{
    QStatus status = ER_OK;

    if (!objectsIndx || !objectsArray) {
        return ER_FAIL;
    }

    for (size_t i = 0; i < objects.size(); i++) {

        const std::vector<qcc::String>& interfaces = objects[i].getInterfaces();
        std::vector<const char*> interfacesVector(interfaces.size());
        std::vector<qcc::String>::const_iterator interfaceIt;
        int interfaceIndex = 0;

        for (interfaceIt = interfaces.begin(); interfaceIt != interfaces.end(); ++interfaceIt) {
            interfacesVector[interfaceIndex++] = interfaceIt->c_str();
        }

        status = objectsArray[(*objectsIndx)++].Set(AJPARAM_INTERFACE_INFO.c_str(), objects[i].getObjectPath().c_str(),
                                                    objects[i].getIsPrefix(), interfaceIndex, interfacesVector.data());
        if (status != ER_OK) {
            return status;
        }
    }
    return status;
}

QStatus AclAdapter::marshalMetadata(const std::map<qcc::String, qcc::String>& metadata, MsgArg* metadataArray, size_t* metadataIndx)
{
    QStatus status = ER_OK;

    if (!metadataIndx || !metadataArray) {
        return ER_FAIL;
    }

    std::map<qcc::String, qcc::String>::const_iterator iter;

    for (iter = metadata.begin(); iter != metadata.end(); iter++) {
        status = metadataArray[(*metadataIndx)++].Set(AJPARAM_ACL_METADATA.c_str(), iter->first.c_str(), iter->second.c_str());
        if (status != ER_OK) {
            return status;
        }
    }

    return status;
}

QStatus AclAdapter::marshalAcl(GatewayAcl* acl, ajn::MsgArg* msgArg)
{
    if (acl == 0 || msgArg == 0) {
        return ER_INVALID_DATA;
    }

    GatewayMetadataManager* metadataManager = GatewayMgmt::getInstance()->getMetadataManager();
    if (!metadataManager) {
        QCC_DbgHLPrintf(("metadataManager is NULL"));
        return ER_FAIL;
    }

    QStatus status = ER_OK;
    size_t indx = 0;

    status = msgArg[indx++].Set(AJPARAM_STR.c_str(), acl->getAclName().c_str());
    if (status != ER_OK) {
        return status;
    }

    const GatewayRuleObjectDescriptions& exposedServices = acl->getAclRules().getExposedServicesRules();
    MsgArg* exposedServicesArray = new MsgArg[exposedServices.size()];
    size_t exposedServicesIndx = 0;

    status = marshalObjectDesciptions(exposedServices, exposedServicesArray, &exposedServicesIndx);
    if (status != ER_OK) {
        delete[] exposedServicesArray;
        return status;
    }

    status = msgArg[indx].Set(AJPARAM_INTERFACE_INFO_ARRAY.c_str(), exposedServicesIndx, exposedServicesArray);
    if (status != ER_OK) {
        delete[] exposedServicesArray;
        return status;
    }
    msgArg[indx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);

    const GatewayRemoteAppRules& remoteAppPerm = acl->getAclRules().getRemoteAppRules();
    GatewayRemoteAppRules::const_iterator it;

    MsgArg* remoteAppPermsArray = new MsgArg[remoteAppPerm.size()];
    size_t remoteAppPermsIndx = 0;

    std::map<qcc::String, qcc::String> metadata;

    for (it = remoteAppPerm.begin(); it != remoteAppPerm.end(); it++) {

        //add metadata values for this remoteApp to map
        metadataManager->addMetadataValues(it->first, &metadata);

        MsgArg* remotedObjectsArray = new MsgArg[it->second.size()];
        size_t remotedObjectsIndx = 0;
        status = marshalObjectDesciptions(it->second, remotedObjectsArray, &remotedObjectsIndx);
        if (status != ER_OK) {
            delete[] remotedObjectsArray;
            delete[] remoteAppPermsArray;
            return status;
        }

        status = remoteAppPermsArray[remoteAppPermsIndx].Set(AJPARAM_REMOTED_APPS.c_str(), it->first.getDeviceId().c_str(),
                                                             it->first.getAppIdHexLength(), it->first.getAppIdHex(),
                                                             remotedObjectsIndx, remotedObjectsArray);
        if (status != ER_OK) {
            delete[] remotedObjectsArray;
            delete[] remoteAppPermsArray;
            return status;
        }
        remoteAppPermsArray[remoteAppPermsIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    }
    status = msgArg[indx].Set(AJPARAM_REMOTED_APPS_ARRAY.c_str(), remoteAppPermsIndx, remoteAppPermsArray);
    if (status != ER_OK) {
        delete[] remoteAppPermsArray;
        return status;
    }
    msgArg[indx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);

    MsgArg* metadataArray = new MsgArg[metadata.size()];
    size_t metadataIndx = 0;

    status = marshalMetadata(metadata, metadataArray, &metadataIndx);
    if (status != ER_OK) {
        delete[] metadataArray;
        return status;
    }

    status = msgArg[indx].Set(AJPARAM_ACL_METADATA_ARRAY.c_str(), metadataIndx, metadataArray);
    if (status != ER_OK) {
        delete[] metadataArray;
        return status;
    }
    msgArg[indx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);

    const std::map<qcc::String, qcc::String>& customMetadata = acl->getCustomMetadata();
    MsgArg* customMetadataArray = new MsgArg[customMetadata.size()];
    size_t customMetadataIndx = 0;

    status = marshalMetadata(customMetadata, customMetadataArray, &customMetadataIndx);
    if (status != ER_OK) {
        delete[] customMetadataArray;
        return status;
    }

    status = msgArg[indx].Set(AJPARAM_ACL_METADATA_ARRAY.c_str(), customMetadataIndx, customMetadataArray);
    if (status != ER_OK) {
        delete[] customMetadataArray;
        return status;
    }
    msgArg[indx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);

    return status;
}

QStatus AclAdapter::marshalMergedAcl(std::map<qcc::String, GatewayAcl*> const& acls, ajn::MsgArg* msgArg)
{
    QStatus status = ER_OK;
    size_t exposedServicesSize = 0;
    size_t remotedAppsSize = 0;
    std::map<qcc::String, GatewayAcl*>::const_iterator it;
    for (it = acls.begin(); it != acls.end(); it++) {

        if (it->second->getAclStatus() != GW_AS_ACTIVE) {
            continue;
        }
        exposedServicesSize += it->second->getAclRules().getExposedServicesRules().size();
        remotedAppsSize += it->second->getAclRules().getRemoteAppRules().size();
    }

    MsgArg* exposedServicesArray = new MsgArg[exposedServicesSize];
    size_t exposedServicesIndx = 0;
    MsgArg* remoteAppPermsArray = new MsgArg[remotedAppsSize];
    size_t remoteAppPermsIndx = 0;

    for (it = acls.begin(); it != acls.end(); it++) {

        if (it->second->getAclStatus() != GW_AS_ACTIVE) {
            continue;
        }

        const GatewayRuleObjectDescriptions& exposedServices = it->second->getAclRules().getExposedServicesRules();
        status = marshalObjectDesciptions(exposedServices, exposedServicesArray, &exposedServicesIndx);
        if (status != ER_OK) {
            delete[] exposedServicesArray;
            delete[] remoteAppPermsArray;
            return status;
        }

        const GatewayRemoteAppRules& remoteAppRules = it->second->getAclRules().getRemoteAppRules();
        GatewayRemoteAppRules::const_iterator iter;

        for (iter = remoteAppRules.begin(); iter != remoteAppRules.end(); iter++) {

            if (status != ER_OK) {
                delete[] exposedServicesArray;
                delete[] remoteAppPermsArray;
                return status;
            }

            MsgArg* remotedObjectsArray = new MsgArg[iter->second.size()];
            size_t remotedObjectsIndx = 0;
            status = marshalObjectDesciptions(iter->second, remotedObjectsArray, &remotedObjectsIndx);
            if (status != ER_OK) {
                delete[] exposedServicesArray;
                delete[] remoteAppPermsArray;
                delete[] remotedObjectsArray;
                return status;
            }

            status = remoteAppPermsArray[remoteAppPermsIndx].Set(AJPARAM_REMOTED_APPS.c_str(), iter->first.getDeviceId().c_str(),
                                                                 iter->first.getAppIdHexLength(), iter->first.getAppIdHex(),
                                                                 remotedObjectsIndx, remotedObjectsArray);
            if (status != ER_OK) {
                delete[] exposedServicesArray;
                delete[] remoteAppPermsArray;
                delete[] remotedObjectsArray;
                return status;
            }
            remoteAppPermsArray[remoteAppPermsIndx++].SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }
    }

    status = msgArg[0].Set(AJPARAM_INTERFACE_INFO_ARRAY.c_str(), exposedServicesIndx, exposedServicesArray);
    if (status != ER_OK) {
        delete[] exposedServicesArray;
        delete[] remoteAppPermsArray;
        return status;
    }

    status = msgArg[1].Set(AJPARAM_REMOTED_APPS_ARRAY.c_str(), remoteAppPermsIndx, remoteAppPermsArray);
    if (status != ER_OK) {
        delete[] exposedServicesArray;
        delete[] remoteAppPermsArray;
        return status;
    }

    msgArg[0].SetOwnershipFlags(MsgArg::OwnsArgs, true);
    msgArg[1].SetOwnershipFlags(MsgArg::OwnsArgs, true);

    return status;
}

} /* namespace gw */
} /* namespace ajn */
