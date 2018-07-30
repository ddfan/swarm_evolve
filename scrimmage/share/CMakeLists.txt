add_subdirectory(scrimmage)
add_subdirectory(scrimmage-learn)
add_subdirectory(scrimmage-learncompete)
add_subdirectory(scrimmage-playlearned)
if (NOT EXTERNAL AND ${VTK_FOUND})
    add_subdirectory(scrimmage-viz)
    add_subdirectory(scrimmage-playback)
endif()

