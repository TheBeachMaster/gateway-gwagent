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
#include <alljoyn/AboutData.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>
#include <alljoyn/gateway/GatewayMgmt.h>
#include <alljoyn/gateway/GatewayBusListener.h>
#include "../GatewayConstants.h"
#include "SrpKeyXListener.h"
#include "GuidUtil.h"

using namespace ajn;
using namespace gw;
using namespace qcc;

#define SERVICE_PORT 900

GatewayMgmt* gatewayMgmt = NULL;
BusAttachment* bus = NULL;
AboutData* aboutData = NULL;
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

QStatus prepareBusListener()
{
    if (!bus || !busListener) {
        return ER_FAIL;
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

    return status;
}

QStatus fillAboutData()
{
    if (!aboutData) {
        return ER_BAD_ARG_1;
    }

    QStatus status = ER_OK;

    qcc::String deviceId;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceId);
    qcc::String appId;
    GuidUtil::GetInstance()->GenerateGUID(&appId);

    status = aboutData->SetDeviceId(deviceId.c_str());
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetDeviceName("AllJoyn Gateway Agent", "en");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetAppId(appId.c_str());
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetAppName("AllJoyn Gateway Configuration Manager", "en");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetDefaultLanguage("en");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetSupportUrl("http://www.allseenalliance.org");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetManufacturer("AllSeen Alliance", "en");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetModelNumber("1.0");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetSoftwareVersion("1.0");
    if (status != ER_OK) {
        return status;
    }
    status = aboutData->SetDescription("AllJoyn Gateway Configuration Manager Application", "en");
    if (status != ER_OK) {
        return status;
    }
    return status;
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
            bus->UnregisterBusListener(*busListener);
            bus->UnbindSessionPort(busListener->getSessionPort());
        }
    }
    if (busListener) {
        delete busListener;
        busListener = NULL;
    }
    if (aboutData) {
        delete aboutData;
        aboutData = NULL;
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
qcc::String policyFileOption = "--gwagent-policy-file=";
qcc::String appsPolicyDirOption = "--apps-policy-dir=";

int main(int argc, char** argv)
{
    if (AllJoynInit() != ER_OK) {
        return 1;
    }
#ifdef ROUTER
    if (AllJoynRouterInit() != ER_OK) {
        AllJoynShutdown();
        return 1;
    }
#endif
    // Allow CTRL+C to end application
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);
    signal(SIGCHLD, signal_callback_handler);

start:

    // Initialize GatewayMgmt object
    gatewayMgmt = GatewayMgmt::getInstance();

    for (int i = 1; i < argc; i++) {
        qcc::String arg(argv[i]);
        if (arg.compare(0, policyFileOption.size(), policyFileOption) == 0) {
            qcc::String policyFile = arg.substr(policyFileOption.size());
            QCC_DbgPrintf(("Setting gatewayPolicyFile to: %s", policyFile.c_str()));
            gatewayMgmt->setGatewayPolicyFile(policyFile.c_str());
        }
        if (arg.compare(0, appsPolicyDirOption.size(), appsPolicyDirOption) == 0) {
            qcc::String policyDir = arg.substr(appsPolicyDirOption.size());
            QCC_DbgPrintf(("Setting appsPolicyDir to: %s", policyDir.c_str()));
            gatewayMgmt->setAppPolicyDir(policyDir.c_str());
        }
    }

    QStatus status = prepareBusAttachment();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not initialize BusAttachment."));
        cleanup();
        return 1;
    }

    keyListener = new SrpKeyXListener();
    keyListener->setPassCode("000000");
    status = bus->EnablePeerSecurity("ALLJOYN_SRP_KEYX ALLJOYN_SRP_LOGON ALLJOYN_ECDHE_PSK", keyListener);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not enable PeerSecurity"));
        cleanup();
        return 1;
    }

    aboutData = new AboutData("en");
    status = fillAboutData();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not fill AboutData."));
        cleanup();
        return 1;
    }

    busListener = new GatewayBusListener(bus, DaemonDisconnectHandler);
    status = prepareBusListener();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not set up the BusListener."));
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

    AboutObj aboutObj(*bus);
    status = aboutObj.Announce(SERVICE_PORT, *aboutData);
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
    AllJoynShutdown();
    return 0;
}
