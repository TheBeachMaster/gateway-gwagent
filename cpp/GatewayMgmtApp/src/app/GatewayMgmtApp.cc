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
#include <signal.h>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/about/AboutPropertyStoreImpl.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayBusListener.h>
#include "../GatewayConstants.h"
#include "SrpKeyXListener.h"
#include "GuidUtil.h"

using namespace ajn;
using namespace gw;
using namespace services;
using namespace qcc;

#define SERVICE_PORT 900

GatewayMgmt* gatewayMgmt = NULL;
BusAttachment* bus = NULL;
AboutPropertyStoreImpl* propertyStoreImpl = NULL;
GatewayBusListener*  busListener = NULL;
SrpKeyXListener* keyListener = NULL;
static volatile sig_atomic_t s_interrupt = false;
static volatile sig_atomic_t s_restart = false;

static void DaemonDisconnectHandler()
{
    s_restart = true;
}

void WaitForSigInt(void) {
    while (s_interrupt == false && s_restart == false) {
        usleep(100 * 1000);
    }
}

QStatus prepareBusAttachment()
{
    bus = new BusAttachment("GatewayMgmtApp", true);

    QStatus status = bus->Start();
    if (status != ER_OK) {
        return status;
    }

    uint16_t retry = 0;
    do {
        status = bus->Connect();
        if (status != ER_OK) {
            std::cout << "Could not connect BusAttachment. Retrying" << std::endl;
            sleep(1);
            retry++;
        }
    } while (status != ER_OK && retry != 180 && !s_interrupt);

    return status;
}

QStatus prepareAboutService()
{
    if (!bus || !propertyStoreImpl || !busListener) {
        return ER_FAIL;
    }

    AboutServiceApi::Init(*bus, *propertyStoreImpl);
    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        return ER_BUS_NOT_ALLOWED;
    }

    busListener->setSessionPort(SERVICE_PORT);
    bus->RegisterBusListener(*busListener);

    TransportMask transportMask = TRANSPORT_ANY;
    SessionPort sp = SERVICE_PORT;
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, transportMask);

    QStatus status = bus->BindSessionPort(sp, opts, *busListener);
    if (status != ER_OK) {
        return status;
    }

    status = aboutService->Register(SERVICE_PORT);
    if (status != ER_OK) {
        return status;
    }

    status = bus->RegisterBusObject(*aboutService);
    return status;
}

QStatus fillPropertyStore()
{
    if (!propertyStoreImpl) {
        return ER_BAD_ARG_1;
    }

    QStatus status = ER_OK;

    qcc::String deviceId;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceId);
    qcc::String appId;
    GuidUtil::GetInstance()->GenerateGUID(&appId);

    status = propertyStoreImpl->setDeviceId(deviceId);
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setDeviceName("AllJoyn Gateway Agent", "en");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setAppId(appId);
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setAppName("AllJoyn Gateway Configuration Manager", "en");
    if (status != ER_OK) {
        return status;
    }
    std::vector<qcc::String> languages;
    languages.push_back("en");
    status = propertyStoreImpl->setSupportedLangs(languages);
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setDefaultLang("en");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setAjSoftwareVersion(ajn::GetVersion());
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setSupportUrl("http://www.allseenalliance.org");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setManufacturer("AllSeen Alliance", "en");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setModelNumber("1.0");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setSoftwareVersion("1.0");
    if (status != ER_OK) {
        return status;
    }
    status = propertyStoreImpl->setDescription("AllJoyn Gateway Configuration Manager Application", "en");
    if (status != ER_OK) {
        return status;
    }
    return status;
}

void aboutServiceDestroy(BusAttachment* bus, GatewayBusListener* busListener)
{
    if (bus && busListener) {
        bus->UnregisterBusListener(*busListener);
        bus->UnbindSessionPort(busListener->getSessionPort());
    }

    AboutServiceApi::DestroyInstance();
    return;
}

void cleanup()
{
    if (gatewayMgmt) {
        gatewayMgmt->shutdownGatewayMgmt();
        gatewayMgmt = NULL;
    }
    if (bus) {
        bus->CancelAdvertiseName(GW_WELLKNOWN_NAME, TRANSPORT_ANY);
        bus->ReleaseName(GW_WELLKNOWN_NAME);

        if (busListener) {
            aboutServiceDestroy(bus, busListener);
        }
    }
    if (busListener) {
        delete busListener;
        busListener = NULL;
    }
    if (propertyStoreImpl) {
        delete propertyStoreImpl;
        propertyStoreImpl = NULL;
    }
    if (keyListener) {
        delete keyListener;
        keyListener = NULL;
    }
    if (bus) {
        delete bus;
        bus = NULL;
    }
}

void signal_callback_handler(int32_t signum)
{
    if (signum == SIGCHLD) {
        signal(SIGCHLD, signal_callback_handler); //reset signal handler
        GatewayMgmt::sigChildCallback(signum);
    } else {
        s_interrupt = true;
    }
}

int main()
{
    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);
    signal(SIGCHLD, signal_callback_handler);

start:

    // Initialize GatewayMgmt object
    gatewayMgmt = GatewayMgmt::getInstance();

    QStatus status = prepareBusAttachment();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize BusAttachment."));
        cleanup();
        return 1;
    }

    keyListener = new SrpKeyXListener();
    keyListener->setPassCode("000000");
    status = bus->EnablePeerSecurity("ALLJOYN_PIN_KEYX ALLJOYN_SRP_KEYX ALLJOYN_ECDHE_PSK", keyListener);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not enable PeerSecurity"));
        cleanup();
        return 1;
    }

    propertyStoreImpl = new AboutPropertyStoreImpl();
    status = fillPropertyStore();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not fill PropertyStore."));
        cleanup();
        return 1;
    }

    busListener = new GatewayBusListener(bus, DaemonDisconnectHandler);
    status = prepareAboutService();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not set up the AboutService."));
        cleanup();
        return 1;
    }

    const uint32_t flags = DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE;
    status = bus->RequestName(GW_WELLKNOWN_NAME, flags);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not request Wellknown name"));
        cleanup();
        return 1;
    }

    status = bus->AdvertiseName(GW_WELLKNOWN_NAME, TRANSPORT_ANY);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not advertise Wellknown name"));
        cleanup();
        return 1;
    }

    status = bus->AdvertiseName(bus->GetUniqueName().c_str(), TRANSPORT_ANY);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not advertise Unique name"));
        cleanup();
        return 1;
    }

    status = gatewayMgmt->initGatewayMgmt(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize Gateway App - exiting application"));
        cleanup();
        return 1;
    }

    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        QCC_LogError(status, ("Could not initialize about Service"));
        cleanup();
        return 1;
    }

    status = aboutService->Announce();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not announce."));
        cleanup();
        return 1;
    }

    QCC_DbgPrintf(("Finished initializing Gateway App"));

    WaitForSigInt();

    cleanup();
    if (s_restart) {
        s_restart = false;
        goto start;
    }
    return 0;
}
