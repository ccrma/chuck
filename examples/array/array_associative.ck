// declare regular array (capacity doesn't matter so much)
float foo[4];

// use as int-based array
2.5 => foo[0];

// use as associative array with key "yoyo"
4.0 => foo["yoyo"];

// access as associative (print)
<<< foo["yoyo"] >>>;

// access empty element
<<< foo["gaga"] >>>;  // -> should print 0.0

// add a second associative element
44.0 => foo["yiyi"];

// get the array keys with all of the keys in the array
string keys[0];
foo.getKeys(keys);

<<< keys[0], keys[1] >>>;

// call .help() on an array to get more information about its type, methods, etc.
// foo.help();
