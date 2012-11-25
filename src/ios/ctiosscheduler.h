#import <UIKit/UIKit.h>

@interface CtIOSScheduler : NSObject
{
@private
    BOOL running;
    id displayLink;
    NSInteger frameInterval;
}

+ (CtIOSScheduler *) instance;

- (void) start;
- (void) stop;
- (void) update:(id)sender;
- (void) setFrameInterval:(NSInteger)value;

@end
