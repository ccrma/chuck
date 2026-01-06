
// if no superclass specified, implicitly extend Object
class Child
{
    fun void test()
    {
        // should yield compile-time error since super class
        // 'Object' has no test() finction
        super.test();
    }
}