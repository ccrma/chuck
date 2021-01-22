Event e;

spork ~ e.signal();

select {
    case e: <<< "success" >>>;
}