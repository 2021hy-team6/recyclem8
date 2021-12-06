#include "classifier.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

Classifier::Classifier(std::string filename)
    : _filename(std::move(filename))
{
}

bool Classifier::initial_file_load() {
    if(!std::filesystem::exists(_filename)) {
        return false;
    }

    FILE* fp = fopen(_filename.c_str(), "r");
    char read_buffer[65536];
    rapidjson::FileReadStream is(fp, read_buffer, sizeof(read_buffer));
    fclose(fp);

    return !_doc.ParseStream(is).HasParseError();
}

void Classifier::load_file_values() {
    rapidjson::GenericObject superclasses_value = _doc["supercategories"].GetObject();

    for(rapidjson::Value::ConstMemberIterator itr = superclasses_value.MemberBegin(); itr != superclasses_value.MemberEnd(); ++itr) {
        rapidjson::GenericObject json_info = itr->value.GetObject();

        SuperCategory supercategory_info;
        supercategory_info.name = itr->name.GetString();
        supercategory_info.recyclable = json_info["recyclable"].GetBool();

        _super_categorymap.emplace(json_info["index"].GetInt(), supercategory_info);
    }

    rapidjson::GenericObject classifications_value = _doc["classifications"].GetObject();
    _recycle_infovector.clear();
    _recycle_infovector.reserve(classifications_value.MemberCount());

    for(rapidjson::Value::ConstMemberIterator itr = classifications_value.MemberBegin(); itr != classifications_value.MemberEnd(); ++itr) {
        RecycleInfo info;
        info.item = itr->name.GetString();

        rapidjson::GenericArray json_val = itr->value.GetArray();
        size_t arr_size = json_val.Size();
        if(arr_size) {
            info.superclass_id = json_val[0].GetInt();
            info.recycle_class = _super_categorymap.at(info.superclass_id);
            if(arr_size > 1) {
                info.special_instructions = json_val[1].GetString();
            }
        }
        _recycle_infovector.emplace_back(info);
    }
}

Classifier::RecycleInfo Classifier::classify(int index) const {
    return _recycle_infovector.at(index);
}

std::unordered_map<int, Classifier::SuperCategory> Classifier::get_supercategorymap() const {
    return _super_categorymap;
}
