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

#import <Foundation/Foundation.h>
/**
 * AJGWCAnnouncementData - information coming from the announcement
 * The application should create and maintain an array of AJGWCAnnouncementData objects based on the current set of
 * announcement in the network. See sample application for more.
 */
@interface AJGWCAnnouncementData : NSObject

/**
 * Constructor
 * @param port The port used by the service
 * @param busName Bus name for this announcement
 * @param aboutData The data sent with the Announcement
 * @param objectDescs The objectDescriptions received in announce
 */
- (id)initWithPort:(uint16_t) port  busName:(NSString *)busName aboutData:(NSDictionary *)aboutData objectDescriptions:(NSDictionary *)objectDescs;

/**
 * objectDescriptions getter
 * @return objectDescs The objectDescriptions received in announce
 */
- (NSDictionary *)objectDescriptions;

/**
 * aboutData getter
 * @return aboutData The data sent with the Announcement
 */
- (NSDictionary *)aboutData;

/**
 * port getter
 * @return The port used by the service
 */
- (uint16_t)port;

/**
 * busName getter
 * @return The bus name used by the service
 */
- (NSString*)busName;

@end
