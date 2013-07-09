# specific headers
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.h
	${DEV_ROOT_PATH}/core/types/basics.h
	${DEV_ROOT_PATH}/tools/anim_player/AnimPlayer.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
	${DEV_ROOT_PATH}/core/debug/OgreText.h
)

# specific sources
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/app_tester/AppTester.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
	${DEV_ROOT_PATH}/tools/anim_player/AnimPlayer.cpp
	${DEV_ROOT_PATH}/core/debug/OgreText.cpp
	${DEV_ROOT_PATH}/tools/anim_player/main.cpp
)

