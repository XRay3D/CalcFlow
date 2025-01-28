#pragma once

#include <QObject>
#include <functional>
#include <map>

class Calc : public QObject {
    Q_OBJECT

    Q_PROPERTY(double kinematicViscosity WRITE setKinematicViscosity)
    Q_PROPERTY(double dynamicViscosity WRITE setDynamicViscosity)
    Q_PROPERTY(double density WRITE setDensity)

public:
    explicit Calc(QObject* parent = nullptr);

    void calc();
    void updateMap();
    double Dn_t_(const QString& str = {});

    void setCbx_paramCalc(const QString& newCbx_paramCalc);

    // std::function<double()> kinematic_viscosity_t; // TODO удалить в будущем когда ланные будут внутри Calc
    // std::function<double()> dynamic_viscosity_t;
    // std::function<double()> density_t;

    class QDoubleSpinBox* dsbxOfParamCalcDouble; // TODO необходима для быстрого пересчёта

    void setKinematicViscosity(double newVal) { kinematicViscosity_ = newVal, calc(); }
    void setDynamicViscosity(double newVal) { dynamicViscosity_ = newVal, calc(); }
    void setDensity(double newVal) { density_ = newVal, calc(); }

private:
    void loadJson(const QString& str_cbx);

    void load();
    void save();

    double calc_flow_speed();
    double calc_heat_power();
    double calc_mass_flow();
    double calc_volume_flow();
    double calc_volume_flow_();

    /////////////////////

    double specific_volume();

    double flow_t();
    double Re();

    //    double dencity_ {};
    //    double dynamic_viscocity_ {};
    //    double kinematic_viscocity_ {};

    QString cbxparamCalc;

    double flow_speed_{};
    double heat_power_{};
    double mass_flow_{};
    double volume_flow_{};

    //    std::function<void(double )>;

    double Re_;
    double S_{};
    double entalpy_{};
    double value_{};
    double Dn_{};

    double S();
    using CalcFunc = decltype(&Calc::calc_volume_flow);

    std::map<QStringView, double*, std::less<>> map{
        {  L"Массовый расход",   &mass_flow_},
        {  L"Объёмный расход", &volume_flow_},
        {  L"Скорость потока",  &flow_speed_},
        {L"Тепловая мощность",  &heat_power_},
    };

    std::map<QStringView, QString, std::less<>> map_suff{
        {  L"Массовый расход", " м³/ч*кг/м³"},
        {  L"Объёмный расход",       " м³/ч"},
        {  L"Скорость потока",        " м/с"},
        {L"Тепловая мощность",         " Вт"},
    };

    double kinematicViscosity_;

    double dynamicViscosity_;

    double density_;

signals:

    void dsbxS_setValue(double);
    void dsbxDencity_setValue(double);
    void dsbxDynamic_viscocity_setValue(double);
    void dsbxEntalpy_setValue(double);
    void dsbxFlow_speed_setValue(double);
    void dsbxKinematic_viscocity_setValue(double);
    void dsbxMass_flow_setValue(double);
    void dsbxValue_flow_setValue(double);
    void dsbxValue_setValue(double);
    void dsbxValue_re_setValue(double);
};
