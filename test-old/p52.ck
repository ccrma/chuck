// float comp

1.0 => float x;
0 => int ret;
if ( x == 1.0 ) { 
	1 +=> ret;
}

if ( x != 1.0 ) { 
	2 +=> ret;
}

if ( x < 1.0 ) { 
	4 +=> ret;
}

if ( x > 1.0 ) { 
	8 +=> ret;
}

if ( x >= 1.0 ) { 
	16 +=> ret;
}

if ( x <= 1.0 ) { 
	32 +=> ret;
}

<<<ret>>>;
if ( ret == 49 ) <<<"success">>>;