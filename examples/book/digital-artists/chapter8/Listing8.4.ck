//  Listing 8.4 Using Machine.add, Machine.remove, and Machine.replace

// Example of using Machine functions
// by Sporkie Griswold, June 2014

                           // (1) Add foo/bar program from listing 8.1...
Machine.add(me.dir()+"/Listing8.1.ck") => int foobar;
2.0 :: second => now;
                           // (2) ...then add2 code of listing 8.2...
Machine.add(me.dir()+"/Listing8.2.ck") => int drumnbass;
6.0 :: second => now;
                           // (3) ...add 8.3 code (modalbend program)...
Machine.add(me.dir()+"/Listing8.3.ck") => int modalbend;
4.0 :: second => now;
                           // (4) ...remove 8.3...
Machine.remove(modalbend);
2.0 :: second => now;
                           // (5) ...replace foobar with modalbend code...
Machine.replace(foobar,me.dir()+"/Listing8.3.ck") => modalbend;
4.0 :: second => now;
Machine.remove(modalbend); // (6) ...then remove it.
