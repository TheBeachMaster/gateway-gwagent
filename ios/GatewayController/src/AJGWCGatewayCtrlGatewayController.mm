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

#import "AJGWCGatewayCtrlGatewayController.h"
#include <map>
#import "alljoyn/gateway/GatewayCtrlGatewayController.h"
#import "alljoyn/gateway/GatewayCtrlGateway.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "alljoyn/about/AJNAboutDataConverter.h"
#import "AJGWCGatewayCtrlGateway.h"

@interface AJGWCGatewayCtrlGatewayController ()

@end

@implementation AJGWCGatewayCtrlGatewayController

+ (id)sharedInstance
{
    static AJGWCGatewayCtrlGatewayController *gwController;
    static dispatch_once_t donce;
    dispatch_once(&donce, ^{
        gwController = [[self alloc] init];
    });
    return gwController;
}

- (id)init
{
    if (self = [super init]) {
    }
    return self;
}

+ (void)startWithBus:(AJNBusAttachment *) bus
{
    ajn::services::GatewayCtrlGatewayController::getInstance()->init((ajn::BusAttachment *)bus.handle);
}

- (void)shutdown
{
    ajn::services::GatewayCtrlGatewayController::getInstance()->shutdown();
}

- (AJNBusAttachment*)busAttachment
{
    AJNBusAttachment* bus = [[AJNBusAttachment alloc] initWithHandle:ajn::services::GatewayCtrlGatewayController::getInstance()->getBusAttachment()];
    return bus;
}

- (AJGWCGatewayCtrlGateway*)createGatewayWithBusName:(NSString*) gatewayBusName objectDescs:(NSDictionary *) objectDescs  aboutData:(NSDictionary *) aboutData
{
    ajn::services::AboutClient::ObjectDescriptions objectDescsMap;
    ajn::services::AboutClient::AboutData aboutDataMap;

    // Iterate over the NSDictionary and convert to std::map<qcc::String, std::vector<qcc::String> > (ObjectDescriptions)
    for (NSString *key in objectDescs.allKeys)
    {
        std::vector <qcc::String>* tVect = new std::vector <qcc::String>;
        for (NSString *tStr in [objectDescs objectForKey:key]) {
            tVect->push_back([AJNConvertUtil convertNSStringToQCCString:tStr]);
        }
        objectDescsMap.insert(std::make_pair([AJNConvertUtil convertNSStringToConstChar:key], *tVect));
    }

    // Iterate over the NSDictionary and convert to std::map<qcc::String, ajn::MsgArg> (AboutData)
    for (NSString *key in aboutData.allKeys) {
        // Put key/ value in the std::map<qcc::String, ajn::MsgArg>
        AJNMessageArgument *ajnMsgArg = [aboutData objectForKey :key];

        ajn::MsgArg *cppValue = (ajn::MsgArg *)ajnMsgArg.handle;
        aboutDataMap.insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], *cppValue));
    }

    //Convert from NSDictionary * to ObjectDescriptions
    return [[AJGWCGatewayCtrlGateway alloc] initWithHandle: ajn::services::GatewayCtrlGatewayController::getInstance()->createGateway([AJNConvertUtil convertNSStringToConstChar:gatewayBusName], objectDescsMap, aboutDataMap) ];
}

- (AJGWCGatewayCtrlGateway*)gateway:(NSString *) gatewayBusName
{
    return [[AJGWCGatewayCtrlGateway alloc] initWithHandle: ajn::services::GatewayCtrlGatewayController::getInstance()->getGateway([AJNConvertUtil convertNSStringToQCCString:gatewayBusName])];
}

- (QStatus)deleteGateway:(NSString *) gatewayBusName
{
    return ajn::services::GatewayCtrlGatewayController::getInstance()->deleteGateway([AJNConvertUtil convertNSStringToQCCString:gatewayBusName]);
}

- (QStatus)deleteAllGateways
{
    return ajn::services::GatewayCtrlGatewayController::getInstance()->deleteAllGateways();
}

- (NSDictionary*)gateways
{
    NSMutableDictionary* gwsDict = [[NSMutableDictionary alloc] init];
    std::map<qcc::String, ajn::services::GatewayCtrlGateway*> gwMap= ajn::services::GatewayCtrlGatewayController::getInstance()->getGateways();
    // Populate NSMutableDictionary with std::map data
    for (std::map <qcc::String, ajn::services::GatewayCtrlGateway*>::iterator it = gwMap.begin(); it != gwMap.end(); ++it) {
        [gwsDict setValue:[[AJGWCGatewayCtrlGateway alloc] initWithHandle:it->second ] forKey:[AJNConvertUtil convertQCCStringtoNSString:it->first]];
    }

    return gwsDict;
}

@end
