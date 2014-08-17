////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, AllSeen Alliance. All rights reserved.
//
//    Permission to use, copy, modify, and/or distribute this software for any
//    purpose with or without fee is hereby granted, provided that the above
//    copyright notice and this permission notice appear in all copies.
//
//    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
//    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
//    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
//    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#import "AJGWCEnums.h"
@interface AJGWCEnums ()
@end

@implementation AJGWCEnums

+ (NSString*)AJGWCConnectionStatusToString:(AJGWCConnectionStatus) connectionStatus
{
    NSString* connectionStatusStr;

    switch (connectionStatus) {
        case GW_CS_NOT_INITIALIZED:
            connectionStatusStr = @"Not initialized";
            break;

        case GW_CS_IN_PROGRESS:
            connectionStatusStr = @"In progress";
            break;

        case GW_CS_CONNECTED:
            connectionStatusStr = @"Connected";
            break;

        case GW_CS_NOT_CONNECTED:
            connectionStatusStr = @"Not connected";
            break;

        case GW_CS_ERROR:
            connectionStatusStr = @"Error";
            break;

        default:
            break;
    }

    return connectionStatusStr;
}

+ (NSString*)AJGWCInstallStatusToString:(AJGWCInstallStatus) installStatus
{
    NSString* installStatusStr;

    switch (installStatus) {
        case GW_IS_INSTALLED:
            installStatusStr = @"Installed";
            break;

        case GW_IS_INSTALL_IN_PROGRESS:
            installStatusStr = @"Install in progress";
            break;

        case GW_IS_UPGRADE_IN_PROGRESS:
            installStatusStr = @"Upgrade in progress";

            break;

        case GW_IS_UNINSTALL_IN_PROGRESS:
            installStatusStr = @"Uninstall in progress";
            break;

        case GW_IS_INSTALL_FAILED:
            installStatusStr = @"Installation failed";
            break;

        default:
            break;
    }

    return installStatusStr;
}

+ (NSString*)AJGWCOperationalStatusToString:(AJGWCOperationalStatus) operationalStatus
{
    NSString* operationalStatusStr;

    switch (operationalStatus) {
        case GW_OS_RUNNING:
            operationalStatusStr = @"Running";
            break;

        case GW_OS_STOPPED:
            operationalStatusStr = @"Stopped";
            break;
        default:
            break;
    }

    return operationalStatusStr;
}

+ (NSString*)AJGWCRestartStatusToString:(AJGWCRestartStatus) restartStatus
{
    NSString* restartStatusStr;

    switch (restartStatus) {
        case GW_RESTART_APP_RC_SUCCESS:
            restartStatusStr = @"Restarted";
            break;

        case GW_RESTART_APP_RC_INVALID:
            restartStatusStr = @"Restart failed";
            break;

        default:
            break;
    }

    return restartStatusStr;
}

+ (NSString*)AJGWCAclResponseCodeToString:(AJGWCAclResponseCode) aclResponseCode
{
    NSString* aclResponseCodeStr;

    switch (aclResponseCode) {
        case GW_ACL_RC_SUCCESS:
            aclResponseCodeStr = @"Success";
            break;

        case GW_ACL_RC_INVALID:
            aclResponseCodeStr = @"Invalid";
            break;

        case GW_ACL_RC_REGISTER_ERROR:
            aclResponseCodeStr = @"Register error";
            break;

        case GW_ACL_RC_ACL_NOT_FOUND:
            aclResponseCodeStr = @"Not found";
            break;

        case GW_ACL_RC_PERSISTENCE_ERROR:
            aclResponseCodeStr = @"Persistence error";
            break;

        case GW_ACL_RC_POLICYMANAGER_ERROR:
            aclResponseCodeStr = @"Policy manager error";

        default:
            break;
    }

    return aclResponseCodeStr;
}

+ (NSString*)AJGWCAclStatusToString:(AJGWCAclStatus) aclStatus
{
    NSString* aclStatusStr;

    switch (aclStatus) {
        case GW_AS_INACTIVE:
            aclStatusStr = @"Inactive";
            break;

        case GW_AS_ACTIVE:
            aclStatusStr = @"Active";

        default:
            break;
    }

    return aclStatusStr;
}
@end
