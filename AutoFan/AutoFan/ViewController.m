//
//  ViewController.m
//  AutoFan
//
//  Created by Shawn Zhang on 11/24/22.
//

#import "ViewController.h"

@interface ViewController ()
@end

@implementation ViewController

@synthesize AutoManualTempTo;
@synthesize AutoManualTempFrom;
@synthesize AutoManualRPM;
@synthesize AutoManualList;

- (void) manualLevelUpdate:(int)level {
    self.manualLevel.subviews[current_level].backgroundColor = UIColor.systemGray3Color;
    self.manualLevel.subviews[level].backgroundColor = UIColor.systemRedColor;
    current_level = level;
    list_size = 0;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.manualView.hidden = true;
    current_level = 0;
    [self manualLevelUpdate:current_level];
}

- (IBAction)autoDefaultChange:(id)sender {
    if (self.segControl.selectedSegmentIndex == 1)
        self.autoDefaultView.hidden = true;
    else
        self.autoDefaultView.hidden = false;
}

- (IBAction)manualOnClickFromAutoManual:(id)sender {
    self.manualView.hidden = false;
}

- (IBAction)manualOnClickFromAutoDefault:(id)sender {
    self.manualView.hidden = false;
}

- (IBAction)autoOnClickFromManual:(id)sender {
    self.manualView.hidden = true;
}

- (IBAction)manualLevel1OnSelect:(id)sender {
    [self manualLevelUpdate:0];
}

- (IBAction)manualLevel2OnSelect:(id)sender {
    [self manualLevelUpdate:1];
}

- (IBAction)manualLevel3OnSelect:(id)sender {
    [self manualLevelUpdate:2];
}

- (IBAction)manualLevel4OnSelect:(id)sender {
    [self manualLevelUpdate:3];
}

- (IBAction)manualLevel5OnSelect:(id)sender {
    [self manualLevelUpdate:4];
}

- (IBAction)AutoManualAddOnClick:(id)sender {
    if (list_size >= 8) {
        [self alert:@"Maximum manual setting reached, please clear before adding anymore setting."];
        return;
    }
    
    if ([AutoManualTempFrom.text isEqualToString:@""] || [AutoManualTempTo.text isEqualToString:@""] || [AutoManualRPM.text isEqualToString:@""]) {
        [self alert:@"Empty input"];
        return;
    }
    
    NSString *str = [NSString stringWithFormat:@"From %.01f to %.01f with RPM: %i", [AutoManualTempFrom.text floatValue], [AutoManualTempTo.text floatValue], [AutoManualRPM.text intValue]];
    
    UILabel *myLabel = [[UILabel alloc]initWithFrame:CGRectMake(10, (list_size++)*25, 280, 40)];
    [myLabel setBackgroundColor:[UIColor clearColor]];
    [myLabel setText:str];
    [[self AutoManualList] addSubview:myLabel];
}

- (IBAction)clearOnClick:(id)sender {
    list_size = 0;
    for (UIView *view in AutoManualList.subviews) {
        [view removeFromSuperview];
    }
}

- (void) alert:(NSString*)str {
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"Warning"
                               message:str
                               preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action) {}];

    [alert addAction:defaultAction];
    [self presentViewController:alert animated:YES completion:nil];
}


@end
