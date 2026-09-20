from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    return LaunchDescription([
        # Start image publisher from file
        Node(
            package='andromina_img_publisher',
            executable='andromina_img_publisher',
            name='andromina_img_publisher',
            output='screen'
        ),
        Node(
            package='andromina_ai_model',
            executable='andromina_ai_model',
            name='andromina_ai_model',
            output='screen'
        ),
        Node(
            package='andromina_ai_driver',
            executable='andromina_ai_driver',
            name='andromina_ai_driver',
            output='screen'
        ),
    ])
    
    
   
