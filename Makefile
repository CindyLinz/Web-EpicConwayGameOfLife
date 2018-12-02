all:
	emcc life.c -O2 -s USE_SDL=2 -s EXPORTED_FUNCTIONS='["_main","_resize"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -o life.js
