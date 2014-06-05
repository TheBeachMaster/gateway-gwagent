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

#import "AJGWCAnnouncementData.h"

@interface AJGWCAnnouncementData ()

@property (nonatomic) uint16_t port;
@property (nonatomic) NSString* busName;
@property (strong, nonatomic) NSDictionary* objectDescriptions;
@property (strong, nonatomic) NSDictionary* aboutData;

@end

@implementation AJGWCAnnouncementData

- (id)initWithPort:(uint16_t) port busName:(NSString *)busName aboutData:(NSDictionary *)aboutData objectDescriptions:(NSDictionary *) objectDescs
{
    self = [super init];
    if (self) {
        self.port = port;
        self.busName = busName;
        self.objectDescriptions = [[NSDictionary alloc] initWithDictionary:objectDescs];
        self.aboutData = [[NSDictionary alloc] initWithDictionary:aboutData];
    }
    return self;
}

- (NSDictionary *)objectDescriptions
{
    return _objectDescriptions;
}

- (NSDictionary *)aboutData
{
    return _aboutData;
}

- (uint16_t)port
{
    return _port;
}

- (NSString*)busName
{
    return _busName;
}
@end
