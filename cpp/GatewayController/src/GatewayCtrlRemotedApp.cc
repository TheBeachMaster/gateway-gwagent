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
namespace gwcontroller {

using namespace gwcConsts;

QStatus GatewayCtrlRemotedApp::init(const qcc::String& busUniqueName, const qcc::String& appName, uint8_t*appId, const qcc::String& deviceName, const qcc::String& deviceId, const std::vector<GatewayCtrlRuleObjectDescription*>& ruleObjDescriptions)
{
    m_RuleObjDescriptions = ruleObjDescriptions;

    setAppId(appId, UUID_LENGTH);
    setBusName(busUniqueName);
    setAppName(appName);
    setDeviceId(deviceId);
    setDeviceName(deviceName);

    return ER_OK;
}

QStatus GatewayCtrlRemotedApp::init(ajn::services::AboutClient::AboutData const& aboutData, const std::vector<GatewayCtrlRuleObjectDescription*>& ruleObjDescriptions)
{
    m_RuleObjDescriptions = ruleObjDescriptions;

    return GatewayCtrlAnnouncedApp::init("", aboutData);
}

QStatus GatewayCtrlRemotedApp::init(const ajn::MsgArg*remotedAppInfo, const std::vector<GatewayCtrlRuleObjectDescription*>& ruleObjDescriptions, const std::map<qcc::String, qcc::String>& internalMetadata)
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

    std::map<qcc::String, qcc::String>::const_iterator itr = internalMetadata.find(key);

    if (itr != internalMetadata.end()) {
        setAppName(itr->second);
    }


    key = keyPrefix + AJSUFFIX_DEVICE_NAME;

    itr = internalMetadata.find(key);

    if (itr != internalMetadata.end()) {
        setDeviceName(itr->second);
    }

    for (size_t i = 0; i != ObjDescRulesCount; i++) {

        GatewayCtrlRuleObjectDescription* objDescRule = PayloadAdapter::unmarshalObjectDescriptionsWithoutNames(&ObjDescRulesArray[i], ruleObjDescriptions, status);
        if (status != ER_OK) {
            QCC_LogError(status, ("unmarshalObjectDesciptionsWithoutNames failed"));
            return status;
        }

        m_RuleObjDescriptions.push_back(objDescRule);
    }

    return ER_OK;
}


QStatus GatewayCtrlRemotedApp::init(GatewayCtrlAnnouncedApp* announcedApp, const std::vector<GatewayCtrlRuleObjectDescription*>& ruleObjDescriptions)
{
    m_RuleObjDescriptions = ruleObjDescriptions;

    GatewayCtrlAnnouncedApp::init(announcedApp->getBusName(),  announcedApp->getAppName(),  announcedApp->getAppId(),  announcedApp->getDeviceName(),  announcedApp->getDeviceId());

    return ER_OK;
}

GatewayCtrlRemotedApp::~GatewayCtrlRemotedApp()
{

}

const std::vector<GatewayCtrlRuleObjectDescription*>& GatewayCtrlRemotedApp::getRuleObjDesciptions()        {
    return m_RuleObjDescriptions;
}

void GatewayCtrlRemotedApp::emptyVector()
{
    for (size_t indx = 0; indx < m_RuleObjDescriptions.size(); indx++) {
        QStatus status = m_RuleObjDescriptions[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_RuleObjDescriptions[indx];
    }

    m_RuleObjDescriptions.clear();
}


QStatus GatewayCtrlRemotedApp::release()
{
    emptyVector();

    return ER_OK;

}
}
}
