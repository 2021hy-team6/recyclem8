#include "settingsmenu.h"
#include "ui_settingsmenu.h"

#include "confighandler.h"

SettingsMenu::SettingsMenu(std::weak_ptr<ConfigHandler> confighandler, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsMenu)
    , _confighandler(std::move(confighandler))
{
    ui->setupUi(this);
    sync_settings_display();
}

void SettingsMenu::sync_settings_display() {
    auto settings = _confighandler.lock()->settings;
    //OpenCV Settings
    switch(settings.source_type) {
    case 0:
        ui->index_radio->setChecked(true);
        break;
    case 1:
        ui->webcam_radio->setChecked(true);
        break;
    default:
        throw "Invalid Settings";
    }

    ui->index_spinbox->setValue(settings.source_index);
    ui->ip_lineedit->setText(settings.source_str.c_str());

    //App Settings
    ui->skip_start_chkbox->setChecked(settings.skip_start);

    //DB Settings
    ui->db_chkbox->setChecked(settings.db_enabled);
}

SettingsMenu::~SettingsMenu() {
    delete ui;
}

void SettingsMenu::on_index_radio_clicked() {
    _confighandler.lock()->settings.source_type = 0;
    _confighandler.lock()->save_file();
}
void SettingsMenu::on_webcam_radio_clicked() {
    _confighandler.lock()->settings.source_type = 1;
    _confighandler.lock()->save_file();
}
void SettingsMenu::on_index_spinbox_valueChanged(int arg){
    _confighandler.lock()->settings.source_index = arg;
    _confighandler.lock()->save_file();
}
void SettingsMenu::on_ip_lineedit_editingFinished() {
    const std::string ip_str_val = ui->ip_lineedit->text().toStdString();
    _confighandler.lock()->settings.source_str = ip_str_val;
    _confighandler.lock()->save_file();
}
void SettingsMenu::on_skip_start_chkbox_clicked(bool checked) {
    _confighandler.lock()->settings.skip_start = checked;
    _confighandler.lock()->save_file();
}
void SettingsMenu::on_db_chkbox_clicked(bool checked) {
    _confighandler.lock()->settings.db_enabled = checked;
    _confighandler.lock()->save_file();
}

