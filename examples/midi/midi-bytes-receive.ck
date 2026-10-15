MidiIn min;

if (!min.open(0)) {
	me.exit();
}

int bytes[0];

while (min => now) {
	while (min.recv(bytes)) {
		chout <= "bytes: ";

		for (auto byte : bytes) {
			chout <= byte <= " ";
		}

		chout <= IO.newline();
	}
}
