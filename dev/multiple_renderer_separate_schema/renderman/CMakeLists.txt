INCLUDE_DIRECTORIES ( ../base )

ADD_EXECUTABLE ( multiple_renderer_separate_schema_renderman
  RendermanPointsSchemaHandler.cpp
  RendermanPolyMeshSchemaHandler.cpp
  RendermanUtils.cpp
  abc2rib_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( multiple_renderer_separate_schema_renderman
  SimbaBase
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
