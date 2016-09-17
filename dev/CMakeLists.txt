SET ( SIMBA_DEV_REQUIRED_LINK_LIBRARIES
  ${Alembic_Alembic_LIBRARY}
  ${Ilmbase_HALF_LIBRARY}
  ${HDF5_hdf5_LIBRARY}
  )

ADD_EXECUTABLE ( query_alembic
  query_alembic_main.cpp
  )

ADD_EXECUTABLE ( abc2obj_velocities_as_normals
  abc2obj_velocities_as_normals_main.cpp
  )

ADD_EXECUTABLE ( abc2ass
  abc2ass_main.cpp
  )

TARGET_LINK_LIBRARIES ( query_alembic
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  )

TARGET_LINK_LIBRARIES ( abc2obj_velocities_as_normals
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  )

TARGET_LINK_LIBRARIES ( abc2ass
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
