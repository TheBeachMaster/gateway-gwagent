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
#import "alljoyn/gateway/AJGWCRuleObjectDescription.h"

@interface VisualInterfaceInfo : NSObject <NSCopying>
@property (strong,nonatomic) AJGWCRuleInterface *interface;
@property (nonatomic) BOOL configured;              // is this interface configured to be included in the ACL - if YES, all object paths under it are included
@property (nonatomic) BOOL enabled;                 // can this interface be configured or was a parent configured for it?
@end

@interface VisualObjPathInfo : NSObject <NSCopying>
@property (strong,nonatomic) AJGWCRuleObjectPath *objectPath;
@property (nonatomic) BOOL enabled;                 // can this objPath be configured
@property (nonatomic) BOOL configured;              // is this path configured to be included in the ACL
@end

@interface VisualAclRules : NSObject
@property (nonatomic) BOOL configured;              // are these access rules configured to be included in the ACL
@property (strong,nonatomic) NSMutableDictionary *aclRulesDictionary; // key - VisualInterfaceInfo, value - array of VisualObjPathInfo

-(id)initWithArrayOfRuleObjectDescription:(NSArray *)arrayOfRuleObjectDescription;

// return array of AJGWCRuleInterface
- (NSArray *)interfaces;

// return array of AJGWCRuleObjectPath
- (NSArray *)objectPathsForInterface:(VisualInterfaceInfo *)interface;

-(void) switchAllAclRules;

-(void) switchInterfaceConfiguration:(VisualInterfaceInfo *)interfaceInfo;

-(void) switchObjectPathConfiguration:(VisualObjPathInfo *)objectPathInfo;

-(void) switchObjectPathAllowSubObjects:(VisualObjPathInfo *)objectPathInfo;

-(NSArray *) createAJGWCRuleObjectDescriptions;

@end

























