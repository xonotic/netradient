radiant_plugin(mapxml
        plugin.cpp
        xmlparse.cpp xmlparse.h
        xmlwrite.cpp xmlwrite.h
        )

target_include_directories(mapxml PRIVATE xmllib)
target_link_libraries(mapxml PRIVATE xmllib)
