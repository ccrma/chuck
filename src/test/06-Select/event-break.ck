Event e;

spork ~ e.signal();

select {
    case e: {
        <<< "success" >>>;
        break;
        <<< "didn't break" >>>;
    }
    case 1::ms: <<< "shouldn't have timed out" >>>;
}