// function declaration before definition ( fail-correct )

fun int foo();

fun int bar() { 
	return 4 * foo();
}

fun int foo() { 
	return 5;
}

bar() => int answer;

if ( answer == 20 ) <<<"success">>>; 