[ 1, 2, 3, 4, 5 ] @=> int a[];
[ 1, 2, 4, 5] @=> int a_expected[];

true => int success;

if (a.size() != 5) {
   <<< "array is wrong size" >>>;
   false => success;
}

a.popOut( 2 );

fun int compareArr(int got[], int want[]) {
    if (got.size() != want.size()) {
       <<< "arrays have different sizes, got:", got.size(), "want:", want.size() >>>;
       return false;
    }

    for (0 => int i; i < got.size(); i++) {
        if (got[i] != want[i]) {
	   <<< "incorrect values, want", want[i], "got", got[i] >>>;
	   return false;
	}
    }

    return true;
}

compareArr(a, a_expected) && success => success;

// validate that resize is working as expected
a << 6 << 7 << 8 << 9 << 10 << 11 << 12;

[ 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12 ] @=> a_expected;

compareArr(a, a_expected) && success => success;

// pop empty array
[ 1 ] @=> int b[];

b.popOut(0);
b.popOut(0);
b.popOut(1);

if (success) <<< "success" >>>;
