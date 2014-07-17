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

#include <alljoyn/gateway/GatewayCtrlManifestRules.h>
#include <alljoyn/gateway/LogModule.h>

#include "PayloadAdapter.h"

namespace ajn {
namespace services {



QStatus GatewayCtrlManifestRules::init(const ajn::MsgArg*manifRulesAJ)
{
    QStatus status;

    status = PayloadAdapter::FillManifestObjectDescriptionVector(&manifRulesAJ[0], m_ExposedServices);

    if (status != ER_OK) {
        QCC_LogError(status, ("Filling m_ExposedServices failed"));
        return status;
    }

    status = PayloadAdapter::FillManifestObjectDescriptionVector(&manifRulesAJ[1], m_RemotedServices);

    if (status != ER_OK) {
        QCC_LogError(status, ("Filling m_RemotedServices failed"));
        return status;
    }

    return ER_OK;
}

GatewayCtrlManifestRules::~GatewayCtrlManifestRules()
{

}

const std::vector<GatewayCtrlManifestObjectDescription*>& GatewayCtrlManifestRules::getExposedServices() const
{
    return m_ExposedServices;
}

const std::vector<GatewayCtrlManifestObjectDescription*>& GatewayCtrlManifestRules::getRemotedServices() const
{
    return m_RemotedServices;
}


void GatewayCtrlManifestRules::emptyVectors()
{
    for (size_t indx = 0; indx < m_ExposedServices.size(); indx++) {
        QStatus status = m_ExposedServices[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_ExposedServices[indx];
    }

    m_ExposedServices.clear();


    for (size_t indx = 0; indx < m_RemotedServices.size(); indx++) {
        QStatus status = m_RemotedServices[indx]->release();

        if (status != ER_OK) {
            QCC_LogError(status, ("Could not release object"));
        }
        delete m_RemotedServices[indx];
        m_RemotedServices[indx] = NULL;
    }

    m_RemotedServices.clear();

}


QStatus GatewayCtrlManifestRules::release()
{
    emptyVectors();

    return ER_OK;
}
}
}