// Listing 11.5 Receive lines of strings from an Arduino via a SerialIO object

SerialIO cereal;                      // (1) Declares a Serial Input/Output object 
cereal.open(0, SerialIO.B9600, SerialIO.ASCII);  // (2) Opens it with specific parameters

while(true)
{
    cereal.onLine() => now;           // (3) Sleeps until Serial object has something
    cereal.getLine() => string line;  // (4) Gets a line of text from it
    chout <= "line: " <= line;        // (5) Prints that out
}


