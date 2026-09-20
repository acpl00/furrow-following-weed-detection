cmake_minimum_required(VERSION 3.8)
project(andromina_img_publisher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(serial REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(andromina_msgs REQUIRED)

find_package(OpenCV REQUIRED PATHS /opt/ros/${ROS_DISTRO}/lib/cmake/opencv4 NO_DEFAULT_PATH)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()


add_executable(andromina_img_publisher src/andromina_img_publisher_client.cpp)
ament_target_dependencies(andromina_img_publisher rclcpp std_msgs rclcpp sensor_msgs std_msgs cv_bridge andromina_msgs)
install(TARGETS andromina_img_publisher DESTINATION lib/${PROJECT_NAME})



# === Include OpenCV ===
target_include_directories(andromina_img_publisher PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(andromina_img_publisher ${OpenCV_LIBS})
install(TARGETS andromina_img_publisher DESTINATION lib/${PROJECT_NAME})


ament_package()
