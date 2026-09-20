from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    return LaunchDescription([
        # Start the joystick package
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node',
            output='screen',
            parameters=[{'device_name': 'PS3 Controller'}]
        ),
        # Start the joystick for andromina    
        Node(
            package='joystick_trigger',
            executable='joystick_trigger',
            name='joystick_trigger',
            output='screen'
       
       
       
    ])
    
    
   
