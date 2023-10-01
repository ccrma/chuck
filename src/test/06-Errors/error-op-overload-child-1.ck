// test: this should compiler error, since SinOsc is a child of UGen
// and => is already defined for UGen => UGen
fun void @operator =>( SinOsc lhs, SinOsc rhs )
{
    <<< "cannot get here" >>>;
}
