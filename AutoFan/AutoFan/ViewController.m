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

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}


- (IBAction)autoDefaultChange:(id)sender {
    if (self.segControl.selectedSegmentIndex == 1)
        self.autoDefaultView.hidden = true;
    else
        self.autoDefaultView.hidden = false;
}


@end
