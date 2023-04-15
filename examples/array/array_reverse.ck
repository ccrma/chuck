// example reversing the order of an array
// version: need chuck-1.5.0.0 or higher

// an array
[1,2,3,4,5] @=> int a[];

// print array in-order
print_arr(a);

// reverse
a.reverse();

// print reversed order
print_arr(a);

// return to original order
a.reverse();
print_arr(a);


fun void print_arr(int a[]) {
    for (0 => int i; i < a.size(); i++) {
        cherr <= a[i] <= " "; 
    }
    cherr <= IO.nl();
}
