class Foo {}

Foo foo;
Foo@ null_foo;
SinOsc@ null_osc;
SinOsc osc;

fun void checkNull(Object@ o, string name) {
    if (o) <<< name, "is NOT null" >>>;
    // if (o) <<< name, "is NOT null" >>>;
    else <<< name, "is null" >>>;
}

checkNull(foo, "foo");
checkNull(null_foo, "null_foo");
checkNull(osc, "osc");
checkNull(null_osc, "null_osc");

fun Object@ getNullObj() { return null_osc; }
fun Object@ getObj() { return foo; }

if (getObj()) <<< "getObj is not null" >>>;
if (!getNullObj()) <<< "getNullObj is null" >>>;

if (osc) <<< "if (object)" >>>;
if (foo) <<< "if (user)" >>>;
if (!null_foo) <<< "!User" >>>;
if (!null_osc) <<< "!Object" >>>;

if (null_foo || foo) <<< "User || User" >>>;
if (osc || null_osc) <<< "Object || Object" >>>;
if (osc || null_foo) <<< "Object || User" >>>;
if (foo || null_osc) <<< "User || Object" >>>;

if (foo && foo) <<< "User && User" >>>;
if (osc && osc) <<< "Object && Object" >>>;
if (foo && osc) <<< "User && Object" >>>;
if (osc && foo) <<< "User && Object" >>>;

while (null_foo) {}
while (null_osc) {}
while (osc) { break; }
while (foo) { break; }
