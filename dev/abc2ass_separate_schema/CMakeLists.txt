ADD_EXECUTABLE ( abc2ass_separate_schema
  abc2ass_separate_schema_main.cpp
  )

TARGET_LINK_LIBRARIES ( abc2ass_separate_schema
  ${SIMBA_DEV_REQUIRED_LINK_LIBRARIES}
  ${Arnold_ai_LIBRARY}
  )
