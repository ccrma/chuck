
"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);


//MUST be called before any other gluck function
gluck.Init();

gluck.InitBasicWindow("danger! teapot!"); //generate a simple window

gluck.InitCallbacks(1, 1, 1); //register callbacks for the window
//arguments monitor ( mouse, motion, keyboard );

//why is dummy here?
//using actual values ( from the glut.h headers ) for these because 
//we haven't set up the constant (huge) namespace..
5889 => uint dummy; //PROJECTION_MATRIX
gl.MatrixMode(dummy); 
gl.LoadIdentity();
gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

5888 => dummy; //GL MODELVIEW_MATRIX
gl.MatrixMode(dummy); 
gl.LoadIdentity();
gl.ClearColor ( 0.0 , 0.0, 0.3, 0.0 );



//our variables

0.0 => float tm;
440.0 => float fc;
220.0 => float ampM;
0.0 => float frac;

0 => int lastx;
0 => int lasty;

0.0 => float curx;
0.0 => float cury;

0.0 => float bg;
0.0 => float avol;

noise n => biquad b => dac;

0.99 => b.prad;
1    => b.eqzs;

100.0 => b.pfreq;
0.1 => b.gain;


sinosc a => dac;
fc => a.freq;
0.3 => a.gain;

function void thedrawloop() { 

		16640 => dummy; //GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
		gl.Clear( dummy );

		gl.LineWidth(2.0);
		gl.PushMatrix();
		gl.Translatef ( curx, cury, 0.0);
//		gl.Scalef  ( 1.1 + math.sin ( tm * 0.2 ), 1.1 + math.sin ( tm * 0.2 ), 1.1 + math.sin ( tm * 0.2 ) ); 
		gl.Scalef  ( 0.1 + avol , 0.1 + avol , 0.1 + avol ); 
		gl.Rotatef ( tm * 45.0, 1.0, 0.0 , 0.0 );
		gl.Rotatef (tm * 60.0, 0.0, 1.0 , 0.0 );
		
		if ( bg > 0.0 ) { 
			gl.Color4f( 0.0, 0.0, 0.0, 0.95 ); 
			gluck.SolidTeapot ( 0.5  );
			gl.Color4f( frac , 0.5 + 0.5 * curx , 0.05 + 0.5 * cury, 0.5 );
			gluck.WireTeapot ( 0.5 + bg );

		}
		else { 
			gl.Color4f( frac , 0.5 + 0.5 * curx , 0.05 + 0.5 * cury, 0.5 );
			gluck.WireTeapot( 0.5 );
		}
		gl.PopMatrix();
		gl.Flush();
		gluck.SwapBuffers();

}

function void theeventloop() { 
	while ( gluck.HasEvents() ) { 
		gluck.GetNextEvent() => int id;

		if ( gluck.GetEventType(id) == 0 ) { 
			if ( gluck.GetEventX(id) == lastx & gluck.GetEventY(id) == lasty  ) { 
				0.2 => bg; 
				avol + 0.04 => avol;
			}
			gluck.GetEventX(id) => lastx;
			gluck.GetEventY(id) => lasty;

		}
		if ( gluck.GetEventType(id) == 1 ) { 
			2.0 * ( (float) gluck.GetEventX(id) / (float) gluck.GetViewDims(0) ) - 1.0  => curx;
			-2.0 * ( (float) gluck.GetEventY(id) / (float) gluck.GetViewDims(1) ) + 1.0  => cury;
			(float) gluck.GetEventX(id) => fc;
			(float) gluck.GetEventY(id) => ampM;
			gl.ClearColor(0.0, 0.5 + -0.5 * curx, 0.5 + -0.5 * cury, 0.0);
		}
		if ( gluck.GetEventType(id) == 3 ) {
			gluck.GetEventKey(id) => uint k;
			100.0 + (float) ( k - 65 ) * 50.0 => b.pfreq;
			0.2 => bg; 
			avol + 0.04 => avol;
		}
	}
}

function void theaudiocontrol( ) { 
	fc + ampM * frac => a.sfreq;
//	0.4 + 0.3 * math.sin ( tm * 0.2 ) => a.gain;
	if ( avol > 1.5 ) 1.5 => avol;
	0.1 + avol => a.gain;
	avol + ( 0.033 * -0.1 ) => avol;
	if ( avol < 0.0 ) { 0.0 => avol; }
	bg + ( 0.033 * -0.4 ) => bg ; //drain bg
	if ( bg < 0.0 ) { 0.0 => bg; } 
	bg => b.gain;
}

function void thegraphicscall( ) { 
	
	gluck.MainLoopEvent(); //...

	if ( gluck.NeedDraw()  ) { thedrawloop(); }
	if ( gluck.NeedEvent() ) { theeventloop(); }

	gluck.PostRedisplay();

}

while ( true ) { 

	std.abs ( 2.0 * ( -0.5 + tm - math.floor(tm)) ) => frac;

	thegraphicscall ();

	theaudiocontrol();

	tm + 0.033 => tm;
	
	33::ms => now;
}
