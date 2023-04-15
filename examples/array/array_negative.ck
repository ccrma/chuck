// arrays can have negative indices!
// version: need chuck-1.5.0.0 or higher

// an array
[1,2,3,4] @=> int a[];

// same as a(s.size(()-1)
<<<  a[-1] >>>; // 4

// same as a(s.size(()-3)
<<< a[-3] >>>; // 2
