if (ENABLE_PYTHON_BINDINGS)
  add_subdirectory(scrimmage/bindings)
endif()

set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in)
set(SETUP_PY ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY})