// led objects
MAUI_LED a, b, c;

// view object
MAUI_View v;

// set the view size
v.size( 150, 50 );

// initialize color, size position of first led
a.color( a.red );
a.size( 50, 50 );
a.position( 0, 0 );

// initialize color, size position of second led
b.color( b.green );
b.size( 50, 50 );
b.position( 50, 0 );

// initialize color, size position of third led
c.color( c.blue );
c.size( 50, 50 );
c.position( 100, 0 );

// add to view
v.addElement( a );
v.addElement( b );
v.addElement( c );

// display view
v.display();

while( true )
{
    // probabilistically light each led
    
	if( maybe ) a.light();
	else a.unlight();
	
	if( maybe ) b.light();
	else b.unlight();

	if( maybe ) c.light();
	else c.unlight();
	
    // let time pass
	.51::second => now;
}
