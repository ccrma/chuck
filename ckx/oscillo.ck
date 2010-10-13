
"gl.ckx" => (:gl:);
"glu.ckx" => (:glu:);
"gluck.ckx" => (:gluck:);

512.0 => float div;
0.033 => float tspan;

0 => uint ml;

fun void gluckDraw() {

	gl.Clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );
	gl.LineWidth(3.0);
	gl.Color4f ( 1.0, 1.0, 1.0, 0.8 );
	gl.Begin(gl.QUAD_STRIP);
	0.0 => float f;
	tspan / div => float res;
	for ( 0.0 => f; f <= tspan; f+res => f ) { 
		dac.last => float smp;
		gl.Normal3f ( 0.0, 0.0, 1.0 );
		gl.Vertex2f ( f / tspan, 0.0 );
		gl.Normal3f ( 0.0, -smp * 0.2, smp );
		gl.Vertex3f ( f / tspan, smp, smp * 0.2 );

		1::second  * res => now;
	} 
	gl.End();
	gl.Flush();
	gluck.SwapBuffers();

}

fun void gluckStart() { 

	gluck.Init();
	gluck.InitSizedWindow( "demo", 80,80, 640, 640);
	gluck.InitCallbacks( 0,0,0 );
	gl.MatrixMode(gl.PROJECTION); 
	gl.LoadIdentity();
	gl.Ortho(-1.0, 1.0, -1.0 , 1.0, -4.0 , 4.0 );

	gl.MatrixMode(gl.MODELVIEW); 
	gl.LoadIdentity();
	gl.Translatef ( -1.0, 0.0, 0.0 );
	gl.Scalef ( 2.0, 1.0, 1.0 );
	
	gl.ClearColor ( 0.3 , 0.3, 0.5, 0.0 );

	gl.Enable(gl.LIGHTING);

	gl.LightOn(ml);
	gl.LightPosf(ml, 0.0, 0.0, 0.3, 1.0 );
	gl.LightSpotDirf(ml, 0.0, 0.0, -1.0 );
	gl.LightAmbientf( ml, 0.1, 0.3,0.0,0.9);
	gl.LightDiffusef( ml, 0.5, 1.0, 1.0, 0.8 );
	gl.LightSpecularf( ml, 1.0, 1.0, 1.0, 0.2 );

	while ( true ) { 
		gluck.MainLoopEvent();
		if ( gluck.NeedDraw() ) gluckDraw();
		gluck.PostRedisplay();
	}
}


spork ~gluckStart();

while ( true ) { 
	1::second => now;
}
