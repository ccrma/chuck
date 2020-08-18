// use a variety of arithmetic chuck operators 
// to directly modify a value
2 => int i;
<<<i>>>;

5 +=> i; // 7
<<<i>>>;
3 -=> i; // 4
<<<i>>>;
12 *=> i; // 48
<<<i>>>;
3 /=> i; // 16
<<<i>>>;
7 |=> i; // 10000 | 00111 = 10111 = 23
<<<i>>>;
14 &=> i; // 10111 & 01110 = 00110 = 6; 
<<<i>>>;
31 ^=> i; // 00110 ^ 11111 = 11001 = 25;
<<<i>>>;
if ( i == 25 ) <<<"success!">>>;
