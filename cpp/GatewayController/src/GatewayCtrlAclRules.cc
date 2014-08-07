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

#include <alljoyn/gateway/GatewayCtrlAclRules.h>
#include <alljoyn/gateway/LogModule.h>
#include "PayloadAdapter.h"

namespace ajn {
namespace gwcontroller {

GatewayCtrlAclRules::GatewayCtrlAclRules()
{

}

QStatus GatewayCtrlAclRules::init(std::vector<GatewayCtrlRuleObjectDescription*> const& exposedServices, std::vector<GatewayCtrlRemotedApp*> const& remotedApps)
{
    m_ExposedServices = exposedServices;
    m_RemotedApps = remotedApps;

    return ER_OK;
}

QStatus GatewayCtrlAclRules::init(const MsgArg*exposedServicesArrayArg, const MsgArg*remotedAppsArrayArg, const GatewayCtrlConnectorCapabilities& connectorCapabilities, const std::map<qcc::String, qcc::String>& internalMetadata)
{
    const ajn::MsgArg* exposedServicesArray;
    size_t exposedServicesCount;

    QStatus status = exposedServicesArrayArg->Get("a(obas)", &exposedServicesCount, &exposedServicesArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting Manifest object"));
        return status;
    }

    for (int i = 0; i != exposedServicesCount; i++) {

        GatewayCtrlRuleObjectDescription* exposedService = PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(&exposedServicesArray[i], connectorCapabilities.getExposedServices(), status);

        if (status != ER_OK) {
            QCC_LogError(status, ("Failed unmarshalObjectDesciptionsWithoutNames"));
            return status;
        }

        m_ExposedServices.push_back(exposedService);
    }

    const ajn::MsgArg* remotedAppsArray;
    size_t remotedAppsCount;

    status = remotedAppsArrayArg->Get("a(saya(obas))", &remotedAppsCount, &remotedAppsArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting remoted app object"));
        return status;
    }

    for (int i = 0; i != remotedAppsCount; i++) {
        GatewayCtrlRemotedApp*remotedApp = new GatewayCtrlRemotedApp();

        QStatus status = remotedApp->init(&remotedAppsArray[i], connectorCapabilities.getRemotedServices(), internalMetadata);

        if (status != ER_OK) {
            delete remotedApp;
            return status;
        }

        m_RemotedApps.push_back(remotedApp);
    }

    return ER_OK;
}

GatewayCtrlAclRules::~GatewayCtrlAclRules()
{

}

const std::vector<GatewayCtrlRemotedApp*>&  GatewayCtrlAclRules::getRemotedApps()
{
    return m_RemotedApps;
}

const std::vector<GatewayCtrlRuleObjectDescription*>& GatewayCtrlAclRules::getExposedServices()
{
    return m_ExposedServices;
}

void GatewayCtrlAclRules::setMetadata(std::map<qcc::String, qcc::String> const& metadata)
{
    m_Metadata.insert(metadata.begin(), metadata.end());
}

qcc::String*GatewayCtrlAclRules::getMetadata(const qcc::String& key)
{
    std::map<qcc::String, qcc::String>::iterator value = m_Metadata.find(key);

    if (value != m_Metadata.end()) {
        return &value->second;
    } else {
        return NULL;
    }


}

const std::map<qcc::String, qcc::String>& GatewayCtrlAclRules::getMetadata()
{
    return m_Metadata;
}


void GatewayCtrlAclRules::emptyVectors()
{
    for (size_t indx = 0; indx < m_ExposedServices.size(); indx++) {
        QStatus status = m_ExposedServices[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_ExposedServices[indx];
    }

    m_ExposedServices.clear();


    for (size_t indx = 0; indx < m_RemotedApps.size(); indx++) {
        QStatus status = m_RemotedApps[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_RemotedApps[indx];
        m_RemotedApps[indx] = NULL;
    }

    m_RemotedApps.clear();

}

QStatus GatewayCtrlAclRules::release()
{
    emptyVectors();

    return ER_OK;
}


}
}



