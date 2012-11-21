/////////////////////////////
// ScopeTest.ck
// Test Scoping of variables and virtual functions in extended classes.
//
/////////////////////////////
 

function void Assert(int condition, string message)
{
  if (!condition)
  {
    <<< "assertion failed: ", message >>>;
    me.exit();
  }
}
 
1 => int globalVariable;
Assert( globalVariable == 1, "ScopeTest-1 Global Assignment" );
 
string globalAndBaseclassVariable;
 

class Base
{
  int globalAndBaseclassVariable;
 
  2 => globalVariable;
  Assert(globalVariable == 2,"ScopeTest-2 Global Assignment from constructor.");
 
  3 => int v;
  Assert(v == 3,"ScopeTest-3 Assigment to class variable in ctor");
 
  function int GetV()
  {
    return v;
  }

  function int GetVVirtual()
  {
    return v;
  }
}
 

class Super extends Base
{
  4 => int v2;
  Assert(v2 == 4,"ScopeTest-4 Assigment to class scope from extended class.");
  5 => v;
  Assert(v == 5, "ScopeTest-5 Assignment to variable in superclass.");
  6 => v;
  Assert(GetV() == 6, "ScopeTest-6 Assignment to variable in superclass.");
 
  7 => globalVariable;
  Assert(globalVariable == 7, "ScopeTest-7 assignement to global variable in extended class");
 
  function int Test()
  {
    return GetV();
  }

  function int GetVVirtual()
  {
    return v2;
  }

  function int GetglobalAndBaseclassVariable()
  {
    return globalAndBaseclassVariable;
  }
}
 

Base b;
Assert(globalVariable == 2, "ScopeTest-2.1 constructor side-effects in global scope.");
 

Super s;
Assert(globalVariable == 7, "ScopeTest-7.1 superclass constructor side-effects in global scope.");
 

8 => b.v;
Assert(b.v == b.GetV(),"ScopeTest-8 assigment to class-scope variable.");

9 => s.v;
Assert(s.v == s.GetV(),"ScopeTest-9 assigment to base-class-scope variable.");

10 => s.v2;
Assert(s.v == s.GetV(),"ScopeTest-10 assigment to super-class-scope variable.");
Assert(s.v2 == 10,"ScopeTest-10.1 assigment to super-class-scope variable.");
Assert(s.v2 == s.GetVVirtual(),"ScopeTest-10.1 virtual function resolution.");
 

"110" => globalAndBaseclassVariable;
111 => s.globalAndBaseclassVariable;
// Assert(globalAndBaseclassVariable == "110","ScopeTest-11.1 aliased global and base-class variable.");
Assert(s.GetglobalAndBaseclassVariable() == 111,"ScopeTest-11.2 aliased global and base-class variable.");
Assert(s.globalAndBaseclassVariable == 111,"ScopeTest-11.3 aliased global and base-class variable.");

<<< "success" >>>;