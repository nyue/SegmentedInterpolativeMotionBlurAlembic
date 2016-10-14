INCLUDE_DIRECTORIES ( ../base )

ADD_EXECUTABLE ( multiple_renderer_separate_schema_arnold
  ArnoldPointsSchemaHandler.cpp
  ArnoldPolyMeshSchemaHandler.cpp
  ArnoldUtils.cpp
  abc2ass_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( multiple_renderer_separate_schema_arnold
  SimbaBase
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )

