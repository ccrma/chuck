/*
 *  chuck_ui.mm
 *  chuck
 *
 *  Created by Spencer Salazar on 10/17/11.
 *  Copyright 2011 Spencer Salazar. All rights reserved.
 *
 */

#include "chuck_ui.h"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import "chuck_vm.h"
#import "chuck_globals.h"


@class Chuck_UI_Controller;


static Chuck_UI_Manager * g_ui_manager = NULL;
static Chuck_UI_Controller * g_ui_controller = nil;


@interface Chuck_UI_Controller : NSObject<NSApplicationDelegate>
{
@private
}

- (void)quit;

@end



Chuck_UI_Manager * Chuck_UI_Manager::instance()
{
    if(g_ui_manager == NULL)
        g_ui_manager = new Chuck_UI_Manager;
    return g_ui_manager;
}

Chuck_UI_Manager::Chuck_UI_Manager()
{
    g_ui_controller = [Chuck_UI_Controller new];
}

void Chuck_UI_Manager::go()
{
    NSAutoreleasePool * arpool = [NSAutoreleasePool new];
    
    NSAutoreleasePool * temp_pool = [NSAutoreleasePool new];
    
    NSApplication * app = [NSApplication sharedApplication];
    
    [app setDelegate:g_ui_controller];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    NSMenu * mainMenu = [[[NSMenu alloc] initWithTitle:@"main menu"] autorelease];
    NSMenu * chuckMenu = [[[NSMenu alloc] initWithTitle:@"chuck"] autorelease];
    
    NSMenuItem * menuItem;
    
    menuItem = [[[NSMenuItem new] initWithTitle:@"chuck" 
                                          action:NULL
                                   keyEquivalent:@""]
                autorelease];
    [menuItem setSubmenu:chuckMenu];
    [mainMenu addItem:menuItem];
    
    menuItem = [[[NSMenuItem new] initWithTitle:@"About chuck" 
                                         action:NULL
                                  keyEquivalent:@""]
                autorelease];
    [chuckMenu addItem:menuItem];
    
    [chuckMenu addItem:[NSMenuItem separatorItem]];
    
    menuItem = [[[NSMenuItem new] initWithTitle:@"Quit" 
                                         action:NULL
                                  keyEquivalent:@"q"]
                autorelease];
    [menuItem setTarget:g_ui_controller];
    [menuItem setAction:@selector(quit)];
    [chuckMenu addItem:menuItem];
    
    [app setMainMenu:mainMenu];
    
    [app activateIgnoringOtherApps:YES];
    
    [temp_pool release];
    
    [app run];
    
    [arpool release];
}


void Chuck_UI_Manager::shutdown()
{
    NSAutoreleasePool * arpool = [NSAutoreleasePool new];
    
    [[NSApplication sharedApplication] performSelectorOnMainThread:@selector(terminate:)
                                                        withObject:NSApp
                                                     waitUntilDone:NO];
    
    [arpool release];
}



@implementation Chuck_UI_Controller

- (void)quit
{
    if(g_vm) g_vm->stop(); // termination of vm will cause application exit
}

@end


