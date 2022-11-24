//
//  ViewController.h
//  AutoFan
//
//  Created by Shawn Zhang on 11/24/22.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIView *autoDefaultView;
@property (weak, nonatomic) IBOutlet UIView *autoManualView;
@property (weak, nonatomic) IBOutlet UISegmentedControl *segControl;

@end

