#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "ctioswindow.h"
#import "ctiosglview.h"
#import "ctiosviewcontroller.h"

@implementation CtIOSWindow

- (id) initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {
        glView = [[CtIOSGLView alloc] initWithFrame: [self bounds]];

        viewController = [[CtIOSViewController alloc] initWithNibName:nil bundle:nil];
        viewController.wantsFullScreenLayout = YES;
        viewController.view = glView;

        self.rootViewController = viewController;

        [self addSubview: glView];
        [self makeKeyAndVisible];
    }

    return self;
}

- (void) dealloc
{
    [glView release];
    [viewController release];

    [super dealloc];
}

- (void) makeCurrent
{
    [glView makeCurrent];
}

- (void) prepareGL
{
    [glView prepareGL];
}

- (void) presentGL
{
    [glView presentGL];
}

@end
