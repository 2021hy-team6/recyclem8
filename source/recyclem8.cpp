#include "recyclem8.h"
#include "ui_recyclem8.h"

#include "settingsmenu.h"
#include "statisticsmenu.h"
#include "confighandler.h"
#include "databasehandler.h"

#include "QMessageBox"
#include "QMovie"

#include "opencv_handler.h"

enum DIMENSION { WIDTH = 854, HEIGHT = 480 };
enum WIDGET_INDEX { START_MENU = 0, SETTINGS_NOTIFY = 1, SCANNER = 2 };

RecycleM8::RecycleM8(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecycleM8)
{
    ui->setupUi(this);
    ui->menubar->show();
    ui->result_label->hide();
    ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::START_MENU);

    _statistics_menu = std::make_shared<StatisticsMenu>();
    _confighandler = std::make_shared<ConfigHandler>("config.json");
    _settings_invalid = !_confighandler->initial_file_load();
}

RecycleM8::~RecycleM8() {
    delete ui;
}

void RecycleM8::perform_additional_prestart_actions() {
    if(_settings_invalid) {
        return;
    }

    _confighandler->load_file_values();

    if(_confighandler->settings.db_enabled) {
        _dbhandler = std::make_shared<DatabaseHandler>(_confighandler->settings.db_conn_str);
        _statistics_menu->add_dashboard_link(_confighandler->settings.dashboard_link);
    }
    if(_confighandler->settings.skip_start) {
        ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::SCANNER);
        start_scanner();
    }
}

void RecycleM8::on_startButton_pressed() {
    if(_settings_invalid) {
        ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::SETTINGS_NOTIFY);
    }
    else {
        ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::SCANNER);
        start_scanner();
    }
}

void RecycleM8::on_settings_default_generator_rejected() {
    ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::START_MENU);
}
void RecycleM8::on_settings_default_generator_accepted() {
    _confighandler->populate_with_defaults();
    _confighandler->save_file(true);

    ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::SCANNER);
    start_scanner();
}

void RecycleM8::on_actionAdjust_Settings_triggered() {
    SettingsMenu settings_menu(_confighandler);
    settings_menu.setModal(true);
    settings_menu.exec();
}

void RecycleM8::on_actionView_Statistics_triggered() {
    _statistics_menu->show();
}

void RecycleM8::on_actionReconnect_to_Camera_triggered() {
    start_scanner();
}

void RecycleM8::start_scanner() {
    OpenCVHandler cv_handler(DIMENSION::WIDTH, DIMENSION::HEIGHT, _statistics_menu);

    switch(_confighandler->settings.source_type) {
    case 0:
        cv_handler.set_camera_source(_confighandler->settings.source_index);
        break;
    case 1:
        cv_handler.set_camera_source(_confighandler->settings.source_str);
        break;
    default:
        ui->stackedWidget->setCurrentIndex(WIDGET_INDEX::SETTINGS_NOTIFY);
        return;
    }

    if(cv_handler.camera_available()) {
        if(_confighandler->settings.db_enabled) {
            cv_handler.add_database_functionality(_dbhandler);
        }
        cv_handler.start(*ui->opencv_label, *ui->result_label);
    }
    else {
        QMessageBox error_popup;
        error_popup.critical(this, "Camera Unavailable",
                             "Could not connect to Camera Source.");
    }
}


