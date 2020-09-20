
class InfoBlob
{
    // last time
    time when;

	// public!
	int iVector[];
	int iMatrix[][];
	float fVector[];
	float fMatrix[][];
    complex cVector[];
    complex cMatrix[][];
    
    // user object
    Object user;
	
	// deep copy with data
	InfoBlock clone();
	
	// secret (C++ only), UAna that made this
	int origin;
}
