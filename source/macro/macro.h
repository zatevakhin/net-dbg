#ifndef  LIB_MACRO_MISC_MACRO
#define  LIB_MACRO_MISC_MACRO

// #define lambda(l_ret_type, l_arguments, l_body)       \
// ({                                                    \
//     l_ret_type l_anonymous_functions_name l_arguments \
//     l_body                                            \
//     &l_anonymous_functions_name;                      \
// })

#define lambda(r, a, b) ({ r __lambda__ a b &__lambda__; })

#ifndef array_size
  #define array_size(a) (sizeof(a) / sizeof(*a))
#endif

#endif // LIB_MACRO_MISC_MACRO
