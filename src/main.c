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

Color test_color = (Color) {.r=200, .g=100, .b=100, .a=100};
wasm_module_inst_t module_inst;

// these are used in wasm to print args it received

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

static void echo_wasmimport(wasm_exec_env_t exec_env, char* str) {
  printf("%s\n", str);
}

static void return_color_wasmimport(wasm_exec_env_t exec_env, void* retPointer) {
  memcpy(retPointer,  &test_color, sizeof(Color));
}

static uint32_t return_color_pointer_wasmimport(wasm_exec_env_t exec_env) {
  return wasm_runtime_module_dup_data(module_inst, (const char*) &test_color, sizeof(Color));
}

// even though on the other side, half of these are values, I use pointers for all, since they seem to be coerced that way
static NativeSymbol native_symbols[] = {
  { "debug_color", debug_color_wasmimport, "(*)" },
  { "debug_color_pointer", debug_color_pointer_wasmimport, "(*)" },
  { "debug_dimensions", debug_dimensions_wasmimport, "(*)" },
  { "debug_dimensions_pointer", debug_dimensions_pointer_wasmimport, "(*)" },
  { "echo", echo_wasmimport, "($)" },
  { "return_color", return_color_wasmimport, "(*)" },
  { "return_color_pointer", return_color_pointer_wasmimport, "()i" }
};

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

  // setup env & load wasm
  wasm_module_t module = wasm_runtime_load(wasmBytes, wasmSize, error_buf, sizeof(error_buf));
  module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, sizeof(error_buf));
  wasm_exec_env_t exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
  free(wasmBytes);

  // find exports

  // ret_color_by_value(i32) -> nil
  wasm_function_inst_t cart_ret_color_by_value = wasm_runtime_lookup_function(module_inst, "ret_color_by_value", "()*");

  // ret_color_by_pointer() -> i32
  wasm_function_inst_t cart_ret_color_by_pointer = wasm_runtime_lookup_function(module_inst, "ret_color_by_pointer", "()*");

  // param_color_by_pointer(i32) -> nil
  wasm_function_inst_t cart_param_color_by_pointer = wasm_runtime_lookup_function(module_inst, "param_color_by_pointer", "(*)");

  // param_color_by_value(i32) -> nil
  wasm_function_inst_t cart_param_color_by_value = wasm_runtime_lookup_function(module_inst, "param_color_by_value", "(*)");

  // ret_dimensions_by_value(i32) -> nil
  wasm_function_inst_t cart_ret_dimensions_by_value = wasm_runtime_lookup_function(module_inst, "ret_dimensions_by_value", "()*");

  // ret_dimensions_by_pointer() -> i32
  wasm_function_inst_t cart_ret_dimensions_by_pointer = wasm_runtime_lookup_function(module_inst, "ret_dimensions_by_pointer", "()*");

  // param_dimensions_by_pointer(i32) -> nil
  wasm_function_inst_t cart_param_dimensions_by_pointer = wasm_runtime_lookup_function(module_inst, "param_dimensions_by_pointer", "(*)");

  // param_dimensions_by_value(i32) -> nil
  wasm_function_inst_t cart_param_dimensions_by_value = wasm_runtime_lookup_function(module_inst, "param_dimensions_by_value", "(*)");

  if (!cart_ret_color_by_value) {
    printf("no cart_ret_color_by_value\n");
    return 1;
  }

  if (!cart_ret_color_by_pointer) {
    printf("no cart_ret_color_by_pointer\n");
    return 1;
  }

  if (!cart_param_color_by_pointer) {
    printf("no cart_param_color_by_pointer\n");
    return 1;
  }

  if (!cart_param_color_by_value) {
    printf("no cart_param_color_by_value\n");
    return 1;
  }

  if (!cart_ret_dimensions_by_value) {
    printf("no cart_ret_dimensions_by_value\n");
    return 1;
  }

  if (!cart_ret_dimensions_by_pointer) {
    printf("no cart_ret_dimensions_by_pointer\n");
    return 1;
  }

  if (!cart_param_dimensions_by_pointer) {
    printf("no cart_param_dimensions_by_pointer\n");
    return 1;
  }

  if (!cart_param_dimensions_by_value) {
    printf("no cart_param_dimensions_by_value\n");
    return 1;
  }

  // TEST: verify struct-args (WASM(arg)->host) and (host(return)->WASM)

  // call main
  wasm_application_execute_main(module_inst, 0, NULL);

  uint32_t test_args[1];

  // TEST: verify struct-args (host(arg)->WASM)
  // allocate & copy color param into WASM-space, then call function
  test_args[0] = wasm_runtime_module_dup_data(module_inst, (const char*) &test_color, sizeof(Color));
  if (!wasm_runtime_call_wasm(exec_env, cart_param_color_by_value, 1, test_args) ) {
    printf("%s\n", wasm_runtime_get_exception(module_inst));
  }
  wasm_runtime_module_free(module_inst, test_args[0]);

  // TEST: verify struct-pointer-args (host(arg)->WASM)
  // allocate & copy color param into WASM-space, then call function
  test_args[0] = wasm_runtime_module_dup_data(module_inst, (const char*) &test_color, sizeof(Color));
  if (!wasm_runtime_call_wasm(exec_env, cart_param_color_by_pointer, 1, test_args) ) {
    printf("%s\n", wasm_runtime_get_exception(module_inst));
  }
  wasm_runtime_module_free(module_inst, test_args[0]);
  

  return 0;
}

