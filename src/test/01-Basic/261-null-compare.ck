// verifying == and != for null references

null @=> Object a;
Object @ b;

if( (a==b) && !(a!=b) ) <<< "success" >>>;
