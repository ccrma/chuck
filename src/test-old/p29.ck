// define variable inside conditional block, then access outside. ( fail-correct )

1 => int x;

if ( x > 0 ) { 
	5 => int y;
}

y => x;

<<<"success">>>;