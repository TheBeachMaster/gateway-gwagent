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

#import <UIKit/UIKit.h>
#import "AJNSessionOptions.h"
#import "alljoyn/gateway/AJGWCGatewayCtrlAcl.h"


@interface ConnectorAppInfoAclsTableViewCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *aclNameLbl;
@property (weak, nonatomic) IBOutlet UISwitch *aclActiveSwitch;

@property (weak, nonatomic) AJGWCGatewayCtrlAcl* aclObject;
@property (nonatomic) AJNSessionId sessionId;

- (IBAction)aclActiveSwitchValueChanged:(id)sender;

@end
