Event e;
Event e2;

spork ~ e.signal();

select {
    case e2: <<< "failure" >>>;
    case e: {
        <<< "hello" >>>;
        <<< "there" >>>;
        <<< "yes" >>>;
    }
}

<<< "done" >>>;