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

#ifndef GATEWAYCTRLCONSTANTS_H_
#define GATEWAYCTRLCONSTANTS_H_

#include <qcc/String.h>

namespace ajn {
namespace services {
namespace gwcConsts {
//static const uint16_t UUID_LENGTH = 16;

/**
 * The prefix for all the gateway interface names is org.alljoyn.gwagent.ctrl
 */

/**
 * Port number to connect to a Gateway Configuration Manager
 */
static const uint16_t GATEWAYSERVICE_PORT = 1020;

static const qcc::String AJ_OBJECTPATH_PREFIX = "/gw";
static const qcc::String AJ_GATEWAYCONTROLLER_APPMGMT_INTERFACE = "org.alljoyn.gwagent.ctrl.AppMgmt";
static const qcc::String AJ_GATEWAYCONTROLLERAPP_INTERFACE = "org.alljoyn.gwagent.ctrl.App";

static const qcc::String AJ_PROPERTY_VERSION = "Version";

static const qcc::String AJ_SIGNAL_APPSTATUSCHANGED = "AppStatusChanged";

static const qcc::String AJ_METHOD_GETINSTALLEDAPPS = "GetInstalledApps";
static const qcc::String AJ_METHOD_GETMANIFESTFILE = "GetManifestFile";
static const qcc::String AJ_METHOD_GETMANIFESTINTERFACES = "GetManifestInterfaces";
static const qcc::String AJ_METHOD_GETAPPSTATUS = "GetAppStatus";
static const qcc::String AJ_METHOD_RESTARTAPP = "RestartApp";

static const qcc::String AJ_GATEWAYCONTROLLER_ACLMGMT_INTERFACE = "org.alljoyn.gwagent.ctrl.AclMgmt";
static const qcc::String AJ_METHOD_CREATEACL = "CreateAcl";
static const qcc::String AJ_METHOD_DELETEACL = "DeleteAcl";
static const qcc::String AJ_METHOD_LISTACLS = "ListAcls";

static const qcc::String AJ_GATEWAYCONTROLLERACL_INTERFACE = "org.alljoyn.gwagent.ctrl.Acl";
static const qcc::String AJ_METHOD_ACTIVATEACL = "ActivateAcl";
static const qcc::String AJ_METHOD_DEACTIVATEACL = "DeactivateAcl";
static const qcc::String AJ_METHOD_GETACL = "GetAcl";
static const qcc::String AJ_METHOD_GETACLSTATUS = "GetAclStatus";
static const qcc::String AJ_METHOD_UPDATEACL = "UpdateAcl";
static const qcc::String AJ_METHOD_UPDATEACLMETADATA = "UpdateAclMetaData";
static const qcc::String AJ_METHOD_UPDATECUSTOMMETADATA = "UpdateAclCustomMetaData";

static const qcc::String AJPARAM_EMPTY = "";
static const qcc::String AJPARAM_VAR = "v";
static const qcc::String AJPARAM_STR = "s";
static const qcc::String AJPARAM_BOOL = "b";
static const qcc::String AJPARAM_UINT16 = "q";
static const qcc::String AJPARAM_INT16 = "n";
static const qcc::String AJPARAM_UINT32 = "u";
static const qcc::String AJPARAM_INT32 = "i";
static const qcc::String AJPARAM_UINT64 = "t";
static const qcc::String AJPARAM_INT64 = "x";
static const qcc::String AJPARAM_DOUBLE = "d";

/**
 * Suffix of the application name that is sent with the ACL metadata
 */
static const qcc::String AJSUFFIX_APP_NAME = "_APP_NAME";

/**
 * Suffix of the device name that is sent with the ACL metadata
 */
static const qcc::String AJSUFFIX_DEVICE_NAME = "_DEVICE_NAME";

static const uint16_t AJ_METHOD_UPDATEACL_INPUT_PARAM_COUNT = 5;

}         //namespace gwcConsts
}     //namespace services
} //namespace ajn

#endif


