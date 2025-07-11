#include "../include/load_cell_2025/qnode.hpp"
#include <rclcpp/rclcpp.hpp>

namespace load_cell
{

QNode::QNode()
{
  // ********************** SERIAL RECEIVER INIT  **********************
  serialReceiver = new SerialReceiver();
  serialReceiver->openPort("ttyUSB0",115200);

  connect(serialReceiver, &SerialReceiver::dataReceived, this,
  [=](const std::vector<int16_t>& r, const std::vector<int16_t>& l){

      LC_info.l_lc_data.push_back(r[0]);      //이때 R과 L이 반전된 이유는 받아오는 로드셀 값은 사람이 로봇을 봤을 때의 방향을 기준으로 하고있기 때문
      LC_info.l_lc_data.push_back(r[1]);
      LC_info.l_lc_data.push_back(r[2]);
      LC_info.l_lc_data.push_back(r[3]);

      LC_info.r_lc_data.push_back(l[0]);
      LC_info.r_lc_data.push_back(l[1]);
      LC_info.r_lc_data.push_back(l[2]);
      LC_info.r_lc_data.push_back(l[3]);

      Q_EMIT LC_callback();  // MainWindow 쪽으로 신호 전송

      LC_info.l_lc_data.clear();
      LC_info.r_lc_data.clear();
  });

  // ********************** ROS2 NODE INITIALIZE  **********************
	int argc = 0;
	char** argv = NULL;
	rclcpp::init(argc, argv);
	node = rclcpp::Node::make_shared("load_cell");

	// publisher
	Zmp_Pub = node->create_publisher<humanoid_interfaces::msg::ZmpMsg>(             // ZMP 메시지 퍼블리셔
			"zmp", 10);
	// subscriber
	COM_Sub = node->create_subscription<humanoid_interfaces::msg::IkComMsg>(        // COM 메시지 서브스크라이버
			"COM", 10, std::bind(&QNode::COM_Callback,this,std::placeholders::_1));

	this->start();
  // *******************************************************************
}

QNode::~QNode()
{
  if (rclcpp::ok())
  {
    rclcpp::shutdown();
  }
}

void QNode::COM_Callback(const humanoid_interfaces::msg::IkComMsg::SharedPtr msg)
{
    COM_info.x_com = msg->x_com;
    COM_info.y_com = msg->y_com;
}

void QNode::run()
{
  rclcpp::Rate loop_rate(33);
  while (rclcpp::ok())
  {
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

} // namespace load_cell
