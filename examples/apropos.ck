// name: apropos.ck
//       (available in chuck-1.4.1.0 and higher)
// desc: example showing .apropos(), which can be invoked on
//       any object or class; .apropos() dynamically generates
//       information regarding the underlying type of an object
//       or a class and outputs this information to the console.
//       the information includes the inheritance chain of the
//       type in question, its functions as well as functions
//       inherited from its parent (and its parent's parent, and
//       so on). .apropos can be a helpful on-the-fly learning 
//       tool.

// an object/ugen instance
SinOsc osc;
// call apropos() on it to learn about its underlying information
osc.apropos();

// apropos() can be called directly on classes without instances
Machine.apropos();

// 2-d int array
int a[10][10];
// calling apropros on a 2d-array
a.apropos();
// calling apropros on an array
a[0].apropos();

// a user-created class
public class Foo
{
    fun void bar()
    { <<< "hi" >>>; }
}
// calling apropos() on the class
Foo.apropos();
