#include "../include/load_cell_2025/main_window.hpp"

#define FILTERDATA 5
long int temp[FILTERDATA];

namespace load_cell
{
using namespace Qt;
using namespace std;

int serial_cnt = 0;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
    // ***************************** UI INIT *****************************
    ui->setupUi(this);

    QIcon icon("://ros-icon.png");
    this->setWindowIcon(icon);

    qnode = new QNode();
    filter_manager = new FilterManager();

    setWindowIcon(QIcon(":/images/zmp_icon.png"));

    // ************************ INIT QCUSTOMPLOT *************************

    Plot_init();         // plot 등록
    plotArtist();

    QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));
    QObject::connect(qnode, SIGNAL(LC_callback()),this, SLOT(LoadCell_Callback())); //works whenever 'load_cell value' comes in through serial

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::plotArtist);
    connect(timer, SIGNAL(timeout()),this, SLOT(update()));

    // *************************** WORK FILE OPEN *******************************

    ui->LC_Zero_Gain_00->setReadOnly(true);
    ui->LC_Zero_Gain_01->setReadOnly(true);
    ui->LC_Zero_Gain_02->setReadOnly(true);
    ui->LC_Zero_Gain_03->setReadOnly(true);
    ui->LC_Zero_Gain_04->setReadOnly(true);
    ui->LC_Zero_Gain_05->setReadOnly(true);
    ui->LC_Zero_Gain_06->setReadOnly(true);
    ui->LC_Zero_Gain_07->setReadOnly(true);

    ui->LC_Unit_Gain_00->setReadOnly(true);
    ui->LC_Unit_Gain_01->setReadOnly(true);
    ui->LC_Unit_Gain_02->setReadOnly(true);
    ui->LC_Unit_Gain_03->setReadOnly(true);
    ui->LC_Unit_Gain_04->setReadOnly(true);
    ui->LC_Unit_Gain_05->setReadOnly(true);
    ui->LC_Unit_Gain_06->setReadOnly(true);
    ui->LC_Unit_Gain_07->setReadOnly(true);

    for(int i = 0; i < LC_NUM; i++)
    {
        add2unit[i] = 1;
        LC_Zero_Gain[i] = 1;
    }

    QString real_path = QDir::homePath() + "/colcon_ws/src/load_cell_2025/work/fixed_path";
    ifstream is(real_path.toUtf8().constData());
    if(is.is_open())
    {
        cout << "INIT FILE OPEN "<< endl;
        is >> LC_Zero_Gain[0];
        is >> LC_Zero_Gain[1];
        is >> LC_Zero_Gain[2];
        is >> LC_Zero_Gain[3];
        is >> LC_Zero_Gain[4];
        is >> LC_Zero_Gain[5];
        is >> LC_Zero_Gain[6];
        is >> LC_Zero_Gain[7];

        is >> LC_Zero_Gain[0];
        is >> LC_Zero_Gain[1];
        is >> LC_Zero_Gain[2];
        is >> LC_Zero_Gain[3];
        is >> LC_Zero_Gain[4];
        is >> LC_Zero_Gain[5];
        is >> LC_Zero_Gain[6];
        is >> LC_Zero_Gain[7];

        is.close();

        ui->LC_Zero_Gain_00->setText(QString::number(LC_Zero_Gain[0]));
        ui->LC_Zero_Gain_01->setText(QString::number(LC_Zero_Gain[1]));
        ui->LC_Zero_Gain_02->setText(QString::number(LC_Zero_Gain[2]));
        ui->LC_Zero_Gain_03->setText(QString::number(LC_Zero_Gain[3]));
        ui->LC_Zero_Gain_04->setText(QString::number(LC_Zero_Gain[4]));
        ui->LC_Zero_Gain_05->setText(QString::number(LC_Zero_Gain[5]));
        ui->LC_Zero_Gain_06->setText(QString::number(LC_Zero_Gain[6]));
        ui->LC_Zero_Gain_07->setText(QString::number(LC_Zero_Gain[7]));

        ui->LC_Unit_Gain_00->setText(QString::number(LC_Zero_Gain[0]));
        ui->LC_Unit_Gain_01->setText(QString::number(LC_Zero_Gain[1]));
        ui->LC_Unit_Gain_02->setText(QString::number(LC_Zero_Gain[2]));
        ui->LC_Unit_Gain_03->setText(QString::number(LC_Zero_Gain[3]));
        ui->LC_Unit_Gain_04->setText(QString::number(LC_Zero_Gain[4]));
        ui->LC_Unit_Gain_05->setText(QString::number(LC_Zero_Gain[5]));
        ui->LC_Unit_Gain_06->setText(QString::number(LC_Zero_Gain[6]));
        ui->LC_Unit_Gain_07->setText(QString::number(LC_Zero_Gain[7]));

        add2zero[0] = ui->LC_Zero_Gain_00->text().toLong();
        add2zero[1] = ui->LC_Zero_Gain_01->text().toLong();
        add2zero[2] = ui->LC_Zero_Gain_02->text().toLong();
        add2zero[3] = ui->LC_Zero_Gain_03->text().toLong();
        add2zero[4] = ui->LC_Zero_Gain_04->text().toLong();
        add2zero[5] = ui->LC_Zero_Gain_05->text().toLong();
        add2zero[6] = ui->LC_Zero_Gain_06->text().toLong();
        add2zero[7] = ui->LC_Zero_Gain_07->text().toLong();

        add2unit[0] = ui->LC_Unit_Gain_00->text().toLong();
        add2unit[1] = ui->LC_Unit_Gain_01->text().toLong();
        add2unit[2] = ui->LC_Unit_Gain_02->text().toLong();
        add2unit[3] = ui->LC_Unit_Gain_03->text().toLong();
        add2unit[4] = ui->LC_Unit_Gain_04->text().toLong();
        add2unit[5] = ui->LC_Unit_Gain_05->text().toLong();
        add2unit[6] = ui->LC_Unit_Gain_06->text().toLong();
        add2unit[7] = ui->LC_Unit_Gain_07->text().toLong();
    }
    else
    {
        cout<<"CANT OPEN FILE"<<endl;
    }
}

void MainWindow::LoadCell_Callback()
{
    if(ui->CheckBox_UI->isChecked()){ // CheckBox = 1일 때 Plot작동
        if(!(timer->isActive()))
            timer->start(10);
    }
    else{
        if((timer->isActive()))
            timer->stop();
    }

    serial_cnt++;
    for(int i = 0; i < LC_NUM/2; i++) // Right, Left 나눠서 LoadCell값 저장
    {
        L_LC_Data[i] = qnode->LC_info.l_lc_data[i];
        R_LC_Data[i] = qnode->LC_info.r_lc_data[i];

    // ******************** Filtering ********************

    //        L_LC_Data[i] = avg(L_LC_Data[i]);
    //        R_LC_Data[i] = avg(R_LC_Data[i]);

    //        R_LC_Data_Filtering[i] = Low_pass_filter(R_LC_Data[i]);
    //        L_LC_Data_Filtering[i] = Low_pass_filter(L_LC_Data[i]);

    // ***************************************************

    }

    ui->LC_Data_00->setText(QString::number(L_LC_Data[0]));
    ui->LC_Data_01->setText(QString::number(L_LC_Data[1]));
    ui->LC_Data_02->setText(QString::number(L_LC_Data[2]));
    ui->LC_Data_03->setText(QString::number(L_LC_Data[3]));

    ui->LC_Data_04->setText(QString::number(R_LC_Data[0]));
    ui->LC_Data_05->setText(QString::number(R_LC_Data[1]));
    ui->LC_Data_06->setText(QString::number(R_LC_Data[2]));
    ui->LC_Data_07->setText(QString::number(R_LC_Data[3]));

    for(int i = 0; i < LC_NUM; i++) // 단위 변환한 LoadCell값
    {
        if(i < 4)
        {
            LC_Zero_Value[i] = abs(L_LC_Data[i] - add2zero[i]);
            LC_Unit_Value[i] = double(LC_Zero_Value[i]) / double(add2unit[i]);
        }
        else // if(i >= 4)
        {
            LC_Zero_Value[i] = abs(R_LC_Data[i-4] - add2zero[i]);
            LC_Unit_Value[i] = double(LC_Zero_Value[i]) / double(add2unit[i]);
        }
    }

    //////////////print_foot_coordinate/////////////////
    ///각 발의 중심을 원점으로 좌표 설정
    ////////R////////
    LC_Pos_X_Value[0] = -(LC_Unit_Value[0]); //단위화된 값
    LC_Pos_X_Value[1] = +(LC_Unit_Value[1]);
    LC_Pos_X_Value[2] = -(LC_Unit_Value[2]);
    LC_Pos_X_Value[3] = +(LC_Unit_Value[3]);

    LC_Pos_Y_Value[0] = +(LC_Unit_Value[0]);
    LC_Pos_Y_Value[1] = +(LC_Unit_Value[1]);
    LC_Pos_Y_Value[2] = -(LC_Unit_Value[2]);
    LC_Pos_Y_Value[3] = -(LC_Unit_Value[3]);
    ////////L////////
    LC_Pos_X_Value[4] = -(LC_Unit_Value[4]);
    LC_Pos_X_Value[5] = +(LC_Unit_Value[5]);
    LC_Pos_X_Value[6] = -(LC_Unit_Value[6]);
    LC_Pos_X_Value[7] = +(LC_Unit_Value[7]);

    LC_Pos_Y_Value[4] = +(LC_Unit_Value[4]);
    LC_Pos_Y_Value[5] = +(LC_Unit_Value[5]);
    LC_Pos_Y_Value[6] = -(LC_Unit_Value[6]);
    LC_Pos_Y_Value[7] = -(LC_Unit_Value[7]);

    ////////T_R////////_It's depends on how wide your legs are.
    LC_T_Pos_X_Value[0] = -2.4*(LC_Unit_Value[0]);          //한 발 지지와 원점이 다르기 때문에 거리 비율 계산해서 곱하기
    LC_T_Pos_X_Value[1] = -0.4*(LC_Unit_Value[1]);
    LC_T_Pos_X_Value[2] = -2.4*(LC_Unit_Value[2]);
    LC_T_Pos_X_Value[3] = -0.4*(LC_Unit_Value[3]);

    LC_T_Pos_Y_Value[0] = +(LC_Unit_Value[0]);
    LC_T_Pos_Y_Value[1] = +(LC_Unit_Value[1]);
    LC_T_Pos_Y_Value[2] = -(LC_Unit_Value[2]);
    LC_T_Pos_Y_Value[3] = -(LC_Unit_Value[3]);

    ////////T_L////////
    LC_T_Pos_X_Value[4] = +0.4*(LC_Unit_Value[4]);
    LC_T_Pos_X_Value[5] = +2.4*(LC_Unit_Value[5]);
    LC_T_Pos_X_Value[6] = +0.4*(LC_Unit_Value[6]);
    LC_T_Pos_X_Value[7] = +2.4*(LC_Unit_Value[7]);

    LC_T_Pos_Y_Value[4] = +(LC_Unit_Value[4]);
    LC_T_Pos_Y_Value[5] = +(LC_Unit_Value[5]);
    LC_T_Pos_Y_Value[6] = -(LC_Unit_Value[6]);
    LC_T_Pos_Y_Value[7] = -(LC_Unit_Value[7]);

    ////////////////foot ratio///////////////_foot row, column
    /// 증폭을 위해 x,y 비율에 따라 계수 곱하기
    L_Pos_X_Coordinate = 7.7*((LC_Pos_X_Value[0]+LC_Pos_X_Value[1]+LC_Pos_X_Value[2]+LC_Pos_X_Value[3])/4.0);
    L_Pos_Y_Coordinate = 12.9*((LC_Pos_Y_Value[0]+LC_Pos_Y_Value[1]+LC_Pos_Y_Value[2]+LC_Pos_Y_Value[3])/4.0);
    R_Pos_X_Coordinate = 7.7*((LC_Pos_X_Value[4]+LC_Pos_X_Value[5]+LC_Pos_X_Value[6]+LC_Pos_X_Value[7])/4.0);
    R_Pos_Y_Coordinate = 12.9*((LC_Pos_Y_Value[4]+LC_Pos_Y_Value[5]+LC_Pos_Y_Value[6]+LC_Pos_Y_Value[7])/4.0);

    if(LC_Unit_Value[0] >= 1.5 && LC_Unit_Value[1] >= 1.5 && LC_Unit_Value[2] >= 1.5 && LC_Unit_Value[3] >= 1.5 && LC_Unit_Value[4] >= 1.5 && LC_Unit_Value[5] >= 1.5 && LC_Unit_Value[6] >= 1.5 && LC_Unit_Value[7] >= 1.5)
    {
        T_Pos_X_Coordinate = 7.7*(LC_T_Pos_X_Value[0]+LC_T_Pos_X_Value[1]+LC_T_Pos_X_Value[2]+LC_T_Pos_X_Value[3]+LC_T_Pos_X_Value[4]+LC_T_Pos_X_Value[5]+LC_T_Pos_X_Value[6]+LC_T_Pos_X_Value[7])/8.0;
        T_Pos_Y_Coordinate = 12.9*(LC_T_Pos_Y_Value[0]+LC_T_Pos_Y_Value[1]+LC_T_Pos_Y_Value[2]+LC_T_Pos_Y_Value[3]+LC_T_Pos_Y_Value[4]+LC_T_Pos_Y_Value[5]+LC_T_Pos_Y_Value[6]+LC_T_Pos_Y_Value[7])/8.0;
        Both_Feet = true;
        Left_Foot = false;
        Right_Foot = false;
        cout<<"Both_Feet<"<<endl;
    }

    else if(LC_Unit_Value[0] <= 1.5 && LC_Unit_Value[1] <= 1.5 && LC_Unit_Value[2] <= 1.5 && LC_Unit_Value[3] <= 1.5 && LC_Unit_Value[4] <= 1.5 && LC_Unit_Value[5] <= 1.5 && LC_Unit_Value[6] <= 1.5 && LC_Unit_Value[7] <= 1.5)
    {
        T_Pos_X_Coordinate = 7.7*(LC_T_Pos_X_Value[0]+LC_T_Pos_X_Value[1]+LC_T_Pos_X_Value[2]+LC_T_Pos_X_Value[3]+LC_T_Pos_X_Value[4]+LC_T_Pos_X_Value[5]+LC_T_Pos_X_Value[6]+LC_T_Pos_X_Value[7])/8.0;
        T_Pos_Y_Coordinate = 12.9*(LC_T_Pos_Y_Value[0]+LC_T_Pos_Y_Value[1]+LC_T_Pos_Y_Value[2]+LC_T_Pos_Y_Value[3]+LC_T_Pos_Y_Value[4]+LC_T_Pos_Y_Value[5]+LC_T_Pos_Y_Value[6]+LC_T_Pos_Y_Value[7])/8.0;
        Both_Feet = false;
        Left_Foot = false;
        Right_Foot = false;
        cout<<"None<"<<endl;
    }

    else if(LC_Unit_Value[0] <= 1.5 && LC_Unit_Value[1] <= 1.5 && LC_Unit_Value[2] <= 1.5 && LC_Unit_Value[3] <= 1.5)
    {
        T_Pos_X_Coordinate = R_Pos_X_Coordinate+120.0;
        T_Pos_Y_Coordinate = R_Pos_Y_Coordinate;
        Left_Foot = false;
        Right_Foot = true;
        Both_Feet = false;
        cout<<"Right_Foot<"<<endl;
    }
    else if(LC_Unit_Value[4] <= 1.5 && LC_Unit_Value[5] <= 1.5 && LC_Unit_Value[6] <= 1.5 && LC_Unit_Value[7] <= 1.5)
    {
        T_Pos_X_Coordinate = L_Pos_X_Coordinate-120.0;
        T_Pos_Y_Coordinate = L_Pos_Y_Coordinate;
        Right_Foot = false;
        Left_Foot = true;
        Both_Feet = false;
        cout<<"Left_Foot<"<<endl;
    }
    else
    {
        T_Pos_X_Coordinate = 7.7*(LC_T_Pos_X_Value[0]+LC_T_Pos_X_Value[1]+LC_T_Pos_X_Value[2]+LC_T_Pos_X_Value[3]+LC_T_Pos_X_Value[4]+LC_T_Pos_X_Value[5]+LC_T_Pos_X_Value[6]+LC_T_Pos_X_Value[7])/8.0;
        T_Pos_Y_Coordinate = 12.9*(LC_T_Pos_Y_Value[0]+LC_T_Pos_Y_Value[1]+LC_T_Pos_Y_Value[2]+LC_T_Pos_Y_Value[3]+LC_T_Pos_Y_Value[4]+LC_T_Pos_Y_Value[5]+LC_T_Pos_Y_Value[6]+LC_T_Pos_Y_Value[7])/8.0;
        Both_Feet = true;
        Left_Foot = false;
        Right_Foot = false;
        cout<<"Both_Feet_v2<"<<endl;
    }

        cout<<"R_Pos_X_Coordinate        "<< R_Pos_X_Coordinate << endl;
        cout<<"R_Pos_Y_Coordinate        "<< R_Pos_Y_Coordinate << endl;
        cout<<"L_Pos_X_Coordinate        "<< L_Pos_X_Coordinate << endl;
        cout<<"L_Pos_Y_Coordinate        "<< L_Pos_Y_Coordinate << endl;
    //////////////print_foot_size_limit///////////////_It's depends on how wide your legs are and the weight of the robot.
    if(fabs(R_Pos_X_Coordinate)>=300.0) //840
    {
        if(R_Pos_X_Coordinate<0)
        {
            R_Pos_X_Coordinate = -300.0;
        }
        else if(R_Pos_X_Coordinate>0)
        {
            R_Pos_X_Coordinate = 300.0;
        }
    }
    if(fabs(R_Pos_Y_Coordinate)>=300.0) //340
    {
        if(R_Pos_Y_Coordinate<0.0)
        {
            R_Pos_Y_Coordinate = -300.0;
        }
        else if(R_Pos_Y_Coordinate>0.0)
        {
            R_Pos_Y_Coordinate = 300.0;
        }
    }

    if(fabs(L_Pos_X_Coordinate)>=300.0)
    {
        if(L_Pos_X_Coordinate<0.0)
        {
            L_Pos_X_Coordinate = -300.0;
        }
        else if(L_Pos_X_Coordinate>0.0)
        {
            L_Pos_X_Coordinate = 300.0;
        }
    }
    if(fabs(L_Pos_Y_Coordinate)>=300.0)
    {
        if(L_Pos_Y_Coordinate<0.0)
        {
            L_Pos_Y_Coordinate = -300.0;
        }
        else if(L_Pos_Y_Coordinate>0.0)
        {
            L_Pos_Y_Coordinate = 300.0;
        }
    }
    if(fabs(T_Pos_X_Coordinate)>=400.0) //400
    {
        if(T_Pos_X_Coordinate<0.0)
        {
            T_Pos_X_Coordinate = -400.0;
        }
        else if(T_Pos_X_Coordinate>0.0)
        {
            T_Pos_X_Coordinate = 400.0;
        }
    }
    if(fabs(T_Pos_Y_Coordinate)>=400.0) //340
    {
        if(T_Pos_Y_Coordinate<0.0)
        {
            T_Pos_Y_Coordinate = -400.0;
        }
        else if(T_Pos_Y_Coordinate>0.0)
        {
            T_Pos_Y_Coordinate = 400.0;
        }
    }

    /////////////////////real_robot_zmp_point///////////////////_foot x,y
    zmp.right_x_zmp = R_Pos_X_Coordinate;
    zmp.right_y_zmp = R_Pos_Y_Coordinate;
    zmp.left_x_zmp = L_Pos_X_Coordinate;
    zmp.left_y_zmp = L_Pos_Y_Coordinate;
    zmp.total_x_zmp = T_Pos_X_Coordinate;
    zmp.total_y_zmp = T_Pos_Y_Coordinate;
    zmp.right_foot = Right_Foot;//support_leg
    zmp.left_foot = Left_Foot;//support_leg
    zmp.both_feet = Both_Feet;//support_leg

    //////////////////////////COM_point////////////////////////////
    ///ik_walk랑 load_cell의 x,y방향이 반대이기 때문에 반대로 저장해서 사용
    X_com = qnode->COM_info.y_com;
    Y_com = qnode->COM_info.x_com;

    //    cout<<"X_com     ===   "<<X_com<<endl;
    //    cout<<"Y_com     ===   "<<Y_com<<endl;

    ui->R_X_Coordinate->setText(QString::number(R_Pos_X_Coordinate));
    ui->R_Y_Coordinate->setText(QString::number(R_Pos_Y_Coordinate));

    ui->L_X_Coordinate->setText(QString::number(L_Pos_X_Coordinate));
    ui->L_Y_Coordinate->setText(QString::number(L_Pos_Y_Coordinate));

    ui->T_X_Coordinate->setText(QString::number(T_Pos_X_Coordinate));
    ui->T_Y_Coordinate->setText(QString::number(T_Pos_Y_Coordinate));

    ui->COM_X_Coordinate->setText(QString::number(X_com));
    ui->COM_Y_Coordinate->setText(QString::number(Y_com));

    ui->LC_Zero_Value_00->setText(QString::number(LC_Zero_Value[0]));
    ui->LC_Zero_Value_01->setText(QString::number(LC_Zero_Value[1]));
    ui->LC_Zero_Value_02->setText(QString::number(LC_Zero_Value[2]));
    ui->LC_Zero_Value_03->setText(QString::number(LC_Zero_Value[3]));

    ui->LC_Zero_Value_04->setText(QString::number(LC_Zero_Value[4]));
    ui->LC_Zero_Value_05->setText(QString::number(LC_Zero_Value[5]));
    ui->LC_Zero_Value_06->setText(QString::number(LC_Zero_Value[6]));
    ui->LC_Zero_Value_07->setText(QString::number(LC_Zero_Value[7]));

    ui->LC_Unit_Value_00->setText(QString::number(LC_Unit_Value[0]));
    ui->LC_Unit_Value_01->setText(QString::number(LC_Unit_Value[1]));
    ui->LC_Unit_Value_02->setText(QString::number(LC_Unit_Value[2]));
    ui->LC_Unit_Value_03->setText(QString::number(LC_Unit_Value[3]));

    ui->LC_Unit_Value_04->setText(QString::number(LC_Unit_Value[4]));
    ui->LC_Unit_Value_05->setText(QString::number(LC_Unit_Value[5]));
    ui->LC_Unit_Value_06->setText(QString::number(LC_Unit_Value[6]));
    ui->LC_Unit_Value_07->setText(QString::number(LC_Unit_Value[7]));

    qnode->Zmp_Pub->publish(zmp);
}

void MainWindow::Zero_reset(int foot_what)
{
    if(foot_what == RIGHT_FOOT)
    {
        cout << "R_Zero_reset" << endl;

        for (int i = 0; i < LC_NUM; i++)
        {
            add2zero[i] = R_LC_Data[i];
        }

        ui->LC_Zero_Gain_00->setText(QString::number(add2zero[0]));
        ui->LC_Zero_Gain_01->setText(QString::number(add2zero[1]));
        ui->LC_Zero_Gain_02->setText(QString::number(add2zero[2]));
        ui->LC_Zero_Gain_03->setText(QString::number(add2zero[3]));


    }
    if(foot_what == LEFT_FOOT)
    {
        cout << "L_Zero_reset" << endl;

        for (int i =4; i < LC_NUM; i++)
        {
            add2zero[i] = L_LC_Data[i-4];
        }

        ui->LC_Zero_Gain_04->setText(QString::number(add2zero[4]));
        ui->LC_Zero_Gain_05->setText(QString::number(add2zero[5]));
        ui->LC_Zero_Gain_06->setText(QString::number(add2zero[6]));
        ui->LC_Zero_Gain_07->setText(QString::number(add2zero[7]));

    }

}

int compare(const void *a, const void *b)
{
    const int* x = (int*)a;
    const int* y = (int*)b;

    if(*x > *y)
    {
        return 1;
    }
    else if(*x < *y)
    {
        return -1;
    }

    return 0;
}

// ********************** FILTER FUNCTIONS ************************

long int MainWindow::Low_pass_filter(long int initial_data)
{
    double Sampling_time = 0.01;//Load_cell data time (s)
    double Hz = 1/Sampling_time;
    double filtering_Hz = 5;//cutoff frequency 5~10Hz
    double lambda = 2*PI*filtering_Hz*Sampling_time;

    Output = (lambda/(1+lambda))*initial_data + (1/(1+lambda))*data_old;

    data_old = Output;

    //    cout<<"Output  "<<Output<<endl;
    //    cout<<"data_old  "<<data_old<<endl;

    return Output;
}

long int MainWindow::avg(long int x)
{
    unsigned char i;
    long int sum = 0, average;
    for (i = 0; i < FILTERDATA - 1; i++) temp[i] = temp[i+1];
    temp[FILTERDATA - 1] = x;
    for (i = 0; i < FILTERDATA; i++) sum += temp[i];
    average = sum / FILTERDATA;
    //     sort(data,data+FILTERDATA);//
    return /*data[4];*/ average;
}

void MainWindow::median(int data_1,int data_2,int data_3,int data_4,int data_5,int data_6,int data_7,int data_8)
{
    int median[8] = {0,};
    int median_array = median_cnt/2;

    int load_cell_median_buf[8][median_cnt] = {{0,},{0,},{0,},{0,},{0,},{0,},{0,}};
    int data_array[8] = {data_1,data_2,data_3,data_4,data_5,data_6,data_7,data_8};

    for(int i = 0; i < LC_NUM; i++)
    {
        for(int j = 0; j < median_cnt-1; j++)
        {
            load_cell_median_buffer[i][j] = load_cell_median_buffer[i][j+1];
        }

        load_cell_median_buffer[i][median_cnt-1] = data_array[i];
    }

    for(int i = 0; i < LC_NUM; i++)
    {
        for(int j = 0; j < median_cnt; j++)
        {
            load_cell_median_buf[i][j] = load_cell_median_buffer[i][j];
        }
    }

    for(int i = 0; i < LC_NUM; i ++)
    {
        qsort(load_cell_median_buf[i],median_cnt,sizeof(int),compare);
        median[i] = load_cell_median_buf[i][median_array];
    }

    for(int i = 0; i < LC_NUM; i++)
    {
        //        load_cell_Value__[i] = median[i];
    }
}

// *********************** PLOT FUNCTIONS ************************

void MainWindow::update()
{
    repaint();
}
void MainWindow::paintEvent(QPaintEvent *event)
{
// ****************************** Foot Image Painter ******************************
        QPixmap pixmap_1(":/images/Right_foot.png");
        pixmap_1 = pixmap_1.scaled(QSize(160,256));//foot size
        QPainter painter_r_foot(&pixmap_1);

        QPixmap pixmap_1_1(":/images/BackGround.png");
        pixmap_1_1 = pixmap_1_1.scaled(QSize(320,512));//foot size
        QPainter painter_1_1(&pixmap_1_1);

        QPixmap pixmap_2(":/images/Left_foot.png");
        pixmap_2 = pixmap_2.scaled(QSize(160,256));//foot size
        QPainter painter_l_foot(&pixmap_2);

        QPixmap pixmap_2_1(":/images/BackGround.png");
        pixmap_2_1 = pixmap_2_1.scaled(QSize(320,512));//foot size
        QPainter painter_2_1(&pixmap_2_1);

        QPixmap pixmap_3(":/images/Feet.png");
        pixmap_3 = pixmap_3.scaled(QSize(390,240));//foot size
        QPainter painter_3(&pixmap_3);

        QPixmap pixmap_3_1(":/images/BackGround.png");
        pixmap_3_1 = pixmap_3_1.scaled(QSize(460,380));//foot size
        QPainter painter_3_1(&pixmap_3_1);

        ui->Right_Foot_Image->setPixmap(pixmap_1);
        ui->BackGround_R_Foot->setPixmap(pixmap_1_1);
        ui->Left_Foot_Image->setPixmap(pixmap_2);
        ui->BackGround_L_Foot->setPixmap(pixmap_2_1);
        ui->Both_Feet_Image->setPixmap(pixmap_3);
        ui->BackGround_Both_Feet->setPixmap(pixmap_3_1);

        if(ui->CheckBox_UI->isChecked()){

            //Right_zmp_painter
            painter_r_foot.setPen(blue);
            painter_r_foot.setBrush(blue);
            painter_r_foot.drawEllipse(R_Pos_X_Coordinate+70,-R_Pos_Y_Coordinate+120,20,20);
            painter_1_1.setPen(blue);
            painter_1_1.setBrush(blue);
            painter_1_1.drawEllipse(R_Pos_X_Coordinate+140,-R_Pos_Y_Coordinate+240,20,20);

            //Left_zmp_painter
            painter_l_foot.setPen(red);
            painter_l_foot.setBrush(red);
            painter_l_foot.drawEllipse(L_Pos_X_Coordinate+70,-L_Pos_Y_Coordinate+120,20,20);
            painter_2_1.setPen(red);
            painter_2_1.setBrush(red);
            painter_2_1.drawEllipse(L_Pos_X_Coordinate+140,-L_Pos_Y_Coordinate+240,20,20);

            //Total_zmp_painter
            painter_3.setPen(darkGreen);
            painter_3.setBrush(darkGreen);
            painter_3.drawEllipse(T_Pos_X_Coordinate+190,-T_Pos_Y_Coordinate+115,10,10);
            painter_3_1.setPen(darkGreen);
            painter_3_1.setBrush(darkGreen);
            painter_3_1.drawEllipse(T_Pos_X_Coordinate+225,-T_Pos_Y_Coordinate+185,10,10);

            //COM_painter
            painter_3.setPen(black);
            painter_3.setBrush(black);
            painter_3.drawEllipse(X_com+190,-Y_com+115,10,10);
            painter_3_1.setPen(black);
            painter_3_1.setBrush(black);
            painter_3_1.drawEllipse(X_com+225,-Y_com+185,10,10);
        }

}

// File IO
void load_cell::MainWindow::on_Save_Button_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/lwj/colcon_ws/src/load_cell_2025/work/");

    if (fileName.isEmpty())
    {
        qDebug() << "Save Cancel";
    }
    else
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->LC_Zero_Gain_00->text().toInt() << "\n"
                << ui->LC_Zero_Gain_01->text().toInt() << "\n"
                << ui->LC_Zero_Gain_02->text().toInt() << "\n"
                << ui->LC_Zero_Gain_03->text().toInt() << "\n"
                << ui->LC_Zero_Gain_04->text().toInt() << "\n"
                << ui->LC_Zero_Gain_05->text().toInt() << "\n"
                << ui->LC_Zero_Gain_06->text().toInt() << "\n"
                << ui->LC_Zero_Gain_07->text().toInt() << "\n"
                << ui->LC_Unit_Gain_00->text().toInt() << "\n"
                << ui->LC_Unit_Gain_01->text().toInt() << "\n"
                << ui->LC_Unit_Gain_02->text().toInt() << "\n"
                << ui->LC_Unit_Gain_03->text().toInt() << "\n"
                << ui->LC_Unit_Gain_04->text().toInt() << "\n"
                << ui->LC_Unit_Gain_05->text().toInt() << "\n"
                << ui->LC_Unit_Gain_06->text().toInt() << "\n"
                << ui->LC_Unit_Gain_07->text().toInt() << "\n";
        }

        file.close();
        resetLineEditStyle();
        cout << "File Saved" << endl;
    }
}

void load_cell::MainWindow::on_Open_Button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"), "/home/robit/colcon_ws/src/load_cell_2025/work/");

    if(fileName.isEmpty() == true)
    {
        qDebug() << "Load Cancel";
    }

    else
    {
        std::ifstream is;

        is.open(fileName.toStdString().c_str());

        is >> LC_Zero_Gain[0];
        is >> LC_Zero_Gain[1];
        is >> LC_Zero_Gain[2];
        is >> LC_Zero_Gain[3];
        is >> LC_Zero_Gain[4];
        is >> LC_Zero_Gain[5];
        is >> LC_Zero_Gain[6];
        is >> LC_Zero_Gain[7];

        is >> LC_Zero_Gain[0];
        is >> LC_Zero_Gain[1];
        is >> LC_Zero_Gain[2];
        is >> LC_Zero_Gain[3];
        is >> LC_Zero_Gain[4];
        is >> LC_Zero_Gain[5];
        is >> LC_Zero_Gain[6];
        is >> LC_Zero_Gain[7];

        is.close();

        ui->LC_Zero_Gain_00->setText(QString::number(LC_Zero_Gain[0]));
        ui->LC_Zero_Gain_01->setText(QString::number(LC_Zero_Gain[1]));
        ui->LC_Zero_Gain_02->setText(QString::number(LC_Zero_Gain[2]));
        ui->LC_Zero_Gain_03->setText(QString::number(LC_Zero_Gain[3]));
        ui->LC_Zero_Gain_04->setText(QString::number(LC_Zero_Gain[4]));
        ui->LC_Zero_Gain_05->setText(QString::number(LC_Zero_Gain[5]));
        ui->LC_Zero_Gain_06->setText(QString::number(LC_Zero_Gain[6]));
        ui->LC_Zero_Gain_07->setText(QString::number(LC_Zero_Gain[7]));

        ui->LC_Unit_Gain_00->setText(QString::number(LC_Zero_Gain[0]));
        ui->LC_Unit_Gain_01->setText(QString::number(LC_Zero_Gain[1]));
        ui->LC_Unit_Gain_02->setText(QString::number(LC_Zero_Gain[2]));
        ui->LC_Unit_Gain_03->setText(QString::number(LC_Zero_Gain[3]));
        ui->LC_Unit_Gain_04->setText(QString::number(LC_Zero_Gain[4]));
        ui->LC_Unit_Gain_05->setText(QString::number(LC_Zero_Gain[5]));
        ui->LC_Unit_Gain_06->setText(QString::number(LC_Zero_Gain[6]));
        ui->LC_Unit_Gain_07->setText(QString::number(LC_Zero_Gain[7]));

        add2zero[0] = ui->LC_Zero_Gain_00->text().toLong();
        add2zero[1] = ui->LC_Zero_Gain_01->text().toLong();
        add2zero[2] = ui->LC_Zero_Gain_02->text().toLong();
        add2zero[3] = ui->LC_Zero_Gain_03->text().toLong();
        add2zero[4] = ui->LC_Zero_Gain_04->text().toLong();
        add2zero[5] = ui->LC_Zero_Gain_05->text().toLong();
        add2zero[6] = ui->LC_Zero_Gain_06->text().toLong();
        add2zero[7] = ui->LC_Zero_Gain_07->text().toLong();

        add2unit[0] = ui->LC_Unit_Gain_00->text().toLong();
        add2unit[1] = ui->LC_Unit_Gain_01->text().toLong();
        add2unit[2] = ui->LC_Unit_Gain_02->text().toLong();
        add2unit[3] = ui->LC_Unit_Gain_03->text().toLong();
        add2unit[4] = ui->LC_Unit_Gain_04->text().toLong();
        add2unit[5] = ui->LC_Unit_Gain_05->text().toLong();
        add2unit[6] = ui->LC_Unit_Gain_06->text().toLong();
        add2unit[7] = ui->LC_Unit_Gain_07->text().toLong();

        cout << "file opened" << endl;
        resetLineEditStyle();
    }
}

// zero gain IO
void load_cell::MainWindow::on_ZG_Reset_Button_clicked()
{
    ui->LC_Zero_Gain_00->setText("0");
    ui->LC_Zero_Gain_01->setText("0");
    ui->LC_Zero_Gain_02->setText("0");
    ui->LC_Zero_Gain_03->setText("0");
    ui->LC_Zero_Gain_04->setText("0");
    ui->LC_Zero_Gain_05->setText("0");
    ui->LC_Zero_Gain_06->setText("0");
    ui->LC_Zero_Gain_07->setText("0");

    add2zero[0] = ui->LC_Zero_Gain_00->text().toLong();
    add2zero[1] = ui->LC_Zero_Gain_01->text().toLong();
    add2zero[2] = ui->LC_Zero_Gain_02->text().toLong();
    add2zero[3] = ui->LC_Zero_Gain_03->text().toLong();
    add2zero[4] = ui->LC_Zero_Gain_04->text().toLong();
    add2zero[5] = ui->LC_Zero_Gain_05->text().toLong();
    add2zero[6] = ui->LC_Zero_Gain_06->text().toLong();
    add2zero[7] = ui->LC_Zero_Gain_07->text().toLong();

    // ******** reset UX ***********
    ui->LC_Zero_Gain_00->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_01->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_02->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_03->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_04->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_05->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_06->setStyleSheet("background-color: lightcoral;");
    ui->LC_Zero_Gain_07->setStyleSheet("background-color: lightcoral;");

    ui->ZG_Reset_Button->setEnabled(false);
    ui->ZG_Insert_Button->setEnabled(true);
}
void load_cell::MainWindow::on_ZG_Insert_Button_clicked()
{
    ui->LC_Zero_Gain_00->setText(ui->LC_Data_00->text());
    add2zero[0] = ui->LC_Zero_Gain_00->text().toLong();

    ui->LC_Zero_Gain_01->setText(ui->LC_Data_01->text());
    add2zero[1] = ui->LC_Zero_Gain_01->text().toLong();

    ui->LC_Zero_Gain_02->setText(ui->LC_Data_02->text());
    add2zero[2] = ui->LC_Zero_Gain_02->text().toLong();

    ui->LC_Zero_Gain_03->setText(ui->LC_Data_03->text());
    add2zero[3] = ui->LC_Zero_Gain_03->text().toLong();

    ui->LC_Zero_Gain_04->setText(ui->LC_Data_04->text());
    add2zero[4] = ui->LC_Zero_Gain_04->text().toLong();

    ui->LC_Zero_Gain_05->setText(ui->LC_Data_05->text());
    add2zero[5] = ui->LC_Zero_Gain_05->text().toLong();

    ui->LC_Zero_Gain_06->setText(ui->LC_Data_06->text());
    add2zero[6] = ui->LC_Zero_Gain_06->text().toLong();

    ui->LC_Zero_Gain_07->setText(ui->LC_Data_07->text());
    add2zero[7] = ui->LC_Zero_Gain_07->text().toLong();

    ui->LC_Zero_Gain_00->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_01->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_02->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_03->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_04->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_05->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_06->setStyleSheet("background-color: #66BB6A; ");
    ui->LC_Zero_Gain_07->setStyleSheet("background-color: #66BB6A; ");

    ui->ZG_Reset_Button->setEnabled(true);
    ui->ZG_Insert_Button->setEnabled(false);
}

// specific unit gain puxh
void load_cell::MainWindow::on_UG_Push_00_clicked()
{
    ui->LC_Unit_Gain_00->setText(ui->LC_Zero_Value_00->text());
    add2unit[0] = ui->LC_Unit_Gain_00->text().toLong();
    ui->LC_Unit_Gain_00->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_00->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_01_clicked()
{
    ui->LC_Unit_Gain_01->setText(ui->LC_Zero_Value_01->text());
    add2unit[1] = ui->LC_Unit_Gain_01->text().toLong();
    ui->LC_Unit_Gain_01->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_01->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_02_clicked()
{
    ui->LC_Unit_Gain_02->setText(ui->LC_Zero_Value_02->text());
    add2unit[2] = ui->LC_Unit_Gain_02->text().toLong();
    ui->LC_Unit_Gain_02->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_02->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_03_clicked()
{
    ui->LC_Unit_Gain_03->setText(ui->LC_Zero_Value_03->text());
    add2unit[3] = ui->LC_Unit_Gain_03->text().toLong();
    ui->LC_Unit_Gain_03->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_03->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_04_clicked()
{
    ui->LC_Unit_Gain_04->setText(ui->LC_Zero_Value_04->text());
    add2unit[4] = ui->LC_Unit_Gain_04->text().toLong();
    ui->LC_Unit_Gain_04->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_04->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_05_clicked()
{
    ui->LC_Unit_Gain_05->setText(ui->LC_Zero_Value_05->text());
    add2unit[5] = ui->LC_Unit_Gain_05->text().toLong();
    ui->LC_Unit_Gain_05->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_05->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_06_clicked()
{
    ui->LC_Unit_Gain_06->setText(ui->LC_Zero_Value_06->text());
    add2unit[6] = ui->LC_Unit_Gain_06->text().toLong();
    ui->LC_Unit_Gain_06->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_06->setEnabled(false);
}
void load_cell::MainWindow::on_UG_Push_07_clicked()
{
    ui->LC_Unit_Gain_07->setText(ui->LC_Zero_Value_07->text());
    add2unit[7] = ui->LC_Unit_Gain_07->text().toLong();
    ui->LC_Unit_Gain_07->setStyleSheet("background-color: #66BB6A; ");
    ui->UG_Push_07->setEnabled(false);
}

// Unit Gain IO
void load_cell::MainWindow::on_UG_Reset_Button_clicked()
{
    ui->LC_Unit_Gain_00->setText("1");
    add2unit[0] = ui->LC_Unit_Gain_00->text().toLong();

    ui->LC_Unit_Gain_01->setText("1");
    add2unit[1] = ui->LC_Unit_Gain_01->text().toLong();

    ui->LC_Unit_Gain_02->setText("1");
    add2unit[2] = ui->LC_Unit_Gain_02->text().toLong();

    ui->LC_Unit_Gain_03->setText("1");
    add2unit[3] = ui->LC_Unit_Gain_03->text().toLong();

    ui->LC_Unit_Gain_04->setText("1");
    add2unit[4] = ui->LC_Unit_Gain_04->text().toLong();

    ui->LC_Unit_Gain_05->setText("1");
    add2unit[5] = ui->LC_Unit_Gain_05->text().toLong();

    ui->LC_Unit_Gain_06->setText("1");
    add2unit[6] = ui->LC_Unit_Gain_06->text().toLong();

    ui->LC_Unit_Gain_07->setText("1");
    add2unit[7] = ui->LC_Unit_Gain_07->text().toLong();

    ui->LC_Unit_Gain_00->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_01->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_02->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_03->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_04->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_05->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_06->setStyleSheet("background-color: lightcoral;");
    ui->LC_Unit_Gain_07->setStyleSheet("background-color: lightcoral;");

    ui->UG_Reset_Button->setEnabled(false);
    ui->UG_Insert_Button->setEnabled(true);

    ui->UG_Push_00->setEnabled(true);
    ui->UG_Push_01->setEnabled(true);
    ui->UG_Push_02->setEnabled(true);
    ui->UG_Push_03->setEnabled(true);
    ui->UG_Push_04->setEnabled(true);
    ui->UG_Push_05->setEnabled(true);
    ui->UG_Push_06->setEnabled(true);
    ui->UG_Push_07->setEnabled(true);
}

void MainWindow::on_UG_Insert_Button_clicked()
{
    ui->LC_Unit_Gain_00->setText(ui->LC_Zero_Value_00->text());
    add2unit[0] = ui->LC_Unit_Gain_00->text().toLong();
    ui->LC_Unit_Gain_00->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_01->setText(ui->LC_Zero_Value_01->text());
    add2unit[1] = ui->LC_Unit_Gain_01->text().toLong();
    ui->LC_Unit_Gain_01->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_02->setText(ui->LC_Zero_Value_02->text());
    add2unit[2] = ui->LC_Unit_Gain_02->text().toLong();
    ui->LC_Unit_Gain_02->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_03->setText(ui->LC_Zero_Value_03->text());
    add2unit[3] = ui->LC_Unit_Gain_03->text().toLong();
    ui->LC_Unit_Gain_03->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_04->setText(ui->LC_Zero_Value_04->text());
    add2unit[4] = ui->LC_Unit_Gain_04->text().toLong();
    ui->LC_Unit_Gain_04->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_05->setText(ui->LC_Zero_Value_05->text());
    add2unit[5] = ui->LC_Unit_Gain_05->text().toLong();
    ui->LC_Unit_Gain_05->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_06->setText(ui->LC_Zero_Value_06->text());
    add2unit[6] = ui->LC_Unit_Gain_06->text().toLong();
    ui->LC_Unit_Gain_06->setStyleSheet("background-color: #66BB6A; ");

    ui->LC_Unit_Gain_07->setText(ui->LC_Zero_Value_07->text());
    add2unit[7] = ui->LC_Unit_Gain_07->text().toLong();
    ui->LC_Unit_Gain_07->setStyleSheet("background-color: #66BB6A; ");

    ui->UG_Reset_Button->setEnabled(true);
    ui->UG_Insert_Button->setEnabled(false);

    ui->UG_Push_00->setEnabled(false);
    ui->UG_Push_01->setEnabled(false);
    ui->UG_Push_02->setEnabled(false);
    ui->UG_Push_03->setEnabled(false);
    ui->UG_Push_04->setEnabled(false);
    ui->UG_Push_05->setEnabled(false);
    ui->UG_Push_06->setEnabled(false);
    ui->UG_Push_07->setEnabled(false);
}

MainWindow::~MainWindow() {
}


// filter plot
void MainWindow::on_manager_on_toggled(bool checked)
{

}

void MainWindow::on_yScaleSlider_valueChanged(int value)
{
    plotArtist();
}

// ********************************** QCUSTOMPLOT MANAGE ***************************************
void MainWindow::registerPlot(const QString& name, QCustomPlot* plot, bool fixedYAxis) // called by plot_init
{
    plot_map[name] = plot;

    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::red));
    plot->graph(0)->setName("raw");
    plot->setBackground(QColor(255,255,255));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    plot->xAxis->setTicker(timeTicker);
    plot->xAxis->setLabel("Time (100ms)");
    plot->yAxis->setLabel(name);
    plot->axisRect()->setupFullAxesBox();

    plot->xAxis->setTickLabelFont(QFont(QFont().family(),10));
    plot->yAxis->setTickLabelFont(QFont(QFont().family(),10));

    plot->xAxis->setTickLabelColor(Qt::black);
    plot->xAxis->setLabelColor(Qt::black);
    plot->yAxis->setTickLabelColor(Qt::black);
    plot->yAxis->setLabelColor(Qt::black);
    plot->xAxis2->setTickLabelColor(Qt::black);
    plot->yAxis2->setTickLabelColor(Qt::black);
    plot->xAxis2->setLabelColor(Qt::black);
    plot->yAxis2->setLabelColor(Qt::black);

    plot->xAxis2->setVisible(true);
    plot->yAxis2->setVisible(true);
    plot->xAxis2->setTicks(true);
    plot->yAxis2->setTicks(true);
    plot->xAxis2->setTickLabels(true);
    plot->yAxis2->setTickLabels(true);

    if (fixedYAxis)
    {
        if (name == "LR")
            plot->yAxis->setRange(-300, 300);
        else if (name == "FB")
            plot->yAxis->setRange(-200, 200);
    }
    else
    {
        plot->addGraph(); // graph(1): avg
        plot->graph(1)->setPen(QPen(Qt::blue));
        plot->graph(1)->setName("avg");

        plot->addGraph(); // graph(2): median
        plot->graph(2)->setPen(QPen(Qt::green));
        plot->graph(2)->setName("median");

        plot->addGraph(); // graph(3): lpf
        plot->graph(3)->setPen(QPen(Qt::magenta));
        plot->graph(3)->setName("lpf");
    }
}

void MainWindow::Plot_init()
{
    registerPlot("LR", ui->customPlot_LR, true);
    registerPlot("FB", ui->customPlot_FB, true);
    registerPlot("Filter", ui->customPlot_raw_data, false);
}


void MainWindow::plotArtist() // make_plot
{
    if (!plot_timer.isValid())
        plot_timer.start();

    double key = plot_timer.elapsed() / 100.0; // time interval >> 100 ms

// *************************** ADD ADITIONAL PLOT DATA HERE ******************************

    plot_data["LR_raw"].append(qMakePair(key, T_Pos_X_Coordinate));
    plot_data["FB_raw"].append(qMakePair(key, T_Pos_Y_Coordinate));

    filter_manager->setRawInput(std::vector<double>(std::begin(LC_Unit_Value), std::end(LC_Unit_Value)));

    // calibration data
    if (ui->manager_on->isChecked()) {
        plot_data["Filter_raw"].append(qMakePair(key,
        LC_Unit_Value[selected_sensor_index]));
    }

    // avg + lpf
    if (ui->avg_lpf->isChecked()) {
        filter_manager->applyAvg_LPF();
        double avg_value = avg(LC_Unit_Value[selected_sensor_index]); // avg 필터된 값
        plot_data["Filter_avg"].append(qMakePair(key,
        filter_manager->getFilteredValues()[selected_sensor_index]));
    }

    // median + lpf
    if (ui->median_lpf->isChecked()) {
        filter_manager->applyMedian_LPF();
        plot_data["Filter_median"].append(qMakePair(key,
        filter_manager->getFilteredValues()[selected_sensor_index]));
    }

    // LPF만
    if (ui->lpf->isChecked()) {
        filter_manager->applyLPF();
        plot_data["Filter_lpf"].append(qMakePair(key,
        filter_manager->getFilteredValues()[selected_sensor_index]));
    }

// *************************** ADD ADITIONAL PLOT DATA HERE ******************************

    // plot_name (plot_map의 키값) : "LR", "FB", "Filter"
    // key_name (plot_data의 키값) : "LR_raw", "FB_raw", "Filter_raw", "Filter_avg", "Filter_median", "Filter_lpf"
    // Qcustomplot 객체에 매핑된 이름에 "_raw", "_avg", "_median", "_lpf"를 붙여서 개별 그래프를 관리함

    for (auto it = plot_map.begin(); it != plot_map.end(); ++it)
    {
        const QString& plot_name = it.key();
        QCustomPlot* plot = it.value();

        int graph_count = 1;

        if(plot->graphCount() > 1) graph_count = 4;
        else graph_count = plot->graphCount();

        for (int i = 0; i < graph_count; ++i)
        {
            QString key_name = plot_name + "_" + plot->graph(i)->name();
            QVector<QPair<double, double>>& data = plot_data[key_name];

            if (data.size() > 1000)
                data.remove(0, data.size() - 1000);

            QVector<double> x, y;
            for (const auto& p : data) {
                x.append(p.first);
                y.append(p.second);
            }

            plot->graph(i)->setData(x, y);
            plot->xAxis->setRange(x.last(), 100, Qt::AlignRight);
        }

        if (plot_name != "LR" && plot_name != "FB") {
            int yRange = ui->yScaleSlider->value();
            plot->yAxis->setRange(-yRange, yRange);
        }

        plot->replot();
    }
}
// ************************** FILTER PLOT UI BUTTONS ******************************

void MainWindow::on_filter_button_0_clicked()
{
    updateFilterButtons(0);
}

void MainWindow::on_filter_button_1_clicked()
{
    updateFilterButtons(1);
}

void MainWindow::on_filter_button_2_clicked()
{
    updateFilterButtons(2);
}

void MainWindow::on_filter_button_3_clicked()
{
    updateFilterButtons(3);
}

void MainWindow::on_filter_button_4_clicked()
{
    updateFilterButtons(4);
}

void MainWindow::on_filter_button_5_clicked()
{
    updateFilterButtons(5);
}

void MainWindow::on_filter_button_6_clicked()
{
    updateFilterButtons(6);
}

void MainWindow::on_filter_button_7_clicked()
{
    updateFilterButtons(7);
}

void MainWindow::on_avg_lpf_checkStateChanged(const Qt::CheckState &arg1)
{

}

void MainWindow::on_median_lpf_checkStateChanged(const Qt::CheckState &arg1)
{

}

void MainWindow::on_lpf_checkStateChanged(const Qt::CheckState &arg1)
{

}

} // namespace load_cell
