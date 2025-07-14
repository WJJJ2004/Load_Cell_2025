#ifndef load_cell_MAIN_WINDOW_H
#define load_cell_MAIN_WINDOW_H

#include <QMainWindow>
#include "QIcon"
#include "qnode.hpp"
#include "filter_manager.hpp"
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
#include "humanoid_interfaces/msg/ik_ltc_msg.hpp"
#include "humanoid_interfaces/msg/lc_msgs.hpp"

#define median_cnt 5
#define PI 3.141592653589793

#define LC_NUM 8
#define LC_NUM_2 4
#define LEFT_FOOT     0
#define RIGHT_FOOT    1

using namespace std;

namespace load_cell {
class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
	~MainWindow();

    // ROS2 MSG //
    humanoid_interfaces::msg::ZmpMsg zmp;

    void paintEvent(QPaintEvent *event);

    long int R_LC_Data[LC_NUM_2] = {0,};
    long int L_LC_Data[LC_NUM_2] = {0,};

// ******************** LoadCell LPF Data container ********************

    // long int R_LC_Data_Filtering[LC_NUM] = {0,};
    // long int L_LC_Data_Filtering[LC_NUM] = {0,};

// *********************************************************************

    long int LC_Zero_Value[LC_NUM] = {0,};
    double LC_Unit_Value[LC_NUM] = {0,};

    double LC_Pos_X_Value[LC_NUM] = {0,};
    double LC_Pos_Y_Value[LC_NUM] = {0,};

    double LC_T_Pos_X_Value[LC_NUM] = {0,};
    double LC_T_Pos_Y_Value[LC_NUM] = {0,};

    long int add2zero[LC_NUM] = {0,};
    long int add2unit[LC_NUM] = {0,};

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
    void update();

// *************************** Filter Functions ************************

    void median(int data_1,int data_2,int data_3,int data_4,int data_5,int data_6,int data_7,int data_8);
    long int Low_pass_filter(long int initial_data);
    long int avg(long int x);

// *********************************************************************

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

    // filter plot
    void on_manager_on_toggled(bool checked);
    void on_yScaleSlider_valueChanged(int value);

    void on_filter_button_0_clicked();
    void on_filter_button_1_clicked();
    void on_filter_button_2_clicked();
    void on_filter_button_3_clicked();
    void on_filter_button_4_clicked();
    void on_filter_button_5_clicked();
    void on_filter_button_6_clicked();
    void on_filter_button_7_clicked();

    void on_avg_lpf_checkStateChanged(const Qt::CheckState &arg1);
    void on_median_lpf_checkStateChanged(const Qt::CheckState &arg1);
    void on_lpf_checkStateChanged(const Qt::CheckState &arg1);

private:

// ***************** QCUSTOMPLOT MANAGE *********************

    // plot_map: Qcustomplot 객체의 이름과 객체를 매핑
    // plot_data: plot_map의 키에 "_data"를 붙인 키와 plot할 좌표를 매핑

    QMap<QString, QCustomPlot*> plot_map;
    QMap<QString, QVector<QPair<double, double>>> plot_data;
    QElapsedTimer plot_timer;

    int selected_sensor_index = -1;
    bool plot_enabled = false;

    void registerPlot(const QString& name, QCustomPlot* plot, bool fixedYAxis);
    void Plot_init();
    void plotArtist();

	Ui::MainWindowDesign *ui;
	QNode *qnode;
    QTimer *timer;

// ************************ UI MANAGE ****************************
    // File IO 과정에서 UI 리셋
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
    void updateFilterButtons(int selected_index)
    {
        selected_sensor_index = selected_index;

        for (int i = 0; i < 8; ++i)
        {
            QString buttonName = QString("filter_button_%1").arg(i);
            QPushButton* button = this->findChild<QPushButton*>(buttonName);

            if (button)
                button->setEnabled(i != selected_index);  // 자기 자신은 비활성화
        }
    };
// ************************** FILTER MANAGER *****************************
FilterManager* filter_manager;
};

}  // namespace load_cell
#endif // load_cell_MAIN_WINDOW_H
