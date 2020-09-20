// Listing 6.1 Connecting audio input to output using adc and dac

//  !!!!CAUTION CAUTION CAUTION CAUTION CAUTION!!!! //

//connect audio input to audio output through Gain UG
adc => Gain g => dac;

//let it run for 10 seconds
10.0 :: second => now;

//  !!!!CAUTION CAUTION CAUTION CAUTION CAUTION!!!! //
/*  Be extremely careful before you execute this
    code, making sure that there won' t be an 
    ear-splitting feedback loop (as can happen 
    when sound from your speakers gets back into 
    your microphone).  Plug in headphones, or turn 
    your speaker volume way down before hooking
    the adc to the dac (running this code). */
//  !!!!CAUTION CAUTION CAUTION CAUTION CAUTION!!!! //
