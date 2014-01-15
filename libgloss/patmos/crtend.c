typedef void (*funptr_t)(void);
funptr_t __init_array_end[0] __attribute__((section(".init_array"), used)) = { };
funptr_t __fini_array_end[0] __attribute__((section(".fini_array"), used)) = { };
