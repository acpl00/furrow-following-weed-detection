cmake_minimum_required(VERSION 3.8)
project(andromina_ai_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Set the path to your extracted LibTorch directory
set(Torch_DIR "$ENV{HOME}/libtorch/libtorch-shared-with-deps-2.9.0+cpu/libtorch/share/cmake/Torch")

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(serial REQUIRED)
###find_package(Torch REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(andromina_msgs REQUIRED)

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


add_executable(andromina_ai_driver src/andromina_ai_furrow.cpp)
ament_target_dependencies(andromina_ai_driver rclcpp std_msgs serial sensor_msgs cv_bridge  andromina_msgs)
# === Include OpenCV ===
target_include_directories(andromina_ai_driver PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(andromina_ai_driver ${OpenCV_LIBS} ${TORCH_LIBRARIES} Boost::filesystem)  
install(TARGETS andromina_ai_driver DESTINATION lib/${PROJECT_NAME})

ament_package()
