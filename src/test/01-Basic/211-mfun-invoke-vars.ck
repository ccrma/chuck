// verify a system-invoked member function can properly access
// variables at different relative scopes

// event
EventFoo e;

// file-scope
1 => int fileVar;
// global
2 => global float globalVar;

// class def
class EventFoo extends Event
{
    // member variable
    3 => int memberVar;
    // static variable
    4 => static int staticVar;

    // VM calls this special function when a shred waits on events
    fun void waiting_on()
    {
        // print different scopes
        <<< fileVar, globalVar, memberVar, staticVar >>>;
        // this should reactivate the shred
        this.broadcast();
    }
}

// wait to trigger the waiting_on()
e => now;
