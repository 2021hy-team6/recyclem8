#ifndef RECYCLEM8_H
#define RECYCLEM8_H

#include <QMainWindow>
#include <thread>

#include "databasehandler.h"
#include "opencv_handler.h"
#include "settingsmenu.h"
#include "statisticsmenu.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RecycleM8; }
QT_END_NAMESPACE

class RecycleM8 : public QMainWindow {
    Q_OBJECT

public:
    RecycleM8(QWidget* parent = nullptr);
    ~RecycleM8();

    void perform_additional_prestart_actions();

private slots:
    void on_startButton_pressed();
    void on_settings_default_generator_accepted();
    void on_settings_default_generator_rejected();
    void on_actionAdjust_Settings_triggered();
    void on_actionView_Statistics_triggered();
    void on_actionReconnect_to_Camera_triggered();

private:
    Ui::RecycleM8* ui;

    std::shared_ptr<ConfigHandler> _confighandler;
    std::shared_ptr<DatabaseHandler> _dbhandler;
    std::shared_ptr<StatisticsMenu> _statistics_menu;

    bool _settings_invalid;

    void start_scanner();
};
#endif // RECYCLEM8_H
