// Spherical Harmonics in the ChucK!
// ---------------------------------
// example by : everett m. carpenter

// init variables for ambisonics
2 => int order;
(order+1)*(order+1) => int n_channels;

// sound source
SndBuf homer; 
"special:dope" => homer.read;
homer.loop(1);

// direction & elevation
Math.random2f(0.0, 360.0) => float azimuth;
Math.random2f(0.0, 360.0) => float zenith;
Math.sh(order,azimuth,zenith) @=> float m_sh[];

// Ambisonic encoder block
Gain encoder[n_channels];
for(int i; i < n_channels; i++)
{
    encoder[i].gain(m_sh[i]/n_channels); // set gain of each ambisonic channel (normalizing gain here as well)
    homer => encoder[i]; // patch in homer
}

// channel 0 of the encoder is omnidirectional, volume is not dependent upon angular orientation, so we'll listen to that
encoder[0] => dac; 

// let's print out the harmonics just for fun
for(int i; i < m_sh.size(); i++)
{
    cherr <= "ACN: " <= i <= " // " <= m_sh[i] <= IO.newline();
}

while(true) // let homer sing
{
    5::second => now;
    me.exit();
}