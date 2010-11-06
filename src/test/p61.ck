// function overloading

function int howMany() { 
	return 0;
}

function int howMany( int a ) { 
	return 1;
}

function int howMany( int a, int b ) { 
	return 2;
}

0 => int ret;
if ( howMany() == 0 ) { 
	1 +=> ret;
}

if ( howMany( 1 ) == 1 ) { 
	2 +=> ret;
}

if ( howMany( 2, 2 ) == 2 ) { 
	4 +=> ret;
}

if ( ret == 7 ) <<<"success">>>;
