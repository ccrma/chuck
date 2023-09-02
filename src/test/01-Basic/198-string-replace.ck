// testing string.replace( from, to )
// added 1.5.1.3 (nshaheed)

// Replace something
"Hello, world!" => string replaced;
replaced.replace("world", "Chuck");

if (replaced != "Hello, Chuck!") {
   <<< "Expected \"Hello, Chuck!\", got", replaced >>>;
   me.exit();
}

// Replace multiple
"Hello, the world! Hello, the world! Hello, the world! Hello, the world!" => replaced;
replaced.replace("world", "Chuck");

if (replaced != "Hello, the Chuck! Hello, the Chuck! Hello, the Chuck! Hello, the Chuck!") {
   <<< "Expected \"Hello, the Chuck! Hello, the Chuck! Hello, the Chuck! Hello, the Chuck!\", got", replaced >>>;
   me.exit();
}

// Replacement to empty string
"Replace me!" => replaced;
replaced.replace("me", "");

if (replaced != "Replace !") {
   <<< "Expected \"Replace !\", got", replaced >>>;
   me.exit();
}

// Replacement from empty string
"Replace me!" => replaced;
replaced.replace("", "me");

if (replaced != "Replace me!") {
   <<< "Expected \"Replace me!\", got", replaced >>>;
   me.exit();
}

// No match
"No replacement here!" => replaced;
replaced.replace("ChucK", "NotChucK");

if (replaced != "No replacement here!") {
   <<< "Expected \"No replacement here!\", got", replaced >>>;
   me.exit();
}

// done
<<< "success" >>>;
