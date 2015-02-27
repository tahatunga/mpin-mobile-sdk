//
//  CycleProgressBar.m
//  progressTest
//
//  Created by Admin on 2/23/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "CycleProgressBar.h"

#define kBeginAngle		1.57
#define kEndAngle		7.85
#define kLablelWidth	30
#define kLabelHeight	30
#define kArcWidth		10

@implementation CycleProgressBar

@synthesize isLoading;

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        context = 0;
		ind = 1.0;
		initialRadian = kEndAngle;
        self.counterLabel = [[UILabel alloc] initWithFrame:CGRectMake(((frame.size.width - kLablelWidth)/2), ((frame.size.height - kLabelHeight)/2), kLablelWidth, kLabelHeight)];
        [self.counterLabel setTextAlignment:NSTextAlignmentCenter];
		[self addSubview:self.counterLabel];
		self.backgroundColor = [UIColor whiteColor];
		loadingIndicator  = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		[self addSubview:loadingIndicator];
		[self bringSubviewToFront:loadingIndicator];
		loadingIndicator.hidesWhenStopped = YES;
		CGRect frame = loadingIndicator.frame;
		frame.origin.x += 6.0;
		frame.origin.y += 6.0;
		loadingIndicator.frame = frame;
        isLoading = NO;
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
	if(context == 0) {
		context = UIGraphicsGetCurrentContext();
	}
	
	CGContextSetRGBStrokeColor(context, 0.52, 0.53, 0.67, 1.0);
	CGContextSetLineWidth(context, kArcWidth);
	CGContextAddArc(context, (rect.size.width/2) ,(rect.size.height/2), ((rect.size.height/2) - (kArcWidth/2) ), - kBeginAngle, - initialRadian, FALSE);
 	CGContextStrokePath(context);

	initialRadian = initialRadian - 0.01;
	if(initialRadian < kBeginAngle) {
			[self stopAnimation];
            [self sendActionsForControlEvents:UIControlEventTouchUpInside];
    }
}

- (void) onUpdateCounter {
    self.counterLabel.text =  [NSString stringWithFormat:@"%d", --counter];
}

- (void) setDuration:(NSTimeInterval) duration {
	durInLine = duration/((initialRadian - kBeginAngle)*100);
    counter = duration;
    self.counterLabel.text = [NSString stringWithFormat:@"%d",counter];
}

- (void) startAnimation:(NSTimeInterval) withDuration {
	[self setDuration:withDuration];
	[self startAnimation];
}

- (void) startAnimation {
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)(durInLine) target:self selector:@selector(setNeedsDisplay) userInfo:nil repeats:TRUE];
    counterTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)(1.0) target:self selector:@selector(onUpdateCounter) userInfo:nil repeats:TRUE];
}

- (void) stopAnimation {
	if(animationTimer != nil) {
        [animationTimer invalidate];
        animationTimer  = nil;
    }
    if(counterTimer != nil) {
        [counterTimer invalidate];
        counterTimer  = nil;
    }
}

@end
