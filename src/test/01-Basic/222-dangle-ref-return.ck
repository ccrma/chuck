// test cleaning up dangling references
// code structure borrowed from Michael Heuer's LicK library

class Procedure // or Runnable
{
    fun void run()
    {
        // empty
    }
}

class Append extends Procedure
{
    Procedure g;
    Procedure h;
    
    fun void run()
    {
        g.run();
        h.run();
    }
}

class SleepProcedure extends Procedure
{
    Sleep @ sleep;
    
    fun void run()
    {
        sleep.sleep();
    }
}

class Sleep
{
    0::ms => dur value;
    SleepProcedure _procedure;    

    {
        this @=> _procedure.sleep;
    }
    
    fun void sleep()
    {
        value => now;
    }
    
    fun Procedure asProcedure()
    {
        return _procedure;
    }
}


public class Loops
{
    0::ms => static dur none;
    // ChucK bug:  this value appears not to be set as static
    52::week => static dur forever;
    
    fun static Procedure append(Procedure g, Procedure h)
    {
        Append app;
        g @=> app.g;
        h @=> app.h;
        return app;
    }
    
    fun static Procedure append(Procedure g, dur wait)
    {
        Sleep sleep;
        wait => sleep.value;
        return append(g, sleep.asProcedure());
    }
}

// a procedure
Procedure g;
// call a function that returns an Object
Loops.append(g, 64::ms) @=> Procedure p;

// if we got here
<<< "success" >>>;
