// function calls another function

fun int foo() { 
	return 5;
}

fun int bar() { 
	return 4 * foo();
}

bar() => int answer;

if ( answer == 20 ) { 
	<<<"success">>>;
}

