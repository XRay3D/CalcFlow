
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tempmodel.h"
#include <QDebug>
#include <cmath>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QSettings>
#include <ranges>

#ifndef _MSC_VER
#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

#define DSBXHIDE(X) ui->lbl##X, ui->dsbx##X
#define CBXHIDE(X)  ui->lbl##X, ui->cbx##X

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    qDebug(/*__PRETTY_FUNCTION__*/ __FUNCSIG__);

    { // setupUi
        ui->setupUi(this);
        ui->gridLayout->removeWidget(ui->tableView);
        auto addRow = [r = ui->gridLayout->rowCount(), this]<class W>(W* lbl, QWidget* w = nullptr) mutable {
            if constexpr(std::is_same_v<W, QLabel>) lbl->setAlignment(Qt::AlignRight);
            if constexpr(std::is_same_v<W, QFrame>) lbl->setFrameShape(QFrame::HLine);
            ui->gridLayout->addWidget(lbl, r, 0, 1, w ? 1 : 2);
            if(w) ui->gridLayout->addWidget(w, r, 1);
            ++r;
        };

        addRow(new QFrame{this});

        addRow(new QLabel{"Площадь", this}, dsbxS_ = new MyLe{this});
        addRow(new QLabel{"Плотность среды", this}, dsbxDencity_ = new MyLe{this});
        addRow(new QLabel{"Динамическая вязкость среды", this}, dsbxDynamic_viscocity_ = new MyLe{this});
        addRow(new QLabel{"Энтальпия среды", this}, dsbxEntalpy_ = new MyLe{this});
        addRow(new QLabel{"Скорость потока", this}, dsbxFlow_speed_ = new MyLe{this});
        addRow(new QLabel{"Кинематическая вязкость среды", this}, dsbxKinematic_viscocity_ = new MyLe{this});
        addRow(new QLabel{"Массовый расход среды", this}, dsbxMass_flow_ = new MyLe{this});
        addRow(new QLabel{"Объёмный расход среды", this}, dsbxValue_flow_ = new MyLe{this});
        addRow(new QLabel{"Удельный объём среды", this}, dsbxValue_ = new MyLe{this});
        addRow(new QLabel{"Число Рейнольдса", this}, dsbxValue_re_ = new MyLe{this});

        addRow(new QFrame{this});

        addRow(ui->tableView);

        dsbxS_->setSuffix(" м²"); // ²³
        dsbxDencity_->setSuffix(" кг/м³");
        dsbxDynamic_viscocity_->setSuffix(" ");
        dsbxEntalpy_->setSuffix(" ");
        dsbxFlow_speed_->setSuffix(" м/с");
        dsbxKinematic_viscocity_->setSuffix(" ");
        dsbxMass_flow_->setSuffix(" м³/ч*кг/м³");
        dsbxValue_flow_->setSuffix(" м³/ч");
        dsbxValue_->setSuffix(" м³");
        dsbxValue_re_->setSuffix(" ");
    }
#if 0
    for(auto* dsbx: ui->widget_2->findChildren<QDoubleSpinBox*>()) {// todo widgets with has property "canHidden"
        dsbx->setRange(-std::numeric_limits<double>::max(), +std::numeric_limits<double>::max());
        dsbx->setReadOnly(true);
        dsbx->setButtonSymbols(QDoubleSpinBox::NoButtons);
        dsbx->setDecimals(6);
    }
#endif

    using key = std::pair<int, int>;
    struct val {
        std::vector<QWidget*> widgets;
        QStringList lstDn;
        QStringList lstParamDescr;
    };

    std::vector<QWidget*> qweNotParamGas{DSBXHIDE(Viscocity), CBXHIDE(ViscocityType)};
    std::vector<QWidget*> qweNotParamWaterSteam{DSBXHIDE(Viscocity), CBXHIDE(ViscocityType), CBXHIDE(MeasGas)};
    std::vector<QWidget*> qweNotParamFluid{CBXHIDE(MeasGas), DSBXHIDE(Pressure), CBXHIDE(TypeOfPressure), CBXHIDE(Temperature)};
    QStringList lstDnMortiseGas{"25", "32", "40", "50 с сужением на 19", "50 с сужением на 25", "50", "65", "80 с сужением на 50", "80", "100", "150", "200", "250", "300"};
    QStringList lstDnProbe{"100", "150", "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000"};
    QStringList lstDnMortise{"25", "32", "40", "50", "65", "80", "100", "150", "200", "250", "300"};
    QStringList lstParamGasWater{"Объёмный расход", "Массовый расход", "Скорость потока"};
    QStringList lstParamSteam{"Объёмный расход", "Массовый расход", "Скорость потока", "Тепловая мощность"};
    QStringList lstParamFluid{"Объёмный расход", "Скорость потока"};

    ui->cbxMeasGas->addItem("Воздух", .0);
    ui->cbxMeasGas->addItem("Кислород", .0);
    ui->cbxMeasGas->addItem("Азот", .0);
    ui->cbxMeasGas->addItem("Метан", .0);
    ui->cbxMeasGas->addItem("Природный газ", .0);
    ui->cbxMeasGas->addItem("Попутный нефтяной газ", .0);

    ui->cbxTypeOfPressure->addItems({"Избыточное", "Абсолютное"});
    ui->cbxViscocityType->addItems({"Кинематическая вязкость", "Динамическая вязкость и плотность"});

    std::map<key, val> ma{
        {{0, 0},     {qweNotParamGas, lstDnMortiseGas, lstParamGasWater}},
        {{0, 1},          {qweNotParamGas, lstDnProbe, lstParamGasWater}},
        {{1, 0}, {qweNotParamWaterSteam, lstDnMortise, lstParamGasWater}},
        {{1, 1},   {qweNotParamWaterSteam, lstDnProbe, lstParamGasWater}},
        {{2, 0},    {qweNotParamWaterSteam, lstDnMortise, lstParamSteam}},
        {{2, 1},      {qweNotParamWaterSteam, lstDnProbe, lstParamSteam}},
        {{3, 0},         {qweNotParamFluid, lstDnMortise, lstParamFluid}},
        {{3, 1},           {qweNotParamFluid, lstDnProbe, lstParamFluid}},
    };

    auto hw = [ma = std::move(ma), this] {
        auto filter = std::views::filter([](QWidget* w) { return w->property("canHidden").toBool(); });
        for(auto* w: findChildren<QWidget*>() | filter) w->setVisible(true);

        auto& [wgt, diam, paramCalc] = ma.at({ui->cbxMeasuredMedium->currentIndex(), ui->cbxTypeFlowmeter->currentIndex()});
        for(auto hideWidget: wgt) hideWidget->setVisible(false);
        ui->cbxDn->clear();
        ui->cbxDn->addItems(diam);
        ui->cbxParamCalc->clear();
        ui->cbxParamCalc->addItems(paramCalc);
        // ui->cbxMeasGas->clear();
        // ui->cbxMeasGas->addItems(TypeMeasGas);
        // ui->cbxTypeOfPressure->clear();
        // ui->cbxTypeOfPressure->addItems(TypeOfPressure);
        // ui->cbxViscocityType->clear();
        // ui->cbxViscocityType->addItems(ViscocityType);

        // for (int i {}; i < ui->cbxDn->count(); ++i) {
        // static QRegularExpression re(R"((.+\s)?(\d+))");
        // bool ok {};
        // double Dn_ { re.match(diam[i]).captured(2).toDouble(&ok) };
        // ui->cbxDn->setItemData(i, Dn_);
        // }
    };
    hw();

    connect(ui->cbxTemperature, &QComboBox::currentIndexChanged, [this] {
        calc.setKinematicViscosity(ui->cbxTemperature->currentData(Qt::UserRole + 1).toDouble());
        calc.setDynamicViscosity(ui->cbxTemperature->currentData(Qt::UserRole + 0).toDouble());
        calc.setDensity(ui->cbxTemperature->currentData(Qt::UserRole + 2).toDouble());
    });

    calc.dsbxOfParamCalcDouble = ui->dsbxOfParamCalcDouble;

    connect(ui->cbxMeasuredMedium, &QComboBox::currentIndexChanged, hw);
    connect(ui->cbxTypeFlowmeter, &QComboBox::currentIndexChanged, hw);

    // connect(ui->cbxMeasGas, &QComboBox::currentIndexChanged,loadJson());
    // double S;
    // double res_Dn = ui->cbxDn->currentData().toDouble();
    // S = acos(-1.0)*pow(res_Dn/1000,2)/4;
    // qDebug()<<S;
    // qDebug() << __FUNCTION__ << ui->cbxDn->currentData().toDouble();

    connect(ui->cbxParamCalc, &QComboBox::currentTextChanged, &calc, &Calc::setCbx_paramCalc);

    connect(ui->cbxDn, &QComboBox::currentTextChanged, &calc, &Calc::Dn_t_);

    connect(ui->cbxMeasuredMedium, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    connect(ui->cbxTypeFlowmeter, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    // connect(ui->cbxMeasGas, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    // connect(ui->cbxParamCalc, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    connect(ui->cbxTemperature, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    connect(ui->cbxTypeOfPressure, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    connect(ui->cbxViscocityType, &QComboBox::currentIndexChanged, &calc, &Calc::calc);
    connect(ui->dsbxOfParamCalcDouble, &QDoubleSpinBox::valueChanged, &calc, &Calc::calc);
    connect(ui->dsbxPressure, &QDoubleSpinBox::valueChanged, &calc, &Calc::calc);
    connect(ui->dsbxViscocity, &QDoubleSpinBox::valueChanged, &calc, &Calc::calc);

    connect(ui->cbxMeasGas, &QComboBox::currentTextChanged, this, &MainWindow::loadJson); ///
    // loadJson(ui->cbxMeasGas->currentText());
    // connect(ui->cbxMeasGas, &QComboBox::currentIndexChanged,&MainWindow::
    loadJson(ui->cbxMeasGas->currentText());
    // loadJson();

    connect(&calc, &Calc::dsbxS_setValue, dsbxS_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxDencity_setValue, dsbxDencity_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxDynamic_viscocity_setValue, dsbxDynamic_viscocity_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxEntalpy_setValue, dsbxEntalpy_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxFlow_speed_setValue, dsbxFlow_speed_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxKinematic_viscocity_setValue, dsbxKinematic_viscocity_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxMass_flow_setValue, dsbxMass_flow_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxValue_flow_setValue, dsbxValue_flow_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxValue_re_setValue, dsbxValue_re_, &MyLe::setValue);
    connect(&calc, &Calc::dsbxValue_setValue, dsbxValue_, &MyLe::setValue);

    load();

    // for(auto* w: centralWidget()->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly)) w->setToolTip(w->objectName());

    ui->cbxParamCalc->currentIndexChanged(0);
}

MainWindow::~MainWindow() {
    save();
    qDebug(__FUNCSIG__);
    delete ui;
}

void MainWindow::loadJson(const QString& str_cbx) {
    QFile file("../CalcViscocity.json");
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        auto obj{QJsonDocument::fromJson(file.readAll()).object()};
        delete ui->cbxTemperature->model();
        ui->cbxTemperature->setModel(new TempModel(obj[str_cbx].toArray(), ui->cbxTemperature));
        ui->cbxTemperature->setModelColumn(3);
        ui->cbxTemperature->update();
    } else {
        qDebug() << file.errorString();
    }
}

void MainWindow::load() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());

    ui->cbxDn->setCurrentIndex(settings.value("cbxDn").toInt());
    ui->cbxMeasuredMedium->setCurrentIndex(settings.value("cbxMeasuredMedium").toInt());
    ui->cbxTypeFlowmeter->setCurrentIndex(settings.value("cbxTypeFlowmeter").toInt());
    ui->cbxMeasGas->setCurrentIndex(settings.value("cbxMeasGas").toInt());
    ui->cbxParamCalc->setCurrentIndex(settings.value("cbxParamCalc").toInt());
    ui->cbxTemperature->setCurrentIndex(settings.value("cbxTemperature").toInt());
    ui->cbxTypeOfPressure->setCurrentIndex(settings.value("cbxTypeOfPressure").toInt());
    ui->cbxViscocityType->setCurrentIndex(settings.value("cbxViscocityType").toInt());

    ui->dsbxOfParamCalcDouble->setValue(settings.value("dsbxOfParamCalcDouble").toDouble());
    ui->dsbxPressure->setValue(settings.value("dsbxPressure").toDouble());
    ui->dsbxViscocity->setValue(settings.value("dsbxViscocity").toDouble());

    settings.endGroup();
}

void MainWindow::save() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());

    settings.setValue("cbxDn", ui->cbxDn->currentIndex());
    settings.setValue("cbxMeasuredMedium", ui->cbxMeasuredMedium->currentIndex());
    settings.setValue("cbxTypeFlowmeter", ui->cbxTypeFlowmeter->currentIndex());
    settings.setValue("cbxMeasGas", ui->cbxMeasGas->currentIndex());
    settings.setValue("cbxParamCalc", ui->cbxParamCalc->currentIndex());
    settings.setValue("cbxTemperature", ui->cbxTemperature->currentIndex());
    settings.setValue("cbxTypeOfPressure", ui->cbxTypeOfPressure->currentIndex());
    settings.setValue("cbxViscocityType", ui->cbxViscocityType->currentIndex());

    settings.setValue("dsbxOfParamCalcDouble", ui->dsbxOfParamCalcDouble->value());
    settings.setValue("dsbxPressure", ui->dsbxPressure->value());
    settings.setValue("dsbxViscocity", ui->dsbxViscocity->value());
    settings.endGroup();
}
