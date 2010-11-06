public class human
{
    8 => float weight;
    0 => int degrees;	

    Event Birthday;

    fun void life()
    {
        while(true)
        {
            dur => now;
            Adam_Tindale.Birthday.signal();
            Adam_Tindale.weight() * 1.5 => Adam_Tindale.weight;
        }
    }
}

class male extends human
{
    public time born(){
        return now;
    }
    // fun int study(string school){
    //     return degrees++;
    // }
    fun string study(string school, string program, int level, int completed){
        degrees++;
        if (level == 0)
            return "Undergraduate Degree";
        if (level == 1)
            return "Masters Degree";
        if (level == 2)
            return "Doctor of Philosophy";
    }
    fun void party(Event e){
        while(true){
            e => now;
            if ( now % 10::year != 0)
                <<<"It is your Birthday!!!!">>>;
            else
                <<<"No, it isn't my birthday...">>>;
        }
    }
}

male Adam_Tindale;

Adam_Tindale.born() => time birth;
spork ~ life() => Shred lifetime;

Adam_Tindale.study("LougboroughPublicSchool");
Adam_Tindale.study("BayridgePublicSchool");
Adam_Tindale.study("BayridgeSecondarySchool");
Adam_Tindale.study("FrontenacSecondarySchool");	
<<< "Public Schools Attended: ",degrees >>>;

<<< Adam_Tindale.study("QueensUnivsersity","MusicPerformance",0,true) >>>;	
<<< Adam_Tindale.study("McGillUniversity","MusicTechnology",1,true) >>>;
// still working on this one
<<< 
Adam_Tindale.study("UnivsersityOfVictoria","InterdisciplinaryStudies",2,false) 
>>>;

spork ~ party(Adam_Tindale.birthday);

//born 1978/05/10
//today 2005/09/30
27::year + 4::month + 20::day => now;

//you never know...
( birth + std.rand2f(0., 80.) )::year => time death;

//it is going to happen!
death => now;
machine.remove( lifetime.id() );
me.exit();
