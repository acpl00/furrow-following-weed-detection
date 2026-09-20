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
#include <rclcpp/rclcpp.hpp>
#include <serial/serial.h>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <boost/filesystem.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/float32.hpp>




namespace fs = boost::filesystem;
using namespace std::chrono_literals;

class Velocity : public rclcpp::Node{
public:
   //=======================
   //--function declaration--
   //========================   
   void loop();
   void send_velocity_servo_command(int velocity, int servo_angle);
   void read_serial_feedback();
   void xtion_callback(const sensor_msgs::msg::Image::SharedPtr msg);
   void arducam_callback(const sensor_msgs::msg::Image::SharedPtr msg);
   void serial_reader();
   
  Velocity() : Node("velocity"), velocity(80), servo_angle(90), running_(true) {
    // --- Open serial port ---
    try {
      serial_.setPort("/dev/ttyACM0"); // adjust if needed
      serial_.setBaudrate(9600);
      //serial_.setBaudrate(115200);
      serial::Timeout to = serial::Timeout::simpleTimeout(200);
      serial_.setTimeout(to);
      serial_.open();
      RCLCPP_INFO(this->get_logger(), "Connected to Arduino on /dev/ttyACM0");
    } catch (std::exception &e) {
      RCLCPP_ERROR(this->get_logger(), "Error opening serial port: %s", e.what());
      rclcpp::shutdown();
    }

    
    xtion_sub_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/rgb/image_raw",10, std::bind(&Velocity::xtion_callback, this, std::placeholders::_1));
    
    arducam_sub_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/image_raw",10, std::bind(&Velocity::arducam_callback, this, std::placeholders::_1));
    
    
    velocity_sub_ = this->create_subscription<std_msgs::msg::Int32>(
    "/motor_velocity", 10, 
    [this](std_msgs::msg::Int32::SharedPtr msg){
        velocity = msg->data;
    });
    
    angle_sub_ = this->create_subscription<std_msgs::msg::Int32>(
    "/servo_angle", 10,
    [this](std_msgs::msg::Int32::SharedPtr msg){
        servo_angle = msg->data;
    });
    
 
    
    
    ///////////////////////////////////////////////////
    
     rclcpp::sleep_for(3s);  // wait 2 seconds for Arduino to boot
    // --- Timer for periodic tasks ---
    timer_ = this->create_wall_timer(
      80ms, std::bind(&Velocity::loop, this)
    );
    
    
    // create directories if not exist
    // Paths for your dataset
    std::vector<std::string> dirs = {"data/images", "data/velocity", "data/angle", "data/arducam_images"};

    for (const auto& dir : dirs) {
        try {
            if (fs::exists(dir)) {
                RCLCPP_WARN(this->get_logger(), "Directory '%s' exists. Deleting old data...", dir.c_str());
                fs::remove_all(dir);  // delete directory and contents
            }
            fs::create_directories(dir);  // recreate empty directory
            RCLCPP_INFO(this->get_logger(), "Created directory: %s", dir.c_str());
        } catch (const fs::filesystem_error& e) {
            RCLCPP_ERROR(this->get_logger(), "Filesystem error: %s", e.what());
        }
    }
    
    
  // --- Start serial thread ---
  serial_thread_ = std::thread(&Velocity::serial_reader, this);  
  }
  
  ~Velocity() override {
        running_ = false;
        if (serial_thread_.joinable()) serial_thread_.join();
  }

private:

  sensor_msgs::msg::Image::ConstSharedPtr latest_arducam_;
  std::mutex arducam_mutex_;
  
  int velocity;      // default starting velocity
  int servo_angle;    // default starting servo angle
  
  serial::Serial serial_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr vel_pub_;
  
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr xtion_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr arducam_sub_;
  
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr velocity_sub_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr angle_sub_;
  
  cv::Mat latest_frame_;
  std::mutex img_mutex_;
  
  ////////////////
  // --- Core threads and data ---
  //serial::Serial serial_;
  std::thread serial_thread_;
  std::atomic<bool> running_;
  std::atomic<float> latest_vel_{0.0f};
  std::atomic<float> latest_angle_{90.0f};
  std::atomic<int> image_index_{0};
 

};

// --- Separate thread continuously reading Arduino serial ---
void Velocity::serial_reader(){

   while (running_){
      //std::cout << "IT IS HERE " << std::endl;
      try{
         if(serial_.available()) {
            std::string line = serial_.readline(128, "\n");
            line.erase(line.find_last_not_of("\r\n") + 1);

            if (line.rfind("VEL_AVG:", 0) == 0)
               latest_vel_ = std::stof(line.substr(8));
            else if(line.rfind("SER_ANGLE:", 0) == 0)
               latest_angle_ = std::stof(line.substr(10));
         }
      }catch (...){
          RCLCPP_WARN(this->get_logger(), "Serial read error");
      }
      std::this_thread::sleep_for(1ms);
   }

}

void Velocity::arducam_callback(const sensor_msgs::msg::Image::SharedPtr msg){
   std::lock_guard<std::mutex> lock(arducam_mutex_);
   latest_arducam_ = msg;
}

void Velocity::xtion_callback(const sensor_msgs::msg::Image::SharedPtr xtion_msg){
   
   static int counter = 0;
   counter++;

   if (counter % 2	 != 0) {
      return;   // <-- exit the callback RIGHT HERE
   }
   
   sensor_msgs::msg::Image::ConstSharedPtr arducam_msg;
   {
   std::lock_guard<std::mutex> lock(arducam_mutex_);
   arducam_msg = latest_arducam_;
   }
   
   
   if (!arducam_msg) {
      RCLCPP_WARN(this->get_logger(), "No Arducam frame yet");
      return;
   }

   RCLCPP_INFO(this->get_logger(), "Received xtion image of size: %dx%d", xtion_msg->width, xtion_msg->height);
   RCLCPP_INFO(this->get_logger(), "Received arducam image of size: %dx%d", arducam_msg->width, arducam_msg->height);

   // Example: convert to OpenCV
        
   cv::Mat xtion_frame;
   cv::Mat arducam_frame;
        
        
   try {
            //std::lock_guard<std::mutex> lock(img_mutex_);
            xtion_frame   = cv_bridge::toCvCopy(xtion_msg,   "bgr8")->image;
            arducam_frame = cv_bridge::toCvCopy(arducam_msg, "bgr8")->image;
       } catch (const cv_bridge::Exception &e) {
            RCLCPP_ERROR(this->get_logger(), "CV Bridge error: %s", e.what());
       }
        // now you can use frame with OpenCV
        
       
        
        // Get synchronized values
        float   vel = latest_vel_;
        float angle = latest_angle_;
        int     idx = image_index_.fetch_add(1);
        
        std::stringstream name;
        name << std::setw(5) << std::setfill('0') << idx;

        // Save xtion image
        std::string img_path = "data/images/" + name.str() + ".png";
        cv::imwrite(img_path, xtion_frame);
        
        // Save arducam image
        std::string arducam_img_path = "data/arducam_images/" + name.str() + ".png";
        cv::imwrite(arducam_img_path, arducam_frame);
        
        // Save numeric data
        {
            std::ofstream vfile("data/velocity/" + name.str() + ".txt");
            vfile << vel;
            ///vfile << vel << " " << velocity;
        }
        {
            std::ofstream afile("data/angle/" + name.str() + ".txt");
            afile << angle;
        }

        RCLCPP_INFO(this->get_logger(),
            "Saved %s: vel=%.3f angle=%.1f", name.str().c_str(), vel, angle);
        
    }


 void Velocity::loop() {
 
  send_velocity_servo_command(velocity,  servo_angle);
 
}

 void Velocity::send_velocity_servo_command(int velocity, int servo_angle) {
  // Clamp both to safe ranges
  velocity = std::clamp(velocity, 0, 255);
  servo_angle = std::clamp(servo_angle, 0, 180);
  
 
    RCLCPP_INFO(this->get_logger(), "\n \n");
    RCLCPP_DEBUG(this->get_logger(), "servo_velocity: %d", velocity );
    RCLCPP_INFO(this->get_logger(), "\n \n");

  // Combine both into a single message
  std::string cmd = "VEL:" + std::to_string(velocity) +
                    ",SER:" + std::to_string(servo_angle) + "\n";

  if (serial_.isOpen()) {
    serial_.write(cmd);
    RCLCPP_INFO(this->get_logger(), "\n \n");
    RCLCPP_DEBUG(this->get_logger(), "Sent: %s", cmd.c_str());
    RCLCPP_INFO(this->get_logger(), "\n \n");
  } else {
    RCLCPP_WARN(this->get_logger(), "Serial port not open, failed to send command");
  }
}


int main(int argc, char *argv[]){
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Velocity>());
  rclcpp::shutdown();
  return 0;
}
