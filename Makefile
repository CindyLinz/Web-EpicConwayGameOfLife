all:
	emcc life.c -O2 -s USE_SDL=2 -s INVOKE_RUN=0 -s EXPORTED_FUNCTIONS=@exports.json -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -o life.js
