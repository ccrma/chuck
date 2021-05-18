// 55.ck : arrays and functions

// return new array
fun int[][] foo()
{
    return [[1,2],[3,4]];
}

// return new array
fun int[] bar()
{
    return new int[3];
}

// call
bar() @=> int b[];

// test
if( foo()[1][1] == 4 && b.size() == 3 ) <<<"success">>>;
