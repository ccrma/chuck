// Listing 8.10 Jazz band:
// save as "initialize.ck"
// Important things will go here soon,
// like Classes, which we learn about in the next chapter
// for now, we just add our score.ck

me.dir() + "/score.ck" => string scorePath;

Machine.add(scorePath);  // (1) Adds score file for jazz band
