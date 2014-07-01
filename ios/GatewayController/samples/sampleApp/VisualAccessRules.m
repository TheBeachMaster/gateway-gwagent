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


#import "VisualAccessRules.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlTPInterface.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlTPObjectPath.h"


@implementation VisualInterfaceInfo

-(id) init:(AJGWCGatewayCtrlTPInterface *)interface isConfigured:(BOOL)configured enabled:(BOOL)enabled
{
    self = [super init];
    
    _interface = interface;

    _configured = configured;
    
    _enabled = enabled;
    
    return self;
}

- (id)copyWithZone:(NSZone *)zone {
    VisualInterfaceInfo *objectCopy = [[VisualInterfaceInfo allocWithZone:zone] init:self.interface isConfigured:self.configured enabled:self.enabled];
    
    return objectCopy;
}

- (NSUInteger)hash {
    return [self.interface hash];
}

- (BOOL)isEqual:(id)anObject {
    if (![anObject isKindOfClass:[VisualInterfaceInfo class]]) return NO;
    VisualInterfaceInfo *otherTPInterface = (VisualInterfaceInfo *)anObject;

    return [otherTPInterface.interface isEqual:self.interface];
}
@end

@implementation VisualObjPathInfo
-(id) init:(AJGWCGatewayCtrlTPObjectPath *)objectPath isConfigured:(BOOL)configured enabled:(BOOL)enabled
{
    self = [super init];
    
    _objectPath = objectPath;
    
    _configured = configured;
    
    _enabled = enabled;
    
    return self;
}

- (id)copyWithZone:(NSZone *)zone {
    VisualObjPathInfo *objectCopy = [[VisualObjPathInfo allocWithZone:zone] init:self.objectPath isConfigured:self.configured enabled:self.enabled];

    
    return objectCopy;
}

- (NSUInteger)hash {
    return [self.objectPath hash];
}

- (BOOL)isEqual:(id)anObject {
    if (![anObject isKindOfClass:[VisualObjPathInfo class]]) return NO;
    VisualObjPathInfo *otherObjPathInfo = (VisualObjPathInfo *)anObject;
    
    return [otherObjPathInfo.objectPath isEqual:self.objectPath];
}


@end

@interface VisualAccessRules()
@end

@implementation VisualAccessRules

-(id)initWithArrayOfManifestObjectDescription:(NSArray *)ArrayOfManifestObjectDescription
{
    self = [super init];
    
    self.accessRulesDictionary = [[NSMutableDictionary alloc]init];

    // Gather the object paths for each interface
    for (AJGWCGatewayCtrlManifestObjectDescription *objectDescription in ArrayOfManifestObjectDescription) {
        NSSet *interfaces = [objectDescription interfaces];
        
        VisualObjPathInfo *objPathInfo = [[VisualObjPathInfo alloc] init:[objectDescription objectPath] isConfigured:[objectDescription isConfigured] enabled:YES] ;
        
        for (AJGWCGatewayCtrlTPInterface *interface in interfaces) {
            
            VisualInterfaceInfo *interfaceInfo = [[VisualInterfaceInfo alloc]init:interface isConfigured:NO enabled:YES]; // we will populate the configured interfaces later in this function

            // check if this interface already has object paths connected to it
            NSMutableArray *array = (NSMutableArray *)[self objectPathsForInterface:interfaceInfo];

            // if not, allocate
            if ([array count] == 0) {
                array = [[NSMutableArray alloc] init];
            }

            // Gather the object paths for this interface into the array
            [array addObject:objPathInfo];

            // Sorting the object paths so the same entries will show at the same lines, only for aesthetics
            [array sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
                VisualObjPathInfo *objPathInfo1 = (VisualObjPathInfo *)obj1;
                VisualObjPathInfo *objPathInfo2 = (VisualObjPathInfo *)obj2;

                return [objPathInfo1.objectPath.path compare:objPathInfo2.objectPath.path];
            }];

            // Add the list of object paths to the interface. this is the important part.
            [self.accessRulesDictionary setObject:array forKey:interfaceInfo];
        }
    }
    
    // run through all of the interfaces and disable object paths that are all enabled under an interface. set that interface to be enabled
    for (VisualInterfaceInfo *interfaceInfo in [self.accessRulesDictionary allKeys]) {
        NSArray *array = [self objectPathsForInterface:interfaceInfo];
        
        BOOL allPathsAreConfigured = YES;
        for (VisualObjPathInfo *objPathInfo in array) {
            allPathsAreConfigured &= [objPathInfo configured];
        }
        
        if (allPathsAreConfigured) {
            [interfaceInfo setConfigured:YES];
            NSArray *array = [self objectPathsForInterface:interfaceInfo];
            
            for (VisualObjPathInfo *objPathInfo in array) {
                [objPathInfo setEnabled:NO];
            }
            
        }
    }
    
    return self;
}

- (NSArray *)interfaces
{
    return [self.accessRulesDictionary allKeys];
}


- (NSArray *)objectPathsForInterface:(VisualInterfaceInfo *)interface
{
    return (NSArray *)[self.accessRulesDictionary objectForKey:interface];
}

-(void) switchAllAccessRules
{
    self.configured=!self.configured;
    
    if (self.configured==YES) { //configure all elements
        for (VisualInterfaceInfo *interfaceInfo in [self.accessRulesDictionary allKeys]) {
            NSArray *array = [self objectPathsForInterface:interfaceInfo];
            interfaceInfo.configured = YES;
            
            for (VisualObjPathInfo *objPathInfo in array) {
                objPathInfo.configured = YES;
            }
        }
    }
    
    [self changeAllAccessRulesElements:!self.configured]; //this should be the opposite of the configured state at the top level. if all access rules should be configured in gui then all of the elements should be disabled, and vice versa.
    
}


-(void) changeAllAccessRulesElements:(BOOL)enable
{
    for (VisualInterfaceInfo *interfaceInfo in [self.accessRulesDictionary allKeys]) {
        NSArray *array = [self objectPathsForInterface:interfaceInfo];
        interfaceInfo.enabled=enable;
        
        if ((!interfaceInfo.configured && (enable == YES)) || (enable == NO)) {
            for (VisualObjPathInfo *objPathInfo in array) {
                objPathInfo.enabled=enable;
            }
        }
    }
}

-(void) switchInterfaceConfiguration:(VisualInterfaceInfo *)interfaceInfo
{
    NSArray *array = [self objectPathsForInterface:interfaceInfo];
    interfaceInfo.configured=!interfaceInfo.configured;
    
    for (VisualObjPathInfo *objPathInfo in array) {
        objPathInfo.enabled=!interfaceInfo.configured;
    }
}

-(void) switchObjectPathConfiguration:(VisualObjPathInfo *)objectPathInfo
{
    objectPathInfo.configured = !objectPathInfo.configured;
}

-(void) switchObjectPathAllowSubObjects:(VisualObjPathInfo *)objectPathInfo
{
    [objectPathInfo.objectPath setPrefix:![objectPathInfo.objectPath isPrefix]];
}

-(NSArray *) createAJGWCGatewayCtrlManifestObjectDescriptions
{
    NSMutableArray *objectDescriptions = [[NSMutableArray alloc]init];
    
    NSMutableDictionary *objectPathToInterfaces = [[NSMutableDictionary alloc]init]; //key - VisualObjPathInfo, value - array of VisualInterfaceInfo
    
    for (VisualInterfaceInfo *interfaceInfo in [self.accessRulesDictionary allKeys]) {
        NSArray *array = [self objectPathsForInterface:interfaceInfo];
        
            for (VisualObjPathInfo *objPathInfo in array) {
                if (objPathInfo.configured == YES || interfaceInfo.configured == YES || self.configured == YES) {
                    NSMutableArray *arrayOfInterfaces = [objectPathToInterfaces objectForKey:objPathInfo];
                    
                    if (arrayOfInterfaces==nil) {
                        arrayOfInterfaces = [[NSMutableArray alloc]init];
                    }
                    
                    [arrayOfInterfaces addObject:interfaceInfo];
                    
                    [objectPathToInterfaces setObject:arrayOfInterfaces forKey:objPathInfo];
                }
            }
    }
    
    for (VisualObjPathInfo *objPathInfo in [objectPathToInterfaces allKeys]) {
        NSArray *interfacesForObjPath = [objectPathToInterfaces objectForKey:objPathInfo];

        NSMutableSet *arrayOfInterfaces = [[NSMutableSet alloc]init];
        for (VisualInterfaceInfo *interfaceInfo in interfacesForObjPath) {
            [arrayOfInterfaces addObject:interfaceInfo.interface];
        }

        AJGWCGatewayCtrlManifestObjectDescription *objDesc = [[AJGWCGatewayCtrlManifestObjectDescription alloc]
                                                              initWithObjectPath:objPathInfo.objectPath interfaces:arrayOfInterfaces isConfigured:YES];
        
        
        [objectDescriptions addObject:objDesc];
        
    }
    
    return objectDescriptions;
    
}

@end
