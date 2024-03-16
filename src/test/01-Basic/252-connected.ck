Gain g => dac;

<<< g.isConnectedTo(dac), g.isConnectedTo(dac.left) >>>;

Pan2 p => dac;

<<< p.isConnectedTo(dac), p.left.isConnectedTo(dac.left) >>>;

