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

#include <alljoyn/gateway/GatewayCtrlRemotedApp.h>
#include <alljoyn/gateway/LogModule.h>
#include <qcc/StringUtil.h>

#include "PayloadAdapter.h"
#include "GatewayCtrlConstants.h"

namespace ajn {
namespace services {

using namespace gwcConsts;

GatewayCtrlRemotedApp::GatewayCtrlRemotedApp(qcc::String busUniqueName, qcc::String appName, uint8_t*appId, qcc::String deviceName, qcc::String deviceId, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules)
    : GatewayCtrlDiscoveredApp(busUniqueName,  appName,  appId,  deviceName,  deviceId)
{
    m_ObjDescRules = objDescRules;
}

GatewayCtrlRemotedApp::GatewayCtrlRemotedApp(AboutClient::AboutData const& aboutData, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules) : GatewayCtrlDiscoveredApp("", aboutData)
{
    m_ObjDescRules = objDescRules;
}

GatewayCtrlRemotedApp::GatewayCtrlRemotedApp(const ajn::MsgArg*remotedAppInfo, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules, const std::map<qcc::String, qcc::String>& internalMetaData)
{
    char*DeviceId;
    const ajn::MsgArg* ObjDescRulesArray;
    size_t ObjDescRulesCount;

    uint8_t* AppIdBuffer;
    size_t len;

    QStatus status = remotedAppInfo->Get("(saya(obas))", &DeviceId, &len, &AppIdBuffer, &ObjDescRulesCount, &ObjDescRulesArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting interface object"));
        return;
    }

    SetDeviceId(DeviceId);

    SetAppId(AppIdBuffer, len);

    qcc::String AppId;

    AppId = qcc::BytesToHexString(AppIdBuffer, len);

    qcc::String keyPrefix = GetDeviceId() + "_" + AppId;

    qcc::String key =  keyPrefix + AJSUFFIX_APP_NAME;

    std::map<qcc::String, qcc::String>::const_iterator itr = internalMetaData.find(key);

    if (itr != internalMetaData.end()) {
        SetAppName(itr->second);
    }


    key = keyPrefix + AJSUFFIX_DEVICE_NAME;

    itr = internalMetaData.find(key);

    if (itr != internalMetaData.end()) {
        SetDeviceName(itr->second);
    }

    for (size_t i = 0; i != ObjDescRulesCount; i++) {

        GatewayCtrlManifestObjectDescription* objDescRule = PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(&ObjDescRulesArray[i], objDescRules, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("unmarshalObjectDesciptionsWithoutNames failed"));
            return;
        }

        m_ObjDescRules.push_back(objDescRule);
    }
}


GatewayCtrlRemotedApp::GatewayCtrlRemotedApp(GatewayCtrlDiscoveredApp* discoveredApp, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules)
    : GatewayCtrlDiscoveredApp(discoveredApp->GetBusName(),  discoveredApp->GetAppName(),  discoveredApp->GetAppId(),  discoveredApp->GetDeviceName(),  discoveredApp->GetDeviceId())
{
    m_ObjDescRules = objDescRules;
}

GatewayCtrlRemotedApp::~GatewayCtrlRemotedApp()
{

}

const std::vector<GatewayCtrlManifestObjectDescription*>& GatewayCtrlRemotedApp::GetObjDescRules()        {
    return m_ObjDescRules;
}

void GatewayCtrlRemotedApp::EmptyVector()
{
    for (size_t indx = 0; indx < m_ObjDescRules.size(); indx++) {
        QStatus status = m_ObjDescRules[indx]->Release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_ObjDescRules[indx];
    }

    m_ObjDescRules.clear();
}


QStatus GatewayCtrlRemotedApp::Release()
{
    EmptyVector();

    return ER_OK;

}
}
}
