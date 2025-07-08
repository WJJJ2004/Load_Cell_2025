#ifndef load_cell_QNODE_HPP_
#define load_cell_QNODE_HPP_

#include <rclcpp/rclcpp.hpp>
#endif
#include <QThread>
#include <iostream>
#include <sstream>
#include <string>
#include <QStringListModel>
#include <std_msgs/msg/string.hpp>

#include "humanoid_interfaces/msg/zmp_msg.hpp"
#include "humanoid_interfaces/msg/ik_com_msg.hpp"
#include "humanoid_interfaces/msg/lc_msgs.hpp"

using namespace std;

namespace load_cell {
class QNode : public QThread {
	Q_OBJECT
public:
	QNode();
	~QNode();
	bool init();

	long int R_LC_Data[4];
	long int L_LC_Data[4];
	humanoid_interfaces::msg::LCMsgs LC_info;
	humanoid_interfaces::msg::IkComMsg COM_info;

	rclcpp::Publisher<humanoid_interfaces::msg::ZmpMsg>::SharedPtr Zmp_Pub;
protected:
	void run();
private:
	std::shared_ptr<rclcpp::Node> node;

	std::shared_ptr<rclcpp::Subscription<humanoid_interfaces::msg::LCMsgs>> loadcell_Sub;
	std::shared_ptr<rclcpp::Subscription<humanoid_interfaces::msg::IkComMsg>> COM_Sub;

	void LoadCell_Callback(const humanoid_interfaces::msg::LCMsgs::SharedPtr msg);
	void COM_Callback(const humanoid_interfaces::msg::IkComMsg::SharedPtr msg);
Q_SIGNALS:
	void rosShutDown();
	void LC_callback();
};

}  // namespace load_cell

#endif /* load_cell_QNODE_HPP_ */
