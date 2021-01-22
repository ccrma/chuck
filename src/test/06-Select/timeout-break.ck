Event e;

select {
    case e: <<< "shouldn't have caught event" >>>;
    case 1::ms: {
        <<< "success" >>>;
        break;
        <<< "failure" >>>;
    }
}