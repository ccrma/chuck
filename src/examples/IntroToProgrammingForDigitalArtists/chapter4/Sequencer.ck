// Sound Network 
SndBuf click => dac;
SndBuf kick => dac;

// Setup Sound Files
"sounds/kick_01.wav" => kick.read; // Open kick file
"sounds/click_05.wav" => click.read; // Open snare file
// take away playback at beginning
kick.samples() => kick.pos;
click.samples() => click.pos;

// Global Variables
[1,0,0,0,1,0,0,0] @=> int kick_ptrn_1[];
[0,0,1,0,0,0,1,0] @=> int kick_ptrn_2[];
[1,0,1,0,1,0,1,0] @=> int click_ptrn_1[];
[1,1,1,1,1,1,1,1] @=> int click_ptrn_2[];

// Functions
fun void Section(int KickArray[], int ClickArray[], float beattime)
{
    // Sequencer:: for Bass Drum and Snare Drum Beats
    for (0 => int i; i < KickArray.cap(); i++)
    {
        if (KickArray[i] == 1) // if 1 in Array than:
        {
            0 => kick.pos; // play kick
        }
        if (ClickArray[i] == 1) // If 1 in Array than:
        {
            0 => click.pos;
        }
        beattime::second => now;
    }
}

// Main Program
while(true)
{
    // Procedural: ABA form
    Section(kick_ptrn_1, click_ptrn_1, .2);
    Section(kick_ptrn_2, click_ptrn_2, .2);
    Section(kick_ptrn_1, click_ptrn_1, .4);
}
