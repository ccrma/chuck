
"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);

"namespaces loaded" => stdout;

gluck.Init();

"gluck init " => stdout;

gluck.InitCallbacks(1, 0 ,0);
42 => uint placeholder;
gluck.InitBasicWindow(placeholder);

"gluck setup " => stdout;
0.0 => float rx;
0.0 => float ry;

sinosc a => dac;
440.0 => a.freq;
0.3 => a.gain;

5889 => uint dummy;

"audio setup" => stdout;

gl.MatrixMode(dummy); //projection
gl.LoadIdentity();
gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -1.0 , 1.0 );

5888 => dummy;
gl.MatrixMode(dummy); //modelview
gl.LoadIdentity();

gl.ClearColor ( 0.0 , 0.3, 0.0, 0.0 );
0.0 => float tm;

"gl_setup" => stdout;

function void thedrawloop () { 

		"the draw loop!" => stdout;
//		tm => stdout;
//		now => stdout;
		16640 => dummy; //GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
		gl.Clear(dummy);
		gl.Color3f(1.0, 0.0 ,0.0);
		gl.LineWidth(2.0);
		gl.PushMatrix();
		gl.Rotatef ( 45.0, 1.0, 0.0 , 0.0 );
		gl.Rotatef (tm * 60.0, 0.0, 1.0 , 0.0 );
		"setup" => stdout;
		gluck.WireTeapot( 0.5 );
		"teapot" => stdout; 
		gl.PopMatrix();
		gl.Flush();
		"flush" => stdout;
		gluck.SwapBuffers();
		"swap" => stdout;

}

"draw func def" => stdout; 

function void theeventloop() { 
	"the event loop!" => stdout;
	while ( gluck.HasEvents() ) { 
		gluck.GetNextEvent() => int id;
		gluck.GetEventX(id) => stdout;
		gluck.GetEventY(id) => stdout;
	}
}

"event func def" => stdout;
//chuck loop!

while ( true ) { 

	"enter loop" => stdout; 
	gluck.MainLoopEvent(); //...

	"mainloopev" => stdout;
	gluck.NeedDraw();
	"need_draw test" => stdout;
//	if ( gluck.NeedDraw() ) {	
		"try draw " => stdout;	
		thedrawloop();
		"did draw" => stdout; 
//	}
	"draw check" => stdout;
	if ( gluck.NeedEvent() ) {
		theeventloop();
	}
	"event check" => stdout;

	tm + 0.033 => tm;
	math.sin ( tm ) => rx;
	math.cos ( tm * 3.0 ) => ry;
	"chuck loop" => stdout;
	33::ms => now;
}
