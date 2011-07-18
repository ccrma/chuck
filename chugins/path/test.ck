
test(Path.basename("/Users/spencer/somefile.ck"), "somefile.ck", "Path.basename(\"/Users/spencer/somefile.ck\")");
test(Path.basename("/Users/spencer/"), "", "Path.basename(\"/Users/spencer/\")");

test(Path.dirname("/Users/spencer/somefile.ck"), "/Users/spencer/", "Path.dirname(\"/Users/spencer/somefile.ck\")");
test(Path.dirname("somefile.ck"), "", "Path.dirname(\"somefile.ck\")");

test(Path.extension("/Users/spencer/somefile.ck"), "ck", "Path.extension(\"/Users/spencer/somefile.ck\")");
test(Path.extension("/Users/spencer/somefile"), "", "Path.extension(\"/Users/spencer/somefile\")");
test(Path.extension(".bashrc"), "", "Path.extension(\".bashrc\")");

<<< "All tests passed.", "" >>>;

fun void test(string a, string b, string name)
{
    if(a != b)
    {
        <<< "test '" + name + "' failed: " + a + " != " + b, "" >>>;
        me.exit();
    }
}
