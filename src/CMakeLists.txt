
add_subdirectory( test )

add_library(controller OBJECT Controller.cpp MqttClient.cpp Observer.cpp )
