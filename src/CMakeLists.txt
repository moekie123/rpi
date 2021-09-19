add_subdirectory(core)
add_subdirectory(mqtt)
add_subdirectory(actuator)

add_library(controller OBJECT Controller.cpp mqtt/MqttStateMachine.cpp mqtt/MqttController.cpp)
