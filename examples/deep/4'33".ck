// 4'33" by John Cage
// Please reduce buffer size as much as possible
// Running in terminal? chuck --bufsize:16 4\'33\".ck
// PiM 10/12/25

//connect audio input to output
adc => dac;
//advance time by 4'33"
4::minute + 33:second => now;
//disconnect audio with the UnChucK operator
//adc =< dac;
adc !=> dac;