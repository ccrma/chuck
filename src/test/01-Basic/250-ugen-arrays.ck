// connecting => with arrays of UGens
// (requires chuck-1.5.2.2 or higher)
// there are several possibilities...

// 1) LHS[X] => RHS: each elements in LHS => to RHS
Gain A[3] => Gain B;
for( UGen a : A ) <<< "1) connected:", a.isConnectedTo(B) >>>;

// 2) LHS[X] => RHS[X]: one to one mapping 
Gain C[3] => Gain D[3];
for( int i; i < C.size(); i++ )
    <<< "2) connected:", C[i].isConnectedTo(D[i]) >>>;

// 3) LHS[X] => RHS[Y]: one to one mapping up to min(X,Y), after
// which elements in the smaller array will modulo to the beginning
// and connect to remaining elements in larger array
Gain E[2] => Gain F[3];
Math.max(E.size(), F.size()) => int greater;
for( int i; i < greater; i++ )
    <<< "3) connected:", E[i%E.size()].isConnectedTo(F[i%F.size()]) >>>;

// 4) LHS => RHS[X]: LHS => to each element in RHS
Gain G => Gain H[3];
for( UGen h : H ) <<< "4) connected:", G.isConnectedTo(h) >>>;
