Notes on Building WebChucK
--------------------------
winter 2023: this document created (Ge)

WebChucK is built from ChucK core and with an emcripten-based host.
Both are maintained in the chuck repository:
https://github.com/ccrma/chuck

In your local clone of the repository,
chuck/src/core: contains the ChucK compiler, VM, and audio engine
chuck/host_web: contains the emcripten-based host


compiling WebChucK
------------------
1. install emscripten
https://emscripten.org/docs/getting_started/downloads.html

2. in chuck/src/, first 'make XXX', where XXX is your local platform;
this is to pick up the lexer and parser generated files.
(FYI: run 'make' without arguments to see available platform targets)

3. in chuck/src/, next 'make web'
(may need to source emsdk_env.sh, wherever it's installed from step 1)

4. if the compilation is successful, it should create two files:
chuck/src/host_web/webchuck/js/webchuck.wasm
chuck/src/host_web/webchuck/js/webchuck.js


deploying webchuck
------------------
WebChucK modules can be deployed on websites; they runs entirely on the 
client-side within web browsers (i.e., no server backend needed).

see webchuck repository
https://github.com/ccrma/webchuck

helpful overview for creating your own WebChucK project
https://github.com/ccrma/webchuck/blob/main/README.md

As explained in the README.md above, to deploy WebChucK you only need to 
"link" your HTML/JS files to access three files: webchuck.wasm, 
webchuck.js, and webchuck_host.js. This can either be done
1) by referencing a CDN service URL from your index.html (easy to get
started; works with respect to CORS-related HTTP security restrictions)
2) OR by hosting these three files on your webspace (self-contained; 
does not rely on external CDN services; allows customizing your own 
webchuck_host.js). You can download the most recent versions of these
files here: https://chuck.stanford.edu/webchuck/src/

a deployment of WebChucK IDE on the ChucK website
https://chuck.stanford.edu/webchuck/
https://chuck.cs.stanford.edu/webchuck/


----------------
"How much web could a WebChucK ChucK if we all could ChucK on the web?"
