#import <UIKit/UIKit.h>

@class CtIOSGLView;
@class CtIOSViewController;

@interface CtIOSWindow : UIWindow
{
@public
    CtIOSGLView *glView;
    UIViewController *viewController;
}

- (void) makeCurrent;
- (void) prepareGL;
- (void) presentGL;

@end
