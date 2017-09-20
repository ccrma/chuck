// class with function taking class as argument

class Other
{ 
    10 => int x;
    int y;
}

class X
{ 
    int x;
    int y;

    public void set( Other a )
    { 
        a.x => x;
        a.y => y;
    }
} 

// instantiate
Other a;
X x;

// call the function
x.set(a);

// test
if( a.x == 10 ) <<<"success">>>;
