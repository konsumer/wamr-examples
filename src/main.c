// this is the native host for wasm

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "wasm_c_api.h"
#include "wasm_export.h"

// these types are shared with cart

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;

typedef struct Dimensions {
  u32 width;
  u32 height;
} Dimensions;

typedef struct Color {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} Color;

// these are used in wasm to print args it received
// even though on the other side, half of these are values, I use pointers for all

static void debug_color_wasmimport(wasm_exec_env_t exec_env, Color* c) {
  printf("Color: (%d, %d, %d, %d)\n", c->r, c->g, c->b, c->a);
}

static void debug_color_pointer_wasmimport(wasm_exec_env_t exec_env, Color* c) {
  printf("Color*: (%d, %d, %d, %d)\n", c->r, c->g, c->b, c->a);
}

static void debug_dimensions_wasmimport(wasm_exec_env_t exec_env, Dimensions* d) {
  printf("Dimensions: (%d, %d)\n", d->width, d->height);
}

static void debug_dimensions_pointer_wasmimport(wasm_exec_env_t exec_env, Dimensions* d) {
  printf("Dimensions*: (%d, %d)\n", d->width, d->height);
}

static NativeSymbol native_symbols[] = {
  { "debug_color", debug_color_wasmimport, "(*)" },
  { "debug_color_pointer", debug_color_pointer_wasmimport, "(*)" },
  { "debug_dimensions", debug_dimensions_wasmimport, "(*)" },
  { "debug_dimensions_pointer", debug_dimensions_pointer_wasmimport, "(*)" },
};

// this makes iwasm --native-lib option work
uint32_t get_native_lib(char** p_module_name, NativeSymbol** p_native_symbols) {
  *p_module_name = "null0";
  *p_native_symbols = native_symbols;
  return sizeof(native_symbols) / sizeof(NativeSymbol);
}

unsigned char* load_file(char* filename, unsigned long* byteSize) {
  FILE *f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  *byteSize = fsize;
  unsigned char* bytes = malloc(fsize + 1);
  fread(bytes, fsize, 1, f);
  fclose(f);
  return bytes;
}

int main(int argc, char *argv[]) {
  unsigned long wasmSize;
  char *buffer, error_buf[128];
  unsigned char* wasmBytes;
  static char global_heap_buf[512 * 1024];
  
  // setup wasm runtime
  wasmBytes = load_file(argv[1], &wasmSize);
  RuntimeInitArgs init_args;
  memset(&init_args, 0, sizeof(RuntimeInitArgs));
  uint32_t stack_size = 8092, heap_size = 8092;
  init_args.mem_alloc_type = Alloc_With_Pool;
  init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
  init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

  // setup imports
  init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
  init_args.native_module_name = "null0";
  init_args.native_symbols = native_symbols;

  if (!wasm_runtime_full_init(&init_args)) {
    printf("Init runtime environment failed.\n");
    return 1;
  }

  // setup env
  wasm_exec_env_t exec_env;
  wasm_module_inst_t module_inst;
  wasm_module_t module = wasm_runtime_load(wasmBytes, wasmSize, error_buf, sizeof(error_buf));
  module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, sizeof(error_buf));
  exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);

  free(wasmBytes);

  return 0;
}

