/*----------------------------------------------------------------------------
miniAudicle
Cocoa GUI to chuck audio programming environment

Copyright (c) 2005 Spencer Salazar.  All rights reserved.
http://chuck.cs.princeton.edu/
http://soundlab.cs.princeton.edu/

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: miniAudicle_ui_elements.h
// desc: for supporting UI controls created and used in ChucK code
//
// author: Spencer Salazar (ssalazar@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#import <Cocoa/Cocoa.h>

#include "miniAudicle_ui_elements.h"
using namespace std;
//using namespace miniAudicle::UI;
using namespace UI;

//------------------------------------------------------------------------------
// name: mAFlippedView
// desc: NSView with flipped (traditional) Y axis
//------------------------------------------------------------------------------
@interface mAFlippedView : NSView
- (BOOL)isFlipped;
@end

@implementation mAFlippedView

- (BOOL)isFlipped
{
    return YES;
}

@end

//------------------------------------------------------------------------------
// name: mAUIElement
// desc: ...
//------------------------------------------------------------------------------
@interface mAUIElement : NSObject
{
    Element * owner;
    NSView * master_view;
}

- (void)link:( Element * )e;
- (void)unlink;
- (id)init;
- (void)display;
- (void)hide;
- (void)destroy;
- (void)setSize;
- (void)setPosition;
- (void)setName;

- (void)removeParent;

- (NSView * )masterView;
- (void)setMasterView:(NSView *)v;

@end

@implementation mAUIElement

- (void)link:( Element * )e
{
    owner = e;    
}

- (void)unlink
{
    owner = NULL;
}

- (id)init
{
    return [super init];
}

- (void)display
{
    if( master_view )
        [master_view setHidden:NO];
}

- (void)hide
{
    if( master_view )
        [master_view setHidden:YES];
}

- (void)destroy
{
    if( master_view )
    {
        [master_view release];
        master_view = NULL;
    }
    
    [self autorelease];
}

- (void)setSize
{
    if( master_view )
        [master_view setFrameSize:NSMakeSize( owner->get_width(), owner->get_height() )];
}

- (void)setPosition
{
    if( master_view )
        [master_view setFrameOrigin:NSMakePoint( owner->get_x(), owner->get_y() )];
}

- (void)setName
{
    
}

- (void)removeParent
{
    [master_view removeFromSuperview];
}

- (NSView * )masterView
{
    return master_view;
}

- (void)setMasterView:(NSView *)v
{
    master_view = v;
}

@end

namespace UI
{

t_CKBOOL Element::display()
{
    pi_display();
    
    [native_element performSelectorOnMainThread:@selector(display)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::hide()
{
    pi_hide();
    
    [native_element performSelectorOnMainThread:@selector(hide)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::destroy()
{
    pi_destroy();
    
    
    [native_element performSelectorOnMainThread:@selector(unlink)
                                     withObject:nil
                                  waitUntilDone:YES];
    

    [native_element performSelectorOnMainThread:@selector(destroy)
                                     withObject:nil
                                  waitUntilDone:YES];
    
    return TRUE;    
}

t_CKBOOL Element::set_name( std::string & name )
{
    pi_set_name( name );
    
    [native_element performSelectorOnMainThread:@selector(setName)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::set_parent( View * p )
{
    pi_set_parent( p );
    
    [parent->native_view performSelectorOnMainThread:@selector(addElement:)
                                          withObject:native_element
                                       waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::remove_parent( View * p )
{
    pi_remove_parent( p );
    
    [native_element performSelectorOnMainThread:@selector(removeParent)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::set_size( t_CKFLOAT w, t_CKFLOAT h )
{
    pi_set_size( w, h );
    
    [native_element performSelectorOnMainThread:@selector(setSize)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}

t_CKBOOL Element::set_position( t_CKFLOAT x, t_CKFLOAT y )
{
    pi_set_position( x, y );
    
    [native_element performSelectorOnMainThread:@selector(setPosition)
                                     withObject:nil
                                  waitUntilDone:NO];
    
    return TRUE;    
}
    
}

//------------------------------------------------------------------------------
// name: mAUIView
// desc: ...
//------------------------------------------------------------------------------
@interface mAUIView : mAUIElement
{
    NSWindow * panel;
}

- (id)init;
- (void)display;
- (void)hide;
- (void)destroy;
- (void)setName;
- (void)setSize;
- (void)setPosition;
- (void)addSubview:(NSView *)v;
- (void)addElement:(mAUIElement *)e;

@end

@implementation mAUIView

- (id)init
{
    if( self = [super init] )
    {
        panel = [[NSWindow alloc] initWithContentRect:NSMakeRect( 0, 0, 500, 500 )
                                            styleMask:NSTitledWindowMask | NSMiniaturizableWindowMask | NSClosableWindowMask
                                              backing:NSBackingStoreBuffered
                                                defer:YES];
        [panel setReleasedWhenClosed:NO];
        master_view = [[[mAFlippedView alloc] init] autorelease];
        [panel setContentView:master_view];
        [panel center];
    }
    
    return self;
}

- (void)display
{
    [panel makeKeyAndOrderFront:nil];
}

- (void)hide
{
    [panel close];
}

- (void)destroy
{
    [panel close];
    [panel release];
    [self autorelease];
}

- (void)setName
{
    if( owner )
        [panel setTitle:[NSString stringWithCString:(owner->get_name()).c_str()]];
}

- (void)setSize
{
    if( owner )
        [panel setContentSize:NSMakeSize( owner->get_width(), owner->get_height() )];
}

- (void)setPosition
{
    if( owner )
        [panel setFrameOrigin:NSMakePoint( owner->get_x(), owner->get_y() )];
}

- (void)addSubview:(NSView *)v
{
    [[panel contentView] addSubview:v];
}

- (void)addElement:(mAUIElement *)e
{
    [[panel contentView] addSubview:[e masterView]];
}

@end

//namespace miniAudicle
//{
namespace UI
{

t_CKBOOL View::init()
{
    NSAutoreleasePool * arpool = [[NSAutoreleasePool alloc] init];

    native_element = native_view = [mAUIView alloc];
    [native_view link:this];
    [native_view performSelectorOnMainThread:@selector(init)
                                  withObject:nil
                               waitUntilDone:NO];
    
    [arpool release];
    
    return TRUE;
}

t_CKBOOL View::display()
{
    std::vector< Element * >::size_type i, len = elements.size();
    
    for( i = 0; i < len; i++ )
    {
        if( elements[i] )
            elements[i]->display();
    }
    
    [native_view performSelectorOnMainThread:@selector(display)
                                  withObject:nil
                               waitUntilDone:NO];
        
    return TRUE;
}

t_CKBOOL View::hide()
{
    [native_view performSelectorOnMainThread:@selector(hide)
                                  withObject:nil
                               waitUntilDone:NO];
    
    return TRUE;
}

t_CKBOOL View::destroy()
{
    std::vector< Element * >::size_type i, len = elements.size();

    for( i = 0; i < len; i++ )
    {
        if( elements[i] )
            elements[i]->remove_parent( this );
    }
    
    [native_view performSelectorOnMainThread:@selector(unlink)
                                  withObject:nil
                               waitUntilDone:YES];
    
    [native_view performSelectorOnMainThread:@selector(destroy)
                                  withObject:nil
                               waitUntilDone:YES];
    
    return TRUE;
}

t_CKBOOL View::set_parent( View * p )
{
    return TRUE;
}

t_CKBOOL View::add_element( Element * e )
{
    if( e == NULL )
        return FALSE;
    
    elements.push_back( e );
    e->set_parent( this );
    
    return TRUE;
}

t_CKBOOL View::remove_element( Element * e )
{
    if( e == NULL )
        return FALSE;
    
    std::vector< Element * >::size_type i, len = elements.size();
    
    for( i = 0; i < len; i++ )
    {
        if( elements[i] == e )
        {
            elements[i]->remove_parent( this );
            elements[i] = NULL;
        }
    }
    
    return TRUE;
}

} /* namespace UI */

//------------------------------------------------------------------------------
// name: mAUISlider
// desc: ...
//------------------------------------------------------------------------------
@interface mAUISlider : mAUIElement
{
    Slider * s_owner;
    NSSlider * slider;
    NSTextField * title;
    NSTextField * value;
    NSString * display_format;
}

- (void)link:( Slider * )s;
- (void)unlink;
- (id)init;
- (void)destroy;
- (void)setName;
- (void)setValue;
- (void)setRange;
- (void)setDisplayFormat;
- (void)updateDisplay;
- (void)sliderDidChange;

@end

@implementation mAUISlider

- (void)link:( Slider * )s
{
    owner = s_owner = s;
}

- (void)unlink
{
    owner = s_owner = NULL;
}

- (id)init
{
    if( self = [super init] )
    {
        slider = [[NSSlider alloc] initWithFrame:NSMakeRect( Slider::default_margin, 
                                                             Slider::default_margin,
                                                             Slider::default_width, 
                                                             Slider::default_slider_height )];        
        [slider setAutoresizingMask:NSViewWidthSizable | NSViewMinYMargin | NSViewMaxYMargin];
        [slider setDoubleValue:0.0];
        [slider setTarget:self];
        [slider setAction:@selector(sliderDidChange)];
        
        title = [[NSTextField alloc] initWithFrame:NSMakeRect( Slider::default_margin,
                                                               Slider::default_margin + Slider::default_slider_height + Slider::default_inner_margin,
                                                               Slider::default_width,
                                                               Slider::default_text_height )];
        [title setEditable:NO];
        [title setBezeled:NO];
        [title setDrawsBackground:NO];
        [title setAutoresizingMask:NSViewWidthSizable | NSViewMinYMargin | NSViewMaxYMargin];
        
        value = [[NSTextField alloc] initWithFrame:NSMakeRect( Slider::default_margin,
                                                               Slider::default_margin + Slider::default_slider_height + Slider::default_inner_margin,
                                                               Slider::default_width,
                                                               Slider::default_text_height )];
        [value setEditable:NO];
        [value setBezeled:NO];
        [value setDrawsBackground:NO];
        [value setAutoresizingMask:NSViewWidthSizable | NSViewMinYMargin | NSViewMaxYMargin];
        [value setAlignment:NSRightTextAlignment];
        
        t_CKFLOAT w = Slider::default_width + Slider::default_margin * 2,
            h = Slider::default_slider_height + Slider::default_inner_margin + 
            Slider::default_text_height + 
            Slider::default_margin * 2;
        master_view = [[NSView alloc] initWithFrame:NSMakeRect( 0, 0, w, h )];
        [master_view setAutoresizingMask:NSViewNotSizable];
        [master_view addSubview:slider];
        [master_view addSubview:title];
        [master_view addSubview:value];
        
        [self setDisplayFormat];
        [self updateDisplay];
    }
    
    return self;
}

- (void)destroy
{
    [title release];
    [value release];
    [slider release];
    [master_view removeFromSuperview];
    [master_view release];
    [self autorelease];
}

- (void)setName
{
    if( !owner )
        return;
    [title setStringValue:[NSString stringWithCString:owner->get_name().c_str()]];
}

- (void)setValue
{
    if( !s_owner )
        return;
    [slider setDoubleValue:s_owner->get_value()];
}

- (void)setRange
{
    if( !s_owner )
        return;
    
    t_CKFLOAT min = s_owner->get_min(), max = s_owner->get_max(), 
        fvalue = s_owner->get_value();
    
    [slider setMaxValue:max];
    [slider setMinValue:min];
    
    if( fvalue < min )
        s_owner->set_value( min );
    else if( fvalue > max )
        s_owner->set_value( max );
}

- (void)setDisplayFormat
{
    if( !s_owner )
        return;
    
    UI::Slider::display_format df = s_owner->get_display_format();
    
    if( df == UI::Slider::best_format )
        display_format = @"%.*g";
    else if( df == UI::Slider::integer_format )
        display_format = @"%d";
    else if( df == UI::Slider::decimal_format )
        display_format = @"%.*f";
    else if( df == UI::Slider::scientific_format )
        display_format = @"%.*e";
}

- (void)updateDisplay
{
    if( !s_owner )
        return;
    
    if( s_owner->get_display_format() == UI::Slider::integer_format )
        [value setStringValue:[NSString stringWithFormat:display_format, 
            ( t_CKINT ) lrint( [slider doubleValue] )]];
    else
        [value setStringValue:[NSString stringWithFormat:display_format, 
            s_owner->get_precision(), [slider doubleValue]]];
}

- (void)sliderDidChange
{
    if( !s_owner )
        return;
    
    s_owner->slider_changed( [slider doubleValue] );
}

@end

namespace UI
{

const t_CKUINT Slider::default_margin = 18;
const t_CKUINT Slider::default_width = 202;
const t_CKUINT Slider::default_slider_height = 21;
const t_CKUINT Slider::default_inner_margin = 5;
const t_CKUINT Slider::default_text_height = 17;

t_CKBOOL Slider::init()
{   
    w = default_width + default_margin * 2;
    h = default_slider_height + default_inner_margin + 
        default_text_height + default_margin * 2;
    
    name = "";
    e = NULL;
    
    df = best_format;
    precision = 4;
        
    native_element = native_slider = [mAUISlider alloc];
    [native_slider link:this];
    [native_slider performSelectorOnMainThread:@selector(init)
                                          withObject:nil
                                       waitUntilDone:NO];
    
    return TRUE;
}

t_CKDOUBLE Slider::get_value()
{   
    return value;
}

t_CKBOOL Slider::set_value( t_CKDOUBLE v )
{
    value = v;
    
    if( value > max )
        value = max;
    if( value < min )
        value = min;
    
    [native_slider performSelectorOnMainThread:@selector(setValue)
                                          withObject:nil
                                       waitUntilDone:NO];
    
    [native_slider performSelectorOnMainThread:@selector(updateDisplay)
                                          withObject:nil
                                       waitUntilDone:NO];
    
    return TRUE;
}

t_CKBOOL Slider::set_range( t_CKDOUBLE low, t_CKDOUBLE high )
{
    min = low;
    max = high;
    [native_slider performSelectorOnMainThread:@selector(setRange)
                                          withObject:nil
                                       waitUntilDone:NO];
        
    set_value( get_value() );
        
    return TRUE;
}

t_CKDOUBLE Slider::get_max()
{
    return max;
}

t_CKDOUBLE Slider::get_min()
{
    return min;
}

t_CKBOOL Slider::set_precision( t_CKUINT precision )
{
    this->precision = precision;
    
    // refresh
    set_value( get_value() );
    
    return TRUE;
}

t_CKUINT Slider::get_precision()
{
    return precision;
}

t_CKBOOL Slider::set_display_format( Slider::display_format f )
{
    this->df = f;
        
    [native_slider performSelectorOnMainThread:@selector(setDisplayFormat)
                                          withObject:nil
                                       waitUntilDone:NO];
    
    // refresh
    set_value( get_value() );
    
    return TRUE;
}

Slider::display_format Slider::get_display_format()
{
    return df;
}

void Slider::slider_changed( t_CKDOUBLE v )
{
    if( df == integer_format )
        value = ( t_CKINT ) lrint( v );
    else
        value = v;
    
    if( e != nil )
        e->queue_broadcast();
    
    // refresh
    [native_slider performSelectorOnMainThread:@selector(updateDisplay)
                                          withObject:nil
                                       waitUntilDone:NO];
}

} /* namespace UI */

//------------------------------------------------------------------------------
// name: mAUIButton
// desc: ...
//------------------------------------------------------------------------------
@interface mAUIButton : mAUIElement
{
    Button * b_owner;
    NSButton * button;
}

- (void)link:( Button * )b;
- (void)unlink;
- (id)init;
- (void)destroy;
- (void)setActionType;
- (void)setState;
- (void)setName;
- (void)buttonDidChange;

@end

@implementation mAUIButton

- (void)link:( Button * )b
{
    owner = b_owner = b;
}

- (void)unlink
{
    owner = b_owner = NULL;
}

- (id)init
{
    if( self = [super init] )
    {
        t_CKFLOAT w = Button::default_button_width + Button::default_margin * 2;
        t_CKFLOAT h = Button::default_button_height + Button::default_margin * 2;
        
        button = [[NSButton alloc] initWithFrame:NSMakeRect( Button::default_margin,
                                                             Button::default_margin,
                                                             Button::default_button_width,
                                                             Button::default_button_height )];
        [button setBezelStyle:NSShadowlessSquareBezelStyle];
        
        if( owner )
            [button setTitle:[NSString stringWithCString:owner->get_name().c_str()]];
        [button setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        
        master_view = [[NSView alloc] initWithFrame:NSMakeRect( 0, 0, w, h )];
        [master_view addSubview:button];
        
        [button setTarget:self];
        [button setAction:@selector( buttonDidChange )];
        
        [self setActionType];
    }
    
    return self;
}

- (void)destroy
{
    if( button != nil )
        [button release];
    if( master_view != nil )
    {
        [master_view removeFromSuperview];
        [master_view release];
    }
    
    [self autorelease];
}

- (void)setActionType
{
    if( !b_owner )
        return;
    
    Button::action_type at = b_owner->get_action_type();
    
    [button setState:NSOffState];
    
    if( at == Button::toggle_type )
    {
        [button setButtonType:NSPushOnPushOffButton];        
        [button sendActionOn:NSLeftMouseDownMask];
    }
    
    else if( at == Button::push_type )
    {
        [button setButtonType:NSMomentaryPushInButton];
        [button sendActionOn:NSLeftMouseDownMask | NSLeftMouseUpMask];
    }
    
}

- (void)setState
{
    if( !b_owner )
        return;
    
    if( b_owner->get_state() )
        [button setState:NSOnState];
    else
        [button setState:NSOffState];
}

- (void)setName
{
    if( !owner )
        return;
    
    if( button != nil )
        [button setTitle:[NSString stringWithCString:owner->get_name().c_str()]];
}

- (void)buttonDidChange
{
    if( !b_owner )
        return;
    
    b_owner->button_changed();
}

@end

namespace UI
{
    
const t_CKUINT Button::default_margin = 18;
const t_CKUINT Button::default_button_width = 55;
const t_CKUINT Button::default_button_height = 55;

t_CKBOOL Button::init()
{
    x = y = 0;
    w = Button::default_button_width + Button::default_margin * 2;
    h = Button::default_button_height + Button::default_margin * 2;
    name = "";
    
    pushed = FALSE;
    at = push_type;
    
    NSAutoreleasePool * arpool = [[NSAutoreleasePool alloc] init];
    
    native_element = native_button = [mAUIButton alloc];
    [native_button link:this];
    [native_button performSelectorOnMainThread:@selector( init )
                                           withObject:nil
                                        waitUntilDone:NO];
    
    [arpool release];
    
    return TRUE;
}

t_CKBOOL Button::get_state()
{
    return pushed;
}

t_CKBOOL Button::set_state( t_CKBOOL state )
{
    if( state )
        pushed = 1;
    else
        pushed = 0;
    
    [native_button performSelectorOnMainThread:@selector(setState)
                                          withObject:nil
                                       waitUntilDone:NO];
    
    return FALSE;
}

t_CKBOOL Button::set_action_type( action_type t )
{
    if( t == toggle_type || t == push_type )
    {
        if( pushed )
            button_changed();
        at = t;
        [native_button performSelectorOnMainThread:@selector(setActionType)
                                              withObject:nil
                                           waitUntilDone:NO];
        return TRUE;
    }
    
    return FALSE;
}

Button::action_type Button::get_action_type()
{
    return at;
}

void Button::button_changed()
{
    pushed = !pushed;

    if( e != NULL )
        e->queue_broadcast();
}

} /* namespace UI */

const static NSString * off_image_name = @"led-off.png";
const static NSString * red_image_name = @"led-red.png";
const static NSString * green_image_name = @"led-green.png";
const static NSString * blue_image_name = @"led-blue.png";

@interface mAUILED : mAUIElement
{
    NSImage * off_image;
    NSImage * on_image;
    NSImageView * image_view;
    
    LED * l_owner;
}

- (void)link:( LED * )l;
- (void)unlink;
- (id)init;
- (void)destroy;
- (void)light;
- (void)unlight;
- (void)setColor;

@end

@implementation mAUILED

- (void)link:( LED * )l
{
    owner = l_owner = l;
}

- (void)unlink
{
    owner = l_owner = NULL;
}

- (id)init
{
    if( self = [super init] )
    {
        on_image = [[NSImage imageNamed:red_image_name] retain];
        off_image = [[NSImage imageNamed:off_image_name] retain];
        
        image_view = [[NSImageView alloc] initWithFrame:NSMakeRect( LED::default_margin,
                                                                    LED::default_margin,
                                                                    LED::default_led_width,
                                                                    LED::default_led_height )];
        [image_view setImage:off_image];
        [image_view setEnabled:NO];
        [image_view setAutoresizingMask:NSViewMinXMargin | NSViewMaxXMargin | 
            NSViewMinYMargin | NSViewMaxYMargin];
        
        NSRect r = [image_view frame];
        r.origin.x = 0;
        r.origin.y = 0;
        r.size.height += LED::default_margin * 2;
        r.size.width += LED::default_margin * 2;
        master_view = [[NSView alloc] initWithFrame:r];
        
        [master_view addSubview:image_view];        
    }
    
    return self;
}

- (void)destroy
{
    if( image_view != nil )
        [image_view release];
    if( on_image != nil )
        [on_image release];
    if( off_image != nil )
        [off_image release];
    if( master_view != nil )
    {
        [master_view removeFromSuperview];
        [master_view release];
    }
    
    [self autorelease];
}

- (void)light
{
    [image_view setImage:on_image];
}

- (void)unlight
{
    [image_view setImage:off_image];
}

- (void)setColor
{
    if( l_owner == NULL )
        return;
    
    LED::color c = l_owner->get_color();
    
    if( c == LED::red )
    {
        [on_image autorelease];
        on_image = [[NSImage imageNamed:red_image_name] retain];
    }
    
    else if( c == LED::green )
    {
        [on_image autorelease];
        on_image = [[NSImage imageNamed:green_image_name] retain];
    }
    
    else if( c == LED::blue )
    {
        [on_image autorelease];
        on_image = [[NSImage imageNamed:blue_image_name] retain];
    }
}

@end

namespace UI
{

const t_CKUINT LED::default_margin = 18; 
const t_CKUINT LED::default_led_width = 28;
const t_CKUINT LED::default_led_height = 28;

t_CKBOOL LED::init()
{
    
    NSAutoreleasePool * arpool = [[NSAutoreleasePool alloc] init];
    
    c = red;
    x = y = 0;
    w = default_margin * 2 + default_led_width;
    h = default_margin * 2 + default_led_height;
        
    native_element = native_led = [mAUILED alloc];
    [native_led link:this];
    
    [native_led performSelectorOnMainThread:@selector(init)
                                 withObject:nil
                              waitUntilDone:NO];
        
    [arpool release];
    
    return TRUE;
}

t_CKBOOL LED::light()
{    
    [native_led performSelectorOnMainThread:@selector(light)
                                 withObject:nil
                              waitUntilDone:NO];
    
    return TRUE;
}

t_CKBOOL LED::unlight()
{
    [native_led performSelectorOnMainThread:@selector(unlight)
                                 withObject:nil
                              waitUntilDone:NO];
    
    return TRUE;
}

t_CKBOOL LED::set_color( color c )
{    
    unlight();
    
    if( c == red || c == green || c == blue )
    {
        this->c = c;
        [native_led performSelectorOnMainThread:@selector(setColor)
                                     withObject:nil
                                  waitUntilDone:NO];
    }

    return TRUE;
}

LED::color LED::get_color()
{
    return c;
}

} /* namespace UI */
//} /* namespace miniAudicle */

//------------------------------------------------------------------------------
// name: mAUIText
// desc: ...
//------------------------------------------------------------------------------
@interface mAUIText : mAUIElement
{
    Text * t_owner;
    NSTextField * text;
}

- (void)link:( Text * )t;
- (void)unlink;
- (id)init;
- (void)destroy;
- (void)setName;

@end

@implementation mAUIText

- (void)link:( Text * )t
{
    owner = t_owner = t;
}

- (void)unlink
{
    owner = t_owner = NULL;
}

- (id)init
{
    if( self = [super init] )
    {
        NSRect frame_rect = NSMakeRect( owner->get_x(), owner->get_y(), 
                                        owner->get_width(), owner->get_height() );
        master_view = [[NSView alloc] initWithFrame:frame_rect];
        
        frame_rect = NSMakeRect( owner->get_left_margin(), 
                                 owner->get_top_margin(), 
                                 owner->get_width() - owner->get_right_margin() - owner->get_left_margin(),
                                 owner->get_height() - owner->get_bottom_margin() - owner->get_top_margin() );
        text = [[NSTextField alloc] initWithFrame:frame_rect];
        [text setEditable:NO];
        [text setBezeled:NO];
        [text setSelectable:YES];
        [text setDrawsBackground:NO];
        [text setAutoresizingMask:NSViewMinXMargin | NSViewMaxXMargin | 
            NSViewMinYMargin | NSViewMaxYMargin];

        [master_view addSubview:text];
        
        [text setStringValue:[NSString stringWithCString:owner->get_name().c_str()]];
    }
    
    return self;
}

- (void)destroy
{
    if( text )
        [text autorelease];
    if( master_view )
        [master_view autorelease];
    [self autorelease];
}

- (void)setName
{
    if( owner )
    {
        [text setStringValue:[NSString stringWithCString:owner->get_name().c_str()]];
        [text sizeToFit];
    }
}

@end

namespace UI
{

const t_CKUINT Text::default_margin = 0;
const t_CKUINT Text::default_text_width = 75;
const t_CKUINT Text::default_text_height = 40;

t_CKBOOL Text::init()
{
    NSAutoreleasePool * arpool = [[NSAutoreleasePool alloc] init];

    native_element = native_text = [mAUIText alloc];
    
    x = y = 0;
    w = default_text_width;
    h = default_text_height;
    lmargin = rmargin = tmargin = bmargin = Text::default_margin;
    
    [native_text link:this];
    [native_text performSelectorOnMainThread:@selector(init)
                                  withObject:nil
                               waitUntilDone:NO];
    
    [arpool release];
    
    return TRUE;
}

} /* namespace UI */

/*@interface mAProgressIndicator : NSProgressIndicator

- (void)heartBeat:(void *)v;

@end

@implementation mAProgressIndicator : NSProgressIndicator

- (void)heartBeat:(void *)v
{
    Class ___NSUIHeartBeat = NSClassFromString( @"NSUIHeartBeat" );
    if( ___NSUIHeartBeat != nil )
        [[___NSUIHeartBeat sharedHeartBeat] removeHeartBeatView:self];
}

@end*/

@interface mAUIGauge : mAUIElement
{
    Gauge * g_owner;
    NSProgressIndicator * gauge;
}

- (void)link:(Gauge *)g;
- (void)unlink;
- (id)init;
- (void)display;
- (void)destroy;
- (void)setValue;

@end

@implementation mAUIGauge

- (void)link:(Gauge *)g
{
    owner = g_owner = g;
}

- (void)unlink
{
    owner = g_owner = NULL;
}

- (id)init
{
    if( self = [super init] )
    {
        NSRect frame_rect = NSMakeRect( owner->get_x(), owner->get_y(), 
                                        owner->get_width(), owner->get_height() );
        master_view = [[NSView alloc] initWithFrame:frame_rect];
    
        frame_rect = NSMakeRect( owner->get_left_margin(), 
                                 owner->get_top_margin(), 
                                 owner->get_width() - owner->get_right_margin() - owner->get_left_margin(),
                                 owner->get_height() - owner->get_bottom_margin() - owner->get_top_margin() );
        gauge = [[NSProgressIndicator alloc] initWithFrame:frame_rect];
        [gauge setIndeterminate:FALSE];
        [gauge setDoubleValue:g_owner->get_value()];
        [gauge setMinValue:0.0];
        [gauge setMaxValue:100.0];
        
        [master_view addSubview:gauge];
    }
    
    return self;
}

- (void)display
{
    [super display];
}

- (void)destroy
{
    if( gauge )
        [gauge autorelease];
    if( master_view )
        [master_view autorelease];
    [self autorelease];
}

- (void)setValue
{
    if( gauge && g_owner )
        [gauge setDoubleValue:( g_owner->get_value() * 100.0 )];
}

@end

namespace UI
{

const t_CKUINT Gauge::default_margin = 18;
const t_CKUINT Gauge::default_gauge_width = 200;
const t_CKUINT Gauge::default_gauge_height = 58;

t_CKBOOL Gauge::init()
{
    NSAutoreleasePool * arpool = [[NSAutoreleasePool alloc] init];
    
    native_element = native_gauge = [mAUIGauge alloc];
    
    w = default_gauge_width;
    h = default_gauge_height;
    lmargin = rmargin = tmargin = bmargin = default_margin;
    
    [native_gauge link:this];
    [native_gauge performSelectorOnMainThread:@selector(init)
                                   withObject:nil
                                waitUntilDone:NO];
    
    [arpool release];
    
    return TRUE;
}

t_CKBOOL Gauge::set_value( t_CKFLOAT _value )
{
    if( _value < 0.0 )
        _value = 0.0;
    else if( _value > 1.0 )
        _value = 1.0;
    
    value = _value;
    [native_gauge performSelectorOnMainThread:@selector(setValue)
                                   withObject:nil
                                waitUntilDone:NO];
    
    return TRUE;
}

t_CKFLOAT Gauge::get_value()
{
    return value;
}

}

