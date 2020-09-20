        Step s => dac;  
        -1.0 => float amp;  
        // square wave using step  
        while( true ) {      
	        -amp => amp => s.next;      
                800::samp => now;  
        } 
