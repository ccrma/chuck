// morse code - gluck keyboard event demo 
// june 2004 Philip Davidson ( philipd@alumni.princeton.edu )
//
//
//

"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);

//our variables

Shakers shake1 => JCRev r => Echo a => Echo b => Echo c => dac;
Shakers shake2 => r;
TubeBell t => r;
0.1 => t.gain;
r => dac;

1    => int oldskool;

0.01 => float dt;
0.0 => float p1f;
0.0 => float p2f;

-1.0 => float grav;

0.0 => float puckx;
0.0 => float pucky;

0.0 => float puckvx;
0.0 => float puckvy; 

0.0 => float puckax;
0.0 => float puckay;

0.02 => float puckrad;

0.0 => float pad1y;
0.0 => float pad1vy;

0.0 => float pad2y; 
0.0 => float pad2vy;

0.03 => float padw;
0.3 => float padh;

1.0 => float boardw;
1.0 => float boardh;

0.1 => float padspace;
-boardw + padspace => float pad1x;
boardw - padspace => float pad2x;
0.0 => float pad1vx;
0.0 => float pad2vx;



0 => int bouncex;
0 => int bouncey;

0 => int out1;
0 => int out2;

now => time lasttime;
0 => int lastx;
0 => int lasty;

0.0 => float lastdx;
0.0 => float lastdy;

0 => int p1score;
0 => int p2score;

1 => int running;

function void keycode ( uint k ) { 

	if ( k == gluck.KEY_q ) { 
		0 => running;
	}	

	if ( k == gluck.KEY_t ) { 
		1 - oldskool => oldskool;
		if ( oldskool ) { 
			-boardw + padspace => pad1x;
			boardw - padspace =>  pad2x;
		}	
	}	

	if ( k == gluck.KEY_w ) { 
		math.max ( 0.2, math.min ( 1.6 , pad1vy + 0.2) ) => pad1vy;
	}
	if ( k == gluck.KEY_s ) { 
		0.0 => pad1vy;
	}
	if ( k == gluck.KEY_x ) { 
		math.min ( -0.2 , math.max (-1.6 , pad1vy - 0.2) ) => pad1vy;
	}

	if ( k == gluck.KEY_i ) { 
		math.max ( 0.2, math.min ( 1.6 , pad2vy + 0.2) ) => pad2vy; 
	}
	if ( k == gluck.KEY_j ) { 
		0.0 => pad2vy;
	}
	if ( k == gluck.KEY_n ) { 
		math.min ( -0.2, math.max (-1.6 , pad2vy - 0.2) ) => pad2vy;
	}
	if ( k == gluck.KEY_SPACE ) { 
		std.rand2f( 0.0, 100.0 ) => float deg;
		std.rand2f( 1.0, 2.0 ) => float mag;
		mag * math.cos(deg) => puckvx;
		mag * math.sin(deg) => puckvy;
	}
}

function void motion ( int x, int y ) { 
	x => lastx;
	y => lasty;
} 


function void drag ( int x, int y ) { 
	x - lastx => int dx;
	y - lasty => int dy;
	if ( x > gluck.GetViewDims(0) / 2 ) { 
		-0.6 * (float)dy => pad2vy;
		 0.6 * (float)dx => pad2vx;
	}
	else { 
		-0.6 * (float)dy => pad1vy;
		 0.6 * (float)dx => pad1vx;
	}	
}


function void theeventpoll() { 
	while ( gluck.HasEvents() && running == 1 ) { 

		gluck.GetNextEvent() => int curid;
		gluck.GetEventType(curid) => uint curtype;

	 	if ( curtype == gluck.EVENT_KEY ) { //keyboard
			gluck.GetEventKey(curid) => uint k;
			keycode(k);
		}
	 	if ( curtype == gluck.EVENT_PMOTION ) { //keyboard
			gluck.GetEventX(curid) => int x;
			gluck.GetEventY(curid) => int y;
			motion(x,y);
		}
	        if ( curtype == gluck.EVENT_MOTION ) { //keyboard
			gluck.GetEventX(curid) => int x;
			gluck.GetEventY(curid) => int y;
			drag(x,y);
			motion(x,y);
		}
	}
}

function void drawWindow() { 

		gl.ClearColor( 0.4, 0.4, 0.4 , 0.0);
		gl.Clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

		gl.LineWidth(3.0);


		0 => int ix;
		gl.Color4f ( 0.5, 1.0, 0.5, 0.5 ) ;
		gl.PushMatrix();
		gl.Translatef( -0.1 , -boardh, 0.0 );
		gl.Scalef( 0.1, 0.1, 1.0 );
		for ( 0 => ix; ix < p1score ; ix + 1 => ix ) { 
			gl.Begin(gl.QUADS);
			gl.Vertex2f( 0.0, 0.0 );
			gl.Vertex2f( 0.0, 0.9 );
			gl.Vertex2f( 0.9, 0.9 );
			gl.Vertex2f( 0.9, 0.0 );
			gl.End();
			gl.Translatef ( 0.0, 1.0 , 0.0 );
		}
		gl.PopMatrix();

		gl.PushMatrix();
		gl.Translatef( 0.01 , -boardh, 0.0 );
		gl.Scalef( 0.1, 0.1, 1.0 );
		
		for ( 0 => ix; ix < p2score ; ix + 1 => ix ) { 
			gl.Begin(gl.QUADS);
			gl.Vertex2f( 0.0, 0.0 );
			gl.Vertex2f( 0.0, 0.9 );
			gl.Vertex2f( 0.9, 0.9 );
			gl.Vertex2f( 0.9, 0.0 );
			gl.End();
			gl.Translatef ( 0.0, 1.0 , 0.0 );
		}
		gl.PopMatrix();


		gl.Color4f ( 0.0, 1.0, 0.0, 0.5 ) ;

		gl.Begin(gl.LINE_LOOP);
		gl.Vertex2f(-boardw, -boardh);
		gl.Vertex2f( boardw, -boardh);
		gl.Vertex2f( boardw,  boardh);
		gl.Vertex2f(-boardw,  boardh);
		gl.End();

		gl.PushMatrix();
		gl.Translatef ( pad1x, pad1y, 0.0 );
		gl.Begin(gl.QUADS );
		gl.Vertex3f ( -padw, -padh, 0.0 );
		gl.Vertex3f (  padw, -padh, 0.0 );
		gl.Vertex3f (  padw,  padh, 0.0 );
		gl.Vertex3f ( -padw,  padh, 0.0 );
		gl.End();
		gl.PopMatrix();

		gl.PushMatrix();
		gl.Translatef (  pad2x , pad2y, 0.0 );
		gl.Begin(gl.QUADS );
		gl.Vertex3f ( -padw, -padh, 0.0 );
		gl.Vertex3f (  padw, -padh, 0.0 );
		gl.Vertex3f (  padw,  padh, 0.0 );
		gl.Vertex3f ( -padw,  padh, 0.0 );
		gl.End();
		gl.PopMatrix();

		gl.PushMatrix();
		gl.Translatef ( puckx ,  pucky , 0.1);
		gl.Begin(gl.QUADS) ;
		gl.Vertex2f ( -puckrad, -puckrad );
		gl.Vertex2f (  puckrad, -puckrad );
		gl.Vertex2f (  puckrad,  puckrad );
		gl.Vertex2f ( -puckrad,  puckrad );
		gl.End();
		gl.PopMatrix();

		gl.Begin(gl.LINES);
		gl.Vertex2f(0.0,  boardh );
		gl.Vertex2f(0.0, -boardh );
		gl.End();


		gl.Flush();
		gluck.SwapBuffers();

}

function void sndtrigger ( int which ) { 

	if ( which == 1 ) { 
		1.5 => shake1.noteOn;
	}
	else if ( which == 2 ) { 
		1.5 => shake2.noteOn;
	}
	else if ( which == 3 ) { 
		220.0 => t.freq;
		0.9  + math.max ( 0.0, math.min ( 1.0 , std.abs( puckvy * 0.3 ))) => t.noteOn;
	}

	else if ( which == 4 ) { 
		440.0 => t.freq;
		0.7  + math.max ( 0.0, math.min ( 1.0 , std.abs( puckvy * 0.3 ))) => t.noteOn;
	}
}



function void score ( int which ) { 
	if ( which == 1 ) { 
		p1score + 1 => p1score;
		p1score => shake1.which;
	}

	else if ( which == 2 ) { 
		p2score + 1 => p2score;
		p2score => shake2.which;
	}
	0.0 => puckx;
	0.0 => pucky;
	0.0 => puckvx;
	0.0 => puckvy;
}

function void testcollisions() { 
	
	//pad1;
	if ( pad1y + padh > boardh ) { 
		pad1vy * -0.1 => pad1vy;
		boardh + (  boardh - ( pad1y + padh ) ) - padh => pad1y;
	}
	if ( pad1y - padh < -boardh ) { 
		pad1vy * -0.1 => pad1vy;
		-boardh + ( -boardh - (pad1y-padh) ) + padh  => pad1y;
	}

	math.max ( -boardw , math.min ( -boardw * 0.5 , pad1x ) ) => pad1x;  
	math.max (  boardw * 0.5 , math.min ( boardw  , pad2x ) ) => pad2x;  

	//pad2;
	if ( pad2y + padh > boardh ) { 
		pad2vy * -0.1 => pad2vy;
		boardh + (  boardh - ( pad2y + padh ) ) - padh => pad2y;
	}
	if ( pad2y - padh < -boardh ) { 
		pad2vy * -0.1 => pad2vy;
		-boardh + ( -boardh - (pad2y-padh) ) + padh  => pad2y;
	}

	//puck

	//top/bottom
	if ( pucky < -boardh ) { 
		sndtrigger(3);
		puckvy * -1.0 => puckvy;
	}	
	if ( boardh < pucky) { 
		sndtrigger(4);
		puckvy * -1.0 => puckvy;
	}

	pad1x + padw => p1f;
	pad2x - padw => p2f;

	if ( puckx < -boardw ) { 
		score( 2 );
	}
	else if ( puckx > boardw ) { 
		score( 1 );
	}


	if ( puckx - puckrad < p1f && pucky < pad1y + padh && pucky > pad1y - padh ) { 
		if ( puckx - puckrad + puckvx * -dt * 5.0  > p1f + pad1vx * -dt * 5.0 ) { 
			p1f + ( p1f - ( puckx - puckrad )  ) + puckrad => puckx;
			puckvy + 0.3 * ( pad1vy - puckvy ) => puckvy; //friction transfer...
			if ( oldskool ) { puckvx * -1.0 => puckvx; }
			else { 
				pad1vx + -0.3 * ( puckvx - pad1vx ) => puckvx; //ricochet
			}
			sndtrigger(1);
		} 
	}	
	else if ( puckx + puckrad > p2f && pucky < pad2y + padh && pucky > pad2y - padh ) { 
		if ( puckx + puckrad + puckvx * -dt * 5.0 < p2f + pad2vx * -dt * 5.0 ) { 
			p2f + ( p2f - ( puckx + puckrad ) ) - puckrad  => puckx;
			puckvy + 0.3 * ( pad2vy - puckvy )  => puckvy; //friction transfer...
			if ( oldskool ) { puckvx * -1.0 => puckvx; }
			else { 
				pad2vx + -0.3 * ( puckvx - pad2vx ) => puckvx; //ricochet
			}
			sndtrigger(2);	
		} 
	}		
}


function void simulshred() { 

	while ( running == 1 ) { 

		0.0 => puckax;
		0.0 => puckay;

		//drag on the paddles. 
		std.sgn( pad1vy ) * math.max( 0.0, std.abs(pad1vy) - 0.2 * dt ) => pad1vy;
		std.sgn( pad2vy ) * math.max( 0.0, std.abs(pad2vy) - 0.2 * dt ) => pad2vy;
		std.sgn( pad1vx ) * math.max( 0.0, std.abs(pad1vx) - 0.2 * dt ) => pad1vx;
		std.sgn( pad2vx ) * math.max( 0.0, std.abs(pad2vx) - 0.2 * dt ) => pad2vx;

		testcollisions();

		//gravittles

		if ( !oldskool ) puckay + grav => puckay;
		
		pad1y + pad1vy * dt => pad1y;
		pad2y + pad2vy * dt => pad2y;

		if ( !oldskool ) { 
			pad1x + pad1vx * dt => pad1x;
			pad2x + pad2vx * dt => pad2x;
		}

		puckvx + puckax * dt => puckvx;
		puckvy + puckay * dt => puckvy;

		puckx + puckvx * dt => puckx;
		pucky + puckvy * dt => pucky;
 
		10::ms => now;

 	}
}

function void eventshred() { 

	//handle events that were buffered by glut's calls.
	//once we have events, this will change. 

	while ( running == 1 ) { 
		theeventpoll();
		30::ms => now;
	}

}

function void gluckinitcall() { 


	gluck.Init(); 	//MUST be called before any other gluck function

	gluck.InitBasicWindow("ponnng?"); //request a window... 

	gluck.InitCallbacks(0, 1, 1); //register callbacks for the window
	//arguments monitor ( mouse, motion, keyboard );

	//set up a simple view matrix

	gl.MatrixMode(gl.PROJECTION); 
	gl.LoadIdentity();
	gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

	gl.MatrixMode(gl.MODELVIEW); 
	gl.LoadIdentity();

	
	gl.ClearColor ( 0.0 , 0.0, 0.3, 0.0 );
}

function void eventloop( ) { 
	
	gluck.MainLoopEvent(); // let glut do its bizness
	if ( gluck.NeedDraw() && running == 1 ) { drawWindow(); }
	gluck.PostRedisplay(); // request that we draw again;

}

function void gluckshred() {

	gluckinitcall();
	spork ~ eventshred();
	while ( running == 1 ) { 
		eventloop(); 
		33::ms => now; //30 fps
	}
}

spork ~ gluckshred();
spork ~ simulshred();

while ( running == 1 ) { 

	0.3::second => now;

}
