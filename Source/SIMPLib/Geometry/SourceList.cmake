
set(SUBDIR_NAME Geometry)


set(SIMPLib_${SUBDIR_NAME}_HDRS
  ${SIMPLib_SOURCE_DIR}/Geometry/CompositeTransformContainer.h
  ${SIMPLib_SOURCE_DIR}/Geometry/EdgeGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/GeometryHelpers.h
  ${SIMPLib_SOURCE_DIR}/Geometry/HexahedralGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry2D.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry3D.h
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometryGrid.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ImageGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ITransformContainer.h
  ${SIMPLib_SOURCE_DIR}/Geometry/MeshStructs.h
  ${SIMPLib_SOURCE_DIR}/Geometry/QuadGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/RectGridGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.h
  ${SIMPLib_SOURCE_DIR}/Geometry/TetrahedralGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/TransformContainer.h
  ${SIMPLib_SOURCE_DIR}/Geometry/TriangleGeom.h
  ${SIMPLib_SOURCE_DIR}/Geometry/VertexGeom.h
)

set(SIMPLib_${SUBDIR_NAME}_SRCS
  ${SIMPLib_SOURCE_DIR}/Geometry/CompositeTransformContainer.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/EdgeGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/GeometryHelpers.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/HexahedralGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry2D.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometry3D.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/IGeometryGrid.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ImageGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/QuadGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/RectGridGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CubeOctohedronOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderAOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderBOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/CylinderCOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/EllipsoidOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/ShapeOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/ShapeOps/SuperEllipsoidOps.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/TetrahedralGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/TransformContainer.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/TriangleGeom.cpp
  ${SIMPLib_SOURCE_DIR}/Geometry/VertexGeom.cpp
)

if(SIMPL_USE_EIGEN)
  set(SIMPLib_${SUBDIR_NAME}_HDRS
    ${SIMPLib_${SUBDIR_NAME}_HDRS}
    ${SIMPLib_SOURCE_DIR}/Geometry/DerivativeHelpers.h
  )

  set(SIMPLib_${SUBDIR_NAME}_SRCS
    ${SIMPLib_${SUBDIR_NAME}_SRCS}
    ${SIMPLib_SOURCE_DIR}/Geometry/DerivativeHelpers.cpp
  )
endif()


cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR_NAME}" "${SIMPLib_${SUBDIR_NAME}_HDRS};${SIMPLib_${SUBDIR_NAME}_Moc_HDRS}" "${SIMPLib_${SUBDIR_NAME}_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/${SUBDIR_NAME}" "" "${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS}" "0")

set(SIMPLib_Geometry_HDRS
  ${SIMPLib_Geometry_HDRS}
)

#if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
#    INSTALL (FILES ${SIMPLib_Geometry_HDRS}
#            DESTINATION include/DREAM3D/Geometry
#            COMPONENT Headers   )
#endif()


#-------------------------------------------------------------------------------
# Add the unit testing sources
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()

