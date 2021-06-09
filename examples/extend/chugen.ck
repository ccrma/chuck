// name: chugen.ck
// name: "chu-gen" -- rhymes with "UGen"
//      create new UGens by performing audio-rate processing in ChucK

// a chugen!
class Fuzz extends Chugen
{
    // p
    1.0/2.0 => float p;
    // intensity
    2 => intensity;

    // single-sample tick function (ChuGen specialty)    
    fun float tick( float in )
    {
        Math.sgn(in) => float sgn;
        return Math.pow(Math.fabs(in), p) * sgn;
    }
    
    // set intensity
    fun void intensity( float i )
    {
        if(i > 1) 1.0/i => p;
    }
}

// patch it in!
adc => Fuzz f => dac;
// set intensity
2.5 => f.intensity;
// advance time
while(true) 1::second => now;
