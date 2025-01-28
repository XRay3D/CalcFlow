
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calc.h"
#include <QLineEdit>
#include <QMainWindow>
#include <map>

class MyLe : public QLineEdit {
    QString suffix_ { "%1" };

public:
    MyLe(QWidget* parent = nullptr)
        : QLineEdit { parent }
    {
        setReadOnly(true);
    }
    virtual ~MyLe() { }
    void setSuffix(QString suffix) { suffix_ = "%1 " + suffix; }
    void setValue(double val) { setText(suffix_.arg(val)); }
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    MyLe* dsbxS_;
    MyLe* dsbxDencity_;
    MyLe* dsbxDynamic_viscocity_;
    MyLe* dsbxEntalpy_;
    MyLe* dsbxFlow_speed_;
    MyLe* dsbxKinematic_viscocity_;
    MyLe* dsbxMass_flow_;
    MyLe* dsbxValue_flow_;
    MyLe* dsbxValue_;
    MyLe* dsbxValue_re_;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    void loadJson(const QString& str_cbx);

    void load();
    void save();

    Calc calc;

    // double S;

    //    double calc_flow_speed();
    //    double calc_heat_power();
    //    double calc_mass_flow();
    //    double calc_volume_flow();
    //    double calc_volume_flow__();

    //    /////////////////////

    //    double kinematic_viscosity_t();
    //    double dynamic_viscosity_t();
    //    double density_t();
    //    double specific_volume();

    //    double Dn_t_();

    //    double flow_t();
    //    double Re();

    //    //    double dencity_ {};
    //    //    double dynamic_viscocity_ {};
    //    //    double kinematic_viscocity_ {};

    //    double flow_speed_ {};
    //    double heat_power_ {};
    //    double mass_flow_ {};
    //    double volume_flow_ {};

    //    double Re_;
    //    double S_ {};
    //    double entalpy_ {};
    //    double value_ {};
    //    double Dn_ {};

    //    double S();

    //    void testj();

    //    using CalcFunc = decltype(&MainWindow::calc_volume_flow);

    //    std::map<QStringView, double*, std::less<>> map {
    //        { L"Массовый расход", &mass_flow_ },
    //        { L"Объёмный расход", &volume_flow_ },
    //        { L"Скорость потока", &flow_speed_ },
    //        { L"Тепловая мощность", &heat_power_ },
    //    };
    //    std::map<QStringView, QString, std::less<>> map_suff {
    //        { L"Массовый расход", " м³/ч*кг/м³" },
    //        { L"Объёмный расход", " м³/ч" },
    //        { L"Скорость потока", " м/с" },
    //        { L"Тепловая мощность", " Вт" },
    //    };
};

#endif // MAINWINDOW_H
