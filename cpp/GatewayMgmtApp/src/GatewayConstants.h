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

#ifndef GATEWAYCONSTANTS_H_
#define GATEWAYCONSTANTS_H_

#include <qcc/String.h>
#include <qcc/Debug.h>
#include <qcc/platform.h>
#include <alljoyn/gateway/GatewayEnums.h>

namespace ajn {
namespace gw {

static char const* const QCC_MODULE = "GATEWAY";

namespace gwConsts {

static const uint16_t GATEWAY_PORT = 1020;
static const uint16_t GATEWAY_MANAGEMENT_VERSION = 1;

static const qcc::String GATEWAY_APPS_DIRECTORY = "/opt/alljoyn/apps";
static const qcc::String GATEWAY_POLICIES_DIRECTORY = "/opt/alljoyn/alljoyn-daemon.d";

static const qcc::String AJPARAM_EMPTY = "";
static const qcc::String AJPARAM_BOOL = "b";
static const qcc::String AJPARAM_STR = "s";
static const qcc::String AJPARAM_OBJECTPATH = "o";
static const qcc::String AJPARAM_UINT16 = "q";
static const qcc::String AJPARAM_UINT32 = "u";
static const qcc::String AJPARAM_ARRAY_UINT16 = "aq";
static const qcc::String AJPARAM_ARRAY_STR = "as";
static const qcc::String AJPARAM_BINARY_ARR = "ay";

static const qcc::String AJPARAM_MANIFEST_INTERFACE_STRUCT = "(ssb)";
static const qcc::String AJPARAM_MANIFEST_INTERFACE_INFO = "((obs)a(ssb))";
static const qcc::String AJPARAM_MANIFEST_INTERFACE_INFO_ARRAY = "a((obs)a(ssb))";
static const qcc::String AJPARAM_INSTALLED_APPS_INFO = "(ssos)";
static const qcc::String AJPARAM_INSTALLED_APPS_INFO_ARRAY = "a(ssos)";
static const qcc::String AJPARAM_INTERFACE_INFO = "(obas)";
static const qcc::String AJPARAM_INTERFACE_INFO_ARRAY = "a(obas)";
static const qcc::String AJPARAM_REMOTED_APPS = "(say" + AJPARAM_INTERFACE_INFO_ARRAY + ")";
static const qcc::String AJPARAM_REMOTED_APPS_ARRAY = "a(say" + AJPARAM_INTERFACE_INFO_ARRAY + ")";
static const qcc::String AJPARAM_ACL_METADATA = "{ss}";
static const qcc::String AJPARAM_ACL_METADATA_ARRAY = "a{ss}";
static const qcc::String AJPARAM_ACLS_STRUCT = "(ssqo)";
static const qcc::String AJPARAM_ACLS_STRUCT_ARRAY = "a(ssqo)";

static const qcc::String AJ_GW_OBJECTPATH = "/gw";
static const qcc::String AJ_GW_APP_WKN_PREFIX = "org.alljoyn.GWAgent.Connector.";
static const qcc::String AJ_PROPERTY_VERSION = "Version";

static const qcc::String AJ_GW_APP_MGMT_INTERFACE = "org.alljoyn.gwagent.ctrl.AppMgmt";
static const qcc::String AJ_GW_APP_INTERFACE = "org.alljoyn.gwagent.ctrl.App";
static const qcc::String AJ_GW_APP_CONNECTOR_INTERFACE = "org.alljoyn.gwagent.connector.App";
static const qcc::String AJ_GW_ACL_MGMT_INTERFACE = "org.alljoyn.gwagent.ctrl.AclMgmt";
static const qcc::String AJ_GW_ACL_INTERFACE = "org.alljoyn.gwagent.ctrl.Acl";

static const qcc::String AJ_METHOD_GET_INSTALLED_APPS = "GetInstalledApps";
static const qcc::String& AJ_GET_INSTALLED_APPS_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String& AJ_GET_INSTALLED_APPS_PARAMS_OUT = AJPARAM_INSTALLED_APPS_INFO_ARRAY;
static const qcc::String AJ_GET_INSTALLED_APPS_PARAM_NAMES = "installedAppsInfoArray";

static const qcc::String AJ_METHOD_GET_APP_STATUS = "GetAppStatus";
static const qcc::String& AJ_GET_APP_STATUS_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_APP_STATUS_PARAMS_OUT = AJPARAM_UINT16 + AJPARAM_STR + AJPARAM_UINT16 + AJPARAM_UINT16;
static const qcc::String AJ_GET_APP_STATUS_PARAM_NAMES = "installStatus,installDescription,connectionStatus,operationalStatus";

static const qcc::String AJ_METHOD_RESTART_APP = "RestartApp";
static const qcc::String& AJ_RESTART_APP_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_RESTART_APP_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_RESTART_APP_PARAM_NAMES = "restartResponseCode";

static const qcc::String AJ_METHOD_GET_MANIFEST_FILE = "GetManifestFile";
static const qcc::String& AJ_GET_MANIFEST_FILE_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_MANIFEST_FILE_PARAMS_OUT = AJPARAM_STR;
static const qcc::String AJ_GET_MANIFEST_FILE_PARAM_NAMES = "manifestFile";

static const qcc::String AJ_METHOD_GET_MANIFEST_INTERFACES = "GetManifestInterfaces";
static const qcc::String& AJ_GET_MANIFEST_INTERFACES_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_MANIFEST_INTERFACES_PARAMS_OUT = AJPARAM_MANIFEST_INTERFACE_INFO_ARRAY + AJPARAM_MANIFEST_INTERFACE_INFO_ARRAY;
static const qcc::String AJ_GET_MANIFEST_INTERFACES_PARAM_NAMES = "exposedServices,remotedServices";

static const qcc::String AJ_SIGNAL_APP_STATUS_CHANGED = "AppStatusChanged";
static const qcc::String& AJ_APP_STATUS_CHANGED_PARAMS = AJPARAM_UINT16 + AJPARAM_STR + AJPARAM_UINT16 + AJPARAM_UINT16;
static const qcc::String AJ_APP_STATUS_CHANGED_PARAM_NAMES = "installStatus,installDescription,connectionStatus,operationalStatus";

static const qcc::String AJ_METHOD_CREATE_ACL = "CreateAcl";
static const qcc::String AJ_CREATE_ACL_PARAMS_IN = AJPARAM_STR + AJPARAM_INTERFACE_INFO_ARRAY + AJPARAM_REMOTED_APPS_ARRAY +
                                                   AJPARAM_ACL_METADATA_ARRAY + AJPARAM_ACL_METADATA_ARRAY;
static const qcc::String AJ_CREATE_ACL_PARAMS_OUT = AJPARAM_UINT16 + AJPARAM_STR + AJPARAM_OBJECTPATH;
static const qcc::String AJ_CREATE_ACL_PARAM_NAMES = "aclName,exposedServices,remotedApps,metadata,customMetadata,aclResponseCode,aclId,objectPath";

static const qcc::String AJ_METHOD_DELETE_ACL = "DeleteAcl";
static const qcc::String AJ_DELETE_ACL_PARAMS_IN = AJPARAM_STR;
static const qcc::String AJ_DELETE_ACL_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_DELETE_ACL_PARAM_NAMES = "aclId,aclResponseCode";

static const qcc::String AJ_METHOD_LIST_ACLS = "ListAcls";
static const qcc::String& AJ_LIST_ACLS_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_LIST_ACLS_PARAMS_OUT = AJPARAM_ACLS_STRUCT_ARRAY;
static const qcc::String AJ_LIST_ACLS_PARAM_NAMES = "aclsList";

static const qcc::String AJ_METHOD_ACTIVATE_ACL = "ActivateAcl";
static const qcc::String& AJ_ACTIVATE_ACL_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_ACTIVATE_ACL_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_ACTIVATE_ACL_PARAM_NAMES = "aclResponseCode";

static const qcc::String AJ_METHOD_GET_ACL = "GetAcl";
static const qcc::String& AJ_GET_ACL_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_ACL_PARAMS_OUT = AJPARAM_STR + AJPARAM_INTERFACE_INFO_ARRAY + AJPARAM_REMOTED_APPS_ARRAY +
                                                 AJPARAM_ACL_METADATA_ARRAY + AJPARAM_ACL_METADATA_ARRAY;
static const qcc::String AJ_GET_ACL_PARAM_NAMES = "aclName,exposedServices,remotedApps,metadata,customMetadata";

static const qcc::String AJ_METHOD_GET_ACL_STATUS = "GetAclStatus";
static const qcc::String& AJ_GET_ACL_STATUS_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_ACL_STATUS_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_GET_ACL_STATUS_PARAM_NAMES = "aclStatus";

static const qcc::String AJ_METHOD_UPDATE_ACL = "UpdateAcl";
static const qcc::String AJ_UPDATE_ACL_PARAMS_IN = AJPARAM_STR + AJPARAM_INTERFACE_INFO_ARRAY + AJPARAM_REMOTED_APPS_ARRAY +
                                                   AJPARAM_ACL_METADATA_ARRAY + AJPARAM_ACL_METADATA_ARRAY;
static const qcc::String AJ_UPDATE_ACL_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_UPDATE_ACL_PARAM_NAMES = "aclName,exposedServices,remotedApps,metadata,customMetadata,aclResponseCode";

static const qcc::String AJ_METHOD_UPDATE_CUSTOM_METADATA = "UpdateAclCustomMetadata";
static const qcc::String AJ_METHOD_UPDATE_METADATA = "UpdateAclMetadata";
static const qcc::String AJ_UPDATE_METADATA_PARAMS_IN = AJPARAM_ACL_METADATA_ARRAY;
static const qcc::String AJ_UPDATE_METADATA_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_UPDATE_METADATA_PARAM_NAMES = "metadata,aclResponseCode";

static const qcc::String AJ_METHOD_DEACTIVATE_ACL = "DeactivateAcl";
static const qcc::String& AJ_DEACTIVATE_ACL_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_DEACTIVATE_ACL_PARAMS_OUT = AJPARAM_UINT16;
static const qcc::String AJ_DEACTIVATE_ACL_PARAM_NAMES = "aclResponseCode";

static const qcc::String AJ_METHOD_GET_MERGED_ACL = "GetMergedAcl";
static const qcc::String& AJ_GET_MERGED_ACL_PARAMS_IN = AJPARAM_EMPTY;
static const qcc::String AJ_GET_MERGED_ACL_PARAMS_OUT = AJPARAM_INTERFACE_INFO_ARRAY + AJPARAM_REMOTED_APPS_ARRAY;
static const qcc::String AJ_GET_MERGED_ACL_PARAM_NAMES = "exposedServices,remotedApps";

static const qcc::String AJ_METHOD_UPDATE_CONNECTION_STATUS = "UpdateConnectionStatus";
static const qcc::String AJ_UPDATE_CONNECTION_STATUS_PARAMS_IN = AJPARAM_UINT16;
static const qcc::String& AJ_UPDATE_CONNECTION_STATUS_PARAMS_OUT = AJPARAM_EMPTY;
static const qcc::String AJ_UPDATE_CONNECTION_STATUS_PARAM_NAMES = "connectionStatus";

static const qcc::String AJ_SIGNAL_ACL_UPDATED = "MergedAclUpdated";
static const qcc::String& AJ_ACL_UPDATED_PARAMS = AJPARAM_EMPTY;
static const qcc::String& AJ_ACL_UPDATED_PARAM_NAMES = AJPARAM_EMPTY;

static const qcc::String AJ_SIGNAL_SHUTDOWN_APP = "ShutdownApp";
static const qcc::String& AJ_SHUTDOWN_APP_PARAMS = AJPARAM_EMPTY;
static const qcc::String& AJ_SHUTDOWN_APP_PARAM_NAMES = AJPARAM_EMPTY;

static const qcc::String GATEWAY_XML_XSD = "/opt/alljoyn/gw-management/manifest.xsd";
static const qcc::String GATEWAY_XML_SCHEMA = "http://www.alljoyn.org/gateway/acl/sample";
static const qcc::String GATEWAY_XML_COMMENT = qcc::String("Copyright (c) 2014, AllSeen Alliance. All rights reserved.\n") +
                                               "\n" +
                                               "   Permission to use, copy, modify, and/or distribute this software for any\n" +
                                               "   purpose with or without fee is hereby granted, provided that the above\n" +
                                               "   copyright notice and this permission notice appear in all copies.\n" +
                                               "" +
                                               "   THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES\n" +
                                               "   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF\n" +
                                               "   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR\n" +
                                               "   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\n" +
                                               "   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN\n" +
                                               "   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF\n" +
                                               "   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.";

} //namespace cpsConsts
} //namespace gw
} //namespace ajn

#endif


