#******************************************************************************
# \file
#
# $Id:$
#
# This file is part of software developed by:  Universidad de Guadalajara; Centro Universitario de Los Valles; Centro de Investigación en Procesamiento de Señales Digitales
#
# Author: Alfredo Chávez Plascencia  acplascencia00@gmail.com
# Author: Himer Avila George         himer.avila@academicos.udg.mx
#
# This file is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this file.  If not, see <http://www.gnu.org/licenses/>.
#/
!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import shutil
import os
import cv2
import numpy as np
import onnxruntime as ort

from andromina_msgs.msg import ControlCommand
# Importing your custom service type
from andromina_msgs.srv import ProcessImage

class AndrominaAI(Node):

    def __init__(self,root):
        super().__init__("andromina_ai_model")
        self.bridge    = CvBridge()
        self.root      = root
        self.root_data = self.root + "/data_ai_model"
        self.velocity  = self.root_data + "/velocity"
        self.angle     = self.root_data + "/angle"
        
        if os.path.exists(self.root_data):
           shutil.rmtree(self.root_data)
           
        os.makedirs(self.velocity)
        os.makedirs(self.angle)
        
        ##print("\n \n")
        ##print("---ROOT---",self.root_data)
        ##print("---ROOT---",self.angle)
        ##print("---ROOT---",self.velocity)
        ##print("\n  \n	")
        # ===================
        # --- Load ONNX ---
        # ===================
        model_path = os.path.expanduser("~/ros2_ws/src/andromina_ai_model/models/best_control_model.onnx")

       
        self.session = ort.InferenceSession(
            model_path,
            providers=["CPUExecutionProvider"]
        )
        
       

        self.input_name = self.session.get_inputs()[0].name
        self.output_name = self.session.get_outputs()[0].name

        self.get_logger().info("ONNX model loaded successfully.")
        
        # ===================
        # --- Service Server ---
        # ===================
        # Changed from a Subscription to a Service Server named 'process_image'
        self.srv = self.create_service(
            ProcessImage,
            "process_image",
            self.process_image_callback
        )
        ###self.get_logger().info("Service Server 'process_image' is ready.")

        # ===================
        # --- Publisher ---
        # ===================
        self.control_pub = self.create_publisher(
            ControlCommand,
            "/control_cmd",
            10
        )

    def process_image_callback(self, request, response):
        """
        This function handles the incoming request from the C++ Node.
        'request.input_image' holds the raw image sent by C++.
        'response.output_image' will hold the image we return back.
        """
        
        true_id = request.image_id
    
        self.get_logger().info(f"Received image index {true_id} request from C++ node. Processing...")
        
       
        
        control_msg = ControlCommand()

        # ============================
        # --- ROS Image -> OpenCV ---
        # ============================
        # Extract the image message from the incoming service request payload
        image = self.bridge.imgmsg_to_cv2(
            request.input_image,
            desired_encoding="bgr8"
        )

        # Keep a copy of the raw image or modify it if you want to draw a filter over it
        # For now, we will save this to return back to C++ later
        processed_image_to_return = image.copy() 

        # Resize
        image = cv2.resize(
            image,
            (256, 256)
        )

        # BGR -> RGB
        image = cv2.cvtColor(
            image,
            cv2.COLOR_BGR2RGB
        )

        # ============================
        # --- OpenCV -> ONNX input ---
        # ============================
        image = image.astype(np.float32) / 255.0

        # HWC -> CHW
        image = np.transpose(
            image,
            (2, 0, 1)
        )

        # Add batch dimension
        image = np.expand_dims(
            image,
            axis=0
        )

        # ===================
        # --- Inference ---
        # ===================
        output = self.session.run(
            [self.output_name],
            {self.input_name: image}
        )[0]

        steering = output[0, 0] * 40 + 70
        velocity = output[0, 1] * 150
        
        
        
        # ===================
        # --- ROS message ---
        # ===================
        control_msg.steering = float(steering)
        control_msg.velocity = float(velocity)

        # Publish control commands to your robot components
        self.control_pub.publish(control_msg)
        ###self.get_logger().info(f"Published control -> Steering: {steering:.2f}, Velocity: {velocity:.2f}")
        
        #===============================
        # --- save steering and velocity
        #===============================
        vel_filename = f"{true_id:05d}.txt"
        ang_filename = f"{true_id:05d}.txt"
        ###self.get_logger().info(f"Saved control data to vel: {vel_filename}")
        ###self.get_logger().info(f"Saved control data to ang: {ang_filename}")
        
        filepath_vel = os.path.join(self.velocity,vel_filename)
        filepath_ang = os.path.join(self.angle,ang_filename)
        
        with open(filepath_vel, "w") as f:
            f.write(f"{velocity}\n")
            
            
        with open(filepath_ang, "w") as f:
            f.write(f"{steering}\n")
              
        
        
        ####self.get_logger().info(f"filepath_vel: {filepath_vel}")
        # ==================================
        # --- Pack Lightweight Response ----
        # ==================================
        # Instead of converting the image back, we just send a tiny text string
        response.status = "SUCCESS"

        ###self.get_logger().info("Finished inference. Sending acknowledgment receipt back to C++ node.")
        return response


def main(args=None):
    rclpy.init(args=args)
    root = os.path.expanduser("~/ros2_ws/src")
    node = AndrominaAI(root)
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()

