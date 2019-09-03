var startButton = document.getElementById( "startChuck" );
var compileButton = document.getElementById( "compileButton" );
var replaceButton = document.getElementById( "replaceButton" );
var removeButton = document.getElementById( "removeButton" );
var clearButton = document.getElementById( "clearButton" );
startButton.addEventListener( "click", function() {
    startButton.disabled = true;
    startChuck();
 });

theChuckReady.then( function() {
    compileButton.disabled = false;
    replaceButton.disabled = false;
    removeButton.disabled = false;
    clearButton.disabled = false;
    
    chuckPrint = (function() {
        var element = document.getElementById('output');
        if (element) element.value = ''; // clear browser cache
        return function(text) {
            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
            // These replacements are necessary if you render to raw HTML
            //text = text.replace(/&/g, "&amp;");
            //text = text.replace(/</g, "&lt;");
            //text = text.replace(/>/g, "&gt;");
            //text = text.replace('\n', '<br>', 'g');
            if (element) {
                element.value += text + "\n";
                element.scrollTop = element.scrollHeight; // focus on bottom
            }
        };
    })();
    
    compileButton.addEventListener( "click", function chuckCompileButton() 
    {
        // send message to compile code 
        theChuck.runCode( chuckEditor.getValue() );
    });
    
    replaceButton.addEventListener( "click", function chuckReplaceButton()
    {
        // send message to replace last shred with this code
        theChuck.replaceCode( chuckEditor.getValue() );
    });
    
    removeButton.addEventListener( "click", function chuckRemoveButton()
    {
        // send message to remove most recent shred
        theChuck.removeLastCode();
    });
    
    clearButton.addEventListener( "click", function chuckClearButton()
    {
        // send message to clear vm
        theChuck.clearChuckInstance();
        // send message to clear globals
        theChuck.clearGlobals();
    });
});
