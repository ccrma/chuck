// Test Std.range functionality.

[0, 1, 2, 3] @=> int want[];
Std.range(4) @=> int got[];

if (want.size() != got.size()) {
    <<< "want[] size different from got[] size" >>>;
    me.exit();
}

for (int i; i < want.size(); i++) {
    if (want[i] != got[i]) {
        <<< "want[", i, "is different from got[", i, "]" >>>;
        me.exit();
    }
}

// Test empty
Std.range(0) @=> got;

if (got.size() != 0) {
    <<< "Std.range(0) does not return an empty array" >>>;
    me.exit();
}

// Test start/stop range
[3,4,5,6] @=> want;
Std.range(3,7) @=> got;

if (want.size() != got.size()) {
    <<< "want[] size different from got[] size" >>>;
    me.exit();
}

for (int i; i < want.size(); i++) {
    if (want[i] != got[i]) {
        <<< "want[", i, "is different from got[", i, "]" >>>;
        me.exit();
    }
}

// validate empty
// Test empty
Std.range(700,-4) @=> got;

if (got.size() != 0) {
    <<< "Std.range(0) does not return an empty array" >>>;
    me.exit();
}

<<< "success" >>>;
