"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);


30::samp => dur refreshrate; 

// walls
-1.0 => float l;
1.0 => float r;
-1.0 => float b;
1.0 => float t;

l => float wx1;
b => float wy1;

r => float wx2;
b => float wy2;

r => float wx3;
t => float wy3;

l => float wx4;
t => float wy4;

0 => int sel1;
0 => int sel2;
0 => int sel3;
0 => int sel4;
0 => int selsrc;
0 => int selrecv;

step c => Envelope sxenv => blackhole;
c => Envelope syenv => blackhole;
c => Envelope rxenv => blackhole;
c => Envelope ryenv => blackhole;
1.0 => c.next;

0.1 => sxenv.time;
0.1 => syenv.time;
0.1 => ryenv.time;
0.1 => rxenv.time;

0.5 => sxenv.target;
0.5 => syenv.target;

-0.5 => rxenv.target;
-0.2 => ryenv.target;

0.0 => float mousex;
0.0 => float mousey;
0.0 => float mousedx;
0.0 => float mousedy;

// source pos

0.5 => float srcx;
0.5 => float srcy;

// source animation
0.0 => float srcrad;
1.0 => float srcfreq;

// receiver animation
0.0 => float recvrad;
1.0 => float recvfreq;

// recv ( dac ) pos

-0.25 => float recvx;
-0.25 => float recvy;

// audio constants
3.0 => float speedofsound;
0.95 => float reflectance;


// tmp reflection point
0 => int hasrx; // is there a reflection?
0.0 => float rx;
0.0 => float ry;


0 => int rbl;
0 => int rbr;
0 => int rbt;
0 => int rbb;

// saved reflection points
0.0 => float rxl;
0.0 => float ryl;
0.0 => float rxr;
0.0 => float ryr;
0.0 => float rxt;
0.0 => float ryt;
0.0 => float rxb;
0.0 => float ryb;

// saved mirror points?



// gluck pile


fun void draw() { 
	gl.Clear ( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );
	gl.Color4f( 0.0, 1.0, 0.0 ,0.5 );
	
	gl.LineWidth ( 1.0 );


 	gl.Begin(gl.LINE_LOOP);
	gl.Vertex2f ( wx1, wy1 );
	gl.Vertex2f ( wx2, wy2 );
	gl.Vertex2f ( wx3, wy3 );
	gl.Vertex2f ( wx4, wy4 );
	gl.End();

	gl.PointSize ( 4.0 );
	gl.Begin(gl.POINTS);
	gl.Color4f( 1.0, 0.5, 0.5, 1.0 );
	gl.Vertex2f ( sxenv.last, syenv.last );
	gl.Color4f( 0.5, 0.5, 1.0, 1.0 );
	gl.Vertex2f ( rxenv.last, ryenv.last );
	gl.End();
	
	gl.Begin(gl.LINES);
	gl.Color4f( 1.0, 0.5, 0.5, 1.0 );
	gl.Vertex2f ( sxenv.last, syenv.last );
	gl.Vertex2f ( srcx, srcy );
	
	gl.Color4f( 0.5, 0.5, 1.0, 1.0 );
	gl.Vertex2f ( rxenv.last, ryenv.last );
	gl.Vertex2f ( recvx, recvy );
	
	gl.Color4f ( 1.0, 0.0 ,1.0, 0.5 );
	gl.Vertex2f( srcx, srcy );
	gl.Vertex2f( recvx, recvy );

	gl.Color4f ( 1.0, 0.0, 0.5, 0.25 );
	gl.Vertex2f( srcx, srcy );
	gl.Vertex2f( rxl, ryl );
	gl.Vertex2f( srcx, srcy );
	gl.Vertex2f( rxb, ryb );
	gl.Vertex2f( srcx, srcy );
	gl.Vertex2f( rxr, ryr );
	gl.Vertex2f( srcx, srcy );
	gl.Vertex2f( rxt, ryt );

	gl.Color4f ( 0.5, 0.0, 1.0, 0.25 );
	gl.Vertex2f( rxl, ryl );
	gl.Vertex2f( recvx, recvy );
	gl.Vertex2f( rxb, ryb );
	gl.Vertex2f( recvx, recvy );
	gl.Vertex2f( rxr, ryr );
	gl.Vertex2f( recvx, recvy );
	gl.Vertex2f( rxt, ryt );
	gl.Vertex2f( recvx, recvy );
	gl.End();

	gl.PointSize ( 8.0 );
	gl.Begin(gl.POINTS);
	gl.Color4f( 1.0, 0.0, 0.0, 1.0 );
	gl.Vertex2f ( srcx, srcy );
	gl.Color4f( 0.0, 0.0, 1.0, 1.0 );
	gl.Vertex2f ( recvx, recvy );
	gl.End();


	gl.Flush();
	gluck.SwapBuffers();
}


fun float dlen ( float x, float y ) { 
	return math.sqrt (x * x + y * y);
}

fun float dist ( float x, float y, float x1, float y1 ) { 
	return dlen ( x-x1, y-y1 );
}


fun float dot ( float x, float y, float xp, float yp ) { 
	return x*xp + y*yp;
}


//
//                    t
//       ----------------------
//	 |                d     |
//   l  |                      |  r
//      |   s                  |
//      |_______o______________|
//                    b
//          s'
//                      
//      


fun void reflectwall( float w1x, float w1y, float w2x, float w2y ) { 

	//set up values for wall
	w2x - w1x => float wdx;
	w2y - w1y => float wdy;
	-wdy / dlen ( wdx, wdy ) => float wnx;
	wdx / dlen ( wdx, wdy ) => float wny;
 	- ( wnx * w1x + wny * w1y ) => float wd;

	// project vect ( wall to d ) to the wall normal
	// then subtract twice that from the point.
	// to find the reflection

	srcx - w1x => float sdx;
	srcy - w1y => float sdy;
	dot ( sdx, sdy , wnx, wny ) => float p_n_dp;
	p_n_dp * wnx => float p_sdx;
	p_n_dp * wny => float p_sdy;
	srcx - 2.0 * p_sdx => float mirx;
	srcy - 2.0 * p_sdy => float miry;

	//find intersection of ray from recv pt. to reflection point
	//with the wall
	//store in global rx, ry

	mirx - recvx => float drefx;
	miry - recvy => float drefy;	
    - ( dot( wnx,wny , recvx,recvy ) + wd ) / dot( wnx, wny, drefx, drefy ) => float twall;
	recvx + drefx * twall => rx;
	recvy + drefy * twall => ry;
	
	rx - w1x => float rdx;
	ry - w1y => float rdy;
	
	0 => hasrx;
	if ( dot (rdx,rdy , wdx, wdy ) > 0.0 && dot ( rdx, rdy, rdx, rdy ) < dot ( wdx, wdy, wdx, wdy ) ) 1 => hasrx;
//	w1x => rx;
//	w1y => ry;

}

fun void setdistdelay ( DelayL del, gain g, float x1, float y1, float x2, float y2 ) { 
	dlen ( x2 - x1 , y2 - y1 ) => float len;
	len / speedofsound => float deltime;
	1.0 / ( len*len ) => float atten;
	1::second * deltime=> del.delay;
	atten => g.gain;
//	"distdelay" => stdout;
//	deltime => stdout;
//	atten => stdout;
}

fun void setecho ( DelayL del, gain g, float len, float baseatten ) { 
	len / speedofsound => float deltime;
	1.0 / ( len*len ) => float atten;
	1::second * deltime=> del.delay;
	baseatten * atten => g.gain;
//	"distdelay" => stdout;
//	deltime => stdout;
//	atten => stdout;
}

gain src => DelayL d_direct => gain g_direct => dac;

//4 1st order reflections
src => DelayL s_t => gain g_t => dac ; 
src => DelayL s_b => gain g_b => dac ; 
src => DelayL s_l => gain g_l => dac ; 
src => DelayL s_r => gain g_r => dac ; 

10::second => d_direct.max;

10::second => s_t.max;
10::second => s_b.max;
10::second => s_l.max;
10::second => s_r.max;

impulse imp => src;
sinosc m => src;
Mandolin mand => src;
440.0 => mand.freq;
0.2 => mand.gain;
220.0 => m.sfreq;
0.00 => m.gain;
sndbuf buf => src;
"puzzler.au" => buf.read;
0.0 => buf.rate;
0 => buf.loop;
0.1 => buf.gain;
noise n => src;
0.00 => n.gain;


fun void refreshenv () { 
//	"refresh" => stdout;
	setdistdelay ( d_direct, g_direct, srcx, srcy, recvx, recvy );

	//wall b
	reflectwall ( wx1 , wy1 , wx2, wy2 ); 
	hasrx => rbb;
	if ( rbb ) { 
		rx => rxb;
		ry => ryb;
		setecho ( s_b, g_b, dist( srcx, srcy, rx, ry ) + dist ( rx, ry, recvx, recvy ), reflectance );
	}
	
	//wall t
	reflectwall ( wx3, wy3 , wx4, wy4 ); 
	hasrx => rbt;
	if ( rbt ) { 
		rx => rxt;
		ry => ryt;
		setecho ( s_t, g_t, dist( srcx, srcy, rx, ry ) + dist ( rx, ry, recvx, recvy ), reflectance );
	}
	
	//wall l
	reflectwall ( wx4 , wy4 , wx1, wy1 ); 
	hasrx => rbl;
	if ( rbl ) { 
		rx => rxl;
		ry => ryl;
		setecho ( s_l, g_l, dist( srcx, srcy, rx, ry ) + dist ( rx, ry, recvx, recvy ), reflectance );
	}	
	//wall r
	reflectwall ( wx2 , wy2 , wx3, wy3 ); 
	hasrx => rbr;
	if ( rbr ) { 
		rx => rxr;
		ry => ryr;
		setecho ( s_r, g_r, dist( srcx, srcy, rx, ry ) + dist ( rx, ry, recvx, recvy ), reflectance );
	}
}


refreshenv();

fun void keyEvent( int i) {

	gluck.GetEventKey(i) => uint key;
 
	if ( key == gluck.KEY_w ) { srcfreq * math.pow ( 2.0, 1.0/12.0) => srcfreq; }
	if ( key == gluck.KEY_s ) { srcfreq * 1.0 / math.pow ( 2.0, 1.0/12.0) => srcfreq; }
	if ( key == gluck.KEY_e ) { srcfreq * -1.0  => srcfreq; }
	if ( key == gluck.KEY_d ) { srcrad + 0.01  => srcrad; }
	if ( key == gluck.KEY_a ) { srcrad - 0.01  => srcrad; }

	if ( key == gluck.KEY_i ) { recvfreq * math.pow ( 2.0, 1.0/12.0) => recvfreq; }
	if ( key == gluck.KEY_k ) { recvfreq * 1.0 / math.pow ( 2.0, 1.0/12.0) => recvfreq; }
	if ( key == gluck.KEY_o ) { recvfreq * -1.0  => recvfreq; }
	if ( key == gluck.KEY_l ) { recvrad + 0.01  => recvrad; }
	if ( key == gluck.KEY_j ) { recvrad - 0.01  => recvrad; }
	
	if ( key == gluck.KEY_y ) { speedofsound * math.pow ( 2.0, 1.0/12.0) => speedofsound; speedofsound => stdout; }
	if ( key == gluck.KEY_h ) { speedofsound * 1.0 / math.pow ( 2.0, 1.0/12.0) => speedofsound; speedofsound => stdout; }

	if ( key == gluck.KEY_x ) { 0.3 => imp.next; }
	if ( key == gluck.KEY_c ) { 0.3 => mand.pluck; }
	if ( key == gluck.KEY_v ) { 0.0 => buf.phase ; 1.0 => buf.rate ; }
	if ( key == gluck.KEY_m ) { if ( m.gain == 0.0 ) { 0.05 => m.gain; } else { 0.0 => m.gain; } } 
	if ( key == gluck.KEY_n ) { if ( n.gain == 0.0 ) { 0.05 => n.gain; } else { 0.0 => n.gain; } } 

}

fun void motionEvent ( int i ) { 
	gluck.GetEventX(i) => int x;
	gluck.GetEventY(i) => int y;
	2.0 * ( (float) x / (float) gluck.GetViewDims(0) ) - 1.0  => float mx;
	-2.0 * ( (float) y / (float) gluck.GetViewDims(1) ) + 1.0  => float my;
	
	2.0 * mx - mousex => mousedx;
	2.0 * my - mousey => mousedy;
	2.0 * mx => mousex;
	2.0 * my => mousey;
	
}

fun void selClear ( ) { 
	0 => sel1;
	0 => sel2;
	0 => sel3;
	0 => sel4;
	0 => selsrc;
	0 => selrecv;
}

fun void selectPoints() { 
	selClear();
	if ( dist ( mousex,mousey,wx1, wy1 ) < 0.1 ) 1 => sel1;
	if ( dist ( mousex,mousey,wx2, wy2 ) < 0.1 ) 1 => sel2;
	if ( dist ( mousex,mousey,wx3, wy3 ) < 0.1 ) 1 => sel3;
	if ( dist ( mousex,mousey,wx4, wy4 ) < 0.1 ) 1 => sel4;
	if ( dist ( mousex,mousey,srcx, srcy ) < 0.1 || dist ( mousex, mousey, sxenv.last, syenv.last ) < 0.1 ) 1 => selsrc;
	if ( dist ( mousex,mousey,recvx, recvy ) < 0.1 || dist ( mousex, mousey, rxenv.last, ryenv.last ) < 0.1 ) 1 => selrecv;
}

fun void mouseEvent( int i ) { 
	gluck.GetEventButton(i) => int button;
	gluck.GetEventState(i) => int state;
	if ( state == gluck.DOWN ) { 
		selectPoints();
		
	}
	if ( state == gluck.UP ) { 
		selClear();
	}
}

fun void dragEvent ( int i ) { 
	if ( sel1 ) { 
		wx1 + mousedx => wx1;
		wy1 + mousedy => wy1;
	}
	if ( sel2 ) { 
		wx2 + mousedx => wx2;
		wy2 + mousedy => wy2;
	}
	if ( sel3 ) { 
		wx3 + mousedx => wx3;
		wy3 + mousedy => wy3;
	}
	if ( sel4 ) { 
		wx4 + mousedx => wx4;
		wy4 + mousedy => wy4;
	}
	if ( selsrc ) { 
		sxenv.target + mousedx => sxenv.target;
		syenv.target + mousedy => syenv.target;		
	}
	if ( selrecv ) { 
		rxenv.target + mousedx => rxenv.target;
		ryenv.target + mousedy => ryenv.target;		
	}
}

fun void gluckEvents() { 

	while ( true ) { 
		while ( gluck.HasEvents() ) { 
			gluck.GetNextEvent() => int curid;
			gluck.GetEventType(curid) => uint curtype;
		 	if ( curtype == gluck.EVENT_KEY ) { //keyboard
				keyEvent(curid);
			}
		 	if ( curtype == gluck.EVENT_MOUSE ) { //mouse
				motionEvent(curid);
				mouseEvent(curid);
			}
		 	if ( curtype == gluck.EVENT_MOTION ) { //motion
				motionEvent(curid);
				dragEvent(curid);
			}
		}
		30::ms => now;
	}
}

fun void gluckStartup ( ) { 

	gluck.Init();
	gluck.InitSizedWindow( "echobox", 80,80, 640, 640);
	gluck.InitCallbacks( 1,1,1 );
	
	spork ~ gluckEvents();
	
	gl.MatrixMode(gl.PROJECTION); 
	gl.LoadIdentity();
	gl.Ortho(-2.0, 2.0, -2.0 , 2.0, -4.0 , 4.0 );

	gl.MatrixMode(gl.MODELVIEW); 
	gl.LoadIdentity();

	gl.ClearColor ( 0.1 , 0.1, 0.1, 0.0 );
	gl.Enable(gl.LINE_SMOOTH);
	gl.Enable(gl.POINT_SMOOTH);
	gl.Enable(gl.POLYGON_SMOOTH);
	gl.Enable(gl.BLEND);
	gl.BlendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
	gl.Disable(gl.DEPTH_TEST);


	while ( true ) { 
		gluck.MainLoopEvent();
		if ( gluck.NeedDraw() ) draw();
		gluck.PostRedisplay();
		33::ms => now;
	}

}

//THE FUN PART...

fun void fiddlebox () { 
	phasor sc => sinosc sx => blackhole;
	sc => sinosc sy => blackhole; 
	step bias => sx;
	
	phasor rc => sinosc rxo => blackhole;
	rc => sinosc ryo => blackhole; 
	bias => rxo;
	
	2 => sx.sync;
	2 => sy.sync;
	2 => rxo.sync;
	2 => ryo.sync;
	1.0  => sc.sfreq;
	1.0  => rc.sfreq;
	0.25 => bias.next;
	0.5 => sxenv.target;
	0.4 => syenv.target;
	while ( true ) { 
	
		sxenv.last + srcrad * sx.last => srcx;
		syenv.last + srcrad * sy.last => srcy;		
		rxenv.last + recvrad * rxo.last => recvx;
		ryenv.last + recvrad * ryo.last => recvy;		
		
		refreshenv();
		srcfreq => sc.sfreq;
		recvfreq => rc.sfreq;
		refreshrate => now;
		
	}
}



spork ~ gluckStartup();

spork ~ fiddlebox();

while ( true ) {
	0.25::second => now;
}


