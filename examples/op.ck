// doesn't get much more pointless than this

"" => stdout;
"arithmetic" => stdout;
1 + 2 => stdout;
1 - 2 => stdout;
2 - 1 => stdout;
3 * 5 => stdout;
6 / 3 => stdout;
6.4 / 3.2 => stdout;
10%6 => stdout;
6 % 10 => stdout;
-12 => stdout;
1 + -12 => stdout;
1 => int a;
a++ => stdout;
a++ => stdout;
++a;
a => stdout;
1::second + 10::ms => stdout;
1::second - 10::ms => stdout;
1::second / 10::ms => stdout;
1::second * 10.0 => stdout;

"" => stdout;
"binary" => stdout;
2 | 1 => stdout;
1 | 1 => stdout;
3 & 1 => stdout;
1 ^ 2 => stdout;
~4 => stdout;

"" => stdout;
"logic" => stdout;
!!1 => stdout;
!0 => stdout;
1 || 0 => stdout;
!(1 && 0) => stdout;
!(0 || 0) => stdout;
1 && 1 => stdout;
(1 == 1 ) => stdout;
(1 == 1 ) && 1 => stdout;
(3.0 >= 4.0) || (4.0 > 3.0) => stdout;
(1 < 2) => stdout;
(1 <= 1) => stdout;
(1 != 0) => stdout;

"" => stdout;
"precedence" => stdout;
1 + 2 * 3 => stdout;
1 * 2 + 3 => stdout;
(1+2)*3 => stdout;
3 * 2 + 1 => stdout;
3 * 2 / 3 => stdout;
1 + 1 == 2 => stdout;

