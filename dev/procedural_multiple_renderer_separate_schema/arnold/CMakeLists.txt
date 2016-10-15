INCLUDE_DIRECTORIES ( ../base )

ADD_LIBRARY ( procedural_multiple_renderer_separate_schema_arnold SHARED
  ArnoldPointsSchemaHandler.cpp
  ArnoldPolyMeshSchemaHandler.cpp
  ArnoldUtils.cpp
  abc2ass_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( procedural_multiple_renderer_separate_schema_arnold
  ProceduralSimbaBase
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )

SET_TARGET_PROPERTIES ( procedural_multiple_renderer_separate_schema_arnold
	PROPERTIES
    PREFIX ""
    )
 