// test logical AND and OR for object references with and without !
// requres chuck-1.5.5.8 or higher

Object obj;

SinOsc foo;
ModalBar bar;

// 0
<<< obj && !obj >>>;

// 1
<<< obj || !obj >>>;

// 1
<<< obj && foo >>>;

// 0
<<< obj && !bar >>>;

// 1
<<< obj && (foo && bar) >>>;
