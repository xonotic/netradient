if (BUILD_DAEMONMAP)
    # Always keep daemonmap libs/
    # up-to-date with NetRadiant libs/
    include_directories(daemonmap/libs)
    include_directories(daemonmap/tools)

    add_subdirectory(daemonmap/libs)
    add_subdirectory(daemonmap/tools)
endif()
