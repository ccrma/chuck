// Verify that the 2 argument form of std.getenv returns the default value when the
// requested environment variable does not exist.

"C28FBF4D-CAF9-4180-B875-C9B9B768DDA5" => string env_key;
"Charles Edward Anderson Berry" => string default_value;

Std.getenv(env_key, default_value) => string env_value;

if ( env_value == default_value ) <<< "success" >>>;
