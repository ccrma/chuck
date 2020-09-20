// test binary write

FileIO f;
f.open(me.dir() + "/file.bin", FileIO.BINARY | FileIO.WRITE);

f.write(1, IO.INT16);
f.write(2, IO.INT16);
f.write(3, IO.INT16);

f.close();

<<< "success" >>>;
