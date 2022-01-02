// name: array_capacity.ck
// desc: this demonstrates how array.size() array.capacity() works;
//       added: 1.4.1.0 (ge)
//
//       (NOTE: .capacity() is not the same as .cap(); the latter is
//       maintained for historical / compatibility reasons; .cap()
//       actually is the same as .size() -- which is rather confusing!
//
//       It is strongly recommended that .cap() be avoided; instead,
//       explicitly use .size() or .capacity() as appropriate.)

// declare an array
// (size = array length; capacity >= size)
int a[4];
// print size and capacity
print(1);

// reset: clear array
// (size is reset to 0; capacity is reset to at least 8)
a.reset();
// print size and capacity
print(2);

// append to array
// (size should reflect the number of elements appended)
a << 1 << 2 << 3 << 5 << 8;
// print size and capacity
print(3);

// resize to 15
// (this changes both size to 15 and capacity to at least 15)
15 => a.size;
// print size and capacity
print(4);

// append to array
// (size should reflect the number of elements appended)
a << 13 << 21 << 34;
// print size and capacity
print(5);

// *reserve* capacity
// (since this is less than current capacity,
// neither size nor capacity is actually changed)
10 => a.capacity;
// print size and capacity
print(6);

// *reserve* a larger capacity
// (this should result in a larger capacity without changing size)
50 => a.capacity;
// print size and capacity
print(7);

// helper print function
fun void print( int iteration )
{
    <<< iteration, "| array size:", a.size(), 
        "capacity:", a.capacity() >>>;
}
