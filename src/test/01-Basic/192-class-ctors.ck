// test pre-constructors running with inheritance

// base
class Foo
{
    <<< "I am a Foo" >>>;
}

// child
class Bar extends Foo
{
    <<< "I am a Bar" >>>;
}

// instantiate Bar should run both Foo's and Bar's pre-constructor
Bar bar;
