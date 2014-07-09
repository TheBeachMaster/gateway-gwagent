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
namespace services {


MsgArg*PayloadAdapter::MarshalMetaData(const std::map<qcc::String, qcc::String>& metadata, QStatus& status)
{
    status = ER_OK;

    MsgArg*metaDataKeyValueArg = new MsgArg[metadata.size()];
    uint32_t pos = 0;

    for (std::map<qcc::String, qcc::String>::const_iterator it = metadata.begin(); it != metadata.end(); ++it) {
        const char*key = (*it).first.c_str();
        const char*value = (*it).second.c_str();
        status = metaDataKeyValueArg[pos].Set("{ss}", key, value);
        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] metaDataKeyValueArg;
            return NULL;
        }
        metaDataKeyValueArg[pos].SetOwnershipFlags(MsgArg::OwnsArgs, true);

        pos++;
    }

    return metaDataKeyValueArg;
}

QStatus PayloadAdapter::marshalObjectDescriptions(const GatewayCtrlManifestObjectDescription& object, MsgArg* objectsArrayEntry)
{

    if (objectsArrayEntry == 0) {
        return ER_INVALID_DATA;
    }

    QStatus status = ER_OK;

    const std::set<GatewayCtrlTPInterface>*interfaces = object.GetInterfaces();

    std::vector<const char*> interfacesVector(interfaces->size());
    int index = 0;
    for (std::set<GatewayCtrlTPInterface>::const_iterator itr = interfaces->begin(); itr != interfaces->end(); itr++) {
        interfacesVector[index++] = (GatewayCtrlTPInterface(*itr)).GetName().c_str();
    }

    objectsArrayEntry->Set("(obas)", object.GetObjectPath()->GetPath().c_str(),
                           object.GetObjectPath()->IsPrefix(), interfaces->size(), interfacesVector.data());

    objectsArrayEntry->SetOwnershipFlags(MsgArg::OwnsArgs, true);

    return status;
}

QStatus PayloadAdapter::FillManifestObjectDescriptionVector(const ajn::MsgArg*inputArray, std::vector<GatewayCtrlManifestObjectDescription*>& vector)
{
    const ajn::MsgArg* exposedServices;

    size_t num;
    QStatus status = inputArray->Get("a((obs)a(ssb))", &num, &exposedServices);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting exposed services"));
        return status;
    }
    for (size_t i = 0; i < num; ++i) {
        GatewayCtrlManifestObjectDescription*manifestObjDesc = unmarshalObjectDescriptionsWithFriendlyNames(&exposedServices[i], status);

        if (status != ER_OK) {
            return status;
        }

        vector.push_back(manifestObjDesc);
    }

    return status;

}

GatewayCtrlManifestObjectDescription*PayloadAdapter::unmarshalObjectDescriptionsWithFriendlyNames(const MsgArg* manifestObjectDescriptionInfo, QStatus& status)
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

    std::set<GatewayCtrlTPInterface> interfaces;
    GatewayCtrlTPObjectPath objectPath(ObjectPath, ObjectPathFriendlyName, IsPrefix, IsPrefix); // a manifest object path, we do not get the isPrefixAllowed from the server so we need to deduce it ourselves. for the manifest it is not important so insert the same as IsPrefix.

    for (size_t x = 0; x < count; x++) {
        char*InterfaceName;
        char*InterfaceFriendlyName;
        bool isSecured;

        status = array[x].Get("(ssb)", &InterfaceName, &InterfaceFriendlyName, &isSecured);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting interface information"));
            return NULL;
        }

        GatewayCtrlTPInterface TPInterface(InterfaceName, InterfaceFriendlyName, isSecured);

        interfaces.insert(TPInterface);
    }

    GatewayCtrlManifestObjectDescription*manifestObjectDescriptionOut = new GatewayCtrlManifestObjectDescription(objectPath, interfaces);

    return manifestObjectDescriptionOut;
}

QStatus PayloadAdapter::unmarshalMetaData(const MsgArg* metaDataArg, std::map<qcc::String, qcc::String>* metaData)
{

    if (metaData == NULL) {
        return ER_BAD_ARG_2;
    }

    const ajn::MsgArg* internalMetaDataMap;
    size_t internalMetaDataCount;

    QStatus status = metaDataArg->Get("a{ss}", &internalMetaDataCount, &internalMetaDataMap);

    if (status != ER_OK) {
        QCC_LogError(status, ("Failed Get"));
        return status;
    }


    for (size_t x = 0; x != internalMetaDataCount; x++) {
        char*key;
        char*value;
        status = internalMetaDataMap[x].Get("{ss}", &key, &value);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed Get"));
            return status;
        }

        metaData->insert(std::pair<qcc::String, qcc::String>(key, value));
    }

    return ER_OK;
}


GatewayCtrlManifestObjectDescription*PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(const ajn::MsgArg*manifestObjectDescriptionInfo, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules, QStatus& status)
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

    std::set<GatewayCtrlTPInterface> interfaces;

    for (size_t x = 0; x < interfaceCount; x++) {
        char*InterfaceName;

        status = interfaceArray[x].Get("s", &InterfaceName);
        if (status != ER_OK) {
            QCC_LogError(status, ("Failed getting interface name"));
            return NULL;
        }

        GatewayCtrlTPInterface TPInterface(InterfaceName, "", false);

        interfaces.insert(TPInterface);
    }

    std::vector<GatewayCtrlManifestObjectDescription*>::const_iterator objDescRulesIter;
    bool isPrefixAllowed = true;
    qcc::String friendlyName = "";

    for (objDescRulesIter = objDescRules.begin(); objDescRulesIter != objDescRules.end(); objDescRulesIter++) {
        GatewayCtrlManifestObjectDescription* objDesc = *objDescRulesIter;

        if (objDesc->GetObjectPath()->GetPath().compare(ObjectPathString) == 0) {
            isPrefixAllowed = objDesc->GetObjectPath()->IsPrefix();
            friendlyName = objDesc->GetObjectPath()->GetFriendlyName();
            break;
        }
    }

    GatewayCtrlTPObjectPath objectPath(ObjectPathString, friendlyName, IsPrefix, isPrefixAllowed);

    GatewayCtrlManifestObjectDescription*manifestObjectDescriptionOut = new GatewayCtrlManifestObjectDescription(objectPath, interfaces);

    return manifestObjectDescriptionOut;
}

QStatus PayloadAdapter::MarshalAccessRules(const GatewayCtrlAccessRules& accessRules, std::vector<MsgArg*>& accessRulesVector)
{


    QStatus status = ER_OK;

    {
        const std::vector<GatewayCtrlManifestObjectDescription*> exposedServices = ((GatewayCtrlAccessRules)accessRules).GetExposedServices();

        MsgArg*exposedServicesArg = new MsgArg[exposedServices.size()];


        for (size_t i = 0; i != exposedServices.size(); i++) {
            status = PayloadAdapter::marshalObjectDescriptions(*exposedServices[i], &exposedServicesArg[i]);
            if (status != ER_OK) {
                QCC_LogError(status, ("GatewayCtrlManifestObjectDescription failed"));
                delete [] exposedServicesArg;
                goto failed;
            }
        }

        MsgArg*exposedServicesArrayArg = new MsgArg;

        status = exposedServicesArrayArg->Set("a(obas)",
                                              exposedServices.size(),
                                              exposedServicesArg);
        if (status != ER_OK) {
            QCC_LogError(status, ("GatewayCtrlAccessRules failed"));
            delete [] exposedServicesArg;
            delete exposedServicesArrayArg;

            goto failed;
        }


        accessRulesVector.push_back(exposedServicesArrayArg);

        const std::vector<GatewayCtrlRemotedApp*> remotedApps = ((GatewayCtrlAccessRules)accessRules).GetRemotedApps();

        MsgArg*remotedAppsArg = new MsgArg[remotedApps.size()];

        for (size_t i = 0; i != remotedApps.size(); i++) {
            GatewayCtrlRemotedApp*app = remotedApps[i];

            MsgArg*objDescRulesArg = new MsgArg[app->GetObjDescRules().size()];

            for (size_t x = 0; x != app->GetObjDescRules().size(); x++) {
                status = PayloadAdapter::marshalObjectDescriptions(*app->GetObjDescRules()[x], &objDescRulesArg[x]);
                if (status != ER_OK) {
                    QCC_LogError(status, ("GatewayCtrlAccessRules failed"));
                    delete [] exposedServicesArg;
                    delete [] objDescRulesArg;
                    delete [] remotedAppsArg;
                    goto failed;
                }
            }

            status = remotedAppsArg[i].Set("(saya(obas))", app->GetDeviceId().c_str(), UUID_LENGTH, app->GetAppId(), app->GetObjDescRules().size(), objDescRulesArg);
            if (status != ER_OK) {
                QCC_LogError(status, ("Set failed"));
                delete [] exposedServicesArg;
                delete [] objDescRulesArg;
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

        accessRulesVector.push_back(remotedAppsArrayArg);

        const std::map<qcc::String, qcc::String> metaData = ((GatewayCtrlAccessRules)accessRules).GetMetadata();

        MsgArg*metaDataKeyValueArg = PayloadAdapter::MarshalMetaData(metaData, status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] exposedServicesArg;
            delete [] metaDataKeyValueArg;
            delete [] remotedAppsArg;

            goto failed;
        }

        MsgArg*metaDataKeyValueMapArg = new MsgArg;

        status = metaDataKeyValueMapArg->Set("a{ss}", metaData.size(), metaDataKeyValueArg);

        if (status != ER_OK) {
            QCC_LogError(status, ("Set failed"));
            delete [] exposedServicesArg;
            delete [] remotedAppsArg;
            delete [] metaDataKeyValueArg;
            delete metaDataKeyValueMapArg;

            goto failed;
        }


        accessRulesVector.push_back(metaDataKeyValueMapArg);

        for (std::vector<MsgArg*>::const_iterator itr = accessRulesVector.begin(); itr != accessRulesVector.end(); itr++) {
            (*itr)->SetOwnershipFlags(MsgArg::OwnsArgs, true);
        }

        return status;
    }
failed:

    accessRulesVector.clear();
    return status;
}


}
}