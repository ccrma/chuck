// import statements in code evaluated by Machine.eval inherits
// the host file/code's path (otherwise it defaults to the working
// directory of the chuck host system)
Machine.eval("
  @import \"Foo.ck\"
  Foo a(1), b(2);
  <<< (a + b).num >>>;
");
