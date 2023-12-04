// verify array pop front with objects

// class
class Foo
{
    int num;
    fun @construct(int x) { x => num; }
}

// array
Foo arr[0];
// objects
arr << new Foo(1) << new Foo(2);
// pop front
arr.popFront();
// test
if( arr[0].num == 2 ) <<< "success" >>>;


