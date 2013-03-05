/*
 Abstract: The EAGLView class is a UIView subclass that renders OpenGL scene.
 If the current hardware supports OpenGL ES 2.0, it draws using OpenGL ES 2.0;
 otherwise it draws using OpenGL ES 1.1.

 Version: 1.0

 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
 ("Apple") in consideration of your agreement to the following terms, and your
 use, installation, modification or redistribution of this Apple software
 constitutes acceptance of these terms.  If you do not agree with these terms,
 please do not use, install, modify or redistribute this Apple software.

 In consideration of your agreement to abide by the following terms, and subject
 to these terms, Apple grants you a personal, non-exclusive license, under
 Apple's copyrights in this original Apple software (the "Apple Software"), to
 use, reproduce, modify and redistribute the Apple Software, with or without
 modifications, in source and/or binary forms; provided that if you redistribute
 the Apple Software in its entirety and without modifications, you must retain
 this notice and the following text and disclaimers in all such redistributions
 of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may be used
 to endorse or promote products derived from the Apple Software without specific
 prior written permission from Apple.  Except as expressly stated in this notice,
 no other rights or licenses, express or implied, are granted by Apple herein,
 including but not limited to any patent rights that may be infringed by your
 derivative works or by other works in which the Apple Software may be
 incorporated.

 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
 WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
 COMBINATION WITH YOUR PRODUCTS.

 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
 DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
 CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
 APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Copyright (C) 2009 Apple Inc. All Rights Reserved.
*/

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include "ctevents.h"
#import "ctiosglview.h"
#import "ctwindow_ios_p.h"
#import "ctapplication_p.h"


@implementation CtIOSGLView

struct WindowWrapper {
    CtTouchPointList touches;
};

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{
    if (self = [super initWithCoder:coder]) {
        [self setupRenderer];
    }
    return self;
}

- (id) initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame]) {
        [self setupRenderer];
    }
    return self;

}

- (void) setupRenderer
{
    CAEAGLLayer *elayer = (CAEAGLLayer *)self.layer;
    elayer.opaque = TRUE;
    elayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                                  [NSNumber numberWithBool:FALSE],
                                              kEAGLDrawablePropertyRetainedBacking,
                                              kEAGLColorFormatRGBA8,
                                              kEAGLDrawablePropertyColorFormat,
                                              nil];

    self->dw = new WindowWrapper();
    self.multipleTouchEnabled = TRUE;

    // create GLES2 context
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
    if (context && [EAGLContext setCurrentContext:context]) {
        glGenFramebuffers(1, &defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    }
}

- (void) layoutSubviews
{
    [self resizeFromLayer:(CAEAGLLayer*)self.layer];

    // XXX: should update this window immediately
    //if (CtApplicationIOSPrivate::current())
    //    CtApplicationIOSPrivate::current()->tick(dt);
}

- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer
{
    // Allocate color buffer backing based on the current layer size
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return NO;
    }

    return YES;
}

- (void) makeCurrent
{
    [EAGLContext setCurrentContext:context];
}

- (void) prepareGL
{
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(0x8642); // Enable pointSize

    glClearColor(0.5f, 0.4f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

- (void) presentGL
{
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void) dealloc
{
    // tear down GL
    if (defaultFramebuffer) {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer) {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    // tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];

    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouches:touches eventType:1];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouches:touches eventType:2];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouches:touches eventType:3];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    CT_WARNING("TODO: touchesCancelled");
}

- (void) handleTouches:(NSSet *)touches eventType:(int)etype
{
    CtTouchPointList points;

    for (UITouch *touch in touches) {
        CGPoint p = [touch locationInView:self];
        CGPoint lp = [touch previousLocationInView:self];
        int x = p.x * self.contentScaleFactor;
        int y = p.y * self.contentScaleFactor;

        int tid = 0;
        int fx = x;
        int fy = y;
        bool isDown = (touch.phase != UITouchPhaseEnded);

        CtTouchPoint point(tid, x, y, fx, fy, 1, isDown);
        points.push_back(point);

        // XXX: handle multitouch (broken)
        //dw->touches.push_back();
    }

    switch (etype) {
    case 1: {
        touchID++;
        CtTouchEvent ev(CtEvent::TouchBegin, touchID, points);
        CtApplicationPrivate::postEvent(dd, &ev);
        break;
    }
    case 2: {
        CtTouchEvent ev(CtEvent::TouchUpdate, touchID, points);
        CtApplicationPrivate::postEvent(dd, &ev);
        break;
    }
    case 3: {
        CtTouchEvent ev(CtEvent::TouchEnd, touchID, points);
        CtApplicationPrivate::postEvent(dd, &ev);
        break;
    }
    default:
        break;
    }
}

@end
