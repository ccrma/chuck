[1,2,3,4,5,6] @=> int a[];
[1,2,3,4,5,6] @=> int b[];

b.reverse();

// test b is the reverse of a
for (int i; i < a.size(); i++) {
    if (a[i] != b[-i-1]) me.exit();
}

<<< "success" >>>;
