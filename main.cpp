#include "mainwindow.h"
#include <QApplication>
#include "customtumbler.h"
#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
//  w.show();
  CustomTumbler obj;
  obj.setGeometry(400,400, 200, 200);
  obj.setStyle(CustomTumbler::SnowWhite);

  CustomTumbler obj2;
  obj2.setGeometry(200,200,200,200);
  obj2.setStyle(CustomTumbler::Dracula);
  QStringList lstLabels;
  lstLabels;
//  lstLabels << "" << "" << "";
  obj2.setDefaultValues(60, 240, lstLabels, 0);
  obj2.setStyleSheet("* {background-color: rgb(62, 70, 73)}");
//  obj2.setStyleSheet()

  obj2.show();

  obj.show();

  return a.exec();
}
