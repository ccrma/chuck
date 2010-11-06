// transform design
// am, gw

sinosc s => dac => x => y;

s ==> rdwt wt == peakfinder ==> x ==> irfft => dac;

wt.coef( level, num );
ft.bin(0).mag;
ft.bin(0).phase;
dct.bin(0).mag;

rfft / rifft
cfft / cifft
rdwt / ridwt
cdwt / cidwt
dct / idct
fht / ifht
