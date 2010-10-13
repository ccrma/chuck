// shows getting command line arguments
//    (example run: chuck args:1:2:foo)

// print number of args
<<< "number of arguments:", me.args() >>>;

// print each
for( int i; i < me.args(); i++ )
{
    <<< "   ", me.arg(i) >>>;
}
