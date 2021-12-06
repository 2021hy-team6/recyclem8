#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QDialog>

#include <memory>

#include "confighandler.h"

namespace Ui {
class SettingsMenu;
}

class SettingsMenu : public QDialog {
    Q_OBJECT

public:
    explicit SettingsMenu(std::weak_ptr<ConfigHandler> confighandler, QWidget* parent = nullptr);
    ~SettingsMenu();

    void sync_settings_display();
    void force_regen_settings();

private slots:
    void on_index_radio_clicked();
    void on_webcam_radio_clicked();
    void on_index_spinbox_valueChanged(int arg);
    void on_ip_lineedit_editingFinished();
    void on_skip_start_chkbox_clicked(bool checked);
    void on_db_chkbox_clicked(bool checked);

private:
    Ui::SettingsMenu* ui;
    std::weak_ptr<ConfigHandler> _confighandler;
};

#endif // SETTINGSMENU_H
