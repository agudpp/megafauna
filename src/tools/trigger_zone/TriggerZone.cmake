set (HDRS ${HDRS}
	${DEV_ROOT_PATH}/ui/cursor/MouseCursor.h
	${DEV_ROOT_PATH}/tools/trigger_zone/TriggerZone.h
	${DEV_ROOT_PATH}/tools/utils/SatelliteCamera.h
	${DEV_ROOT_PATH}/tools/utils/SelectionHelper.h
	${DEV_ROOT_PATH}/core/xml/XMLHelper.h
	
)

set (SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/xml/XMLHelper.cpp
	${DEV_ROOT_PATH}/tools/trigger_zone/TriggerZone.cpp
	${DEV_ROOT_PATH}/core/debug/OgreNameGen.cpp
	${DEV_ROOT_PATH}/core/debug/PrimitiveDrawer.cpp	
	${DEV_ROOT_PATH}/core/app_tester/AppTester.cpp
	${DEV_ROOT_PATH}/core/debug/OgreText.cpp
	${DEV_ROOT_PATH}/tools/utils/SatelliteCamera.cpp
	${DEV_ROOT_PATH}/tools/utils/SelectionHelper.cpp
	${DEV_ROOT_PATH}/tools/trigger_zone/main.cpp	
)

