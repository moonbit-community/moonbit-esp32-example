idf_component_register(SRCS
    "main.c"
    INCLUDE_DIRS "."
    REQUIRES moonbit_app
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-braces)
