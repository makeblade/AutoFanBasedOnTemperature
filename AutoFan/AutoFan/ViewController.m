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
@synthesize temperatureLabel;
@synthesize RPMLabel;
@synthesize timer;

- (void) manualLevelUpdate:(int)level {
    self.manualLevel.subviews[current_level].backgroundColor = UIColor.systemGray3Color;
    self.manualLevel.subviews[level].backgroundColor = UIColor.systemRedColor;
    current_level = level;
    list_size = 0;
    timer = [NSTimer scheduledTimerWithTimeInterval: 1 target:self selector:@selector(updateTempRPM:) userInfo:nil repeats: YES];
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

- (IBAction)connectTest:(id)sender {
    NSLog(@"Sending data...");
    
    NSString* str = @"teststring";
    NSData* data = [str dataUsingEncoding:NSUTF8StringEncoding];
    
    NSString* result = [self sendDataTo:@"http://192.168.1.24" withBody:data];
    
    NSLog(@"%@", result);
}

- (void)updateTempRPM:(NSTimer*)theTimer{
    NSLog(@"Retrieving data...");
    NSArray* content = [[self getDataFrom:@"http://192.168.1.24"] componentsSeparatedByString: @"\n"];
        
    temperatureLabel.text = [NSString stringWithFormat:@" %@ °F / %@ °C", [self formatResponse:content[2]], [self formatResponse:content[3]]];
    RPMLabel.text = [NSString stringWithFormat:@" %@", content[4]];
}

- (NSString *) getDataFrom:(NSString *)url{
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    [request setHTTPMethod:@"GET"];
    [request setURL:[NSURL URLWithString:url]];

    NSError *error = nil;
    NSHTTPURLResponse *responseCode = nil;

    NSData *oResponseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&responseCode error:&error];

    if([responseCode statusCode] != 200){
        NSLog(@"Error getting %@, HTTP status code %i", url, (int)[responseCode statusCode]);
        return nil;
    }

    return [[NSString alloc] initWithData:oResponseData encoding:NSUTF8StringEncoding];
}

- (NSString *) sendDataTo:(NSString *)url withBody:(NSData *)body {
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    [request setHTTPMethod:@"POST"];
    [request setHTTPBody:body];
    [request setValue:[NSString stringWithFormat:@"%lu", [body length]] forHTTPHeaderField:@"Content-Length"];
    [request setURL:[NSURL URLWithString:url]];
    
    NSError *error = nil;
    NSHTTPURLResponse *responseCode = nil;

    NSData *oResponseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&responseCode error:&error];

    if([responseCode statusCode] != 200){
        NSLog(@"Error getting %@, HTTP status code %i", url, (int)[responseCode statusCode]);
        return nil;
    }

    return [[NSString alloc] initWithData:oResponseData encoding:NSUTF8StringEncoding];
}

- (NSString *) formatResponse:(NSString *)response {
    if ([response length] > 0) {
        response = [response substringToIndex:[response length] - 1];
    } else {
        //no characters to delete... attempting to do so will result in a crash
    }
    
    return response;

}




@end
