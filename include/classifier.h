#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <string>
#include <unordered_map>
#include <vector>

#include <rapidjson/document.h>

class Classifier {
public:
    Classifier(std::string filename = "classifications.json");

    struct SuperCategory {
        std::string name = "";
        bool recyclable = false;
    };

    struct RecycleInfo {
        std::string item = "";
        int superclass_id = 0;
        SuperCategory recycle_class;
        std::string special_instructions = "";
    };

    bool initial_file_load();
    void load_file_values();
    Classifier::RecycleInfo classify(int index) const;
    std::unordered_map<int, SuperCategory> get_supercategorymap() const;
    std::vector<RecycleInfo> get_categoryvector() const;

private:
    std::string _filename;
    rapidjson::Document _doc;

    std::unordered_map<int, SuperCategory> _super_categorymap;
    std::vector<RecycleInfo> _recycle_infovector;
};

#endif // CLASSIFIER_H
