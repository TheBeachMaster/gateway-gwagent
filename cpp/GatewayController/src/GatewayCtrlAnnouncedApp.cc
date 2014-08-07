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

#include <alljoyn/gateway/GatewayCtrlAnnouncedApp.h>
#include "GatewayCtrlConstants.h"
#include <alljoyn/gateway/LogModule.h>
#include <qcc/Debug.h>

namespace ajn {
namespace gwcontroller {

using namespace gwcConsts;

void GatewayCtrlAnnouncedApp::init(const qcc::String& busName, const qcc::String& appName, uint8_t*appId, const qcc::String& deviceName, const qcc::String& deviceId)
{

    m_BusName = busName;
    m_AppName  = appName;
    m_DeviceName = deviceName;
    m_DeviceId = deviceId;
    setAppId(appId, UUID_LENGTH);
}

QStatus GatewayCtrlAnnouncedApp::init(const qcc::String& busName, ajn::services::AboutClient::AboutData const& aboutData)
{
    setBusName(busName);

    if (aboutData.find("AppId") == aboutData.end()) {
        QCC_LogError(ER_FAIL, ("AppId missing in about structure, bus name is '%s'", busName.c_str()));
        return ER_FAIL;
    }

    const MsgArg*value = &aboutData.find("AppId")->second;


    uint8_t* appIdBin = NULL;
    size_t len;
    QStatus status = value->Get("ay", &len, &appIdBin);
    if (status != ER_OK) {
        QCC_LogError(status, ("Get appID failed"));
        return status;
    }

    setAppId(appIdBin, len);

//            GetAboutDataEntry(aboutData,"AppId")
    m_BusName = busName;
    m_AppName = getAboutDataEntry(aboutData, "AppName");
    m_DeviceName  = getAboutDataEntry(aboutData, "DeviceName");
    m_DeviceId = getAboutDataEntry(aboutData, "DeviceId");

    return ER_OK;
}


qcc::String GatewayCtrlAnnouncedApp::getAboutDataEntry(ajn::services::AboutClient::AboutData const& aboutData, const qcc::String& key)
{
    if (aboutData.find(key) == aboutData.end()) {
        QCC_LogError(ER_FAIL, ("Called GetAboutDataEntry but couldn't find the key '%s' requested", key.c_str()));
        return "";
    }

    const MsgArg*value = &aboutData.find(key)->second;

    if (value->typeId == ALLJOYN_STRING) {
        return value->v_string.str;
    }

    QCC_LogError(ER_FAIL, ("Called GetAboutDataEntry on a key '%s' that is not an ALLJOYN_STRING", key.c_str()));

    return "";
}

/**
 * Destructor
 */
GatewayCtrlAnnouncedApp::~GatewayCtrlAnnouncedApp()
{
}

const qcc::String& GatewayCtrlAnnouncedApp::getBusName()
{
    return m_BusName;
}


const qcc::String& GatewayCtrlAnnouncedApp::getAppName() const
{
    return m_AppName;
}

uint8_t*GatewayCtrlAnnouncedApp::getAppId()
{
    return m_AppId;
}

const qcc::String& GatewayCtrlAnnouncedApp::getDeviceName() const
{
    return m_DeviceName;
}

const qcc::String& GatewayCtrlAnnouncedApp::getDeviceId()
{
    return m_DeviceId;
}

void GatewayCtrlAnnouncedApp::setBusName(const qcc::String& busName)
{
    m_BusName = busName;
}


void GatewayCtrlAnnouncedApp::setAppName(const qcc::String& appName)
{
    m_AppName = appName;
}

void GatewayCtrlAnnouncedApp::setAppId(uint8_t*appId, size_t len)
{
    if (appId) {
        if (len <= UUID_LENGTH) {
            memcpy(m_AppId, appId, UUID_LENGTH);
        }
    }
}

void GatewayCtrlAnnouncedApp::setDeviceName(const qcc::String& deviceName)
{
    m_DeviceName = deviceName;
}

void GatewayCtrlAnnouncedApp::setDeviceId(const qcc::String& deviceId)
{
    m_DeviceId = deviceId;
}
}
}
