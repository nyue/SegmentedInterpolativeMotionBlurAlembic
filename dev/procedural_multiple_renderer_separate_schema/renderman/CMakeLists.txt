INCLUDE_DIRECTORIES ( ../base )

MESSAGE ( "SIMBA_DEV_REQUIRED_LINK_LIBRARIES = ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}")

SET ( SIMBA_RENDERMAN_PROCEDURAL_SOURCE
  RendermanPointsSchemaHandler.cpp
  RendermanPolyMeshSchemaHandler.cpp
  RendermanUtils.cpp
  abc2rib_separate_schema_main.cpp
  )

SET ( SIMBA_RENDERMAN_PROCEDURAL_LINK_LIBRARIES
  ProceduralSimbaBase
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${DELIGHT_3delight_LIBRARY}
  ${Boost_LIBRARIES}
  )

ADD_LIBRARY ( procedural_multiple_renderer_separate_schema_renderman SHARED
  ${SIMBA_RENDERMAN_PROCEDURAL_SOURCE}
  )

TARGET_LINK_LIBRARIES ( procedural_multiple_renderer_separate_schema_renderman
  ${SIMBA_RENDERMAN_PROCEDURAL_LINK_LIBRARIES}
  )

SET_TARGET_PROPERTIES ( procedural_multiple_renderer_separate_schema_renderman
	PROPERTIES
    PREFIX ""
    )

ADD_EXECUTABLE ( program_multiple_renderer_separate_schema_renderman
  ${SIMBA_RENDERMAN_PROCEDURAL_SOURCE}
  )

TARGET_LINK_LIBRARIES ( program_multiple_renderer_separate_schema_renderman
  ${SIMBA_RENDERMAN_PROCEDURAL_LINK_LIBRARIES}
  )
