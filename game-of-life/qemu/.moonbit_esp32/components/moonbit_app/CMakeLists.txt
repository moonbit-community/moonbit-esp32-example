idf_component_register(
    INCLUDE_DIRS "include"
)
add_prebuilt_library(moonbit_app "libmoonbit_app.a"
)

target_link_libraries(${COMPONENT_LIB} INTERFACE moonbit_app)
