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
#include <boost/filesystem.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/float32.hpp>
#include <filesystem>
#include "andromina_msgs/msg/control_command.hpp"



namespace fs = boost::filesystem;
using namespace std::chrono_literals;
namespace fsys = std::filesystem;

class Andromina_ai : public rclcpp::Node {
public:
   void cmd_callback(const andromina_msgs::msg::ControlCommand::SharedPtr cmd);
   Andromina_ai() : Node("andromina_ai_driver"){
      RCLCPP_INFO(this->get_logger(), "✅ Andromina AI node started!");
      //=========================
      // --- Open serial port ---
      //=========================
      try{
         serial_.setPort("/dev/ttyACM0"); // adjust if needed
         serial_.setBaudrate(9600);
         //serial_.setBaudrate(115200);
         serial::Timeout to = serial::Timeout::simpleTimeout(200);
         serial_.setTimeout(to);
         serial_.open();
         RCLCPP_INFO(this->get_logger(), "Connected to Arduino on /dev/ttyACM0");
      }catch(std::exception &e) {
          RCLCPP_ERROR(this->get_logger(), "Error opening serial port: %s", e.what());
          rclcpp::shutdown();
       }
  
    
    cmd_sub_ = this->create_subscription<andromina_msgs::msg::ControlCommand>("/control_cmd",10, std::bind(&Andromina_ai::cmd_callback, this, std::placeholders::_1));
    
    }
private:
    rclcpp::Subscription<andromina_msgs::msg::ControlCommand>::SharedPtr cmd_sub_;
    
    serial::Serial serial_;
    
    int prev_right = -1;
    int prev_left  = -1;
    
   
};

void Andromina_ai::cmd_callback(const andromina_msgs::msg::ControlCommand::SharedPtr cmd){
   //RCLCPP_INFO(this->get_logger(), "📸 New image received!");
   
   float angle = cmd->steering;
   float velocity = cmd->velocity;
 
   //=======================
   //--- Send to Arduino ---
   //=======================
   float steering = (angle - 90.0) / 20.0 ;
   float k = 1.0f; //0.5f;

   float delta = k * velocity * steering;

   float left_pwm  = velocity + delta;
   float right_pwm = velocity - delta;

   left_pwm  = std::max(0.0f, std::min(153.0f, left_pwm));
   right_pwm = std::max(0.0f, std::min(153.0f, right_pwm));
   
   
   int r = int(right_pwm);
   int l = int(left_pwm);

   if (r != prev_right || l != prev_left) {
      // Combine both into a single message
      std::string cmd = "RIG:" + std::to_string(int(right_pwm)) + ",LEF:" + std::to_string(int(left_pwm)) + "\n";

   if(serial_.isOpen()) {
      serial_.write(cmd);
      //RCLCPP_INFO(this->get_logger(), "Sent: %s", cmd.c_str());
   } else {
      RCLCPP_INFO(this->get_logger(), "Serial port not open, failed to send command");
     }
     prev_right = r;
     prev_left  = l;
   }
 
}

int main(int argc, char *argv[]){
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Andromina_ai>());
  rclcpp::shutdown();
  return 0;
}
