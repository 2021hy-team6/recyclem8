#include "opencv_handler.h"

#include <chrono>
#include <iostream>

#include <opencv2/opencv.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include "qimage.h"
#include "qlabel.h"

#include "classifier.h"
#include "settingsmenu.h"

OpenCVHandler::OpenCVHandler(int width, int height, std::weak_ptr<StatisticsMenu> stats_menu)
    : _width(width), _height(height), _stats_menu(std::move(stats_menu))
{
    _classifier = Classifier();
    if(!_classifier.initial_file_load()) {
        throw std::runtime_error("Could not load Classify file");
    }
    _classifier.load_file_values();

    _db_enabled = false;
}

void OpenCVHandler::set_camera_source(int source_index) {
    _camera = cv::VideoCapture(source_index);
}
void OpenCVHandler::set_camera_source(const std::string& source_string) {
    _camera = cv::VideoCapture(source_string);
    _source_str = source_string;
}

void OpenCVHandler::add_database_functionality(std::weak_ptr<DatabaseHandler> db_handler) {
    _db_handler = db_handler;
    _db_handler.lock()->delete_category_table_data();

    std::unordered_map<int, Classifier::SuperCategory> supercategories = _classifier.get_supercategorymap();
    for(int i = 0; i < supercategories.size(); i++) {
        Classifier::SuperCategory curr_supercategory = supercategories.at(i);
        _db_handler.lock()->insert_super_category(curr_supercategory.name, curr_supercategory.recyclable);
    }

    _db_enabled = true;
}

bool OpenCVHandler::camera_available() {
    return _camera.isOpened();
}

void OpenCVHandler::do_stats(int index) {
    switch(index) {
    case 1:
        _stats_menu.lock()->increment_aluminium();
        break;
    case 2:
        _stats_menu.lock()->increment_plastic();
        break;
    case 4:
        _stats_menu.lock()->increment_glass();
        break;
    default:
        _stats_menu.lock()->increment_other();
        break;
    }
}

void OpenCVHandler::start(QLabel& frame_label, QLabel& text_label) {
    cv::dnn::Net net = cv::dnn::readNetFromTensorflow("frozen_taco.pb", "frozen_taco.pbtxt");

    const int detection_frame_timeout_value = 35;
    int detection_frame_timeout_counter = detection_frame_timeout_value;

    Detection top_detection;

    cv::Mat frame;

    while (true) {
        _camera >> frame;
        cv::resize(frame, frame, cv::Size(_width, _height));

        bool detected = false;

        std::chrono::time_point pre_time = std::chrono::high_resolution_clock::now();

        cv::Mat input_blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), true, true);
        net.setInput(input_blob);
        cv::Mat detection = net.forward("detection_out");

        std::chrono::time_point post_time = std::chrono::high_resolution_clock::now();
        long inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(post_time - pre_time).count();

        cv::Mat detection_res(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        for(int i = 0; i < detection_res.rows; i++) {
            float confidence = detection_res.at<float>(i, 2);

            if(confidence > 0.7) {
                int x1 = static_cast<int>(detection_res.at<float>(i, 3) * _width);
                int y1 = static_cast<int>(detection_res.at<float>(i, 4) * _height);
                int x2 = static_cast<int>(detection_res.at<float>(i, 5) * _width);
                int y2 = static_cast<int>(detection_res.at<float>(i, 6) * _height);
                cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 2, 4);

                const int index = static_cast<int>(detection_res.at<float>(i, 1)) - 1;
                Classifier::RecycleInfo info = _classifier.classify(index);

                cv::Size text_size = cv::getTextSize(info.item, cv::FONT_HERSHEY_DUPLEX, 0.5, 1, 0);
                cv::rectangle(frame, cv::Point(x1, y1 - 15), cv::Point(x1 + text_size.width, y1), cv::Scalar(0, 0, 255), -1);
                cv::putText(frame, info.item, cv::Point(x1, y1), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

                if(confidence >= top_detection.degrading_confidence) {
                    top_detection.info = info;
                    top_detection.obj_index = index;
                    top_detection.x1 = x1;
                    top_detection.y1 = y1;
                    top_detection.x2 = x2;
                    top_detection.y2 = y2;
                    top_detection.real_confidence = std::max(confidence, top_detection.real_confidence);
                    top_detection.degrading_confidence = top_detection.real_confidence;
                    top_detection.inference_time = inference_time;
                    top_detection.frame = frame;
                    top_detection.stats_written = false;
                    detection_frame_timeout_counter = detection_frame_timeout_value;
                }

                const std::string recyclable_status = top_detection.info.recycle_class.recyclable ? "Recyclable" : "Not Recyclable";
                const std::string display_text = fmt::format("{} - {} <{}>\n{}\nConfidence: {}",
                                                              top_detection.info.item, top_detection.info.recycle_class.name,
                                                              recyclable_status, top_detection.info.special_instructions,
                                                              top_detection.real_confidence);

                detected = true;
                text_label.show();
                text_label.setText(display_text.c_str());
                text_label.setAlignment(Qt::AlignCenter);
            }
        }
        if(!detected) {
            if(detection_frame_timeout_counter == 0) {
                text_label.hide();
                top_detection.real_confidence = 0;
                top_detection.degrading_confidence = 0;

                if(!top_detection.stats_written) {
                    do_stats(top_detection.info.superclass_id);
                    top_detection.stats_written = true;
                }
                if(_db_enabled) {
                    uint img_id = _db_handler.lock()->insert_image(top_detection.frame.data, top_detection.inference_time);
                    _db_handler.lock()->insert_detection(img_id, top_detection.obj_index,
                                                         top_detection.x1, top_detection.y1,
                                                         top_detection.x2, top_detection.y2);
                }
            }
            else {
                --detection_frame_timeout_counter;
            }
        }
        top_detection.degrading_confidence -= 0.02;

        cv::cvtColor(frame, frame, cv::COLOR_BGRA2RGBA);
        QImage q_img((uchar*)frame.data, frame.cols, frame.rows,
                   QImage::Format_RGBA8888);
        frame_label.setPixmap(QPixmap::fromImage(q_img));

        cv::waitKey(2);
    }
}
