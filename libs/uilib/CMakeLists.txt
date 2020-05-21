add_library(uilib STATIC
        uilib.cpp
        )

find_package(GLIB REQUIRED)
target_include_directories(uilib PUBLIC ${GLIB_INCLUDE_DIRS})
target_link_libraries(uilib PUBLIC ${GLIB_LIBRARIES})

find_package(Pango REQUIRED)
target_include_directories(uilib PUBLIC ${Pango_INCLUDE_DIRS} ${PangoFT2_INCLUDE_DIRS})
target_link_libraries(uilib PUBLIC ${Pango_LIBRARIES} ${PangoFT2_LIBRARIES})

find_package(GTK${GTK_TARGET} REQUIRED)
target_include_directories(uilib PUBLIC ${GTK${GTK_TARGET}_INCLUDE_DIRS})
target_link_libraries(uilib PUBLIC ${GTK${GTK_TARGET}_LIBRARIES})

target_include_directories(uilib PUBLIC gtkutil)
target_link_libraries(uilib PUBLIC gtkutil)
