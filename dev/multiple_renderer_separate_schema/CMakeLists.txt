ADD_EXECUTABLE ( multiple_renderer_separate_schema
  AbstractGeomSchemaHandler.cpp
  AbstractPointsSchemaHandler.cpp
  AbstractPolyMeshSchemaHandler.cpp
  ArnoldPointsSchemaHandler.cpp
  ArnoldPolyMeshSchemaHandler.cpp
  ArnoldUtils.cpp
  Utils.cpp
  abc2ass_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( multiple_renderer_separate_schema
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
