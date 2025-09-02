// name: platform.ck
// desc: getting the operating system name + OS-provided time
// author: Andrew Zhu Aday
// date: summer 2025

// print the underlying operating system name as a string;
// possible return values of Machine.os():
//   "mac", "linux", "windows", "web", "ios", "android", "unknown"
<<< "underlying operating system:", Machine.os() >>>;

// print the operating system-provided absolute time as a float
// in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC);
// the returned time has microsecond resolution (and could be
// used for e.g. synchronizing events across machines without
// the use of networking)
<<< "machine time:", Machine.timeOfDay() >>>;

// identical to Machine.timeOfDay(), only the return value is a 
// vec2 where x is time in seconds, and y is fractional time in
// microseconds; y is guaranteed to be between 0 and 1,000,000;
// x + y yields the total time. useful for situations needing less
// than 64-bit precision e.g., floats in OSC are 32-bit and one
// must send the x and y values separately -- otherwise a
// significant amount of timing resolution will be lost
<<< "machine time (added precision):", Machine.timeOfDay2() >>>;
