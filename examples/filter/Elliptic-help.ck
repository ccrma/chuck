/////////////////////////////////////////////////////////////
// Elliptic                                                //
//                                                         //
// Elliptic implements cascaded IIR filters and a built-in //
// elliptical filter design. It is capable of low-pass,    //
// high-pass, or bandpass filtering with very steep        //
// slopes.                                                 //
//                                                         //
// The atten determines how much of the signal is          //
// attenuated outside the passband.                        //
//                                                         //
// Elliptic filters come with a trade-off for their steep  //
// slopes: they have a certain amount of ripple outside    //
// the passbands. A very small ripple (0.1 or 0.2 dB)      //
// produces very little ringing, whereas a large ripple    //
// (eg. 20 dB) produces a very strong harmonic ring.       //
//                                                         //
// The filter design algorithm sometimes can't fulfill the //
// design criteria -- a particular combination of cutoff   //
// frequencies, ripple, and attenuation. If that happens,  //
// the user is warned that the filter is not initialized,  //
// and the filter is bypassed. This may happen, for        //
// instance, if you ask for a very steep attenuation with  //
// very low ripple.                                        //
/////////////////////////////////////////////////////////////

// Settings
//
// atten (float): filter attenuation in dB. Default 90.0
// ripple (float): filter ripple in dB. Default 0.2
// bypass (int): switch bypass on or off. Default 0
//
// Before using the filter, you must initialize it as a
// low-pass (lpf), high-pass (hpf), or band-pass filter and
// designate the passband and stopband frequencies.
//
// lpf (passband Hz, stopband Hz): create a low-pass filter
// hpf (stopband Hz, passband Hz): create a high-pass filter
//
// bpf (lower passband, upper passband, stopband):
//   (usually these parameters are ascending values.)
//

Noise n => Elliptic ell => dac;
0.1 => n.gain;

80 => ell.atten; // 80 dB attenuation
10 => ell.ripple; // 10 dB ripple will cause ringing

ell.bpf(500,600,650); // create band-pass filter which
                      // passes the range 500-600 Hz
                      // unattenuated, then slopes from
                      // 600-650 Hz (and 450-500 Hz)

true => ell.bypass; // bypass filter
2::second => now;

false => ell.bypass; // activate filter
minute => now;