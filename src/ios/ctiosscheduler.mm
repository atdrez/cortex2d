#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "ctiosscheduler.h"
#import "ctapplication_ios_p.h"

CtIOSScheduler *ct_ios_scheduler = nil;

@implementation CtIOSScheduler

- (id) init
{
    if (self = [super init]) {
        running = FALSE;
        displayLink = nil;
        frameInterval = 1;
    }
    return self;
}

- (void) update:(id)sender
{
    ctuint dt = 16; // XXX: get interval

    if (CtApplicationIOSPrivate::instance())
        CtApplicationIOSPrivate::instance()->tick(dt);
}

- (void) start
{
    if (running)
        return;

    displayLink = [NSClassFromString(@"CADisplayLink")
                      displayLinkWithTarget:self
                      selector:@selector(update:)];
    [displayLink setFrameInterval:frameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

    running = TRUE;
}

- (void) stop
{
    if (!running)
        return;

    [displayLink invalidate];
    displayLink = nil;

    running = FALSE;
}

- (void) setFrameInterval:(NSInteger)value
{
    if (value <= 0 || value == frameInterval)
        return;

    frameInterval = value;

    if (running) {
        [self stop];
        [self start];
    }
}

+ (CtIOSScheduler *) instance
{
    @synchronized([CtIOSScheduler class]) {
        if (!ct_ios_scheduler)
            ct_ios_scheduler = [[CtIOSScheduler alloc] init];
    }
    return ct_ios_scheduler;
}

@end
