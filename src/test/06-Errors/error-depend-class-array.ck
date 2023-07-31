// class Foo needs 'array', cannot instantiate Foo before 'array' initializes
// array unit test edition

// does not work here
Foo foo[2];

// declaration and initialization
[1, 2, 3] @=> int array[];

// class defintion
class Foo
{
    // need array to initialized
    <<< array[0], array[1], array[2] >>>;
}
