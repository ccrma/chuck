// int comp

1 => int x;

0 => int ret;

if ( x == 1 ) { 
	1 +=> ret;
}

if ( x != 1 ) { 
	2 +=> ret;
}

if ( x <= 1 ) { 
	4 +=> ret;
}

if ( x >= 1 ) { 
	8 +=> ret;
}

if ( x > 1 ) { 
	16 +=> ret;
}

if ( x < 1 ) { 
	32 +=> ret;
}

<<<ret>>>;
if ( ret == 13 ) <<<"success">>>;

