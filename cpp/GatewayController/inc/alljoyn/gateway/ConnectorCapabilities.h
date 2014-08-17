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

#ifndef ConnectorCapabilities_H
#define ConnectorCapabilities_H

#include <vector>
#include <alljoyn/Status.h>
#include <alljoyn/gateway/RuleObjectDescription.h>

namespace ajn {
namespace gwc {
/**
 * The manifest rules of the Connector App
 */
class ConnectorCapabilities {
  public:

    /**
     * Constructor - init must be called
     */
    ConnectorCapabilities() { }

    /**
     * init
     * @param manifRulesAJ MsgArg with manifest rules
     * @return {@link QStatus}
     */
    QStatus init(const ajn::MsgArg*manifRulesAJ);

    /**
     * Destructor
     */
    virtual ~ConnectorCapabilities();

    /**
     * The {@link RuleObjectDescription} objects that the Connector App
     * exposes to its clients
     * @return List of exposed services
     */
    const std::vector<RuleObjectDescription*>& getExposedServices() const;

    /**
     * The {@link RuleObjectDescription} objects that the Connector App
     * supports for being remoted
     * @return List of remoted interfaces
     */
    const std::vector<RuleObjectDescription*>& getRemotedServices() const;

    /**
     * release allocations and empty object. must be called before deletion of object.
     * @return {@link QStatus}
     */
    QStatus release();

  private:
    /**
     * The interfaces that the Connector App exposes to its clients
     */
    std::vector<RuleObjectDescription*> m_ExposedServices;

    /**
     * The interfaces that the Connector App allows to remote
     */
    std::vector<RuleObjectDescription*> m_RemotedServices;

    void emptyVectors();

};
}
}
#endif /* defined(ConnectorCapabilities_H) */
