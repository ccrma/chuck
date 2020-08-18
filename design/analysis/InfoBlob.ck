
class InfoBlob
{
	void setIntMetaData( string, int );
	void setFloatMetaData( string, float );
	void setStringMetaData( string, string );

	int getIntMetaData(string);
	float getFloatMetaData(string);
	string getStringMetaData( string );

	setIntVector( int[] )
	setIntMatrix( int[][] )
	setFloatData( )
	setComplexData( )
	
	getIntData	( )
	getFloatData( )
	getComplexData( )
	
	// public?
	int iVector[];
	int iMatrix[][];
	float fVector[];
	float fMatrix[][];
	
	void setUserObject( Object obj );
	Object getUserObject();
	
	// deep copy with data
	InfoBlock clone();
	
	// secret (C++ only), UAna that made this
	int origin;
}
