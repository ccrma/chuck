// instantiate int array
float argh[0];

// print out relevant info
<<< "array size:", argh.size() >>>;

// append items (array should grow dynamically as needed)
argh << 3.0 << 4 << 5;

// print out relevant info
<<< "array size:", argh.size() >>>;

// print out elements
<<< "contents:", argh[0], argh[1], argh[2] >>>;

// pop last element
argh.popBack();

// print out relevant info
<<< "array size:", argh.size() >>>;

argh << 6 << 7 << 8;

// pop middle element
argh.popOut(2);

// print array
<<< "printing remaining elements of array..." >>>;
for (0 => int i; i < argh.size(); i++) {
    <<< "elem", i, "-", argh[i] >>>;
}
