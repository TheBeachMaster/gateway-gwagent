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

#include "PayloadAdapter.h"
#include <alljoyn/gateway/LogModule.h>

namespace ajn {
namespace gwc {


MsgArg*PayloadAdapter::MarshalMetadata(const std::map<qcc::String, qcc::String>& metadata, QStatus& status)
{
    status = ER_OK;

    MsgArg*metadataKeyValueArg = new MsgArg[metadata.size()];
    uint32_t pos = 0;

    for (std::map<qcc::String, qcc::String>::const_iterator it = metadata.begin(); it != metadata.end(); ++it) {
        const char*key = (*it).first.c_str();
        const char*value = (*it).second.c_str();
        status = metadataKeyValueArg[pos].Set("{ss}", key, value);
        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] metadataKeyValueArg;
            return NULL;
        }
        metadataKeyValueArg[pos].SetOwnershipFlags(MsgArg::OwnsArgs, true);

        pos++;
    }

    return metadataKeyValueArg;
}

QStatus PayloadAdapter::marshalObjectDescriptions(const RuleObjectDescription& object, MsgArg* objectsArrayEntry)
{

    if (objectsArrayEntry == 0) {
        return ER_INVALID_DATA;
    }

    QStatus status = ER_OK;

    const std::set<RuleInterface>*interfaces = object.getInterfaces();

    std::vector<const char*> interfacesVector(interfaces->size());
    int index = 0;
    for (std::set<RuleInterface>::const_iterator itr = interfaces->begin(); itr != interfaces->end(); itr++) {
        interfacesVector[index++] = (RuleInterface(*itr)).getName().c_str();
    }

    objectsArrayEntry->Set("(obas)", object.getObjectPath()->getPath().c_str(),
                           object.getObjectPath()->isPrefix(), interfaces->size(), interfacesVector.data());

    objectsArrayEntry->SetOwnershipFlags(MsgArg::OwnsArgs, true);

    return status;
}

QStatus PayloadAdapter::FillRuleObjectDescriptionVector(const ajn::MsgArg*inputArray, std::vector<RuleObjectDescription*>& vector)
{
    const ajn::MsgArg* exposedServices;

    size_t num;
    QStatus status = inputArray->Get("a((obs)a(ssb))", &num, &exposedServices);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting exposed services"));
        return status;
    }
    for (size_t i = 0; i < num; ++i) {
        RuleObjectDescription*manifestObjDesc = unmarshalObjectDescriptionsWithFriendlyNames(&exposedServices[i], status);

        if (status != ER_OK) {
            return status;
        }

        vector.push_back(manifestObjDesc);
    }

    return status;

}

RuleObjectDescription*PayloadAdapter::unmarshalObjectDescriptionsWithFriendlyNames(const MsgArg* manifestObjectDescriptionInfo, QStatus& status)
{
    char*ObjectPath;
    bool IsPrefix;
    char*ObjectPathFriendlyName;
    const ajn::MsgArg* array;
    size_t count;

    status = manifestObjectDescriptionInfo->Get("((obs)a(ssb))", &ObjectPath, &IsPrefix, &ObjectPathFriendlyName, &count, &array);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting Manifest rules"));
        return NULL;
    }

    std::set<RuleInterface> interfaces;
    RuleObjectPath objectPath(ObjectPath, ObjectPathFriendlyName, IsPrefix, IsPrefix); // a manifest object path, we do not get the isPrefixAllowed from the server so we need to deduce it ourselves. for the manifest it is not important so insert the same as IsPrefix.

    for (size_t x = 0; x < count; x++) {
        char*InterfaceName;
        char*InterfaceFriendlyName;
        bool isSecured;

        status = array[x].Get("(ssb)", &InterfaceName, &InterfaceFriendlyName, &isSecured);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting interface information"));
            return NULL;
        }

        RuleInterface RuleInterface(InterfaceName, InterfaceFriendlyName, isSecured);

        interfaces.insert(RuleInterface);
    }

    RuleObjectDescription*manifestObjectDescriptionOut = new RuleObjectDescription(objectPath, interfaces);

    return manifestObjectDescriptionOut;
}

QStatus PayloadAdapter::unmarshalMetadata(const MsgArg* metadataArg, std::map<qcc::String, qcc::String>* metadata)
{

    if (metadata == NULL) {
        return ER_BAD_ARG_2;
    }

    const ajn::MsgArg* internalMetadataMap;
    size_t internalMetadataCount;

    QStatus status = metadataArg->Get("a{ss}", &internalMetadataCount, &internalMetadataMap);

    if (status != ER_OK) {
        QCC_LogError(status, ("Failed Get"));
        return status;
    }


    for (size_t x = 0; x != internalMetadataCount; x++) {
        char*key;
        char*value;
        status = internalMetadataMap[x].Get("{ss}", &key, &value);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed Get"));
            return status;
        }

        metadata->insert(std::pair<qcc::String, qcc::String>(key, value));
    }

    return ER_OK;
}


RuleObjectDescription*PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(const ajn::MsgArg*manifestObjectDescriptionInfo, const std::vector<RuleObjectDescription*>& ruleObjDescriptions, QStatus& status)
{


    char*ObjectPathString;
    bool IsPrefix;
    const ajn::MsgArg* interfaceArray;
    size_t interfaceCount;

    status = manifestObjectDescriptionInfo->Get("(obas)", &ObjectPathString, &IsPrefix, &interfaceCount, &interfaceArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting interface object"));
        return NULL;
    }

    std::set<RuleInterface> interfaces;

    for (size_t x = 0; x < interfaceCount; x++) {
        char*InterfaceName;

        status = interfaceArray[x].Get("s", &InterfaceName);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting interface name"));
            return NULL;
        }

        RuleInterface RuleInterface(InterfaceName, "", false);

        interfaces.insert(RuleInterface);
    }

    std::vector<RuleObjectDescription*>::const_iterator ruleObjDescriptionsIter;
    bool isPrefixAllowed = true;
    qcc::String friendlyName = "";

    for (ruleObjDescriptionsIter = ruleObjDescriptions.begin(); ruleObjDescriptionsIter != ruleObjDescriptions.end(); ruleObjDescriptionsIter++) {
        RuleObjectDescription* objDesc = *ruleObjDescriptionsIter;

        if (objDesc->getObjectPath()->getPath().compare(ObjectPathString) == 0) {
            isPrefixAllowed = objDesc->getObjectPath()->isPrefix();
            friendlyName = objDesc->getObjectPath()->getFriendlyName();
            break;
        }
    }

    RuleObjectPath objectPath(ObjectPathString, friendlyName, IsPrefix, isPrefixAllowed);

    RuleObjectDescription*manifestObjectDescriptionOut = new RuleObjectDescription(objectPath, interfaces);

    return manifestObjectDescriptionOut;
}

QStatus PayloadAdapter::MarshalAclRules(const AclRules& aclRules, std::vector<MsgArg*>& aclRulesVector)
{


    QStatus status = ER_OK;

    {
        const std::vector<RuleObjectDescription*> exposedServices = ((AclRules)aclRules).getExposedServices();

        MsgArg*exposedServicesArg = new MsgArg[exposedServices.size()];


        for (size_t i = 0; i != exposedServices.size(); i++) {
            status = PayloadAdapter::marshalObjectDescriptions(*exposedServices[i], &exposedServicesArg[i]);
            if (status != ER_OK) {
                QCC_LogError(status, ("RuleObjectDescription failed"));
                delete [] exposedServicesArg;
                goto failed;
            }
        }

        MsgArg*exposedServicesArrayArg = new MsgArg;

        status = exposedServicesArrayArg->Set("a(obas)",
                                              exposedServices.size(),
                                              exposedServicesArg);
        if (status != ER_OK) {
            QCC_LogError(status, ("AclRules failed"));
            delete [] exposedServicesArg;
            delete exposedServicesArrayArg;

            goto failed;
        }


        aclRulesVector.push_back(exposedServicesArrayArg);

        const std::vector<RemotedApp*> remotedApps = ((AclRules)aclRules).getRemotedApps();

        MsgArg*remotedAppsArg = new MsgArg[remotedApps.size()];

        for (size_t i = 0; i != remotedApps.size(); i++) {
            RemotedApp*app = remotedApps[i];

            MsgArg*ruleObjDescriptionsArg = new MsgArg[app->getRuleObjDesciptions().size()];

            for (size_t x = 0; x != app->getRuleObjDesciptions().size(); x++) {
                status = PayloadAdapter::marshalObjectDescriptions(*app->getRuleObjDesciptions()[x], &ruleObjDescriptionsArg[x]);
                if (status != ER_OK) {
                    QCC_LogError(status, ("AclRules failed"));
                    delete [] exposedServicesArg;
                    delete [] ruleObjDescriptionsArg;
                    delete [] remotedAppsArg;
                    goto failed;
                }
            }

            status = remotedAppsArg[i].Set("(saya(obas))", app->getDeviceId().c_str(), UUID_LENGTH, app->getAppId(), app->getRuleObjDesciptions().size(), ruleObjDescriptionsArg);
            if (status != ER_OK) {
                QCC_LogError(status, ("Set failed"));
                delete [] exposedServicesArg;
                delete [] ruleObjDescriptionsArg;
                delete [] remotedAppsArg;

                goto failed;
            }

            remotedAppsArg[i].SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        MsgArg*remotedAppsArrayArg = new MsgArg;

        status = remotedAppsArrayArg->Set("a(saya(obas))",
                                          remotedApps.size(),
                                          remotedAppsArg);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] exposedServicesArg;
            delete [] remotedAppsArg;
            delete remotedAppsArrayArg;

            goto failed;
        }

        aclRulesVector.push_back(remotedAppsArrayArg);

        const std::map<qcc::String, qcc::String> metadata = ((AclRules)aclRules).getMetadata();

        MsgArg*metadataKeyValueArg = PayloadAdapter::MarshalMetadata(metadata, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] exposedServicesArg;
            delete [] metadataKeyValueArg;
            delete [] remotedAppsArg;

            goto failed;
        }

        MsgArg*metadataKeyValueMapArg = new MsgArg;

        status = metadataKeyValueMapArg->Set("a{ss}", metadata.size(), metadataKeyValueArg);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] exposedServicesArg;
            delete [] remotedAppsArg;
            delete [] metadataKeyValueArg;
            delete metadataKeyValueMapArg;

            goto failed;
        }


        aclRulesVector.push_back(metadataKeyValueMapArg);

        for (std::vector<MsgArg*>::const_iterator itr = aclRulesVector.begin(); itr != aclRulesVector.end(); itr++) {
            (*itr)->SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        return status;
    }
failed:

    aclRulesVector.clear();
    return status;
}


}
}