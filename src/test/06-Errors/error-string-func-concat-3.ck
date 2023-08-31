// error case: check for [function] + string; no class def
// https://github.com/ccrma/chuck/issues/162

// a function
fun void foo()
{ }

// function LHS position
foo + "";

