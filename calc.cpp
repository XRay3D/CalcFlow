
#include "calc.h"

#include "timer.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSpinBox>

#include <cmath>

Calc::Calc(QObject* parent)
    : QObject{parent} {
}

void Calc::calc() {
    emit dsbxS_setValue(S());

    //        Гидравлический диаметр м	Dг	0,05
    //        Динамическая вязкость Па*с	µ	0,0000181
    //        Кинематическая вязкость м2/с	ν	0,0000156
    //        Площадь трубы м2	S	0,001963495
    //        Re=(Q/3600*Dг)/(ν*S)

    //   emit dsbxValue_re_setValue(Re());
    // emit dsbxValue_setValue(1 / density_);
    //   emit dsbxMass_flow_setValue(density_ * flow());
    if(map.contains(cbxparamCalc))
        *map[cbxparamCalc] = dsbxOfParamCalcDouble->value();

    static const std::map<QStringView, std::function<void()>, std::less<>> map{
        {  L"Массовый расход", [this] {
 calc_volume_flow_();
 // calc_volume_flow();
 calc_flow_speed();
 calc_heat_power();
 specific_volume();
 }},
        {  L"Объёмный расход", [this] {
 calc_flow_speed();
 calc_mass_flow();
 calc_heat_power();
 specific_volume();
 }},
        {  L"Скорость потока", [this] {
 calc_mass_flow();
 calc_volume_flow();
 calc_heat_power();
 specific_volume();
 }},
        {L"Тепловая мощность", [this] {
 calc_flow_speed();
 calc_mass_flow();
 calc_volume_flow();
 specific_volume();
 }},
    };

    if(map.contains(cbxparamCalc))
        map.at(cbxparamCalc)();

    S();
    Re();

    emit dsbxDencity_setValue(density_);
    emit dsbxDynamic_viscocity_setValue(dynamicViscosity_);
    emit dsbxKinematic_viscocity_setValue(kinematicViscosity_);

    emit dsbxValue_re_setValue(Re_);
    emit dsbxS_setValue(S_);
    emit dsbxValue_flow_setValue(volume_flow_);
    emit dsbxEntalpy_setValue(entalpy_);
    emit dsbxFlow_speed_setValue(flow_speed_);
    emit dsbxMass_flow_setValue(mass_flow_);
    emit dsbxValue_setValue(value_);

    //    ui->emit dsbxOfParamCalcDouble_setValue((this->*map[cbxparamCalc])());
}

void Calc::updateMap() {
    if(map.contains(cbxparamCalc)) {
        dsbxOfParamCalcDouble->setValue(*map[cbxparamCalc]);
        dsbxOfParamCalcDouble->setSuffix(map_suff[cbxparamCalc]);
    }
}

double Calc::calc_flow_speed() { return flow_speed_ = volume_flow_ / S_ / 3600; }

double Calc::calc_mass_flow() { return mass_flow_ = density_ * volume_flow_; }

double Calc::calc_heat_power() { return heat_power_ = flow_t(); }

double Calc::calc_volume_flow() { return volume_flow_ = flow_speed_ * S_ * 3600; }

double Calc::calc_volume_flow_() { return volume_flow_ = mass_flow_ / density_; }

double Calc::specific_volume() { return value_ = 1 / density_; }

double Calc::Dn_t_(const QString& str) {
    static QRegularExpression re(R"((.+\s)?(\d+))");
    if(auto match{re.match(str)}; match.hasMatch()) {
        bool ok{};
        Dn_ = match.captured(2).toDouble(&ok);
    }
    S();
    return Dn_;
}

double Calc::flow_t() { return (map.contains(cbxparamCalc)) ? *map[cbxparamCalc] : 0.0; }

double Calc::Re() { return Re_ = ((volume_flow_ / 3600) * (Dn_ / 1000)) / (S_ * kinematicViscosity_); }

void Calc::setCbx_paramCalc(const QString& newCbx_paramCalc) {
    cbxparamCalc = newCbx_paramCalc;
    updateMap();
}

double Calc::S() { return S_ = acos(-1.0) * pow(Dn_ / 1000, 2) / 4; }
