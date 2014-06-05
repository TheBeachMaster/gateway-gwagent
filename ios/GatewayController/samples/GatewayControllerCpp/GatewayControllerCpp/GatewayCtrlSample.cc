/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <signal.h>
#include <alljoyn/about/AnnouncementRegistrar.h>
#include <alljoyn/gateway/GatewayCtrlGatewayController.h>
#include <SrpKeyXListener.h>
#include <CommonSampleUtil.h>
#include <AnnounceHandlerImpl.h>
#include <alljoyn/services_common/LogModulesNames.h>
#include "GatewayCtrlConstants.h"

#include <alljoyn/services_common/AsyncTaskQueue.h>




#define SERVICE_PORT 900

using namespace ajn;
using namespace services;
using namespace qcc;
using namespace gwcConsts;

BusAttachment* bus = 0;
SrpKeyXListener* srpKeyXListener = 0;
AnnounceHandlerImpl* announceHandler = 0;
GatewayCtrlGatewayController *gatewayController = 0;
std::vector<AnnouncementData *> announcements;

class AnnouncementTaskData : public ajn::services::TaskData
{
public:
    AnnouncementTaskData(qcc::String busName,
                         unsigned short version,
                         unsigned short port,
                         AnnounceHandlerImpl::AnnounceHandler::ObjectDescriptions objectDescs,
                         AnnounceHandlerImpl::AnnounceHandler::AboutData aboutData);
    
    qcc::String m_busName;
    unsigned short m_version;
    unsigned short m_port;
    AnnounceHandlerImpl::AnnounceHandler::ObjectDescriptions m_objectDescs;
    AnnounceHandlerImpl::AnnounceHandler::AboutData m_aboutData;
private:
    
};



AnnouncementTaskData::AnnouncementTaskData(qcc::String busName,
                                           unsigned short version,
                                           unsigned short port,
                                           AnnounceHandlerImpl::AnnounceHandler::ObjectDescriptions objectDescs,
                                           AnnounceHandlerImpl::AnnounceHandler::AboutData aboutData)
{
    m_busName = busName;
    m_version = version;
    m_port = port;
    m_objectDescs = objectDescs;
    m_aboutData = aboutData;
}



void exitApp(int32_t signum);

void PrintGatewayCtrlManifestObjectDescription(const std::vector<GatewayCtrlManifestObjectDescription*> &services)
{
    for(int i=0;i!=services.size();i++)
    {
        std::cout << "  service " << i << ":\n";
        GatewayCtrlTPObjectPath *tpObjectPath = services[i]->GetObjectPath();
        std::cout << "      Object Path:" << tpObjectPath->GetFriendlyName().c_str() << ",path:" << tpObjectPath->GetPath().c_str() << ",IsPrefix:" << tpObjectPath->IsPrefix() << "\n";
        
        const std::set<GatewayCtrlTPInterface> *interfaces = services[i]->GetInterfaces();
        
        for(std::set<GatewayCtrlTPInterface>::const_iterator y=interfaces->begin();y!=interfaces->end();y++)
        {
            GatewayCtrlTPInterface tpInterface = *y;
            std::cout << "      interface " << tpInterface.GetFriendlyName().c_str() << ":'" <<  tpInterface.GetName().c_str() << "'," <<(tpInterface.IsSecured()?"Secured":"Not Secured") << "\n";
        }
    }
}

void PrintTPApplicationStatus(GatewayCtrlTPApplicationStatus *TPAppStatus)
                              {

                                  std::cout << "App Status:\n";
                                  std::cout << "  GetInstallStatus:" << TPAppStatus->GetInstallStatus() << ",GetInstallDescriptions:'" << TPAppStatus->GetInstallDescriptions().c_str() << "',GetConnectionStatus:" << TPAppStatus->GetConnectionStatus() << ",GetOperationalStatus:" << TPAppStatus->GetOperationalStatus() << std::endl;

        }

void PrintTPApplicationStatus(GatewayCtrlTPApplication *TPApplication,SessionId sessionId)
{
    QStatus status;
    
    GatewayCtrlTPApplicationStatus *TPAppStatus= TPApplication->RetrieveStatus(sessionId, status);
                              if (status == ER_OK) {
                                  PrintTPApplicationStatus(TPAppStatus);
                              } else {std::cout<< "RetrieveStatus error occurred, check log\n"; return;}
}

class sampleHandler: public GatewayCtrlApplicationStatusSignalHandler
{
public:
    void onStatusChanged(qcc::String appId, const GatewayCtrlTPApplicationStatus *TPApplicationStatus)
    {
        std::cout << "onStatusChanged called! appId:" << appId.c_str() << "-->";
        PrintTPApplicationStatus((GatewayCtrlTPApplicationStatus *)TPApplicationStatus);
    }
};



void PrintACL(GatewayCtrlAccessControlList *ACL,SessionId sessionId)
{
    QStatus status;
    
    std::cout << " ACL name:'" << ACL->GetName().c_str() << "',ID:" << ACL->GetId().c_str() << ",Object Path:" << ACL->GetObjectPath().c_str() << ",Status:" << ACL->GetStatus() << "\n";
    
    GatewayCtrlManifestRules rules;
    
    
    GatewayCtrlAccessRules *accessRules = ACL->retrieveAcl(sessionId, rules, announcements, status);
    
    if (status == ER_OK) {
        const std::vector<GatewayCtrlManifestObjectDescription*> exposedServices = accessRules->GetExposedServices();
        
        std::cout << "AccessRules exposed services:\n";
        PrintGatewayCtrlManifestObjectDescription (exposedServices);
        
        const std::vector<GatewayCtrlRemotedApp*> remotedApps = accessRules->GetRemotedApps();

        std::cout << "AccessRules remoted apps:\n";
        
        for (int x = 0; x!=remotedApps.size(); x++) {
            GatewayCtrlRemotedApp *remotedApp = remotedApps[x];
            
            std::cout << " Remoted app #" << x <<" name:'" << remotedApp->GetAppName().c_str() << "', device name:'" << remotedApp->GetDeviceName().c_str() << "'\n";
            PrintGatewayCtrlManifestObjectDescription (remotedApp->GetObjDescRules());
            
        }
        
    } else {std::cout<< "retrieveAcl error occurred, check log\n"; return;}

    
}

class AnnouncementTask: public ajn::services::AsyncTask
{
public:
    
    
private:
    
    virtual void OnEmptyQueue() { }
    
    virtual void OnTask(AnnouncementTaskData::TaskData const* taskdata)
    {
        AnnouncementTaskData *data = (AnnouncementTaskData *)taskdata;
        
        ajn::services::AnnouncementData *announcement = new ajn::services::AnnouncementData(data->m_port,data->m_aboutData, data->m_objectDescs);
        
        GatewayCtrlGateway *gateway = NULL;
        
        std::cout << data->m_busName.c_str() << "\n";
        
        for (AboutClient::ObjectDescriptions::const_iterator it = data->m_objectDescs.begin(); it != data->m_objectDescs.end(); ++it) {
            qcc::String key = it->first;
            std::vector<qcc::String> vector = it->second;
            std::cout << "key=" << key.c_str();
            for (std::vector<qcc::String>::const_iterator itv = vector.begin(); itv != vector.end(); ++itv) {
                std::cout << " value=" << itv->c_str() << std::endl;
                
                if (itv->compare(AJ_GATEWAYCONTROLLER_APPMGMT_INTERFACE) == 0) {
                    
                    if (key.compare(AJ_OBJECTPATH_PREFIX) == 0) {
                        std::cout << "Gateway found\n";
                        gateway = gatewayController->CreateGateway(data->m_busName, data->m_objectDescs, data->m_aboutData);
                        
                        break;
                    }
                }
            }
        }
        
        if (!gateway) {
            announcements.push_back(announcement);
            
            std::cout << "Adding non gateway service on bus " << data->m_busName.c_str() << " to announcement vector\n";
            return;
        }
        
        GatewayCtrlSessionResult result = gateway->JoinSession();
        
        if (result.m_status != ER_OK) {
            std::cout << "Joining session failed with code:" << result.m_status << "\n";
        } else {
            std::cout << "Joined session #" << (uint32_t)result.m_sid << " on bus '" << data->m_busName.c_str() << "'\n";
        }
        
        SessionId sessionId = result.m_sid;
        
        std::vector<GatewayCtrlTPApplication*> installedApps;
        
        QStatus status;
        installedApps = gateway->RetrieveInstalledApps(sessionId,  status);
        
        if (status == ER_OK) {
            std::cout << "retreived installed apps\n";
            
            for (int x=0; x!=installedApps.size(); x++) {
                GatewayCtrlTPApplication *TPApplication = installedApps[x];
                
                sampleHandler *handler = new sampleHandler;
                
                
                status = TPApplication->SetStatusChangedHandler(handler);
                if (status != ER_OK) {
                    std::cout << "Error while registering status changed handler" << "\n";
                    return;
                } else { std::cout << "Registered status change handler" << "\n";}
                
                std::cout << x << ")app name:"<< TPApplication->GetFriendlyName().c_str() << ",objectPath:" << TPApplication->GetObjectPath().c_str() << ",AppVersion:" << TPApplication->GetAppVersion().c_str() << "\n";
                
                qcc::String ManifestFile = TPApplication->RetrieveManifestFile(sessionId, status);
                
                if (status == ER_OK) {
                    std::cout << "Manifest file for " << TPApplication->GetFriendlyName().c_str() << ":'" << ManifestFile.c_str() << "'\n";
                } else {std::cout<< "RetrieveManifestFile error occurred, check log\n"; return;}
                
                GatewayCtrlManifestRules *manifestRules = TPApplication->RetrieveManifestRules(sessionId, status);
                
                if (status == ER_OK) {
                    std::cout << "Manifest rules:\n";
                    std::cout << "GetExposedServices:\n";
                    PrintGatewayCtrlManifestObjectDescription(manifestRules->GetExposedServices());
                    
                    std::cout << "GetRemotedServices:\n";
                    PrintGatewayCtrlManifestObjectDescription(manifestRules->GetRemotedServices());
                    
                } else {std::cout<< "RetrieveManifestRules error occurred, check log\n"; return;}
                
                PrintTPApplicationStatus(TPApplication,sessionId);
                
                RestartStatus restart = TPApplication->Restart(sessionId, status);
                
                if (status == ER_OK) {
                    std::cout << "App restart status:" << (restart==GW_RESTART_APP_RC_SUCCESS?"ok":"invalid") << "\n";
                    
                } else {std::cout<< "Restart error occurred, check log\n"; return;}
                
                const std::vector <GatewayCtrlAccessControlList*> accessControlListVector = TPApplication->RetrieveAcls(sessionId, status);
                
                if (status == ER_OK) {
                    std::cout << "retreived "<< accessControlListVector.size() << " ACLs for " << TPApplication->GetFriendlyName().c_str() << "\n";
                    
                    for (int i=0; i!=accessControlListVector.size(); i++) {
                        
                        GatewayCtrlAccessControlList* ACL = accessControlListVector[i];
                        
                        PrintACL(ACL,sessionId);
                        
                        
                        GatewayCtrlAccessRules *accessRules = ACL->retrieveAcl(sessionId, *manifestRules, announcements, status);
                        
                        if (status != ER_OK) {
                            std::cout << "retrieveAcl call failed\n"; return;
                        }
                        
                        ACL->SetName("woohooo");
                        
                        
                        GatewayCtrlAclWriteResponse* aclWriteResponse = ACL->UpdateAcl(sessionId, accessRules, manifestRules, status);
                        
                        if (status != ER_OK) {
                            std::cout << "failed calling Update Acl! code:" << status << "\n";
                            return;
                        }
                        
                        std::cout << "ACL Updated. name:'" << ACL->GetName().c_str() << "'\n";
                        
                        std::cout << "  Object path:'" << aclWriteResponse->GetObjectPath().c_str() << "'\n"; //TODO add printout, maybe use other Print functions?
                    }
                } else {std::cout<< "error occurred, check log\n"; return;}
                
                
                
            }
        } else {std::cout<< "error occurred, check log\n"; return;}
        
        
        
    }
    
};


AnnouncementTask announcementTask;
AsyncTaskQueue applicationSignalQueue(&announcementTask);

static void announceHandlerCallback(qcc::String const& busName, unsigned short version, unsigned short port,
                                    const AnnounceHandler::ObjectDescriptions& objectDescs, const AnnounceHandler::AboutData& aboutData)
{
    
    AnnouncementTaskData *announcementTaskData = new AnnouncementTaskData(busName, version, port, objectDescs, aboutData);
    
    applicationSignalQueue.Enqueue(announcementTaskData);
 
    
    
    
    
    }

int main()
{
    //QCC_SetDebugLevel("ALL", 7);
    
    QStatus status;

    // Allow CTRL+C to end application
    signal(SIGINT, exitApp);
    std::cout << "Beginning Gateway Controller Application. (Press CTRL+C to end application)" << std::endl;

    // Initialize Service objects
#ifdef QCC_USING_BD
    PasswordManager::SetCredentials("ALLJOYN_PIN_KEYX", "000000");
#endif

    QCC_SetDebugLevel(logModules::GATEWAYCTRL_MODULE_LOG_NAME, logModules::ALL_LOG_LEVELS);
    
    applicationSignalQueue.Start();

    srpKeyXListener = new SrpKeyXListener();
    
    
    bus = CommonSampleUtil::prepareBusAttachment(srpKeyXListener);
    if (bus == NULL) {
        std::cout << "Could not initialize BusAttachment." << std::endl;
        exitApp(1);
    }

    srpKeyXListener->setPassCode("000000");
    
    status = bus->EnablePeerSecurity("ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX", srpKeyXListener,
                                               ".alljoyn_keystore/central.ks", true);
    
    if (status != ER_OK) {
        std::cout << "Could not Enable Peer Security" << std::endl;
    }

    
    GatewayCtrlGatewayController::Init(bus);
    
    gatewayController = GatewayCtrlGatewayController::getInstance();

    announceHandler = new AnnounceHandlerImpl(NULL, announceHandlerCallback);
    AnnouncementRegistrar::RegisterAnnounceHandler(*bus, *announceHandler);


    status = CommonSampleUtil::addSessionlessMatch(bus);
    if (status != ER_OK) {
        std::cout << "Could not add Sessionless Match" << std::endl;
        exitApp(1);
    }

    std::cout << "Finished setup. Waiting for Gateways" << std::endl;
    while (1) {
        sleep(1);
    }
}

void exitApp(int32_t signum)
{
    std::cout << "Program Finished" << std::endl;
    
    if (bus && announceHandler) {
        AnnouncementRegistrar::UnRegisterAnnounceHandler(*bus, *announceHandler);
    }
    
    if (announceHandler) {
        delete announceHandler;
    }
    if (srpKeyXListener) {
        delete srpKeyXListener;
    }
    if (bus) {
        delete bus;
    }
    
    if (gatewayController) {
        gatewayController->Shutdown();
    }
    
    applicationSignalQueue.Stop();
    
    std::cout << "Goodbye!" << std::endl;
    exit(signum);
}