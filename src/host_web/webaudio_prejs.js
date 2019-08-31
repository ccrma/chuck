Module['any_port'] = undefined;

Module['print'] = function( text )
{
    console.log( text );
    if( Module['any_port'] )
    {
        Module['any_port'].postMessage( { type: "console print", message: text } );
    }
};

Module['printErr'] = function( text )
{
    console.error( text );
    if( Module['any_port'] )
    {
        Module['any_port'].postMessage( { type: "console print", message: text } );
    }
}
