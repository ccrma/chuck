int glob;

spork ~ {
    glob++;
    
    int zero;
    int one;
    int three;
    
    zero++;
    zero--;
    
    spork ~ {
        int two;
        one++;
        
        spork ~ {   
            spork ~ {
                2 +=> two;
                three++;
                3 *=> three;

            };
            1::samp => now;
        };
        2::samp => now;
        <<< "two?", two >>>; 
        
    };
    3::samp => now;
    <<< "zero?", zero >>>;
    <<< "one?", one >>>;
    <<< "three?", three >>>;
};
4::samp => now;
<<< "global one?", glob >>>;

