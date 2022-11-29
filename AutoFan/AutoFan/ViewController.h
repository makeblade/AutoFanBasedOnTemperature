//
//  ViewController.h
//  AutoFan
//
//  Created by Shawn Zhang on 11/24/22.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController {
    int current_level;
    int list_size;
    NSTimer * timer;
}

@property (weak, nonatomic) IBOutlet UILabel *temperatureLabel;
@property (weak, nonatomic) IBOutlet UILabel *RPMLabel;

@property (weak, nonatomic) IBOutlet UIView *autoDefaultView;
@property (weak, nonatomic) IBOutlet UIView *autoManualView;
@property (weak, nonatomic) IBOutlet UIView *manualView;
@property (weak, nonatomic) IBOutlet UISegmentedControl *segControl;
@property (weak, nonatomic) IBOutlet UIView *manualLevel;

- (void) manualLevelUpdate:(int)level;
- (void) alert:(NSString*)str;
- (NSString *) getDataFrom:(NSString *)url;
- (void) updateTempRPM:(NSTimer *)theTimer;
- (NSString *) formatResponse:(NSString *)response;
- (NSString *) sendDataTo:(NSString *)url withBody:(NSData *)body;

@property (weak, nonatomic) IBOutlet UITextField *AutoManualTempFrom;
@property (weak, nonatomic) IBOutlet UITextField *AutoManualTempTo;
@property (weak, nonatomic) IBOutlet UITextField *AutoManualRPM;
@property (weak, nonatomic) IBOutlet UIView *AutoManualList;

@property (nonatomic, retain) NSTimer * timer;

@end

