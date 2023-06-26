// name: int-dist.ck
// desc: testing int random distribution over range

// how many buckets
10 => int N;

// buckets
int count[N];

// how many times
repeat( 100000 )
{
    // which bucket?
    count[Math.random2(0,N-1)]++;
}

// go over buckets
for( int i; i < N; i++ )
{
    // print bucket contents
    cherr <= count[i] <= IO.nl();
}