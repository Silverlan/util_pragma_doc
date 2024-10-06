include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

option(CONFIG_ENABLE_EXECUTABLE_FLAG "Build as executable?" ON)

set(PROJ_NAME util_pragma_doc)

if(CONFIG_ENABLE_EXECUTABLE_FLAG)
	pr_add_executable(${PROJ_NAME})
else()
	pr_add_library(${PROJ_NAME} STATIC)
endif()

pr_add_dependency(${PROJ_NAME} util_udm TARGET PUBLIC)
if(CONFIG_ENABLE_EXECUTABLE_FLAG)
	pr_add_compile_definitions(${PROJ_NAME} -DUTIL_PRAGMA_DOC_EXE)
endif()

pr_init_module(${PROJ_NAME})

pr_finalize(${PROJ_NAME})
