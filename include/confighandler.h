#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H
#include <string>

#include <rapidjson/document.h>

class ConfigHandler {
public:
    ConfigHandler(std::string filename);

    struct Settings {
        int source_type;
        int source_index;
        std::string source_str;

        bool skip_start;
        std::string dashboard_link;

        bool db_enabled;
        std::string db_conn_str;
    } settings;

    bool initial_file_load();
    void populate_with_defaults();
    bool save_file(bool create_new = false) const;
    void load_file_values();

private:
    std::string _filename;
    rapidjson::Document _doc;
};

#endif // CONFIGHANDLER_H
