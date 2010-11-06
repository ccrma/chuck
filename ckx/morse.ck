// morse code - gluck keyboard event demo 
// june 2004 Philip Davidson ( philipd@alumni.princeton.edu )
//
//
//

"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);

//our variables

sinosc morsetone => dac;

880.0 => float thefreq;
0   => int thekey;
0   => int thedisp;
0.5 => float gain_up;
gain_up => float thegain;
0.0 => float followgain;

0.0 => morsetone.gain;
thefreq => morsetone.sfreq;

//event info
0   => int curid;
0   => int curtype;

//morse

function void dot() { 

	gain_up => thegain;
	1 => thedisp;
	40::ms => now;

	0.0 => thegain;
	0 => thedisp;
	40::ms => now;
}

function void dash() {

	gain_up => thegain;
	2 => thedisp; 
	120::ms => now;
	
	0.0 => thegain;
	0   => thedisp;
	40::ms => now;
}

function void charspace() { 
	0   => thedisp;
	0   => thekey;
	0.0 => thegain;
	80::ms => now;
}

function void wordspace() { 
	0.0 => thegain;
	160::ms => now;
}

function void morsecode( uint key ) { 

	if ( key >= 97 && key <= 122 ) { 
		key - 32 => key;
	}
	key => thekey;

	if ( key == 65 ) { 
		dot();dash();
	}
	else if ( key == 66 ) { 
		dash();dot();dot();dot();
	}
	else if ( key == 67 ) { 
		dash();dot();dash();dot();
	}
	else if ( key == 68 ) { 
		dash();dot();dot();
	}
	else if ( key == 69 ) { 
		dot();
	}
	else if ( key == 70 ) { 
		dot();dot();dash();dot();
	}
	else if ( key == 71 ) { 
		dash();dash();dot();
	}	
	else if ( key == 72 ) { 
		dot();dot();dot();dot();
	}
	else if ( key == 73 ) { 
		dot();dot();
	}
	else if ( key == 74 ) { 
		dot();dash();dash();dash();
	}
	else if ( key == 75 ) { 
		dash();dot();dash();
	}
	else if ( key == 76 ) { 
		dot();dash();dot();dot();
	}
	else if ( key == 77 ) { 
		dash();dash();
	}
	else if ( key == 78 ) { 
		dash();dot();
	}
	else if ( key == 79 ) { 
		dash();dash();dash();
	}
	else if ( key == 80 ) { 
		dot();dash();dash();dot();
	}
	else if ( key == 81 ) { 
		dash();dash();dot();dash();
	}
	else if ( key == 82 ) { 
		dot();dash();dot();
	}
	else if ( key == 83 ) { 
		dot();dot();dot();
	}
	else if ( key == 84 ) { 
		dash();
	}
	else if ( key == 85 ) { 
		dot();dot();dash();
	}
	else if ( key == 86 ) { 
		dot();dot();dot();dash();
	}
	else if ( key == 87 ) { 
		dot();dash();dash();
	}
	else if ( key == 88 ) { 
		dash();dot();dot();dash();
	}
	else if ( key == 89 ) { 
		dash();dot();dash();dash();
	}
	else if ( key == 90 ) { 
		dash();dash();dot();dot();
	}
	else if ( key == 49 ) { //1
		dash();dash();dash();dash();dash();
	}
	else if ( key == 50 ) { 
		dot();dash();dash();dash();dash();
	}
	else if ( key == 51 ) { 
		dot();dot();dash();dash();dash();
	}
	else if ( key == 52 ) { 
		dot();dot();dot();dash();dash();
	}
	else if ( key == 53 ) { 
		dot();dot();dot();dot();dash();
	}
	else if ( key == 54 ) { 
		dot();dot();dot();dot();dot();
	}
	else if ( key == 55 ) { 
		dash();dot();dot();dot();dot();
	}
	else if ( key == 56 ) { 
		dash();dash();dot();dot();dot();
	}
	else if ( key == 57 ) { 
		dash();dash();dash();dot();dot();
	}
	else if ( key == 48 ) { //0
		dash();dash();dash();dash();dot();
	}
	else if ( key == 46 ) { // period
		dot();dash();dot();dash();dot();dash();
	}
	else if ( key == 44 ) { 
		dash();dash();dot();dot();dash();dash();
	}
	else if ( key == 63 ) { 
		dot();dot();dash();dash();dot();dot();
	}
	else if ( key == 45 ) { 
		dash();dot();dot();dot();dot();dash();
	}
	else if ( key == 58 ) { 
		dash();dash();dash();dot();dot();dot();
	}
	else if ( key == 39 ) { 
		dot();dash();dash();dash();dash();dot();
	}
	else if ( key == 40 ) { 
		dash();dot();dash();dash();dot();
	}
	else if ( key == 41 ) { 
		dash();dot();dash();dash();dot();dash();
	}
	else if ( key == 61 ) { 
		dash();dot();dot();dot();dash();
	}
	else if ( key == 64 ) { 
		dot();dash();dash();dot();dash();dot();
	}
	else if ( key == 43 ) { 
		dot();dash();dot();dash();dot();
	}
	else if ( key == 47 ) { 
		dash();dot();dot();dash();dot();
	}
	else if ( key == 32 ) { 
		//space
		wordspace();
	}
	charspace();

}

function void theeventpoll() { 
	while ( gluck.HasEvents() ) { 

		gluck.GetNextEvent() => curid;
		gluck.GetEventType(curid) => curtype;

	 	if ( curtype == gluck.EVENT_KEY ) { //keyboard
			gluck.GetEventKey(curid) => uint k;
			morsecode(k);
		}
	}
}

function void thedrawloop() { 

		gl.ClearColor( 0.4, 0.4, 0.4 , 0.0);
		gl.Clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

		gl.Color3f(0.0, 0.0, 0.0);
		gl.LineWidth(4.0);
		gl.PushMatrix();
		gl.Scalef(0.0008, 0.0009, 0.0008);
		gl.PushMatrix();
		gl.Scalef( 1.3, 1.3, 1.0 );
		gluck.StrokeString(0, "morse");
		gl.PopMatrix();
		gl.PushMatrix();
		gl.LineWidth(2.0);
		gl.Translatef( -300.0, -100.0, 0.0 );
		gluck.StrokeString(0, "press keys to be cool");
		gl.PopMatrix();


		gl.PushMatrix();
		gl.Translatef( 400.0, 200.0 , 0.0 );
		gl.Color3f(1.0, 1.0, 1.0 );
		gl.LineWidth(8.0);
		gl.Scalef(4.0, 4.0, 1.0 );
		gluck.StrokeCharacter ( 0, thekey );
		gl.PopMatrix();

		if ( thedisp != 0 ) { 
		gl.PushMatrix();
		gl.Translatef( -400.0, 200.0 , 0.0 );
		gl.Color3f(1.0, 0.2, 0.2 );
		gl.LineWidth(24.0);
		gl.Scalef(8.0, 8.0, 1.0 );
		if ( thedisp == 1 ) { 
			gl.Scalef(1.5, 1.0, 1.0 );
			gluck.StrokeCharacter ( 0, 46 );
		} 
		else if ( thedisp == 2 ) { 
			gluck.StrokeCharacter ( 0, 45 );
		}
		gl.PopMatrix();
		}

		gl.PushMatrix();
		gl.Translatef( -1000.0, -1100.0 , 0.0 );
		gl.Color3f(0.25, 0.25, 0.25 );
		gl.LineWidth(24.0);
		gl.Scalef(22.0, 22.0, 1.0 );
		gluck.StrokeCharacter ( 0, thekey );
		gl.PopMatrix();

		gl.PopMatrix();
		gl.Flush();
		gluck.SwapBuffers();
}

function void audioshred( ) { 

	thefreq => morsetone.sfreq;
	0.0 => thegain;
	while ( true ) { 
		//do something!
		if ( thegain > followgain ) followgain + 0.025 => followgain;
		if ( thegain < followgain ) followgain - 0.015 => followgain;
		followgain => morsetone.gain;
		5::samp => now;
	}
 
}

function void gluckinitcall() { 

	//MUST be called before any other gluck function

	gluck.Init();

	gluck.InitBasicWindow("morrisey?"); //generate a simple window

	gluck.InitCallbacks(0, 0, 1); //register callbacks for the window
	//arguments monitor ( mouse, motion, keyboard );

	//set up basic camera
	gl.MatrixMode(gl.PROJECTION); 
	gl.LoadIdentity();
	gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

	gl.MatrixMode(gl.MODELVIEW); 
	gl.LoadIdentity();
	gl.ClearColor ( 0.0 , 0.0, 0.3, 0.0 );
}

function void gluckloop( ) { 
	
	gluck.MainLoopEvent(); //...

	if ( gluck.NeedDraw()  ) { thedrawloop(); }

	gluck.PostRedisplay();

}

function void eventshred() { 

	while ( true ) { 
		theeventpoll();
		1::samp => now;
	}
}

function void gluckshred() {

	gluckinitcall();

	spork ~ eventshred();

	while ( true ) { 
		gluckloop(); 
		33::ms => now; //30 fps
	}
}

spork ~ gluckshred();
spork ~ audioshred();

while ( true ) { 
	//sit on yer thumbs;
	1::second => now;
}
