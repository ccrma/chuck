// unit test for dependency through transitive object instance
// i.e., in this case, nest() essentially inherits the dependencies
// of class Bar which includes 'foo'

// nest function definition
fun void nest()
{
    // causes nest to inherit Bar's dependencies
    new Bar @=> Bar @ bar;
}

// initialize
5 => float foo;

// should work here, since nest() needs class Bar,
// which needs 'foo', but nest() is called after 'foo'
// initializes
nest();

// class definition
class Bar
{
    // this should auto-run upon Bar instantiation
    if( foo == 5 ) <<< "success" >>>;
}