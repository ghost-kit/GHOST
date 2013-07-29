IO Library Module
=================
### Using as a submodule
* Add as a submodule to your project <code>git submodule add git://github.com/ghost-kit/io</code>
* In your master CMakeLists.txt file, use cached variable to activate <code>AS_SUBMODULE</code>
* <code> set(AS_SUBMODULE true CACHE INTERNAL "") </code>
* If you have trouble with -fPIC being needed, pass the following parameter
* <code>set(USE_FPIC true CACHE INTERNAL "")</code> 
