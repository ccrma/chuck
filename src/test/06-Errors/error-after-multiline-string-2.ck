// test error reporting after a multi-line string literal
// (without trailing one-line comment)
"
multi
line
chuck
string
" => string foo;

[1,2,3] => int bar[];
