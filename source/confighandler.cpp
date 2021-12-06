#include "confighandler.h"

#include <filesystem>
#include <fstream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

ConfigHandler::ConfigHandler(std::string filename)
    : _filename(std::move(filename))
{
}

bool ConfigHandler::initial_file_load() {
    if(!std::filesystem::exists(_filename)) {
        return false;
    }

    FILE* fp = fopen(_filename.c_str(), "r");
    char read_buffer[65536];
    rapidjson::FileReadStream is(fp, read_buffer, sizeof(read_buffer));
    fclose(fp);

    if(_doc.ParseStream(is).HasParseError()) {
        return false;
    }

    // check int as uint to check that the values are not negative
    bool cv_flag_valid = _doc["source_type"].IsUint() && _doc["source_index"].IsUint() && _doc["source_str"].IsString();
    bool app_flag_valid = _doc["skip_start"].IsBool() && _doc["dashboard_link"].GetString();
    bool db_flag_valid = _doc["db_enabled"].IsBool() && _doc["db_conn_str"].IsString();

    return cv_flag_valid && app_flag_valid && db_flag_valid;
}

void ConfigHandler::load_file_values() {
    settings = Settings{
        _doc["source_type"].GetInt(),
        _doc["source_index"].GetInt(),
        _doc["source_str"].GetString(),

        _doc["skip_start"].GetBool(),
        _doc["dashboard_link"].GetString(),

        _doc["db_enabled"].GetBool(),
        _doc["db_conn_str"].GetString()
    };
}

void ConfigHandler::populate_with_defaults() {
    settings = Settings {
        0, 0, "",
        false, "",
        false, "host=172.28.0.62 user=usr_recm8 password=mysecretpassword dbname=db_recm8"
    };
}

bool ConfigHandler::save_file(bool create_new) const {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("source_type");
    writer.Int(settings.source_type);
    writer.Key("source_index");
    writer.Int(settings.source_index);
    writer.Key("source_str");
    writer.String(settings.source_str.c_str());

    writer.Key("skip_start");
    writer.Bool(settings.skip_start);
    writer.Key("dashboard_link");
    writer.String(settings.dashboard_link.c_str());

    writer.Key("db_enabled");
    writer.Bool(settings.db_enabled);
    writer.Key("db_conn_str");
    writer.String(settings.db_conn_str.c_str());
    writer.EndObject();

    if(!create_new) {
        const std::string backup_name = _filename + "_backup";
        std::filesystem::copy_file(_filename, backup_name,
                                    std::filesystem::copy_options::overwrite_existing);
    }

    std::ofstream file(_filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file.");
        return false;
    }

    file << buffer.GetString();
    file.close();

    return true;
}

