fun void hi() { chout <= "1" <= IO.newline(); }

spork ~ hi();
chout <= "0" <= IO.newline();
spork ~ { chout <= "2" <= IO.newline(); };

1::samp => now;
