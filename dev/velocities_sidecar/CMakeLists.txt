IF (APPLE)
  SET ( CRYPTO_LIBRARY crypto )
ENDIF ()

ADD_EXECUTABLE ( generate_velocities_sidecar
  generate_velocities_sidecar_main.cpp
  )

TARGET_LINK_LIBRARIES ( generate_velocities_sidecar
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CRYPTO_LIBRARY}
  ${OPENSSL_SSL_LIBRARY}
  ${OPENSSL_CRYPTO_LIBRARY}
  )

ADD_EXECUTABLE ( read_velocities_sidecar
  read_velocities_sidecar_main.cpp
  )

TARGET_LINK_LIBRARIES ( read_velocities_sidecar
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CRYPTO_LIBRARY}
  ${OPENSSL_SSL_LIBRARY}
  ${OPENSSL_CRYPTO_LIBRARY}
  )
