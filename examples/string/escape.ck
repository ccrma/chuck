// escape sequences

// newline
<<< "line1\nline2", "\n" >>>;

// tab
<<< "a\tb", "\n" >>>;

// audible bell (uh...)
<<< "bell!\a", "\n" >>>;

// "
<<< "->\"<-", "\n" >>>;

// end of string
<<< "here\0can't see me", "\n" >>>;

// wait a bit
200::ms => now;

// octal (man ascii)
<<< "bell!\007\060---\061\012", "" >>>;
