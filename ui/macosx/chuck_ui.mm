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

Chuck_UI_Manager::Chuck_UI_Manager()
{
    
}

void Chuck_UI_Manager::go()
{
    NSAutoreleasePool * arpool = [NSAutoreleasePool new];
    
    NSAutoreleasePool * temp_pool = [NSAutoreleasePool new];
    
    NSRect frame = NSMakeRect(0, 0, 200, 200);
    
    //AppController *controller = [[AppController alloc] init];
    
    NSApplication * app = [NSApplication sharedApplication];
    [app setDelegate:nil];
    NSWindow* window  = [[NSWindow alloc] initWithContentRect:frame
                                                    styleMask:NSBorderlessWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
    [window setBackgroundColor:[NSColor blueColor]];
    
    NSButton *button = [ [ NSButton alloc ] initWithFrame: NSMakeRect( 30.0, 20.0, 80.0, 50.0 ) ];
    [ button setBezelStyle:NSRoundedBezelStyle];
    [ button setTitle: @"Click" ];
//    [ button setAction:@selector(doSomething:)];
//    [ button setTarget:controller];
    [ [ window contentView ] addSubview: button ];
    
    [window makeKeyAndOrderFront:NSApp];
    
    
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
