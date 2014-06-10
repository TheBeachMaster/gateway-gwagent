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

#ifndef GatewayCtrlTPApplicationStatus_H
#define GatewayCtrlTPApplicationStatus_H

#include <qcc/String.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/gateway/GatewayCtrlEnums.h>

namespace ajn {
namespace services {

class GatewayCtrlTPApplicationStatus {
  public:

    GatewayCtrlTPApplicationStatus(const ajn::MsgArg* returnArgs);

    /**
     * Constructor
     * @param installStatus status of the installed Third Party Application
     * @param installDescription string describing the install of the Third Party Application
     * @param connectionStatus status of connection to Third Party Application
     * @param operationalStatus operational status of the Third Party Application
     */
    GatewayCtrlTPApplicationStatus(InstallStatus installStatus, qcc::String installDescription, ConnectionStatus connectionStatus, OperationalStatus operationalStatus);

    /**
     * Destructor
     */
    virtual ~GatewayCtrlTPApplicationStatus();

    /**
     * @return The installation status of the Third Party Application
     */
    InstallStatus GetInstallStatus();

    /**
     * @return The installation description of the Third Party Application
     */
    qcc::String GetInstallDescriptions();

    /**
     * @return Connection status of the Third Party Application to its cloud service
     */
    ConnectionStatus GetConnectionStatus();

    /**
     * @return The state whether the Third Party Application is running
     */
    OperationalStatus GetOperationalStatus();

  private:
    /**
     * Installation status
     */
    InstallStatus m_InstallStatus;

    /**
     * Installation description
     */
    qcc::String m_InstallDescription;

    /**
     * Connection status of the Third Party Application to its cloud service
     */
    ConnectionStatus m_ConnectionStatus;

    /**
     * The state whether the Third Party Application is working
     */
    OperationalStatus m_OperationalStatus;

};
}
}
#endif /* defined(GatewayCtrlTPApplicationStatus_H) */
