//--------------------------------------------------------------------
// name: expr.ck
// desc: function with an expression in the default args position
//
// author: Jack Atherton (http://www.jack-atherton.com/)
//   date: fall 2016
//--------------------------------------------------------------------

now => time start;

fun void elapsed_bottles( (now - start) / 1::second => float bottles )
{
    chout <= bottles <= " bottles of water in the fridge" <= IO.newline();
}

// provide number of bottles manually
elapsed_bottles(50);

// or, rely on an automated expression to provide it for you
elapsed_bottles();
1::second => now;
elapsed_bottles();
2.5::second => now;
elapsed_bottles();
