// verify that the 2 argument form of std.getenv returns the default 
// value when the requested environment variable does not exist.

// key
"C28FBF4D-CAF9-4180-B875-C9B9B768DDA5" => string env_key;
// default
"Charles Edward Anderson Berry" => string default_value;

// with default
Std.getenv(env_key, default_value) => string env_value1;
// test null
Std.getenv(null, default_value) => string env_value2;

// verify
if ( env_value1 == default_value &&
     env_value2 == default_value ) <<< "success" >>>;
