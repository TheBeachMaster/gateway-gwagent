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

#include <alljoyn/gateway/GatewayApp.h>
#include <alljoyn/gateway/GatewayAppManager.h>
#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/gateway/GatewayPolicyManager.h>
#include "busObjects/AppMgmtBusObject.h"
#include "GatewayConstants.h"
#include <dirent.h>

namespace ajn {
namespace services {
using namespace qcc;
using namespace gwConsts;

GatewayAppManager::GatewayAppManager() : m_AppMgmtBusObject(NULL)
{
}

GatewayAppManager::~GatewayAppManager()
{
}

std::map<String, GatewayApp*> GatewayAppManager::getApps() const
{
    return m_Apps;
}

QStatus GatewayAppManager::init(BusAttachment* bus)
{
    QStatus status = ER_OK;

    if (!bus || !bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (m_AppMgmtBusObject) {
        QCC_DbgPrintf(("Objects already registered. Ignoring request"));
        return ER_OK;
    }

    m_AppMgmtBusObject = new AppMgmtBusObject(bus, this, &status);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create GatewayAppMgmt BusObject"));
        return status;
    }

    status = bus->RegisterBusObject(*m_AppMgmtBusObject);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register GatewayAppMgmt BusObject"));
        return status;
    }

    status = loadInstalledApps();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not load Installed Apps"));
        return status;
    }

    std::map<String, GatewayApp*>::iterator it;
    for (it = m_Apps.begin(); it != m_Apps.end(); it++) {
        status = it->second->init(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register app"));
            return status;
        }
    }

    return status;
}

QStatus GatewayAppManager::shutdown(BusAttachment* bus)
{
    QStatus returnStatus = ER_OK;

    if (!bus || !bus->IsStarted() || !bus->IsConnected()) {
        returnStatus = ER_BAD_ARG_1;
        QCC_LogError(returnStatus, ("Could not acccept this BusAttachment, busAttachment not started or not connected"));
        return returnStatus;
    }

    if (!m_AppMgmtBusObject) {
        QCC_DbgPrintf(("Objects not registered. Ignoring request"));
        return ER_OK;
    }

    GatewayPolicyManager* policyManager = GatewayManagement::getInstance()->getPolicyManager();
    if (!policyManager) {
        QCC_DbgHLPrintf(("PolicyManager not defined"));
        return ER_FAIL;
    }

    bus->UnregisterBusObject(*m_AppMgmtBusObject);
    delete m_AppMgmtBusObject;
    m_AppMgmtBusObject = NULL;

    std::map<String, GatewayApp*>::iterator it;
    std::vector<pthread_t> shutdownThreads;

    for (it = m_Apps.begin(); it != m_Apps.end(); it++) {
        pthread_t thread;
        bool success = it->second->shutdownApp(&thread);
        if (success) {
            shutdownThreads.push_back(thread);
        }
    }

    for (size_t i = 0; i < shutdownThreads.size(); i++) {
        pthread_join(shutdownThreads[i], NULL);
    }

    for (it = m_Apps.begin(); it != m_Apps.end();) {
        GatewayApp* app = it->second;

        QStatus status = app->shutdown(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not unregister app"));
            returnStatus = status;
        }
        m_Apps.erase(it++);

        bool success = policyManager->removeAppPolicies(app->getAppId());
        if (!success) {
            QCC_DbgHLPrintf(("Updating the Policies failed"));
            returnStatus =  ER_FAIL;
        }
        delete app;
    }

    return returnStatus;
}

QStatus GatewayAppManager::loadInstalledApps()
{
    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(GATEWAY_APPS_DIRECTORY.c_str())) == NULL) {
        QCC_DbgHLPrintf(("Could not open gatewayApps directory"));
        return ER_FAIL;
    }

    while ((entry = readdir(dir)) != NULL) {

        qcc::String appId(entry->d_name);
        if (appId.compare(".") == 0 || appId.compare("..") == 0) {
            continue;
        }

        if (entry->d_type != DT_DIR) {   // this is not a directory - does not represents an app
            QCC_DbgTrace(("Ignoring non directory %s", entry->d_name));
            continue;
        }

        GatewayAppManifest manifest;
        qcc::String manifestFileName = GATEWAY_APPS_DIRECTORY + "/" + appId + "/Manifest.xml";
        QStatus status = manifest.parseManifestFile(manifestFileName);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not parse the manifest file for app: %s", appId.c_str()));
            continue;
        }

        GatewayApp* gatewayApp = new GatewayApp(appId, manifest);
        m_Apps.insert(std::pair<qcc::String, GatewayApp*>(appId, gatewayApp));
    }
    closedir(dir);

    return ER_OK;
}

void GatewayAppManager::sigChildReceived(pid_t pid)
{
    std::map<String, GatewayApp*>::iterator it;
    for (it = m_Apps.begin(); it != m_Apps.end(); it++) {
        if (it->second->getProcessId() == pid) {
            it->second->sigChildReceived();
        }
    }
}

} /* namespace services */
} /* namespace ajn */


