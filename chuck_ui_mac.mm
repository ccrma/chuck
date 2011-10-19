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


static Chuck_UI_Manager * g_ui_manager = NULL;


Chuck_UI_Manager::Chuck_UI_Manager()
{
    
}

Chuck_UI_Manager * Chuck_UI_Manager::instance()
{
    if(g_ui_manager == NULL)
        g_ui_manager = new Chuck_UI_Manager;
    return g_ui_manager;
}

void Chuck_UI_Manager::go()
{
    NSAutoreleasePool * arpool = [NSAutoreleasePool new];
    
    NSAutoreleasePool * temp_pool = [NSAutoreleasePool new];
    
    NSApplication * app = [NSApplication sharedApplication];
    [app setDelegate:nil];    
    
    NSMenu * mainMenu = [[NSMenu new] autorelease];
    NSMenu * chuckMenu = [[NSMenu new] autorelease];
    
    NSMenuItem * menuItem;
    
    menuItem = [[NSMenuItem new] autorelease];
    [menuItem setTitle:@"chuck"];
    [menuItem setSubmenu:chuckMenu];
    [mainMenu addItem:menuItem];
    
    menuItem = [[NSMenuItem new] autorelease];
    [menuItem setTitle:@"About chuck"];
    [chuckMenu addItem:menuItem];
    
    [chuckMenu addItem:[NSMenuItem separatorItem]];
    
    menuItem = [[NSMenuItem new] autorelease];
    [menuItem setTitle:@"Quit"];
    [menuItem setKeyEquivalent:@"q"];
    [menuItem setKeyEquivalentModifierMask:NSCommandKeyMask];
    [chuckMenu addItem:menuItem];
    
    [app setMainMenu:mainMenu];
    
    [[NSProcessInfo processInfo] setProcessName:@"chuck"];
    
    [NSMenu setMenuBarVisible:YES];
    
    [temp_pool release];
    
    //[[NSRunLoop currentRunLoop] run];
    [[NSApplication sharedApplication] run];
    
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
