// example of operator overloading for custom class

// a custom Number class
class Number
{
    float num;
}

// overload binary operator +
fun Number @operator + ( Number lhs, Number rhs )
{
    new Number @=> Number @ n;
    lhs.num + rhs.num => n.num;
    return n;
}

// overload binary operator *
fun Number @operator * ( Number lhs, Number rhs )
{
    new Number @=> Number @ n;
    lhs.num * rhs.num => n.num;
    return n;
}

// overload postfix operator ++
fun Number @operator ( Number n ) ++
{
    1 +=> n.num;
    return n;
}

// 2 Numbers
Number A; 1 => A.num;
Number B; 2 => B.num;
// print
<<< (A + B).num >>>;

// add
A + B @=> Number C;
// print
<<< C.num >>>;

// increment
C++;
// print
<<< C.num >>>;

// a more complicated expression
(A+B)*(A+B) @=> Number D;
// print
<<< D.num >>>;
