// this is an example "cart" that will be compiled to wasm

#include <stdlib.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;

#define NULL0_EXPORT(n) __attribute__((export_name(n)))
#define NULL0_IMPORT(n) __attribute__((import_module("null0"), import_name(n)))

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


// these are example values for returning
Color RED = (Color) {.r=230, .g=41, .b=55, .a=255};
Dimensions dim_example = (Dimensions) { .width=100, .height=100 };


// use these to print args that come from host

NULL0_IMPORT("debug_color")
void debug_color(Color v);

NULL0_IMPORT("debug_color_pointer")
void debug_color_pointer(Color* v);

NULL0_IMPORT("debug_dimensions")
void debug_dimensions(Dimensions v);

NULL0_IMPORT("debug_dimensions_pointer")
void debug_dimensions_pointer(Dimensions* v);

NULL0_IMPORT("echo")
void echo(char* str);

NULL0_IMPORT("return_color")
Color return_color();

NULL0_IMPORT("return_color_pointer")
Color* return_color_pointer();

// these functions are exported to host

// ret_color_by_value(i32) -> nil
NULL0_EXPORT("ret_color_by_value")
Color ret_color_by_value() {
  return RED;
}

// ret_color_by_pointer() -> i32
NULL0_EXPORT("ret_color_by_pointer")
Color* ret_color_by_pointer() {
  return &RED;
}

// param_color_by_pointer(i32) -> nil
NULL0_EXPORT("param_color_by_pointer")
void param_color_by_pointer(Color* color) {
  echo("param_color_by_pointer");
  debug_color_pointer(color);
  echo("");
}

// param_color_by_value(i32) -> nil
NULL0_EXPORT("param_color_by_value")
void param_color_by_value(Color color) {
  echo("param_color_by_value");
  debug_color(color);
  echo("");
}

// ret_dimensions_by_value(i32) -> nil
NULL0_EXPORT("ret_dimensions_by_value")
Dimensions ret_dimensions_by_value() {
  return dim_example;
}

// ret_dimensions_by_pointer() -> i32
NULL0_EXPORT("ret_dimensions_by_pointer")
Dimensions* ret_dimensions_by_pointer() {
  return &dim_example;
}

// param_dimensions_by_pointer(i32) -> nil
NULL0_EXPORT("param_dimensions_by_pointer")
void param_dimensions_by_pointer(Dimensions* d) {
  echo("param_dimensions_by_pointer");
  debug_dimensions_pointer(d);
  echo("");
}

// param_dimensions_by_value(i32) -> nil
NULL0_EXPORT("param_dimensions_by_value")
void param_dimensions_by_value(Dimensions d) {
  echo("param_dimensions_by_value");
  debug_dimensions(d);
  echo("");
}

int main () {
  // check sending a struct arg to host
  echo("RED");
  debug_color(RED);
  echo("");

  echo("dim_example");
  debug_dimensions(dim_example);
  echo("");

  // check sending a struct-pointer to host
  echo("&RED");
  debug_color_pointer(&RED);
  echo("");

  echo("&dim_example");
  debug_dimensions_pointer(&dim_example);
  echo("");

  // check getting a return-value from host
  Color c = return_color();
  echo("return_color");
  debug_color(c);
  echo("");

  // check getting a return-pointer from host
  Color* c2 = return_color_pointer();
  echo("return_color_pointer");
  debug_color_pointer(c2);
  echo("");

  return 0;
}