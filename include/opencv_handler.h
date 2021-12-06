#ifndef OPENCV_HANDLER_H
#define OPENCV_HANDLER_H

#include "opencv2/opencv.hpp"
#include "qlabel.h"

#include "classifier.h"
#include "databasehandler.h"
#include "statisticsmenu.h"

class OpenCVHandler {
public:
    OpenCVHandler(int width, int height, std::weak_ptr<StatisticsMenu> stats_menu);

    void set_camera_source(int source_index);
    void set_camera_source(const std::string& source);
    void add_database_functionality(std::weak_ptr<DatabaseHandler> db_handler);
    bool camera_available();

    void start(QLabel& frame_label, QLabel& text_label);

private:
    int _width;
    int _height;
    std::weak_ptr<StatisticsMenu> _stats_menu;

    cv::VideoCapture _camera;
    Classifier _classifier;

    bool _allow_new_detection_log;
    std::string _source_str;

    bool _db_enabled;
    std::weak_ptr<DatabaseHandler> _db_handler;

    void do_stats(int index);
};

#endif // OPENCV_HANDLER_H
