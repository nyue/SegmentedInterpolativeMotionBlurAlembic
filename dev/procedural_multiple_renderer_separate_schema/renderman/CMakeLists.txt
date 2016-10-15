INCLUDE_DIRECTORIES ( ../base )

MESSAGE ( "SIMBA_DEV_REQUIRED_LINK_LIBRARIES = ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}")

ADD_LIBRARY ( procedural_multiple_renderer_separate_schema_renderman SHARED
  RendermanPointsSchemaHandler.cpp
  RendermanPolyMeshSchemaHandler.cpp
  RendermanUtils.cpp
  abc2rib_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( procedural_multiple_renderer_separate_schema_renderman
  ProceduralSimbaBase
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${DELIGHT_3delight_LIBRARY}
  ${Boost_LIBRARIES}
  )

SET_TARGET_PROPERTIES ( procedural_multiple_renderer_separate_schema_renderman
	PROPERTIES
    PREFIX ""
    )
 