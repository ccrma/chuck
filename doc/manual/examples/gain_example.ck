        Noise n => Gain g => dac;  
        SinOsc s => g;  
        .3 => g.gain;  
        while( true ) { 100::ms => now; } 
