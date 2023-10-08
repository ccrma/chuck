// verify passthrough Chugraph
class Node extends Chugraph
{
    // connect inlet to output (passthrough)
    inlet => outlet;
}

// patch in the Node
Step step => Node node => blackhole;
// set step
.5 => step.next;
// let time pass
1::samp => now;

// check if equal (or at least equal enough)
if( Math.equal(.5, node.last() ) )
   <<< "success" >>>;
