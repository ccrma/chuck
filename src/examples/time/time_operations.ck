// ...

// store the current time
now => time t1; 

//value to duration
0.5::second => dur d; 
<<<"one">>>;
//now waits for that duration 
d => now;
<<<"two">>>;

//define new duration
d * 4.0 => dur four_d; 

//wait again. 
four_d => now;
<<<"three">>>;

now => time t2;

t2 - t1 => dur span;
<<<"total">>>;
<<<span>>>;
<<<"relative">>>;
<<<span / d>>>;

