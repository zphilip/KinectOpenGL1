#tf includes
include_directories(../include)

#tf msgs includes
include_directories(../msg_gen/cpp/include)

#tf srvs includes
include_directories(../srv_gen/cpp/include)

#geometry msgs includes
include_directories(${ROS_STACKS}/common_msgs/geometry_msgs/msg_gen/cpp/include)

#sensor msgs includes
include_directories(${ROS_STACKS}/common_msgs/sensor_msgs/msg_gen/cpp/include)

#roslib msgs includes
include_directories(${ROS_ROOT}/core/roslib/msg_gen/cpp/include)

#angles includes
include_directories(${ROS_STACKS}/geometry/angles/include)

FILE(GLOB HEADERS_TF ../include/tf/*.h)

add_library(tf SHARED
			../src/tf.cpp
			../src/transform_listener.cpp
			../src/cache.cpp
			../src/transform_broadcaster.cpp
			${HEADERS_TF}
			)

IF(WIN32)
	SET_TARGET_PROPERTIES( tf PROPERTIES COMPILE_FLAGS "/wd4251 /MP" )
ENDIF( WIN32 )

target_link_libraries(
		tf
		ros
		${BULLET_LINEARMATH_LIB}
		)

#INSTALL		
SET (TF_INCLUDE_INSTALL_DIR ${ROSWIN32_INCLUDE_INSTALL_DIR}/packages/tf)

#libs and dlls
INSTALL(
	TARGETS tf
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	RUNTIME DESTINATION ${BIN_INSTALL_DIR}
)

#headers
INSTALL(
	FILES ${HEADERS_TF}
	DESTINATION ${TF_INCLUDE_INSTALL_DIR}
	)

#msgs
FILE(
	GLOB 
	TF_MSGS 
	../msg_gen/cpp/include/tf/*.h
	)
	
INSTALL(
	FILES
	${TF_MSGS}
	DESTINATION
	${ROSWIN32_INCLUDE_INSTALL_DIR}/msgs/tf
	)
#srvs
FILE(
	GLOB 
	TF_SRVS 
	../srv_gen/cpp/include/tf/*.h
	)
	
INSTALL(
	FILES
	${TF_SRVS}
	DESTINATION
	${ROSWIN32_INCLUDE_INSTALL_DIR}/srvs/tf
	)
		
# Test
# testBroadcaster.cpp

add_executable(
	testBroadcaster
	../test_win32/testBroadcaster.cpp
	)
target_link_libraries(
	testBroadcaster
	tf
	ros
	)
	
# testListener.cpp
add_executable(
	testListener
	../test_win32/testListener.cpp
	)
	
target_link_libraries(
	testListener
	tf
	ros
	${GTEST_DLL_LIB}
	)