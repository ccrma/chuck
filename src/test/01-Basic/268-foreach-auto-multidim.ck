// tests 1) nested foreach with 2) auto
// unit test was introduced in 1.5.4.4
// related: https://github.com/ccrma/chuck/issues/441

// a multidimensional array
[[ 1, 2], [3, 4, 5], [6], [7, 8]] @=> int grid[][];

// nest foreach loops with auto
for( auto arr: grid )
{
    for( auto val: arr )
    {
        <<< val >>>;
    }
}
