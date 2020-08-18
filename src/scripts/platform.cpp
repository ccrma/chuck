// name: platform.cpp
// desc: finds sizes
// author: Ge Wang (gewang@cs.princeton.edu)

#include <iostream>
using namespace std;

// union
union X
{
    short s;
    char  b[2];
};

// entry point
int main( int argc, char ** argv )
{
    X x;
    x.s = 0x0100;

    // go
    cout << "char: " << sizeof(char) << endl;
    cout << "short: " << sizeof(short) << endl;
    cout << "int: " << sizeof(int) << endl;
    cout << "long: " << sizeof(long) << endl;
    cout << "float: " << sizeof(float) << endl;
    cout << "double: " << sizeof(double) << endl;
    cout << "void *: " << sizeof(void *) << endl;
    cout << "endian: " << ( x.b[1] ? "small" : "big" ) << endl;

    return 0;
}
