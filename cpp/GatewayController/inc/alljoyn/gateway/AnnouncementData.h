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

#ifndef _ANNOUNCEMENTDATA_H_
#define _ANNOUNCEMENTDATA_H_

#include <vector>
#include <alljoyn/Status.h>
#include <alljoyn/about/AboutClient.h>

namespace ajn {
namespace gwcontroller {
/**
 * Announcement data - information coming from the announcement
 * The application should create and maintain a vector of AnnouncementData objects based on the current set of announcement in the network. See sample application for more.
 */
class AnnouncementData {
  public:
    /**
     * AnnouncementData
     * @param portNumber Announcemt port number
     * @param aboutData As was received from the announcement handler
     * @param objectDescriptions As was received from the announcement handler
     */

    AnnouncementData(uint16_t portNumber, const ajn::services::AboutClient::AboutData& aboutData, const ajn::services::AboutClient::ObjectDescriptions& objectDescriptions);

    /**
     * GetObjectDescriptions
     * @return objectDescriptions As was received from the announcement handler
     */

    const ajn::services::AboutClient::ObjectDescriptions& GetObjectDescriptions() const { return m_ObjectDescriptions; }

    /**
     * GetAboutData
     * @return aboutData As was received from the announcement handler
     */

    const ajn::services::AboutClient::AboutData& GetAboutData() const { return m_AboutData; }

    const short getPortNumber() const { return m_PortNumber; }

  private:
    ajn::services::AboutClient::AboutData m_AboutData;
    ajn::services::AboutClient::ObjectDescriptions m_ObjectDescriptions;
    short m_PortNumber;


};
}
}



#endif /* defined(_ANNOUNCEMENTDATA_H_) */
