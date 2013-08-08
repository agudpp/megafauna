include(${DEV_ROOT_PATH}/gps/heightmap/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/height_map/HeightMapTool.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/height_map/HeightMapTool.cpp
	${DEV_ROOT_PATH}/tools/height_map/main.cpp
)


