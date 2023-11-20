// this tests casting to a child pointer
// NOTE as of 1.5.2.0, chuck performs runtime cast verification
// see test/06-Errors/error-runtime-cast.ck for an error case

// a subclass of Event
class Foo extends Event { }

// an Foo as Event
new Foo @=> Event @ e;

// cast to subclass
e $ Foo @=> Foo @ f;

// if we got here then ok
<<< "success" >>>;
