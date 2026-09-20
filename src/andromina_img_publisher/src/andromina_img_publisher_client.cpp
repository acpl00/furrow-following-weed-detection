/******************************************************************************
 * \file
 *
 * $Id:$
 *
 * This file is part of software developed by:  Universidad de Guadalajara; Centro Universitario de Los Valles; Centro de Investigación en Procesamiento de Señales Digitales
 *
 * Author: Alfredo Chávez Plascencia  acplascencia00@gmail.com
 * Author: Himer Avila George         himer.avila@academicos.udg.mx
 * 
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"

// Assuming your service is named ProcessImage inside your custom interfaces package
// Change "my_robot_interfaces" to match your actual service package name!
#include "andromina_msgs/srv/process_image.hpp" // Header file to include the client type

namespace fs = std::filesystem;
using namespace std::chrono_literals;

class ImgServiceClient : public rclcpp::Node {
public:
    ImgServiceClient() : Node("andromina_img_client"), index_(0) {
        //================================
        //---get images from the folder---
        //================================
        images.clear();
        images_path.clear();
        
        std::string folder_path = "/home/cuv/ros2_ws/imgs_furrow";
        
        if (!fs::exists(folder_path)) {
            RCLCPP_ERROR(this->get_logger(), "Folder path does not exist: %s", folder_path.c_str());
            return;
        }

        for (const auto& entry : fs::directory_iterator(folder_path)) {
            std::string path = entry.path().string();
            images_path.push_back(path);
        }
        
        std::sort(images_path.begin(), images_path.end());
        
        for (const auto& path_ : images_path) {
            cv::Mat img = cv::imread(path_);
            if (!img.empty()) {   
                images.push_back(img);
            } else {
                std::cerr << "Failed to load image " << path_ << std::endl;
            }   
        }

        RCLCPP_INFO(this->get_logger(), "Loaded %zu images successfully.", images.size());

        // Create the Service Client with the function create_client and we give the name of process_image
        client_ = this->create_client<andromina_msgs::srv::ProcessImage>("process_image");

        // Wait for the Python processing server to come online
        while (!client_->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service. Exiting.");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "Waiting for Python processing server to start...");
        }

        RCLCPP_INFO(this->get_logger(), "Python Server detected! Starting image processing pipeline.");
        
        // Start the very first image processing request
        send_next_image();
    }

private:
    void send_next_image() {
        if (images.empty()) {
            RCLCPP_WARN(this->get_logger(), "No images to process.");
            return;
        }

        // Check if all images have been processed
        if (index_ > images.size()) {
            RCLCPP_INFO(this->get_logger(), "ALL IMAGES PROCESSED → killing robot nodes...");
            
            // Your custom shutdown system hooks from the original code
            int ret1 = system("pkill -2 -f 'ros2 launch andromina_control_bringup andromina_ai.launch.py'");
            int ret2 = system("pkill -2 -f andromina_ai_driver");
            (void)ret1; (void)ret2; // Avoid unused variable warnings
            
            return; // Stops the loop entirely
        }
        RCLCPP_INFO(this->get_logger(), "\n");
        RCLCPP_INFO(this->get_logger(), "Processing image %zu / %zu...", index_, images.size());

        // Prepare the Service Request
        auto request = std::make_shared<andromina_msgs::srv::ProcessImage::Request>();

        // Convert OpenCV matrix into ROS2 image message
        auto msg = cv_bridge::CvImage(
            std_msgs::msg::Header(),
            "bgr8",
            images[index_]
        ).toImageMsg();

        msg->header.stamp = this->now();
        msg->header.frame_id = "camera_frame";

        // Assign to service request payload
        request->input_image = *msg;
        request->image_id = index_; // 👈 Send the true loop index to Python!

        // Send request asynchronously and bind the response handler
        client_->async_send_request(
            request,
            std::bind(&ImgServiceClient::response_callback, this, std::placeholders::_1)
        );
    }

    void response_callback(rclcpp::Client<andromina_msgs::srv::ProcessImage>::SharedFuture future) {
    try {
        auto response = future.get();
        
        // Print the lightweight text acknowledgment sent back by Python
        //RCLCPP_INFO(this->get_logger(), "Server Status: %s", response->status.c_str());
        //RCLCPP_INFO(this->get_logger(), "\n");

        // Increment index and process next frame immediately 
        index_++;
        send_next_image();

    } catch (const std::exception &e) {
        RCLCPP_ERROR(this->get_logger(), "Service communication failed: %s", e.what());
    }
}


    size_t index_;
    std::vector<cv::Mat> images;
    std::vector<std::string> images_path;
    rclcpp::Client<andromina_msgs::srv::ProcessImage>::SharedPtr client_;  //**| 1.-|** we declare the client variable
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImgServiceClient>());
    rclcpp::shutdown();
    return 0;
}

