#ifndef load_cell_QNODE_HPP_
#define load_cell_QNODE_HPP_

#include "SerialReceiver.hpp"
#include <rclcpp/rclcpp.hpp>
#include <QThread>
#include <iostream>
#include <sstream>
#include <string>
#include <QStringListModel>
#include <std_msgs/msg/string.hpp>

#include "humanoid_interfaces/msg/zmp_msg.hpp"
#include "humanoid_interfaces/msg/ik_com_msg.hpp"

using namespace std;

namespace load_cell {
class QNode : public QThread {
	Q_OBJECT
public:
	QNode();
	~QNode();
	bool init();

	struct rawDataContainer {
        std::vector<int32_t> l_lc_data;
        std::vector<int32_t> r_lc_data;
  };

	rawDataContainer LC_info; // 시리얼로 읽은 raw 로드셀 데이터 컨테이너
	humanoid_interfaces::msg::IkComMsg COM_info; // main window의 zmp 연산에서 사용할 COM 정보 컨테이너
	rclcpp::Publisher<humanoid_interfaces::msg::ZmpMsg>::SharedPtr Zmp_Pub; // main_window 에서 사용할 ZMP 퍼블리셔
protected:
	void run();
private:
	SerialReceiver* serialReceiver;
	std::shared_ptr<rclcpp::Node> node;

	std::shared_ptr<rclcpp::Subscription<humanoid_interfaces::msg::IkComMsg>> COM_Sub;
	void COM_Callback(const humanoid_interfaces::msg::IkComMsg::SharedPtr msg);
Q_SIGNALS:
	void rosShutDown();
	void LC_callback();
};

}  // namespace load_cell

#endif /* load_cell_QNODE_HPP_ */
