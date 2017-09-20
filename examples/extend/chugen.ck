// ChuGen
// Create new UGens by performing audio-rate processing in ChucK

class Fuzz extends Chugen
{
    1.0/2.0 => float p;
    
    2 => intensity;
    
    fun float tick(float in)
    {
        Math.sgn(in) => float sgn;
        return Math.pow(Math.fabs(in), p) * sgn;
    }
    
    fun void intensity(float i)
    {
        if(i > 1)
            1.0/i => p;
    }
}

adc => Fuzz f => dac;
2.5 => f.intensity;

while(true) 1::second => now;
