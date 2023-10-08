// verify basic Chugen passthru
class PassThru extends Chugen
{
    fun float tick(float v)
    {
        return v;
    }
}

// patch in the Node
Step step => PassThru node => blackhole;
// set step
.5 => step.next;
// let time pass
1::samp => now;

// check if equal (or at least equal enough)
if( Math.equal(.5, node.last() ) )
   <<< "success" >>>;
