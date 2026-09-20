from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # Path to the OpenNI2 camera launch file
    camera_launch_path = os.path.join(
        get_package_share_directory('openni2_camera'),
        'launch',
        'camera_only.launch.py'
    )

    return LaunchDescription([
        # Include the existing OpenNI2 camera launch
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(camera_launch_path)
        ),

        # Start your velocity and image logger node
        Node(
            package='velocity_image_logger',
            executable='data_recorder',
            name='data_recorder',
            output='screen'
        ),
        # Start the joystick for andromina    
        Node(
            package='andromina_joystick',
            executable='joystick_control',
            name='joystick_control',
            output='screen'
        ),
        Node(
            package='gscam',
            executable='gscam_node',
            name='gscam_node',
            output='screen',
            respawn=True,
            respawn_delay=2.0,
            parameters=[{'gscam_config': 'v4l2src device=/dev/v4l/by-id/usb-Arducam_Technology_Co.__Ltd._Arducam_5MP_USB_Camera_AC20251017V1-video-index0 ! image/jpeg, width=640, height=480, framerate=30/1 ! jpegdec ! videoconvert'}]
        	)
    ])
    
    
   
