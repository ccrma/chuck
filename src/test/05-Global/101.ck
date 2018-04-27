class myEvent extends Event
{
    int myInt;
}

global Event e;
fun void foo()
{
    global myEvent e;
}