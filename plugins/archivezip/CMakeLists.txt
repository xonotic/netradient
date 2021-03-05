radiant_plugin(archivezip
        archive.cpp archive.h
        pkzip.h
        plugin.cpp
        zlibstream.h
        )

find_package(GLIB REQUIRED)
target_include_directories(archivezip PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(archivezip PRIVATE ${GLIB_LIBRARIES})

find_package(ZLIB REQUIRED)
target_include_directories(archivezip PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(archivezip PRIVATE ${ZLIB_LIBRARIES})
