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

#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/notification/Notification.h>
#include <alljoyn/notification/NotificationReceiver.h>
#include <alljoyn/notification/NotificationService.h>
#include "alljoyn/gateway/GatewayConnector.h"
#include <alljoyn/about/AboutServiceApi.h>
#include <alljoyn/PasswordManager.h>
#include <alljoyn/about/AnnounceHandler.h>
#include <alljoyn/about/AnnouncementRegistrar.h>
#include <alljoyn/config/ConfigClient.h>
#include <alljoyn/services_common/GuidUtil.h>

#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <set>

using namespace ajn;
using namespace ajn::services;
using namespace ajn::gw;
using namespace std;

BusAttachment* bus;
NotificationService* notificationService = 0;
NotificationSender* notificationSender = 0;
CommonBusListener*  busListener = 0;
SrpKeyXListener* keyListener = 0;
qcc::String tweetScript = "";

class ConfigSession : public BusAttachment::JoinSessionAsyncCB, public SessionListener {
  private:
    void PrintAboutData(AboutClient::AboutData& aboutData)
    {
        for (AboutClient::AboutData::iterator itx = aboutData.begin(); itx != aboutData.end(); ++itx) {
            qcc::String key = itx->first;
            ajn::MsgArg value = itx->second;
            if (value.typeId == ALLJOYN_STRING) {
                std::cout << "Key name=" << key.c_str() << " value=" << value.v_string.str << std::endl;
            } else if (value.typeId == ALLJOYN_ARRAY && value.Signature().compare("as") == 0) {
                std::cout << "Key name=" << key.c_str() << " values: ";
                const MsgArg* stringArray;
                size_t fieldListNumElements;
                //QStatus status =
                value.Get("as", &fieldListNumElements, &stringArray);
                for (unsigned int i = 0; i < fieldListNumElements; i++) {
                    char* tempString;
                    stringArray[i].Get("s", &tempString);
                    std::cout << tempString << " ";
                }
                std::cout << std::endl;
            } else if (value.typeId == ALLJOYN_BYTE_ARRAY) {
                std::cout << "Key name=" << key.c_str() << " value=" << std::hex << std::uppercase << std::setfill('0');
                uint8_t* AppIdBuffer;
                size_t numElements;
                value.Get("ay", &numElements, &AppIdBuffer);
                for (size_t i = 0; i < numElements; i++) {
                    std::cout <<  std::setw(2) << (unsigned int)AppIdBuffer[i];
                }
                std::cout << std::nouppercase << std::dec << std::endl;
            }
        }
    }

  public:
    virtual void JoinSessionCB(QStatus status, SessionId sessionId, const SessionOpts& opts, void* context) {
        static bool firstJoin = true;
        QStatus myStat;

        if (status != ER_OK) {
            cout << "Error joining session " <<  QCC_StatusText(status) << endl;
            free(context);
        } else {
            bus->EnableConcurrentCallbacks();
            if (firstJoin) {
                firstJoin = false;

                bool isIconInterface = false;
                bool isConfigInterface = false;
                AboutClient aboutClient(*bus);
                int ver = 0;

                AboutClient::ObjectDescriptions ObjectDescriptionsRefill;
                myStat = aboutClient.GetObjectDescriptions((char*)context, ObjectDescriptionsRefill, sessionId);

                if (myStat != ER_OK) {
                    cout << "getObjectDescriptions: status=" << QCC_StatusText(myStat) << endl;
                } else {
                    for (AboutClient::ObjectDescriptions::const_iterator it = ObjectDescriptionsRefill.begin();
                         it != ObjectDescriptionsRefill.end(); ++it) {
                        qcc::String key = it->first;
                        std::vector<qcc::String> vector = it->second;
                        cout << "key=" << key.c_str();
                        for (std::vector<qcc::String>::const_iterator itv = vector.begin(); itv != vector.end(); ++itv) {
                            if (key.compare("/About/DeviceIcon") == 0 && itv->compare("org.alljoyn.Icon") == 0) {
                                isIconInterface = true;
                            }
                            if (key.compare("/Config") == 0 && itv->compare("org.alljoyn.Config") == 0) {
                                isConfigInterface = true;
                            }
                            cout << " value=" << itv->c_str();
                        }
                        cout << endl;
                    }
                }

                AboutClient::AboutData aboutDataRefill;

                std::vector<qcc::String> supportedLanguages;
                myStat = aboutClient.GetAboutData((char*)context, NULL, aboutDataRefill);
                if (myStat != ER_OK) {
                    cout << "getAboutData: status="  << QCC_StatusText(myStat) << endl;
                } else {
                    AboutClient::AboutData::iterator search = aboutDataRefill.find("SupportedLanguages");
                    if (search != aboutDataRefill.end()) {
                        const MsgArg* stringArray;
                        size_t fieldListNumElements;
                        search->second.Get("as", &fieldListNumElements, &stringArray);
                        for (unsigned int i = 0; i < fieldListNumElements; i++) {
                            char* tempString;
                            stringArray[i].Get("s", &tempString);
                            supportedLanguages.push_back(tempString);
                        }
                    }
                }

                for (std::vector<qcc::String>::iterator it = supportedLanguages.begin(); it != supportedLanguages.end();
                     ++it) {
                    cout << endl << (char*)context << " AboutClient AboutData using language=" << it->c_str() << endl;
                    cout << "-----------------------------------" << endl;
                    AboutClient::AboutData aboutDataRefill;
                    myStat = aboutClient.GetAboutData((char*)context, it->c_str(), aboutDataRefill);
                    if (myStat != ER_OK) {
                        cout << "getAboutData: status="  << QCC_StatusText(myStat) << endl;
                    } else {
                        PrintAboutData(aboutDataRefill);
                    }
                }

                myStat = aboutClient.GetVersion((char*)context, ver, sessionId);
                if (myStat != ER_OK) {
                    cout << "getVersion: status=" << QCC_StatusText(myStat) << endl;
                } else {
                    cout << "Version=" << ver << endl;
                }

                if (isIconInterface) {
                    AboutIconClient iconClient(*bus);
                    size_t contentSize = 0;
                    qcc::String url;

                    myStat = iconClient.GetUrl((char*)context, url, sessionId);
                    if (myStat != ER_OK) {
                        cout << "getUrl: status= " << QCC_StatusText(myStat) << endl;
                    } else {
                        cout << "url=" << url.c_str() << endl;
                    }

                    AboutIconClient::Icon icon;
                    myStat = iconClient.GetIcon((char*)context, icon, sessionId);

                    if (myStat != ER_OK) {
                        cout << "GetContent: status=" << QCC_StatusText(myStat) << endl;
                    } else {
                        cout << "Content size=" << icon.contentSize << endl;
                        cout << "Content :\t";
                        for (size_t i = 0; i < icon.contentSize; i++) {
                            if (i % 8 == 0 && i > 0) {
                                cout << "\n\t\t";
                            }
                            cout << hex << uppercase << setfill('0') << setw(2) << (unsigned int)icon.content[i]
                                 << nouppercase << dec;
                        }
                        cout << endl;
                    }

                    myStat = iconClient.GetVersion((char*)context, ver, sessionId);
                    if (myStat != ER_OK) {
                        cout << "getVersion: status=" << QCC_StatusText(myStat) << endl;
                    } else {
                        cout << "Version=" << ver << endl;
                    }

                    qcc::String mimetype;
                    myStat = iconClient.GetMimeType((char*)context, mimetype, sessionId);
                    if (myStat != ER_OK) {
                        cout << "getMimetype: status=" << QCC_StatusText(myStat) << endl;
                    } else {
                        cout << "Mimetype=" << mimetype.c_str() << endl;
                    }

                    myStat = iconClient.GetSize((char*)context, contentSize, sessionId);
                    if (myStat != ER_OK) {
                        cout << "getSize status=" << QCC_StatusText(myStat) << endl;
                    } else {
                        cout << "Size=" << contentSize << endl;
                    }
                } // if (isIconInterface)

                if (isConfigInterface) {
                    ConfigClient configClient(*bus);

                    myStat = configClient.GetVersion((char*)context, ver, sessionId);
                    cout << "GetVersion: status=" << QCC_StatusText(myStat) << " version=" << ver << endl;

                    myStat = configClient.SetPasscode((char*)context, NULL, 6, (const uint8_t*)"000000", sessionId);
                    cout << "SetPasscode: status=" << QCC_StatusText(myStat) << endl;

                    ConfigClient::Configurations updateConfigurations;
                    updateConfigurations.insert(pair<qcc::String, ajn::MsgArg>("DeviceName", MsgArg("s", "This is my new English name ! ! ! !")));
                    myStat = configClient.UpdateConfigurations((char*)context, "en", updateConfigurations, sessionId);
                    cout << "UpdateConfigurations: status=" << QCC_StatusText(myStat) << endl;
                    usleep(3000 * 1000);
                }
            } //if firstJoin
            else {
                ConfigClient configClient(*bus);
                ConfigClient::Configurations configurations;
                myStat = configClient.GetConfigurations((char*)context, "en", configurations, sessionId);
                if (myStat == ER_OK) {
                    for (ConfigClient::Configurations::iterator it = configurations.begin();
                         it != configurations.end(); ++it) {
                        qcc::String key = it->first;
                        ajn::MsgArg value = it->second;
                        if (value.typeId == ALLJOYN_STRING) {
                            cout << "Key name=" << key.c_str() << " value=" << value.v_string.str << endl;
                        } else if (value.typeId == ALLJOYN_ARRAY && value.Signature().compare("as") == 0) {
                            cout << "Key name=" << key.c_str() << " values: ";
                            const MsgArg* stringArray;
                            size_t fieldListNumElements;
                            status = value.Get("as", &fieldListNumElements, &stringArray);
                            for (unsigned int i = 0; i < fieldListNumElements; i++) {
                                char* tempString;
                                stringArray[i].Get("s", &tempString);
                                cout << tempString << " ";
                            }
                            cout << endl;
                        }
                    }
                } else {
                    cout << "GetConfigurations: status=" << QCC_StatusText(myStat) << endl;
                }
            }
            free(context);
            bus->LeaveSession(sessionId);
            delete this;
        }
    }

};

class ConfigAnnounceHandler : public AnnounceHandler {
  public:
    virtual void Announce(uint16_t version, uint16_t port,
                          const char* busName, const ObjectDescriptions& objectDescs, const AboutData& aboutData) {

        cout << "Received Announce from " << busName << endl;

        ObjectDescriptions::const_iterator it1 = objectDescs.find("/Config");
        if (it1 == objectDescs.end()) { return; }

        vector<qcc::String>::const_iterator it2;
        for (it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
            if (0 == it2->compare("org.alljoyn.Config")) { break; }
        }
        if (it2 == it1->second.end()) { return; }

        SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
        ConfigSession* cs = new ConfigSession();
        bus->JoinSessionAsync(busName, port, cs, opts, cs, strdup(busName));
    }

};

ConfigAnnounceHandler* announceHandler = 0;

void cleanup() {
    if (!bus) {
        return;
    }

    if (announceHandler) {
        AnnouncementRegistrar::UnRegisterAnnounceHandler(*bus, *announceHandler, NULL, 0);
        delete announceHandler;
    }

    if (notificationService) {
        notificationService->shutdown();
        notificationService = 0;
    }
    if (busListener) {
        CommonSampleUtil::aboutServiceDestroy(bus, busListener);
        delete busListener;
        busListener = 0;
    }

    bus->Disconnect();
    if (keyListener) {
        delete keyListener;
        keyListener = 0;
    }
    bus->Stop();
    delete bus;
    bus = 0;
}

void signal_callback_handler(int32_t signum) {
    cleanup();
    exit(signum);
}

void dumpObjectSpecs(list<GatewayMergedAcl::ObjectDescription>& specs, const char* indent) {
    list<GatewayMergedAcl::ObjectDescription>::iterator it;
    for (it = specs.begin(); it != specs.end(); it++) {
        GatewayMergedAcl::ObjectDescription& spec = *it;
        cout << indent << "objectPath: " << spec.objectPath.c_str() << endl;
        cout << indent << "isPrefix: " << (spec.isPrefix ? "true" : "false") << endl;

        list<qcc::String>::iterator innerator;
        for (innerator = spec.interfaces.begin(); innerator != spec.interfaces.end(); innerator++) {
            cout << indent << "    " << "interface: " << (*innerator).c_str() << endl;
        }
    }
}

void dumpAcl(GatewayMergedAcl* p) {
    cout << "Exposed Services:" << endl;
    dumpObjectSpecs(p->m_ExposedServices, "");
    cout << endl;


    cout << "Remoted Apps:" << endl;
    list<GatewayMergedAcl::RemotedApp>::iterator it;
    for (it = p->m_RemotedApps.begin(); it != p->m_RemotedApps.end(); it++) {
        GatewayMergedAcl::RemotedApp& rapp = *it;
        cout << rapp.deviceId.c_str() << " ";
        for (int i = 0; i < 16; i++) cout << hex << (unsigned int)rapp.appId[i];
        cout << endl;
        cout << "    Object Specs:" << endl;
        dumpObjectSpecs(rapp.objectDescs, "    ");
    }
}

class MyApp : public GatewayConnector {
  public:
    MyApp(BusAttachment* bus, qcc::String wkn) : GatewayConnector(bus, wkn) { }

  protected:
    virtual void mergedAclUpdated() {
        cout << "Merged Acl updated" << endl;
        GatewayMergedAcl* mergedAcl = new GatewayMergedAcl();
        QStatus status = getMergedAclAsync(mergedAcl);
        if (ER_OK != status) { delete mergedAcl; }
    }
    virtual void shutdown() {
        cout << "shutdown" << endl;
        kill(getpid(), SIGINT);
    }
    virtual void receiveGetMergedAclAsync(QStatus unmarshalStatus, GatewayMergedAcl* response) {
        if (ER_OK != unmarshalStatus) {
            cout << "Profile failed to unmarshal " << unmarshalStatus << endl;
        } else {
            dumpAcl(response);
        }

        delete response;
    }
};

class MyReceiver : public NotificationReceiver {
  public:
    virtual void Receive(Notification const& notification) {
        vector<NotificationText> vecMessages = notification.getText();

        for (vector<NotificationText>::const_iterator it = vecMessages.begin(); it != vecMessages.end(); ++it) {
            cout << "Notification in: " << it->getLanguage().c_str() << "  Message: " << it->getText().c_str() << endl;

            if (tweetScript.size() && it->getLanguage().compare("en") == 0) {
                qcc::String cmd = "sh -i " + tweetScript + " \"" + notification.getAppName() +
                                  " sent: " + it->getText().c_str() + "\"";
                cout << "Command is: " << cmd.c_str() << endl;
                int result = system(cmd.c_str());
                result = WEXITSTATUS(result);
                cout << "system result=" << result << endl;
            }
        }

    }

    virtual void Dismiss(const int32_t msgId, const qcc::String appId) {
        cout << "Received notification dismiss for msg=" << msgId << " from app=" << appId.c_str() << endl;
    }
};

int main(int argc, char** argv) {
    signal(SIGINT, signal_callback_handler);
    bus = new BusAttachment("ConnectorApp", true);

    //====================================
    // Initialize bus
    //====================================
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif

    QStatus status = bus->Start();
    if (ER_OK != status) {
        cout << "Error starting bus: " << QCC_StatusText(status) << endl;
        cleanup();
        return 1;
    }

    status = bus->Connect();
    if (ER_OK != status) {
        cout << "Error connecting bus: " << QCC_StatusText(status) << endl;
        cleanup();
        return 1;
    }

    char* wkn = getenv("WELL_KNOWN_NAME");
    qcc::String wellknownName = wkn ? wkn : "dummyapp1";

    char* interOff = getenv("INTERACTIVE_OFF");
    bool notInteractive = (interOff && (strcmp(interOff, "1") == 0)) ? true : false;

    char* twScript = getenv("TWITTER_SCRIPT");
    tweetScript = twScript ? "/opt/alljoyn/apps/" + wellknownName + "/bin/" +  twScript : "";

    //====================================
    // Initialize authentication
    //====================================
    keyListener = new SrpKeyXListener();
    keyListener->setPassCode("000000");
    qcc::String keystore = "/opt/alljoyn/apps/" + wellknownName + "/store/.alljoyn_keystore.ks";
    status = bus->EnablePeerSecurity("ALLJOYN_PIN_KEYX ALLJOYN_SRP_KEYX ALLJOYN_ECDHE_PSK", keyListener, keystore.c_str(), false);

    //====================================
    // Initialize GwConnector interface
    //====================================
    MyApp myApp(bus, wellknownName.c_str());
    status = myApp.init();
    if (ER_OK != status) {
        cout << "Error connecting bus: " << QCC_StatusText(status) << endl;
        cleanup();
        return 1;
    }

    //====================================
    // Initialize notification consumer
    //====================================
    notificationService = NotificationService::getInstance();
    MyReceiver receiver;
    status = notificationService->initReceive(bus, &receiver);
    if (ER_OK != status) {
        cout << "Error initializing notification receiver: " << QCC_StatusText(status) << endl;
        cleanup();
        return 1;
    }


    //====================================
    // Initialize notification producer
    //====================================
    qcc::String deviceid;
    GuidUtil::GetInstance()->GetDeviceIdString(&deviceid);
    qcc::String appid;
    GuidUtil::GetInstance()->GenerateGUID(&appid);

    AboutPropertyStoreImpl propStore;
    DeviceNamesType deviceNames;
    deviceNames.insert(pair<qcc::String, qcc::String>("en", "ConnectorSampleDevice"));
    status = CommonSampleUtil::fillPropertyStore(&propStore, appid, "ConnectorSample", deviceid, deviceNames);
    if (status != ER_OK) {
        cout << "Could not fill PropertyStore. " <<  QCC_StatusText(status) << endl;
        cleanup();
        return 1;
    }
    busListener = new CommonBusListener();
    status = CommonSampleUtil::prepareAboutService(bus, &propStore, busListener, 900);
    if (status != ER_OK) {
        cout << "Could not set up the AboutService." << endl;
        cleanup();
        return 1;
    }
    notificationSender = notificationService->initSend(bus, &propStore);
    if (!notificationSender) {
        cout << "Could not initialize Sender" << endl;
        cleanup();
        return 1;
    }


    //====================================
    // Register for config announcements
    //====================================
    announceHandler = new ConfigAnnounceHandler();
    AnnouncementRegistrar::RegisterAnnounceHandler(*bus, *announceHandler, NULL, 0);

    //====================================
    // Here we go
    //====================================
    size_t lineSize = 1024;
    char line[1024];
    char* buffy = line;
    while (1) {
        if (notInteractive) {
            sleep(5);
            continue;
        }
        putchar('>');
        if (-1 == getline(&buffy, &lineSize, stdin)) {
            break;
        }
        char* cmd = strtok(buffy, " \r\n\t");
        if (NULL == cmd) {
            continue;
        }
        cout << "Got command " << cmd << endl;

        if (0 == strcmp(cmd, "GetMergedAcl")) {
            GatewayMergedAcl macl;
            QStatus status = myApp.getMergedAcl(&macl);
            cout << "GetMergedAcl returned " << status << endl;
            if (status == ER_OK) {
                dumpAcl(&macl);
            }
        } else if (0 == strcmp(cmd, "UpdateConnectionStatus")) {
            char* s = strtok(NULL, " \r\t\n");
            if (NULL == s) {
                cout << "Please try again and specify the new connection status" << endl;
                continue;
            }
            int i = atoi(s);
            myApp.updateConnectionStatus((ConnectionStatus)i);
        } else if (0 == strcmp(cmd, "Notify")) {
            char* typeStr = strtok(NULL, " \r\t\n");
            if (NULL == typeStr) {
                cout << "Something went wrong sending message" << endl;
                continue;
            }
            char* msg = typeStr + strlen(typeStr) + 1;

            vector<NotificationText> msgs;
            msgs.push_back(NotificationText("en", msg));

            status = notificationSender->send(Notification((NotificationMessageType)atoi(typeStr), msgs), 7200);
            cout << "send returned " << QCC_StatusText(status) << endl;
        } else if (0 == strcmp(cmd, "Exit")) {
            break;
        } else {
            cout << "Type one of:" << endl
                 << "GetMergedAcl<CR>" << endl
                 << "UpdateConnectionStatus 0|1|2|3|4<CR>" << endl
                 << "Notify 0|1|2 the rest of the message<CR>" << endl
                 << "Exit" << endl;
        }
    }

    cleanup();

    return 0;
}
