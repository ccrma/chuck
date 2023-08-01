// error case: test for dependency through transitive function calls
// i.e., in this case, nest() essentially inherits the dependencies
// of class Bar which includes 'foo'

// nest function definition
fun void nest()
{
    // causes nest to inherit Bar's dependencies
    Bar bar;
}

// should not work here, since nest() needs class Bar
// which needs 'foo', but nest() is called before 'foo'
// initializes
nest();

// initialize
5 => float foo;

// class definition
class Bar
{
    <<< foo >>>;
}