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

#ifndef GatewayCtrlDiscoveredApp_H
#define GatewayCtrlDiscoveredApp_H

#include <map>
#include <qcc/String.h>
#include <alljoyn/about/AboutClient.h>

#define UUID_LENGTH  16

namespace ajn {
namespace services {

class GatewayCtrlDiscoveredApp {
  public:

    GatewayCtrlDiscoveredApp() { }

    /**
     * Constructor
     * @param busName The name of the {@link BusAttachment} of the device that sent the Announcement
     * @param appName The name of the application
     * @param appId The application id
     * @param deviceName The name of the device
     * @param deviceId The device id
     */
    GatewayCtrlDiscoveredApp(qcc::String busName, qcc::String appName, uint8_t*appId, qcc::String deviceName, qcc::String deviceId);

    /**
     * Constructor
     * @param busName The name of the {@link BusAttachment} of the device that sent the Announcement
     * @param aboutData The data sent with the Announcement
     */
    GatewayCtrlDiscoveredApp(qcc::String busName, AboutClient::AboutData const& aboutData);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlDiscoveredApp();

    /**
     * @return The bus name
     */
    qcc::String GetBusName();

    /**
     * @return The name of the application that sent the announcement
     */
    qcc::String GetAppName() const;

    /**
     * @return Id of the application that sent the announcement
     */
    uint8_t*GetAppId();

    /**
     * @return The name of the device that the application belongs to
     */
    qcc::String GetDeviceName() const;

    /**
     * @return The id of the device that the application belongs to
     */
    qcc::String GetDeviceId();

    /**
     * set bus name
     */

    void SetBusName(qcc::String busName);

    /**
     * set bus name
     */
    void SetAppName(qcc::String appName);

    /**
     * set app Id
     */
    void SetAppId(uint8_t*appId, size_t len);

    /**
     * set device name
     */
    void SetDeviceName(qcc::String deviceName);

    /**
     * set device Id
     */
    void SetDeviceId(qcc::String deviceId);


  private:
    qcc::String GetAboutDataEntry(AboutClient::AboutData const& aboutData, qcc::String key);

    /**
     * The unique name of the {@link BusAttachment} that sent the Announcement signal
     */
    qcc::String m_BusName;

    /**
     * The name of the application that sent the announcement
     */

    qcc::String m_AppName;

    /**
     * The Id of the application that sent the announcement
     */
    uint8_t m_AppId[UUID_LENGTH];

    /**
     * The name of the device that the application belongs to
     */
    qcc::String m_DeviceName;

    /**
     * The id of the device that the application belongs to
     */
    qcc::String m_DeviceId;
};
}
}
#endif /* defined(GatewayCtrlDiscoveredApp_H) */
