// #2024-func-call-update verification

// test static function
Machine.refcount;

// check for stack overflow
repeat(1000000) 
    Machine.refcount;

// if we get here, ok
<<< "success" >>>;
