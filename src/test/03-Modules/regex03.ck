
string matches[0];
if(RegEx.match("\\[([0-9]+), ([0-9]+), ([0-9]+)\\]", "[1, 2, 3]", matches))
{
    if(matches.cap() == 4 && matches[1] == "1" && matches[2] == "2" && matches[3] == "3")
        <<< "success" >>>;
}
