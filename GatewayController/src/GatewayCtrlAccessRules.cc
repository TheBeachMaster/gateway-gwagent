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

#include <alljoyn/gateway/GatewayCtrlAccessRules.h>
#include <alljoyn/gateway/LogModule.h>
#include "PayloadAdapter.h"

namespace ajn {
namespace services {

GatewayCtrlAccessRules::GatewayCtrlAccessRules(std::vector<GatewayCtrlManifestObjectDescription*> const& exposedServices, std::vector<GatewayCtrlRemotedApp*> const& remotedApps)
{
    m_ExposedServices = exposedServices;
    m_RemotedApps = remotedApps;
}

GatewayCtrlAccessRules::GatewayCtrlAccessRules(const MsgArg*exposedServicesArrayArg, const MsgArg*remotedAppsArrayArg, const GatewayCtrlManifestRules& manifestRules, const std::map<qcc::String, qcc::String>& internalMetaData)
{
    const ajn::MsgArg* exposedServicesArray;
    size_t exposedServicesCount;

    QStatus status = exposedServicesArrayArg->Get("a(obas)", &exposedServicesCount, &exposedServicesArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting Manifest object"));
        return;
    }

    for (int i = 0; i != exposedServicesCount; i++) {

        GatewayCtrlManifestObjectDescription* exposedService = PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(&exposedServicesArray[i], manifestRules.GetExposedServices(), status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed unmarshalObjectDesciptionsWithoutNames"));
            return;

        }

        m_ExposedServices.push_back(exposedService);
    }

    const ajn::MsgArg* remotedAppsArray;
    size_t remotedAppsCount;

    status = remotedAppsArrayArg->Get("a(saya(obas))", &remotedAppsCount, &remotedAppsArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting remoted app object"));
        return;
    }

    for (int i = 0; i != remotedAppsCount; i++) {
        GatewayCtrlRemotedApp*remotedApp = new GatewayCtrlRemotedApp(&remotedAppsArray[i], manifestRules.GetRemotedServices(), internalMetaData);

        m_RemotedApps.push_back(remotedApp);
    }


}

GatewayCtrlAccessRules::~GatewayCtrlAccessRules()
{

}

const std::vector<GatewayCtrlRemotedApp*>&  GatewayCtrlAccessRules::GetRemotedApps()
{
    return m_RemotedApps;
}

const std::vector<GatewayCtrlManifestObjectDescription*>& GatewayCtrlAccessRules::GetExposedServices()
{
    return m_ExposedServices;
}

void GatewayCtrlAccessRules::SetMetadata(std::map<qcc::String, qcc::String> const& metadata)
{
    m_Metadata.insert(metadata.begin(), metadata.end());
}

qcc::String GatewayCtrlAccessRules::GetMetadata(qcc::String key)
{
    return m_Metadata.find(key)->second;

}

const std::map<qcc::String, qcc::String>& GatewayCtrlAccessRules::GetMetadata()
{
    return m_Metadata;
}


void GatewayCtrlAccessRules::EmptyVectors()
{
    for (size_t indx = 0; indx < m_ExposedServices.size(); indx++) {
        QStatus status = m_ExposedServices[indx]->Release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_ExposedServices[indx];
    }

    m_ExposedServices.clear();


    for (size_t indx = 0; indx < m_RemotedApps.size(); indx++) {
        QStatus status = m_RemotedApps[indx]->Release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_RemotedApps[indx];
        m_RemotedApps[indx] = NULL;
    }

    m_RemotedApps.clear();

}

QStatus GatewayCtrlAccessRules::Release()
{
    EmptyVectors();

    return ER_OK;
}


}
}



