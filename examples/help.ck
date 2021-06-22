// name: help.ck
//       (available in chuck-1.4.1.0 and later)
// desc: example showing .help(), which can be invoked on
//       any object or class; .help() dynamically generates
//       information regarding the underlying type of an object
//       or a class and outputs this information to the console.
//       the information includes the inheritance chain of the
//       type in question, its functions as well as functions
//       inherited from its parent (and its parent's parent, and
//       so on).
//
// .help() is designed to be a run-time teaching/learning tool
//       and a quick referencewhile programming. It's fun, sort of!

// an object/ugen instance
SinOsc osc;
// call help() on it to learn about its underlying information
osc.help();

// help() can be called directly on classes without instances
Machine.help();

// 2-d int array
int a[10][10];
// calling apropros on a 2d-array
a.help();
// calling apropros on an array
a[0].help();

// a user-created class
public class Foo
{
    fun void bar()
    { <<< "hi" >>>; }
}
// calling help() on the class
Foo.help();
