#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <pqxx/pqxx>

class DatabaseHandler
{
public:
    DatabaseHandler(const std::string& conn_string);

    void delete_category_table_data();
    uint insert_image(unsigned char* image, double elapsed_sec);
    void insert_detection(int img_id, const std::string& name, float score, double x1, double y1, double x2, double y2);
    void insert_category(int sup_id, const std::string& name, const std::string& special_instruct);
    void insert_super_category(const std::string& name, bool is_recyclable);

private:
    std::unique_ptr<pqxx::connection> _conn;

    enum class Prepareds {
        INSERT_IMAGE,
        INSERT_DETECTION,
        INSERT_CATEGORY,
        INSERT_SUPER_CATEGORY
    };
    struct Prep_Stm {
        std::string name;
        std::string statement;
    };

    std::unordered_map<Prepareds, Prep_Stm> _prepared_statements;

    std::mutex _conn_mtx;
};

#endif // DATABASEHANDLER_H
