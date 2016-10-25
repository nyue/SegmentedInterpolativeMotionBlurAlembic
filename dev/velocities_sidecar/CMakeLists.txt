ADD_LIBRARY ( VelocitySideCar STATIC
  VelocitySideCar.cpp
  )

ADD_EXECUTABLE ( generate_velocities_sidecar
  generate_velocities_sidecar_main.cpp
  )

TARGET_LINK_LIBRARIES ( generate_velocities_sidecar
  VelocitySideCar
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CryptoPP_cryptopp_LIBRARY}
  # ${OPENSSL_SSL_LIBRARY}
  # ${OPENSSL_CRYPTO_LIBRARY}
  )

ADD_EXECUTABLE ( read_velocities_sidecar
  read_velocities_sidecar_main.cpp
  )

TARGET_LINK_LIBRARIES ( read_velocities_sidecar
  VelocitySideCar
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CryptoPP_cryptopp_LIBRARY}
  # ${OPENSSL_SSL_LIBRARY}
  # ${OPENSSL_CRYPTO_LIBRARY}
  )
