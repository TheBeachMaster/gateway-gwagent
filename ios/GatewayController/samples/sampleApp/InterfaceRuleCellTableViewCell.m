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

#import "InterfaceRuleCellTableViewCell.h"

@interface InterfaceRuleCellTableViewCell()
@property (strong,nonatomic) VisualInterfaceInfo *interfaceInfo;

@end

@implementation InterfaceRuleCellTableViewCell

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
}
- (IBAction)configureButtonTouchUpInside:(id)sender {
   
    [self.rules switchInterfaceConfiguration:_interfaceInfo];
    
    [self updateCell];
}

-(void)setIndexPath:(NSIndexPath  *)indexPath
{
    _indexPath = indexPath;
    
    _interfaceInfo = [self.rules.accessRulesDictionary allKeys][[indexPath row]];
    
    self.ruleNameLbl.text = [_interfaceInfo.interface friendlyName];
    
   [self updateCell];
}

-(void)updateCell
{
    if (_interfaceInfo.configured) {
        [self.configureButton setTitle:@"Added" forState:UIControlStateNormal];
    } else {
        [self.configureButton setTitle:@"Add" forState:UIControlStateNormal];
    }
    
    self.configureButton.enabled = _interfaceInfo.enabled;
}

@end
