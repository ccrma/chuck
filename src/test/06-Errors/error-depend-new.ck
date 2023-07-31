// class Foo needs 'array', cannot instantiate Foo before 'array' initializes
// 'new' edition of unit test; ALSO SEE: error-depend-class.ck

// does not work heress
new Foo @=> Foo @ foo;

// declaration and initialization
[1, 2, 3] @=> int array[];

// class defintion
class Foo
{
    // need array to initialized
    <<< array[0], array[1], array[2] >>>;
}
