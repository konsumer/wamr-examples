Install the [wamr sdk](https://github.com/bytecodealliance/wasm-micro-runtime/releases) to `wamr-sdk/`.

```
# config
cmake -B build

# build
cmake --build build

# run
./build/host ./build/example/example.wasm
```
