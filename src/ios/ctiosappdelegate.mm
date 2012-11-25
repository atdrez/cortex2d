#import "ctiosappdelegate.h"
#include "ctevents.h"
#include "ctiosscheduler.h"
#include "ctapplication.h"

@implementation MainAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // change working dir to resource path
    [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];

    CtApplication *app = CtApplication::instance();
    CT_ASSERT(!app, "No application initialized");

    CtEvent ev(CtEvent::ApplicationReady);
    CtObject::sendEvent(app, &ev);

    // start scheduler
    [[CtIOSScheduler instance] start];

    return YES;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.

    [[CtIOSScheduler instance] stop];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    [[CtIOSScheduler instance] stop];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    [[CtIOSScheduler instance] start];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate.
    // Save data if appropriate. See also applicationDidEnterBackground:.

    CtApplication *app = CtApplication::instance();
    CT_ASSERT(!app, "No application initialized");

    CtEvent ev(CtEvent::ApplicationRelease);
    CtObject::sendEvent(app, &ev);
}

@end
