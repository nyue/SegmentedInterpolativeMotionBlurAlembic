INCLUDE_DIRECTORIES ( ../lib )

ADD_EXECUTABLE ( simba_unit_test
  unit_test_main.cpp
  )

TARGET_LINK_LIBRARIES ( simba_unit_test
  ${Boost_LIBRARIES}
  )
