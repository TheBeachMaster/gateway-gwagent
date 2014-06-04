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

#include <alljoyn/about/AboutServiceApi.h>
#include <alljoyn/gateway/GatewayApp.h>
#include <alljoyn/gateway/GatewayManagement.h>
#include <alljoyn/gateway/GatewayPolicyManager.h>
#include <alljoyn/gateway/GatewayMetaDataManager.h>
#include "busObjects/AppBusObject.h"
#include "GatewayConstants.h"
#include <dirent.h>
#include <stdio.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>

namespace ajn {
namespace services {
using namespace gwConsts;
using namespace qcc;

GatewayApp::GatewayApp(qcc::String const& appId, GatewayAppManifest const& manifest) : m_AppId(appId),
    m_ObjectPath(AJ_GW_OBJECTPATH + "/" + appId), m_ConnectionStatus(GW_CS_NOT_INITIALIZED), m_OperationalStatus(GW_OS_STOPPED),
    m_InstallStatus(GW_IS_INSTALLED), m_InstallDescription(""), m_Manifest(manifest), m_AppBusObject(NULL), m_ProcessId(-1)
{
}

GatewayApp::~GatewayApp()
{
}

QStatus GatewayApp::init(BusAttachment* bus)
{
    QStatus status = ER_OK;

    if (!bus || !bus->IsStarted() || !bus->IsConnected()) {
        status = ER_BAD_ARG_1;
        QCC_LogError(status, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return status;
    }

    if (m_AppBusObject) {
        QCC_DbgPrintf(("Objects already registered. Ignoring request"));
        return ER_OK;
    }

    m_AppBusObject = new AppBusObject(bus, this, m_ObjectPath, &status);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not create AppBusObject"));
        return status;
    }

    status = bus->RegisterBusObject(*m_AppBusObject);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register AppBusObject"));
        return status;
    }

    status = loadAppAcls();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not load App Acls"));
        return status;
    }

    std::map<String, GatewayAcl*>::iterator it;
    for (it = m_Acls.begin(); it != m_Acls.end(); it++) {
        status = it->second->init(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not register Acl %s", it->first.c_str()));
            return status;
        }
    }

    if (hasActiveAcl()) {
        bool success = startApp();
        if (!success) {
            QCC_DbgHLPrintf(("Could not start the app %s", m_AppId.c_str()));
        }
    }

    status = updatePolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update Policies for app %s", m_AppId.c_str()));
        return status;
    }

    return status;
}

QStatus GatewayApp::shutdown(BusAttachment* bus)
{
    QStatus returnStatus = ER_OK;

    if (!bus || !bus->IsStarted() || !bus->IsConnected()) {
        returnStatus = ER_BAD_ARG_1;
        QCC_LogError(returnStatus, ("Could not accept this BusAttachment, busAttachment not started or not connected"));
        return returnStatus;
    }

    if (!m_AppBusObject) {
        QCC_DbgPrintf(("Objects not registered. Ignoring request"));
        return ER_OK;
    }

    bus->UnregisterBusObject(*m_AppBusObject);
    delete m_AppBusObject;
    m_AppBusObject = NULL;

    std::map<String, GatewayAcl*>::iterator it;
    for (it = m_Acls.begin(); it != m_Acls.end();) {
        GatewayAcl* acl = it->second;
        m_Acls.erase(it++);

        QStatus status = acl->shutdown(bus);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not unregister acl"));
            returnStatus = status;
        }
        delete acl;
    }

    QStatus status = updatePolicies();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not update Policies for app %s", m_AppId.c_str()));
        returnStatus = status;
    }
    return returnStatus;
}

const qcc::String& GatewayApp::getAppId() const
{
    return m_AppId;
}

ConnectionStatus GatewayApp::getConnectionStatus() const
{
    return m_ConnectionStatus;
}

const qcc::String& GatewayApp::getInstallDescription() const
{
    return m_InstallDescription;
}

InstallStatus GatewayApp::getInstallStatus() const
{
    return m_InstallStatus;
}

const qcc::String& GatewayApp::getObjectPath() const
{
    return m_ObjectPath;
}

OperationalStatus GatewayApp::getOperationalStatus() const
{
    return m_OperationalStatus;
}

const std::map<qcc::String, GatewayAcl*>& GatewayApp::getAcls() const
{
    return m_Acls;
}

const GatewayAppManifest& GatewayApp::getManifest() const
{
    return m_Manifest;
}

AppBusObject* GatewayApp::getAppBusObject() const
{
    return m_AppBusObject;
}

pid_t GatewayApp::getProcessId() const
{
    return m_ProcessId;
}

void GatewayApp::setConnectionStatus(ConnectionStatus connectionStatus)
{
    m_ConnectionStatus = connectionStatus;
    QStatus status = m_AppBusObject->SendAppStatusChangedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not send AppStatusChangedSignal"));
    }
}

QStatus GatewayApp::loadAppAcls()
{
    DIR* dir;
    struct dirent* entry;
    qcc::String dirName = GATEWAY_APPS_DIRECTORY + "/" + m_AppId + "/acls";
    if ((dir = opendir(dirName.c_str())) == NULL) {
        QCC_DbgHLPrintf(("Could not open gatewayApp Profile directory"));
        return ER_OK;
    }

    while ((entry = readdir(dir)) != NULL) {

        qcc::String aclId(entry->d_name);
        if (aclId.compare(".") == 0 || aclId.compare("..") == 0) {
            continue;
        }

        if (entry->d_type != DT_REG) {         // this is not a linked file - does not represent an acl
            QCC_DbgTrace(("Ignoring non file %s", entry->d_name));
            continue;
        }

        GatewayAcl* acl = new GatewayAcl(aclId, this);
        QStatus status = acl->loadFromFile(dirName + "/" + aclId);
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not parse the acl file for aclId: %s", aclId.c_str()));
            delete acl;
            continue;
        }

        m_Acls.insert(std::pair<qcc::String, GatewayAcl*>(aclId, acl));
    }
    closedir(dir);
    return ER_OK;
}

void GatewayApp::sigChildReceived()
{
    m_ConnectionStatus = GW_CS_NOT_INITIALIZED;
    m_OperationalStatus = GW_OS_STOPPED;
    m_ProcessId = -1;

    QStatus status = m_AppBusObject->SendAppStatusChangedSignal();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not send AppStatusChangedSignal"));
    }
}

void* GatewayApp::stop(void* arg)
{
    GatewayApp* myApp = (GatewayApp*)arg;
    bool success = myApp->shutdownApp();
    if (!success) {
        QCC_DbgHLPrintf(("Could not shutdown the Application"));
    }
    return NULL;
}

void* GatewayApp::restart(void* arg)
{
    GatewayApp* myApp = (GatewayApp*)arg;
    if (myApp->m_OperationalStatus == GW_OS_RUNNING && myApp->m_ProcessId != -1) {
        bool success = myApp->shutdownApp();
        if (!success) {
            QCC_DbgHLPrintf(("Could not shutdown the Application"));
            return NULL;
        }
    }

    bool success = myApp->startApp();
    if (!success) {
        QCC_DbgHLPrintf(("Could not start the Application successfully"));
        return NULL;
    }

    QCC_DbgPrintf(("Restarted the Application successfully"));
    return NULL;
}

RestartAppResponseCode GatewayApp::restartApp()
{
    QCC_DbgPrintf(("Restart App has been called"));

    if (!hasActiveAcl()) {
        return GW_RESTART_APP_RC_INVALID;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, GatewayApp::restart, this);

    return GW_RESTART_APP_RC_SUCCESS;
}

bool GatewayApp::shutdownApp(pthread_t* thread)
{
    QCC_DbgTrace(("Shutdown App has been called"));

    if (m_OperationalStatus != GW_OS_RUNNING && m_ProcessId == -1) {
        QCC_DbgPrintf(("App is not running - do not need to shut it down"));
        return false;
    }

    pthread_create(thread, NULL, GatewayApp::stop, this);
    return true;
}

bool GatewayApp::hasActiveAcl()
{
    std::map<String, GatewayAcl*>::iterator it;
    for (it = m_Acls.begin(); it != m_Acls.end(); it++) {
        if (it->second->getAclStatus() == GW_AS_ACTIVE) {
            return true;
        }
    }
    return false;
}

bool GatewayApp::shutdownApp()
{
    QStatus status = m_AppBusObject->SendShutdownAppSignal();
    if (status != ER_OK) {
        QCC_DbgHLPrintf(("Could not send shutdownAppSignal"));
    } else {
        for (int i = 0; i < 30 && m_ProcessId != -1; i++) {
            sleep(2);     //sleep to allow app to shut down gracefully
        }
    }

    if (m_ProcessId != -1) { // app did not shut down during sleep
        QCC_DbgPrintf(("App did not shut down during sleep. Killing Pid %i", m_ProcessId));
        int rc = kill(m_ProcessId, SIGKILL);
        if (rc != 0) {
            QCC_DbgHLPrintf(("Kill signal failed - process is probably already dead. errno is: %i", errno));
        } else {
            QCC_DbgPrintf(("Sent kill signal successfully"));
            for (int i = 0; i < 5 && m_ProcessId != -1; i++) {
                sleep(2);     //sleep to allow sigChild to be sent
            }
        }
    }
    return true;
}

bool GatewayApp::startApp()
{
    QCC_DbgPrintf(("Trying to start the App %s", m_AppId.c_str()));
    pid_t pid = fork();
    if (pid == -1) {
        QCC_DbgHLPrintf(("Could not fork to start App"));
        return false;
    } else if (pid != 0) {   // the parent

        m_ProcessId = pid;
        m_OperationalStatus = GW_OS_RUNNING;
        QCC_DbgPrintf(("App %s started with pid %i", m_AppId.c_str(), m_ProcessId));

        QStatus status = m_AppBusObject->SendAppStatusChangedSignal();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not send AppStatusChangedSignal"));
        }
        return true;
    } else {
        struct passwd* userInfo = getpwnam(m_AppId.c_str());
        if (!userInfo) {
            QCC_DbgHLPrintf(("Could not get the UserInfo for the AppId"));
            _Exit(0);
        }

        uid_t userId = userInfo->pw_uid;
        int rc = setuid(userId);
        if (rc != 0) {
            QCC_DbgHLPrintf(("Could not set the process to the app's userId %u. error no: %i", userId, errno));
            _Exit(0);
        }

        qcc::String appDirectory = GATEWAY_APPS_DIRECTORY + "/" + m_AppId + "/bin";
        rc = chdir(appDirectory.c_str());
        if (rc != 0) {
            QCC_DbgHLPrintf(("Could not change directories to the app's directory"));
            _Exit(0);
        }

        qcc::String executable = appDirectory + "/" + m_Manifest.getExecutableName();
        const std::vector<qcc::String>& appEnvVars = m_Manifest.getEnvironmentVariables();
        char* envVars[appEnvVars.size() + 1];
        envVars[appEnvVars.size()] = 0;
        for (size_t i = 0; i < appEnvVars.size(); i++) {
            envVars[i] = (char*)appEnvVars[i].c_str();
        }

        const std::vector<qcc::String>& appArgs = m_Manifest.getAppArguments();
        char* args[appArgs.size() + 2];
        args[0] = (char*)executable.c_str();
        args[appArgs.size()] = 0;
        for (size_t i = 1; i < appArgs.size(); i++) {
            args[i] = (char*)appArgs[i].c_str();
        }

        QCC_DbgHLPrintf(("Starting the executable %s", executable.c_str()));
        rc = execve(executable.c_str(), args, envVars);
        if (rc != 0) {
            QCC_DbgHLPrintf(("Could not start the executable %s. received error no: %i", executable.c_str(), errno));
        }
        _Exit(0);
    }
    return false;
}

AclResponseCode GatewayApp::createAcl(qcc::String* aclId, qcc::String const& aclName, GatewayPolicy const& policy,
                                      std::map<qcc::String, qcc::String> const& metaData, std::map<qcc::String, qcc::String> const& customMetaData)
{
    BusAttachment* bus = GatewayManagement::getInstance()->getBusAttachment();

    *aclId = generateAclId(aclName);
    QCC_DbgTrace(("Creating Acl with AclId %s", aclId->c_str()));

    GatewayMetaDataManager* metaDataManager = GatewayManagement::getInstance()->getMetaDataManager();
    if (!metaDataManager) {
        QCC_DbgHLPrintf(("metaDataManager is NULL"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    QStatus status = metaDataManager->updateMetaData(metaData);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist metaData"));
        return GW_ACL_RC_METADATA_ERROR;
    }

    GatewayAcl* acl = new GatewayAcl(*aclId, aclName, this, policy, customMetaData, GW_AS_INACTIVE);
    status = acl->init(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not register acl"));
        delete acl;
        return GW_ACL_RC_REGISTER_ERROR;
    }

    status = acl->writeToFile();
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not persist acl"));
        acl->shutdown(bus);
        delete acl;
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    m_Acls.insert(std::pair<qcc::String, GatewayAcl*>(acl->getAclId(), acl));
    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        QCC_DbgHLPrintf(("AboutServiceApi not initialized. Can't announce"));
    } else {
        status = aboutService->Announce();
        if (status != ER_OK) {
            QCC_LogError(status, ("Sending the announcement failed"));
        }
    }

    if (m_OperationalStatus != GW_OS_RUNNING && hasActiveAcl()) {
        bool success = startApp();
        if (!success) {
            QCC_DbgHLPrintf(("Could not start the app %s", m_AppId.c_str()));
        }
    }

    return GW_ACL_RC_SUCCESS;
}

qcc::String GatewayApp::generateAclId(qcc::String const& aclName)
{
    qcc::String aclId = "";
    for (size_t i = 0; i < aclName.size() && aclId.size() <= 20; i++) {
        if ((aclName[i] >= '0' && aclName[i] <= '9') ||
            (aclName[i] >= 'A' && aclName[i] <= 'Z') ||
            (aclName[i] >= 'a' && aclName[i] <= 'z')) {
            aclId.append(aclName[i]);
        }
    }

    std::map<String, GatewayAcl*>::iterator it;
    it = m_Acls.find(aclId);
    if (it == m_Acls.end()) {
        return aclId;
    }

    qcc::String aclIdWithCounter;
    for (int counter = 1; it != m_Acls.end(); counter++) {

        std::stringstream counterStr;
        counterStr << aclId.c_str() << counter;

        aclIdWithCounter.assign(counterStr.str().c_str());
        it = m_Acls.find(aclIdWithCounter);
    }
    return aclIdWithCounter;
}

AclResponseCode GatewayApp::deleteAcl(qcc::String const& aclId)
{
    BusAttachment* bus = GatewayManagement::getInstance()->getBusAttachment();

    std::map<String, GatewayAcl*>::iterator it;
    it = m_Acls.find(aclId);
    if (it == m_Acls.end()) {
        QCC_DbgHLPrintf(("Could not find acl to be deleted"));
        return GW_ACL_RC_ACL_NOT_FOUND;
    }

    GatewayAcl* acl = it->second;
    AclStatus aclStatus = acl->getAclStatus();

    int rc = remove((GATEWAY_APPS_DIRECTORY + "/" + m_AppId + "/acls/" + aclId).c_str());
    if (rc != 0) {
        QCC_DbgHLPrintf(("Could not remove acl successfully"));
        return GW_ACL_RC_PERSISTENCE_ERROR;
    }

    QStatus status = acl->shutdown(bus);
    if (status != ER_OK) {
        QCC_LogError(status, ("Could not unregister acl"));
        //Not returning an error - we should be able to recover from this
    }

    m_Acls.erase(it);
    delete acl;

    if (aclStatus == GW_AS_ACTIVE) {
        //acl was active - update policies and let app know acls changed
        status = updatePolicies();
        if (status != ER_OK) {
            QCC_LogError(status, ("Could not update policies successfully"));
            return GW_ACL_RC_POLICYMANAGER_ERROR;
        }

        status = m_AppBusObject->SendAclUpdatedSignal();
        if (status != ER_OK) {
            QCC_LogError(status, ("Sending AclUpdated Failed"));
        }

        if (m_OperationalStatus == GW_OS_RUNNING && !hasActiveAcl()) {
            pthread_t thread;
            bool success = shutdownApp(&thread);
            if (!success) {
                QCC_DbgHLPrintf(("Could not stop the app %s", m_AppId.c_str()));
            }
        }
    }

    AboutServiceApi* aboutService = AboutServiceApi::getInstance();
    if (!aboutService) {
        QCC_DbgHLPrintf(("AboutServiceApi not initialized. Can't announce"));
    } else {
        status = aboutService->Announce();
        if (status != ER_OK) {
            QCC_LogError(status, ("Sending the announcement failed"));
        }
    }

    return GW_ACL_RC_SUCCESS;
}

QStatus GatewayApp::updatePolicies()
{
    GatewayPolicyManager* policyManager = GatewayManagement::getInstance()->getPolicyManager();
    if (!policyManager) {
        return ER_FAIL;
    }

    std::vector<GatewayPolicy> policies;
    std::map<String, GatewayAcl*>::iterator it;

    for (it = m_Acls.begin(); it != m_Acls.end(); it++) {
        if (it->second->getAclStatus() == GW_AS_ACTIVE) {
            policies.push_back(it->second->getPolicy());
        }
    }

    bool success = policyManager->addAppPolicies(m_AppId, policies);
    if (!success) {
        QCC_DbgHLPrintf(("Updating the Policies failed"));
        return ER_FAIL;
    }
    return ER_OK;
}

} /* namespace services */
} /* namespace ajn */

