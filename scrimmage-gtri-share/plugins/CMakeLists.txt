add_subdirectory(autonomy)
add_subdirectory(interaction)
add_subdirectory(metrics)

# Push up the SCRIMMAGE_LIBS variable
set(PROJECT_LIBS ${PROJECT_LIBS} PARENT_SCOPE)
