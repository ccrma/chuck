Envelope e => dac;

1.0::second => e.duration;
<<< e.time() >>>;
<<< e.duration() >>>;

now + 2::second => time later;

//1 => e.keyOn;
e.keyOn();
while( now < later )
{
    <<< e.value() >>>;
    <<< now / second >>>;

    .1::second => now;
}
