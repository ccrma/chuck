
Bar.sfoo();

Bar b;

10 => b.mvarInt;
b.printMvarInt();
<<< b.mvarInt >>>;

-2.2 => b.mvarFloat;
b.printMvarFloat();
<<< b.mvarFloat >>>;

15::second => b.mvarDur;
b.printMvarDur();
<<< b.mvarDur >>>;

now+10::second => b.mvarTime;
b.printMvarTime();
<<< b.mvarTime >>>;

"hiya" => b.mvarString;
b.printMvarString();
<<< b.mvarString >>>;

b.mfoo();
b.intArg(20);
b.floatArg(1.2);
b.durArg(20::second);
b.timeArg(now+5::second);
b.stringArg("howdy");
b.intFloatStringArg(-5, 1.2, "whatup");

