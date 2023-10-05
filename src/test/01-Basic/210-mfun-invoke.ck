// test calling chuck-side member functions from c++

// instance of our event
TheEvent e;

// custom Event class
class TheEvent extends Event
{
    // override the waiting_on() function, which is called by the VM
    // when a shred has begun to wait on an Event
    fun void waiting_on()
    {
        <<< "success" >>>;
        e.broadcast();
    }
}

// this shred will wait on event
e => now;

