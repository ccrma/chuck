now + 1::ms => time t1;
now + 2::ms => time t2;

select {
    case t1: <<< "success" >>>;
    case t2: <<< "failure" >>>;
}

t1 => time rn;

rn + 2::ms => t1;
rn + 1::ms => t2;

select {
    case t2: <<< "success" >>>;
    case t1: <<< "failure" >>>;
}