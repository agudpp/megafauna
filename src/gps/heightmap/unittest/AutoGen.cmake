include(${DEV_ROOT_PATH}/gps/heightmap/AutoGen.cmake)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/gps/heightmap/unittest/test.cpp
)

# libs
set (COMMON_LIBRARIES ${COMMON_LIBRARIES}
	UnitTest++
)
