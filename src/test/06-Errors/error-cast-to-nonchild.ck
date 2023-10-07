// error case: verify error when casting to a non-subclass
// related:: 01-Basic/211-cast-to-child.ck

// a class (not subclass of Event)
class Foo { }

// an Event
Event e;

// cast to not subclass
e $ Foo @=> Foo @ f;
