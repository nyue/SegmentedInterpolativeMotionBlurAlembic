SET ( SIMBA_DEV_REQUIRED_LINK_LIBRARIES
  ${Alembic_Alembic_LIBRARY}
  ${Ilmbase_Half_LIBRARY}
  ${Ilmbase_Iex_LIBRARY}
  ${HDF5_hdf5_LIBRARY}
  ${HDF5_hdf5_hl_LIBRARY}
  ${GLOG_glog_LIBRARY}
  ${CMAKE_DL_LIBS}
  )

IF (OLD_DEVELOPMENT)
  ADD_EXECUTABLE ( query_alembic
	query_alembic_main.cpp
	)
  
  ADD_EXECUTABLE ( abc2obj_velocities_as_normals
	abc2obj_velocities_as_normals_main.cpp
	)
  
  ADD_EXECUTABLE ( generate_points_test_data
	generate_points_test_data_main.cpp
	)
  
  ADD_EXECUTABLE ( multiarray_arnoldarray_layout_check
	multiarray_arnoldarray_layout_check_main.cpp
	)
  
  TARGET_LINK_LIBRARIES ( query_alembic
	${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
	)
  
  TARGET_LINK_LIBRARIES ( abc2obj_velocities_as_normals
	${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
	)
  
  TARGET_LINK_LIBRARIES ( generate_points_test_data
	${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
	${Arnold_ai_LIBRARY}
	)
  
  TARGET_LINK_LIBRARIES ( multiarray_arnoldarray_layout_check
	${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
	${Arnold_ai_LIBRARY}
	)
  
  ADD_SUBDIRECTORY ( abc2ass_monolithic )
  ADD_SUBDIRECTORY ( abc2ass_separate_schema )
  ADD_SUBDIRECTORY ( multiple_renderer_separate_schema )
ENDIF ()

ADD_SUBDIRECTORY ( velocities_sidecar )
ADD_SUBDIRECTORY ( procedural_multiple_renderer_separate_schema )
