ADD_EXECUTABLE ( query_alembic
  query_alembic_main.cpp
  )

TARGET_LINK_LIBRARIES ( query_alembic
  ${Alembic_Alembic_LIBRARY}
  ${Ilmbase_HALF_LIBRARY}
  ${HDF5_hdf5_LIBRARY}
  )
