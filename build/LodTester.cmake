# specific headers
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.h
	${DEV_ROOT_PATH}/core/xml/XMLHelper.h
	${DEV_ROOT_PATH}/core/types/basics.h
	${DEV_ROOT_PATH}/tools/lod_tester/LodTester.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
	${DEV_ROOT_PATH}/core/debug/OgreText.h
)

# specific sources
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.cpp
	${DEV_ROOT_PATH}/core/xml/XMLHelper.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
	${DEV_ROOT_PATH}/tools/lod_tester/LodTester.cpp
	${DEV_ROOT_PATH}/core/debug/OgreText.cpp
	${DEV_ROOT_PATH}/tools/lod_tester/main.cpp
)

