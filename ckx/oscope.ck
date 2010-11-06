
"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);

512.0 => float div;
1.0 / 30.0 => float refresh;
30.0 => float hz;
0.0 => float scanx;
0.0 => float wscanx;
0 => uint ml;
now => time tstart;
0.0 => float dtime;
0 => int dmode;


fun void gluckDraw() {
	0.0 => float f;
	1.0 / hz => float tspan;
	tspan / div => float res;
	gl.Clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );
	gl.LineWidth(1.0);
	gl.Color4f ( 0.5, 1.0, 0.5, 0.8 );
	
	if		( dmode == 0 ) { gl.Begin(gl.QUAD_STRIP); } 
	else if ( dmode == 1 ) { 
		gl.Begin(gl.LINE_STRIP); 
		gl.Normal3f ( 0.0, 0.0 , 0.0 );
	}
	else if ( dmode == 2 ) { gl.Begin(gl.LINES); }
	 
	for ( 0.0 => f; f <= refresh; f+res => f ) { 
		( now - tstart ) / 1::second => dtime;
		math.fmod ( dtime, tspan ) => scanx;
		dac.last => float smp;
		if ( scanx < wscanx ) {
			gl.End();
			if		( dmode == 0 ) { gl.Begin(gl.QUAD_STRIP); } 
			else if ( dmode == 1 ) { gl.Begin(gl.LINE_STRIP); } 
			else if ( dmode == 2 ) { gl.Begin(gl.LINES); } 
		} 
		if ( dmode == 0 || dmode == 2 ) { 
			gl.Normal3f ( 0.0, 0.0, 1.0 );
			gl.Vertex2f ( scanx / tspan, 0.0 );
			gl.Normal3f ( 0.0, -smp * 0.2, smp );
		}
		gl.Vertex2f ( scanx / tspan, smp );
		scanx => wscanx;
		1::second * res => now;
	}
	gl.End();	
	gl.Flush();
	gluck.SwapBuffers();

}

fun void keycode(uint key) { 
	if ( key == gluck.KEY_w ) { hz * 2.0 => hz; }
	if ( key == gluck.KEY_s ) { hz * 0.5 => hz; }
	if ( key == gluck.KEY_a ) { div * 0.5 => div; }
	if ( key == gluck.KEY_d ) { div * 2.0 => div; }
	if ( key == gluck.KEY_t ) { hz + 10.0 => hz; }
	if ( key == gluck.KEY_g ) { hz - 10.0 => hz; }
	if ( key == gluck.KEY_y ) { hz + 1.0 => hz; }
	if ( key == gluck.KEY_h ) { hz - 1.0 => hz; }
	if ( key == gluck.KEY_u ) { hz + 0.1 => hz; }
	if ( key == gluck.KEY_j ) { hz - 0.1 => hz; }
	if ( key == gluck.KEY_i ) { hz + 0.01 => hz; }
	if ( key == gluck.KEY_k ) { hz - 0.01 => hz; }
	if ( key == gluck.KEY_o ) { hz + 0.001 => hz; }
	if ( key == gluck.KEY_l ) { hz - 0.001 => hz; }
	if ( key == gluck.KEY_c ) { 0 => dmode ; }
	if ( key == gluck.KEY_v ) { 1 => dmode ; }
	if ( key == gluck.KEY_b ) { 2 => dmode ; }
	if ( key == gluck.KEY_q ) { gluck.Shutdown(); }
	math.max ( 1.0, hz ) => hz;
}

fun void gluckEvents() { 

	while ( gluck.Running() ) { 
		while ( gluck.HasEvents() ) { 
			gluck.GetNextEvent() => int curid;
			gluck.GetEventType(curid) => uint curtype;
		 	if ( curtype == gluck.EVENT_KEY ) { //keyboard
				gluck.GetEventKey(curid) => uint k;
				keycode(k);
			}
		}
		30::ms => now;
	}
}

fun void gluckStart() { 

	gluck.Init();
	gluck.InitSizedWindow( "demo", 80,80, 640, 640);
	gluck.InitCallbacks( 0,0,1 );
	gl.MatrixMode(gl.PROJECTION); 
	gl.LoadIdentity();
	gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

	gl.MatrixMode(gl.MODELVIEW); 
	gl.LoadIdentity();
	gl.Translatef ( -1.0, 0.0, 0.0 );
	gl.Scalef ( 2.0, 1.0, 1.0 );

	gl.ClearColor ( 0.7 , 0.7, 0.7, 0.0 );
	gl.Enable(gl.LINE_SMOOTH);
	gl.Enable(gl.POLYGON_SMOOTH);
	gl.Enable(gl.BLEND);
	gl.BlendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
	gl.Disable(gl.DEPTH_TEST);

	
	gl.Enable(gl.LIGHTING);


	gl.LightOn(ml);
	gl.LightPosf(ml, 0.0, 0.0, 0.3, 1.0 );
	gl.LightSpotDirf(ml, 0.0, 0.0, -1.0 );
	gl.LightAmbientf( ml, 0.1, 0.3,0.0,0.9);
	gl.LightDiffusef( ml, 0.5, 1.0, 1.0, 0.8 );
	gl.LightSpecularf( ml, 1.0, 1.0, 1.0, 0.2 );

	while ( gluck.Running() ) { 
		gluck.MainLoopEvent();
		if ( gluck.NeedDraw() ) gluckDraw();
		gluck.PostRedisplay();
	}


	gluck.DestroyWindow();
	gluck.CleanUp();

	gluck.MainLoopEvent();

}


spork ~gluckStart();
spork ~gluckEvents();

1::second => now;
while ( gluck.Running() ) { 
	hz => stdout;
	0.50::second => now;
}

