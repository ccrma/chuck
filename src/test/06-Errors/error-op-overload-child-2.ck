// test: this should compiler error, since SinOsc and NRev are
// children of UGen and => is already defined for UGen => UGen
fun void @operator =>( SinOsc lhs, NRev rhs )
{
    <<< "cannot get here" >>>;
}
