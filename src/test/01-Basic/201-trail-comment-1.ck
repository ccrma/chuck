// unit test for single-line comments followed by end-of-file
//
// verified: was crashing when the code was parsed by ck compiler
//           using flex/bison through yy_scan_string() -- e.g.,
//           how `Machine.eval()` is parsed or originating from
//           a code buffer in miniAudicle or WebChucK IDE
//
//           issue seems related to the `comment_hack` macro
//           in chuck.lex; we have since moved to a different
//           way of handling single-line comments; this checks
//           it still works!
//
// https://github.com/ccrma/chuck/issues/366

<<< "success" >>>; // 