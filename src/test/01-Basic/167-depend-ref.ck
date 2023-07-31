// although class Foo needs 'array', declaring a null reference to Foo should be okay
// ALSO SEE: 06-Errors/error-depend-class.ck

// no actual instance
Foo @ foo;

// the variable and initialization
[1, 2, 3] @=> int array[];

// class definition
class Foo
{
    // need 'array'
    <<< array[0], array[1], array[2] >>>;
}

// no errors then good
<<< "success" >>>;