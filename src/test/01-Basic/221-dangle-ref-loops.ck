// verify func-call returning objects is properly refcounted
// as part or the stmt's object release mechanism | 1.5.1.7

// a class
class Foo { 5 => int x; }
// a file-global instance
Foo foo;
// a function that returns foo
fun Foo bar() { return foo; } 
// another function that returns foo
fun Object zzz() { return foo; }

// for loop
for( int i; i < bar().x; i++ ) { }

// reset
5 => foo.x;

// bar() returns an object every iteration, verify the
// stmt reference count is correct
while( bar().x )
{
    // do this a few times
    bar().x--;
    // break
    if( bar().x == 2 ) { break; }
}

// reset
5 => foo.x;

// try until
until( bar().x == 0 )
{
    // do this a few times
    bar().x--;
    // break
    if( bar().x == 2 ) break;
}

// reset
5 => foo.x;

// do while
do {
    // do this a few times
    bar().x--;
    // break
    if( bar().x == 2 ) break;
} while( bar().x );

// reset
1001 => foo.x;

// do until
do {    
    // do this a few times
    bar().x--;
    // break
    if( bar().x == 2 ) continue;
    // do this a few times
    bar().x--;
} until( bar().x < 1 );

20 => foo.x;

// loop
repeat( bar().x ) { bar().x--; }

// for each with array Object
for( int x : Std.range(0,20) ) { }

// for loop with dangling ref in conditional part 2
for( ; zzz() == null; ) { }

// reset
13 => bar().x;

// since bar() returns an object every iteration, verify the
// stmt reference count is correct
while( bar().x )
{
    // do this a few times
    bar().x--;
    // break
    while( bar().x ) break;
}
    
// check
if( Machine.refcount(foo) == 1 ) <<< "success" >>>;
