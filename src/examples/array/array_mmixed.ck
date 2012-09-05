// based on Robin Haberkorn's example
int a[0][2];

// instantiate arrays
new int[2] @=> a["foo"];
new int[2] @=> a["bar"];

// fill them
23 => a["foo"][0];
24 => a["foo"][1];
32 => a["bar"][0];
42 => a["bar"][1];

// test them
if( a["foo"][0] == 23 && a["bar"][1] == 42 )
    <<< "success" >>>;

