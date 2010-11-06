"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);


"this only works called directly from chuck...?" => stdout; 

gluck.Init();

gluck.InitBasicWindow("it's a time machine");

gluck.InitCallbacks(1, 0 ,1);


0.0 => float rx;
0.0 => float ry;

5889 => uint dummy;

gl.MatrixMode(dummy); //projection
gl.LoadIdentity();
gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

5888 => dummy;
gl.MatrixMode(dummy); //modelview
gl.LoadIdentity();

gl.ClearColor ( 0.0 , 0.0, 0.3, 0.0 );

3042    => dummy; //GL_BLEND
gl.Enable (dummy);

0.0 => float tm;

//chuck loop!

0.0 => float frac;
0.0 => float nf;

440.0 => float fc;
100.0 => float fm;
120.0 => float ampM;
0.0   => float frq;

0.0 => float t;

sinosc a => dac;


noise n => biquad f => dac;

0.99 => f.prad;
.1   => f.gain;
1    => f.eqzs;


function void fmsyn() { 

	while ( true ) { 

		std.abs ( 2.0 * ( -0.5 + t - math.floor(t)) ) => frac;
		440.0 - 220.0 * frac => fc;

		120.0 + 120.0 * math.sin( t * 0.2 ) => fm;  //modulate modulators
	    	100.0 + 50.0 * math.sin ( t * 0.05 ) => ampM; //modulate mod amplitude.
	    	fc + ampM * math.sin ( fm * t * 2.0 * pi) => frq; //fm eq..
//		frq => a.sfreq;
		fc + ampM * math.sin ( fm * t * 2.0 * pi ) => a.sfreq;
		0.7 => a.gain;	
		t + 0.001 => t;
		1::ms => now;
	}
}

//gfx monitor audio vars...

function void thedrawloop () { 

		16640 => dummy; // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
		gl.Clear(dummy);


		//teapot freakout...		
		gl.Color4f( ( fm / 240.0 ) ,  1.0-( fm / 240.0 ) , frac, 0.5);
		gl.LineWidth(2.0);
		gl.PushMatrix();
		gl.Rotatef ( tm * 45.0, 1.0, 0.0 , 0.0 );
		gl.Rotatef (tm * 60.0, 0.0, 1.0 , 0.0 );
		gl.Scalef (1.0 , 0.5 + ( frq / 440.0 ), 1.0 );
		gluck.WireTeapot ( 0.50 );
		gl.PopMatrix();

		//A changes with ampM
		gl.Color3f ( 1.0, 1.0, 1.0);
		gl.PushMatrix();
		gl.Translatef ( 0.50, 0.0, 0.0 );
		gl.Scalef ( 0.003, 0.005, 0.004 );
		gl.Scalef ( ampM * 0.01, ampM * 0.01, ampM * 0.01 );
		gluck.StrokeString ( 0,"ampM");
		gl.PopMatrix();

		//fM changes with mod f
		gl.PushMatrix();
		gl.Scalef ( 0.003, 0.005, 0.004 );
		gl.Scalef ( 0.2 + fm / 240.0 , 0.2 + fm / 240.0, 0.2 + fm / 240.0 );
		gluck.StrokeString ( 0, "Fm");
		gl.PopMatrix();


		//fc change with carrier f.
		gl.PushMatrix();
		gl.Translatef ( -0.50, 0.0, 0.0 );
		gl.Scalef ( 0.003, 0.005, 0.004 );
		gl.Scalef ( fc / 440.0 , fc / 440.0, fc / 440.0 );
		gluck.StrokeString ( 0, "Fc");
		gl.PopMatrix();

		//GLUUUUUUUCK!
		math.fmod ( tm, 4.0 ) * 0.25 => nf;
		gl.Color3f( 0.0 , 0.5 , nf);
		gl.LineWidth(6.0);
		gl.PushMatrix();
		gl.Scalef ( 1.0 + nf, 1.0 + nf, 1.0 + nf);
		gl.Rotatef ( tm * 20.0, 0.0, 0.0 , 1.0 );
		gl.Translatef ( -0.50 , -0.25, 0.0 );
		gl.Scalef ( 0.003, 0.005, 0.004 );

		gluck.StrokeString ( 0, "GLucK!");
		gl.PopMatrix();

		gl.Flush();

		gluck.SwapBuffers();
}

function void theeventloop() { 
	"the event loop!" => stdout;
	while ( gluck.HasEvents() ) { 
		gluck.GetNextEvent() => int id;
		gluck.GetEventX(id) => stdout;
		gluck.GetEventY(id) => stdout;
	}
}


spork ~ fmsyn();


while ( true ) { 

	gluck.MainLoopEvent(); //...

//	if ( gluck.NeedDraw() ) {	
		thedrawloop();
//	}

	if ( gluck.NeedEvent() ) {
		theeventloop();
	}

	0.1 + 0.25 * nf => n.gain;
	100.0 + nf * 1000.0 => f.pfreq;
	
	tm + 0.05 => tm;
	50::ms => now;
}
