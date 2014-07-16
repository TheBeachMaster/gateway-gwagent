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

#include <alljoyn/gateway/GatewayCtrlDiscoveredApp.h>
#include "GatewayCtrlConstants.h"
#include <alljoyn/gateway/LogModule.h>
#include <qcc/Debug.h>

namespace ajn {
namespace services {

using namespace gwcConsts;

GatewayCtrlDiscoveredApp::GatewayCtrlDiscoveredApp(qcc::String busName, qcc::String appName, uint8_t*appId, qcc::String deviceName, qcc::String deviceId) : m_BusName(busName), m_AppName(appName), m_DeviceName(deviceName), m_DeviceId(deviceId)
{
    setAppId(appId, UUID_LENGTH);
}

GatewayCtrlDiscoveredApp::GatewayCtrlDiscoveredApp(qcc::String busName, AboutClient::AboutData const& aboutData) : m_BusName(busName)
{
    if (aboutData.find("AppId") == aboutData.end()) {
        QCC_LogError(ER_FAIL, ("AppId missing in about structure, bus name is '%s'", busName.c_str()));
        return;
    }

    const MsgArg*value = &aboutData.find("AppId")->second;


    uint8_t* appIdBin = NULL;
    size_t len;
    QStatus status = value->Get("ay", &len, &appIdBin);
    if (status != ER_OK) {
        QCC_LogError(status, ("Get appID failed"));
        return;
    }

    setAppId(appIdBin, len);

//            GetAboutDataEntry(aboutData,"AppId")
    m_BusName = busName;
    m_AppName = getAboutDataEntry(aboutData, "AppName");
    m_DeviceName  = getAboutDataEntry(aboutData, "DeviceName");
    m_DeviceId = getAboutDataEntry(aboutData, "DeviceId");
}


qcc::String GatewayCtrlDiscoveredApp::getAboutDataEntry(AboutClient::AboutData const& aboutData, qcc::String key)
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
GatewayCtrlDiscoveredApp::~GatewayCtrlDiscoveredApp()
{
}

qcc::String GatewayCtrlDiscoveredApp::getBusName()
{
    return m_BusName;
}


qcc::String GatewayCtrlDiscoveredApp::getAppName() const
{
    return m_AppName;
}

uint8_t*GatewayCtrlDiscoveredApp::getAppId()
{
    return m_AppId;
}

qcc::String GatewayCtrlDiscoveredApp::getDeviceName() const
{
    return m_DeviceName;
}

qcc::String GatewayCtrlDiscoveredApp::getDeviceId()
{
    return m_DeviceId;
}

void GatewayCtrlDiscoveredApp::setBusName(qcc::String busName)
{
    m_BusName = busName;
}


void GatewayCtrlDiscoveredApp::setAppName(qcc::String appName)
{
    m_AppName = appName;
}

void GatewayCtrlDiscoveredApp::setAppId(uint8_t*appId, size_t len)
{
    if (appId) {
        if (len <= UUID_LENGTH) {
            memcpy(m_AppId, appId, UUID_LENGTH);
        }
    }
}

void GatewayCtrlDiscoveredApp::setDeviceName(qcc::String deviceName)
{
    m_DeviceName = deviceName;
}

void GatewayCtrlDiscoveredApp::setDeviceId(qcc::String deviceId)
{
    m_DeviceId = deviceId;
}
}
}
