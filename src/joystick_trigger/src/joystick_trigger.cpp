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
#include <sensor_msgs/msg/joy.hpp>
#include <cstdlib>
#include <memory>

int system_pid = -1;

class JoystickTrigger : public rclcpp::Node {
public:
    JoystickTrigger() : Node("joystick_trigger"), triggered_(false) 
    {
        sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "/joy", 10,
            std::bind(&JoystickTrigger::joyCallback, this, std::placeholders::_1)
        );

       
    }

private:
    void joyCallback(const sensor_msgs::msg::Joy::SharedPtr msg)
    {
         float forward_axis = 0.0;
        //RCLCPP_INFO(this->get_logger(), "Joystick trigger started.  %f",msg->axes[1]);
         if (msg->axes.size() > 1) {
            forward_axis = msg->axes[1];
        }

        

        // PS3: forward ≈ -1.0   backward ≈ +1.0
        if (!triggered_ && forward_axis > 0.2) {
            triggered_ = true;

            RCLCPP_INFO(this->get_logger(),
                "Forward detected (%f) → Starting robot system...", forward_axis);

            system("ros2 launch andromina_control_bringup andromina_system.launch.py &");
        }
        
        //---------------------------------------------------------
        // STOP ROBOT WHEN ANY STOP-BUTTON IS PRESSED
        //---------------------------------------------------------
        
            
       
       if (msg->buttons[0] || msg->buttons[1] || msg->buttons[2] || msg->buttons[3]) {
         RCLCPP_INFO(this->get_logger(), "STOP → killing robot nodes...");
         system("pkill -2 -f 'ros2 launch andromina_control_bringup andromina_system.launch.py'");
         system("pkill -2 -f data_recorder");
      }
             
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr sub_;
    bool triggered_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<JoystickTrigger>());
    rclcpp::shutdown();
    return 0;
}
