// a subclass of Event
class Foo extends Event { }

// an Event
Event e;

// cast to subclass
e $ Foo @=> Foo @ f;

// if we got here then ok
<<< "success" >>>;
