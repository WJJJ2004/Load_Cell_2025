#ifndef load_cell_MAIN_WINDOW_H
#define load_cell_MAIN_WINDOW_H

#include <QMainWindow>
#include "QIcon"
#include "qnode.hpp"
#include "ui_main_window.h"
#include <QtGui>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QString>
#include <QtCore>
#include <QPen>
#include <QDebug>
#include <fstream>

// MSG_HEADER //
#include "humanoid_interfaces/msg/ik_com_msg.hpp"
#include "humanoid_interfaces/msg/ik_ltc_msg.hpp"
#include "humanoid_interfaces/msg/lc_msgs.hpp"

// ROS1 MSG_HEADER 삭제 요망
// #include <msg_generate/Serial2LC_msg.h>
// #include <msg_generate/R_LC_msg.h>
// #include <msg_generate/L_LC_msg.h>
// #include <msg_generate/com_msg.h>

#define median_cnt 5
#define PI 3.141592653589793

#define LC_NUM 8
#define LEFT_FOOT     0
#define RIGHT_FOOT    1

/*****************************************************************************
** Namespace
*****************************************************************************/
using namespace std;

namespace load_cell {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
	~MainWindow();

    // ROS2 MSG //
    humanoid_interfaces::msg::ZmpMsg zmp;
    humanoid_interfaces::msg::IkComMsg COM_info;


    QSerialPort *serial;

    bool open_serial();
    void paintEvent(QPaintEvent *event);

    QByteArray Tx_data;

    long int R_LC_Data[LC_NUM] = {0,};
    long int L_LC_Data[LC_NUM] = {0,};

    long int R_LC_Data_Filtering[LC_NUM] = {0,};
    long int L_LC_Data_Filtering[LC_NUM] = {0,};

    long int LC_Zero_Value[LC_NUM] = {0,};
    double LC_Unit_Value[LC_NUM] = {0,};

    double LC_Pos_X_Value[LC_NUM] = {0,};
    double LC_Pos_Y_Value[LC_NUM] = {0,};

    double LC_T_Pos_X_Value[LC_NUM] = {0,};
    double LC_T_Pos_Y_Value[LC_NUM] = {0,};

    long int add2zero[LC_NUM] = {0,};
    long int add2unit[LC_NUM] = {0,};

    bool Zero_flag = false;
    bool Unit_flag = false;

    int load_cell_median_buffer[8][median_cnt] = {{0,},{0,},{0,},{0,},{0,},{0,},{0,},{0,}};

    int LC_Zero_Gain[LC_NUM] = {0,};
    int LC_Unit_Gain[LC_NUM] = {0,};

    double R_Pos_X_Coordinate = 0.0;
    double R_Pos_Y_Coordinate = 0.0;
    double L_Pos_X_Coordinate = 0.0;
    double L_Pos_Y_Coordinate = 0.0;
    double T_Pos_X_Coordinate = 0.0;
    double T_Pos_Y_Coordinate = 0.0;

    ////////////Low_pass_filter///////////
    long int Output = 0;
    long int data_old = 0;

    //////////Support_Link////////////
    bool Left_Foot = false;
    bool Right_Foot = false;
    bool Both_Feet = false;

    /////////COM_point/////////////
    double X_com = 0.0;
    double Y_com = 0.0;

public Q_SLOTS:

    void LoadCell_Callback();
    void Zero_reset(int foot_what);
    void makePlot();
    void Plot_init();
    void update();
    void median(int data_1,int data_2,int data_3,int data_4,int data_5,int data_6,int data_7,int data_8);
    long int Low_pass_filter(long int initial_data);
    long int avg(long int x);


private Q_SLOTS:
    // specific Unit Gain Push
    void on_UG_Push_00_clicked();
    void on_UG_Push_01_clicked();
    void on_UG_Push_02_clicked();
    void on_UG_Push_03_clicked();
    void on_UG_Push_04_clicked();
    void on_UG_Push_05_clicked();
    void on_UG_Push_06_clicked();
    void on_UG_Push_07_clicked();

    // File IO
    void on_Save_Button_clicked();
    void on_Open_Button_clicked();

    // zero gain insert
    void on_ZG_Insert_Button_clicked();
    void on_ZG_Reset_Button_clicked();

    // Unit gain insert
    void on_UG_Insert_Button_clicked();
    void on_UG_Reset_Button_clicked();
private:
	Ui::MainWindowDesign *ui;
	QNode *qnode;
    QTimer *timer;

    // UI 리셋
    void resetLineEditStyle()
    {
        QLineEdit* zeroGainEdits[8] = {
            ui->LC_Zero_Gain_00, ui->LC_Zero_Gain_01, ui->LC_Zero_Gain_02, ui->LC_Zero_Gain_03,
            ui->LC_Zero_Gain_04, ui->LC_Zero_Gain_05, ui->LC_Zero_Gain_06, ui->LC_Zero_Gain_07
        };

        QLineEdit* unitGainEdits[8] = {
            ui->LC_Unit_Gain_00, ui->LC_Unit_Gain_01, ui->LC_Unit_Gain_02, ui->LC_Unit_Gain_03,
            ui->LC_Unit_Gain_04, ui->LC_Unit_Gain_05, ui->LC_Unit_Gain_06, ui->LC_Unit_Gain_07
        };

        for (int i = 0; i < 8; ++i) {
            zeroGainEdits[i]->setStyleSheet("background-color: white; color: black;");
            unitGainEdits[i]->setStyleSheet("background-color: white; color: black;");
        }
    };
};

}  // namespace load_cell

#endif // load_cell_MAIN_WINDOW_H
