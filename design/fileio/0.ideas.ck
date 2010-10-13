// design: file I/O

// return current directory's full path
string FS.currdir()
// change to directory
string FS.chdir( string to )
// is it a file?
int FS.isFile( string filename );
// is it a directory?
int FS.isDir( string filename );
// get the size of a file
int FS.getSize( string filename );
// get the basename from a long path
string FS.baseName( string path );
// get all filename in current directory
string[] FS.getContent();

// open a file
IO.openFile( string name, string mode ) @=> File fin;
IO.openSocket( ... ) @=> Socket sock;
IO.openPipe( ... ) @=> Pipe pipe;


class File extend IOBase
{
}

class TextFile IOBase
{
}

class BinaryFile IOBase
{
}

class Pipe IOBase
{
}
