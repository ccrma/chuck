Event e;

spork ~ e.signal();

select {
    case e: <<< "success" >>>;
}

spork ~ e.signal();

select {
    case e: <<< "success again" >>>;
}
