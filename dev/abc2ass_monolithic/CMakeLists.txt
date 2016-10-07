ADD_EXECUTABLE ( abc2ass_monolithic
  abc2ass_monolithic_main.cpp
  )

TARGET_LINK_LIBRARIES ( abc2ass_monolithic
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
