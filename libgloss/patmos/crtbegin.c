typedef void (*funptr_t)(void);
// Use a fake constructor/destructor "priority" to make the symbols
// end up in the right place.
funptr_t __init_array_begin[0] __attribute__((section(".init_array.0"), used)) = { };
funptr_t __fini_array_begin[0] __attribute__((section(".fini_array.0"), used)) = { };
