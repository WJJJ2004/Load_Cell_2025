#include <QApplication>
#include <iostream>

#include "../include/load_cell_2025/main_window.hpp"

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  load_cell::MainWindow w;
  w.show();
  return a.exec();
}
