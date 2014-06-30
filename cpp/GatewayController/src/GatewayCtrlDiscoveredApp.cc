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

}

GatewayCtrlDiscoveredApp::GatewayCtrlDiscoveredApp(qcc::String busName, AboutClient::AboutData const& aboutData) : m_BusName(busName)
{
    const MsgArg*value = &aboutData.find("AppId")->second;


    uint8_t* appIdBin = NULL;
    size_t len;
    QStatus status = value->Get("ay", &len, &appIdBin);
    if (status != ER_OK) {
        QCC_LogError(status, ("Get appID failed"));
    }

    SetAppId(appIdBin, len);

//            GetAboutDataEntry(aboutData,"AppId")
    m_BusName = busName;
    m_AppName = GetAboutDataEntry(aboutData, "AppName");
    m_DeviceName  = GetAboutDataEntry(aboutData, "DeviceName");
    m_DeviceId = GetAboutDataEntry(aboutData, "DeviceId");
}


qcc::String GatewayCtrlDiscoveredApp::GetAboutDataEntry(AboutClient::AboutData const& aboutData, qcc::String key)
{
    const MsgArg*value = &aboutData.find(key)->second;

    if (value->typeId == ALLJOYN_STRING) {
        return value->v_string.str;
    }

    return "NA";
}

/**
 * Destructor
 */
GatewayCtrlDiscoveredApp::~GatewayCtrlDiscoveredApp()
{
}

qcc::String GatewayCtrlDiscoveredApp::GetBusName()
{
    return m_BusName;
}


qcc::String GatewayCtrlDiscoveredApp::GetAppName() const
{
    return m_AppName;
}

uint8_t*GatewayCtrlDiscoveredApp::GetAppId()
{
    return m_AppId;
}

qcc::String GatewayCtrlDiscoveredApp::GetDeviceName() const
{
    return m_DeviceName;
}

qcc::String GatewayCtrlDiscoveredApp::GetDeviceId()
{
    return m_DeviceId;
}

void GatewayCtrlDiscoveredApp::SetBusName(qcc::String busName)
{
    m_BusName = busName;
}


void GatewayCtrlDiscoveredApp::SetAppName(qcc::String appName)
{
    m_AppName = appName;
}

void GatewayCtrlDiscoveredApp::SetAppId(uint8_t*appId, size_t len)
{
    if (appId) {
        if (len <= UUID_LENGTH) {
            memcpy(m_AppId, appId, UUID_LENGTH);
        }
    }
}

void GatewayCtrlDiscoveredApp::SetDeviceName(qcc::String deviceName)
{
    m_DeviceName = deviceName;
}

void GatewayCtrlDiscoveredApp::SetDeviceId(qcc::String deviceId)
{
    m_DeviceId = deviceId;
}
}
}
