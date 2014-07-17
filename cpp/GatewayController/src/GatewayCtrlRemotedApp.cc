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

QStatus GatewayCtrlRemotedApp::init(const qcc::String& busUniqueName, const qcc::String& appName, uint8_t*appId, const qcc::String& deviceName, const qcc::String& deviceId, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules)
{
    m_ObjDescRules = objDescRules;

    setAppId(appId, UUID_LENGTH);
    setBusName(busUniqueName);
    setAppName(appName);
    setDeviceId(deviceId);
    setDeviceName(deviceName);

    return ER_OK;
}

QStatus GatewayCtrlRemotedApp::init(AboutClient::AboutData const& aboutData, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules)
{
    m_ObjDescRules = objDescRules;


    GatewayCtrlDiscoveredApp("", aboutData);

    return ER_OK;
}

QStatus GatewayCtrlRemotedApp::init(const ajn::MsgArg*remotedAppInfo, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules, const std::map<qcc::String, qcc::String>& internalMetaData)
{
    char*DeviceId;
    const ajn::MsgArg* ObjDescRulesArray;
    size_t ObjDescRulesCount;

    uint8_t* AppIdBuffer;
    size_t len;

    QStatus status = remotedAppInfo->Get("(saya(obas))", &DeviceId, &len, &AppIdBuffer, &ObjDescRulesCount, &ObjDescRulesArray);
    if (status != ER_OK) {
        QCC_LogError(status, ("Failed getting interface object"));
        return status;
    }

    setDeviceId(DeviceId);

    setAppId(AppIdBuffer, len);

    qcc::String AppId;

    AppId = qcc::BytesToHexString(AppIdBuffer, len);

    qcc::String keyPrefix = getDeviceId() + "_" + AppId;

    qcc::String key =  keyPrefix + AJSUFFIX_APP_NAME;

    std::map<qcc::String, qcc::String>::const_iterator itr = internalMetaData.find(key);

    if (itr != internalMetaData.end()) {
        setAppName(itr->second);
    }


    key = keyPrefix + AJSUFFIX_DEVICE_NAME;

    itr = internalMetaData.find(key);

    if (itr != internalMetaData.end()) {
        setDeviceName(itr->second);
    }

    for (size_t i = 0; i != ObjDescRulesCount; i++) {

        GatewayCtrlManifestObjectDescription* objDescRule = PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(&ObjDescRulesArray[i], objDescRules, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("unmarshalObjectDesciptionsWithoutNames failed"));
            return status;
        }

        m_ObjDescRules.push_back(objDescRule);
    }

    return ER_OK;
}


QStatus GatewayCtrlRemotedApp::init(GatewayCtrlDiscoveredApp* discoveredApp, const std::vector<GatewayCtrlManifestObjectDescription*>& objDescRules)
{
    m_ObjDescRules = objDescRules;

    GatewayCtrlDiscoveredApp(discoveredApp->getBusName(),  discoveredApp->getAppName(),  discoveredApp->getAppId(),  discoveredApp->getDeviceName(),  discoveredApp->getDeviceId());

    return ER_OK;
}

GatewayCtrlRemotedApp::~GatewayCtrlRemotedApp()
{

}

const std::vector<GatewayCtrlManifestObjectDescription*>& GatewayCtrlRemotedApp::getObjDescRules()        {
    return m_ObjDescRules;
}

void GatewayCtrlRemotedApp::emptyVector()
{
    for (size_t indx = 0; indx < m_ObjDescRules.size(); indx++) {
        QStatus status = m_ObjDescRules[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_ObjDescRules[indx];
    }

    m_ObjDescRules.clear();
}


QStatus GatewayCtrlRemotedApp::release()
{
    emptyVector();

    return ER_OK;

}
}
}
