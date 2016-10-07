ADD_EXECUTABLE ( abc2ass
  abc2ass_main.cpp
  )

TARGET_LINK_LIBRARIES ( abc2ass
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
