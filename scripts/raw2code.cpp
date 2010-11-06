#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


int main( int argc, char ** argv )
{
    for( int i = 1; i < argc; i++ )
    {
        ostringstream sout;
        
        argv[i][strlen(argv[i])-4] = '\0';
        sout << "hexdump -bv " << argv[i] << ".raw | hex2cpp " << argv[i] << " > " << argv[i] << ".c++";
        system( sout.str().c_str() );
        cout << sout.str() << endl;
    }
}

