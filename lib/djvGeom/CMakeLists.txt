set(header
    Namespace.h
    PointList.h
    PointListInline.h
    Shape.h
    ShapeInline.h
    TriangleMesh.h
    TriangleMeshFunc.h
    TriangleMeshInline.h)
set(source
    PointList.cpp
    Shape.cpp
    TriangleMesh.cpp
    TriangleMeshFunc.cpp)

add_library(djvGeom ${header} ${source})
set(LIBRARIES
    djvCore
    GLM)
target_link_libraries(djvGeom ${LIBRARIES})
set_target_properties(
    djvGeom
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvGeom
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvGeom)

