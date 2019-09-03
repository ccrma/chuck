var startButton = document.getElementById( "startChuck" );
var compileButton = document.getElementById( "compileButton" );
var replaceButton = document.getElementById( "replaceButton" );
var removeButton = document.getElementById( "removeButton" );
var clearButton = document.getElementById( "clearButton" );
var shredsTable = document.getElementById( "shredstable" );
var shredsToRows = {}

// use named functions instead of anonymous ones
// so they can be replaced later if desired
var chuckCompileButton = function() 
{
    // send message to compile code 
    theChuck.runCode( chuckEditor.getValue() ).then( function( shredID )
    {
        addShredRow( shredID );
    }, function( failure ) {} );
}

var chuckReplaceButton = function()
{
    // send message to replace last shred with this code
    theChuck.replaceCode( chuckEditor.getValue() ).then( function( shreds )
    {
        removeShredRow( shreds.oldShred );
        addShredRow( shreds.newShred );
    }, function( failure ) {} );
}

var chuckRemoveButton = function()
{
    // send message to remove most recent shred
    theChuck.removeLastCode().then( function( shred )
    {
        removeShredRow( shred );
    }, function( failure ) {} );
}

var chuckClearButton = function()
{
    // send message to clear vm
    theChuck.clearChuckInstance();
    // send message to clear globals
    theChuck.clearGlobals();
}

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
    
    compileButton.addEventListener( "click", chuckCompileButton );
    replaceButton.addEventListener( "click", chuckReplaceButton );
    removeButton.addEventListener( "click", chuckRemoveButton );
    clearButton.addEventListener( "click", chuckClearButton );
});


function addShredRow( theShred )
{
    var row = shredsTable.insertRow();
    var cell0 = row.insertCell(0);
    var cell1 = row.insertCell(1);
    var cell2 = row.insertCell(2);
    var cell3 = row.insertCell(3);
    
    shredsToRows[ theShred ] = row;
    
    cell0.innerHTML = "" + theShred;
    cell1.innerHTML = chuckEditor.getValue().substring(0, 30) + "...";
    (function( cell )
    {
        var getTime = function() { return Math.floor( Date.now() / 1000 ); }
        var formatTime = function(i) {
            // add zero in front of numbers < 10
            if (i < 10) {i = "0" + i};  
            return i;
        }
        
        var startTime = getTime();
        function updateTime()
        {
            var now = getTime();
            var elapsed = now - startTime;
            var m = Math.floor( elapsed / 60 );
            var s = Math.floor( elapsed % 60 );
            if( document.contains( cell ) )
            {
                cell.innerHTML = formatTime(m) + ":" + formatTime(s);
                setTimeout( updateTime, 1000 );
            }
        }
        updateTime();
    })(cell2);
    var removeButton = document.createElement("BUTTON");
    removeButton.innerHTML = "Remove"
    cell3.appendChild( removeButton );
    
    removeButton.addEventListener( "click", (function( shredID )
    {
        return function()
        {
            theChuck.removeShred( shredID ).then( function( removedShred )
            {
                removeShredRow( theShred );    
            }, function( failure ) { console.log( failure ); } );
        }
    })( theShred ) );
}

function removeShredRow( theShred )
{    
    shredsToRows[ theShred ].parentNode.removeChild( shredsToRows[ theShred ] );
    delete shredsToRows[ theShred ];
}
