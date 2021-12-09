#include "databasehandler.h"

#include <string>
#include <iostream>
#include <pqxx/pqxx>

DatabaseHandler::DatabaseHandler(const std::string& conn_string) {
    _conn = std::make_unique<pqxx::connection>(conn_string);

    pqxx::nontransaction txn{*_conn};
    txn.exec0("SET search_path TO recm8;");

    _prepared_statements = {
        { Prepareds::INSERT_IMAGE,          { "Insert_Image",
                                              "INSERT INTO image(image, msec) VALUES($1, $2) RETURNING img_id;" } },
        { Prepareds::INSERT_DETECTION,      { "Insert_Detection",
                                              "INSERT INTO detection(img_id, obj_name, score, x1, y1, x2, y2) VALUES ($1, $2, $3, $4, $5, $6, $7);" } },
        { Prepareds::INSERT_CATEGORY,       { "Insert_Category",
                                              "INSERT INTO category(sup_id, obj_name, special_instruct) VALUES ($1, $2, $3);" } },
        { Prepareds::INSERT_SUPER_CATEGORY, { "Insert_Super_Category",
                                              "INSERT INTO super_category(sup_name, is_recyclable) VALUES ($1, $2);" } },
    };

    _conn_mtx.lock();
    for (const auto& itr : _prepared_statements) {
        const Prep_Stm& current_stm = itr.second;
        _conn->prepare(current_stm.name, current_stm.statement);
    }
    _conn_mtx.unlock();
}

void DatabaseHandler::delete_category_table_data()  {
    _conn_mtx.lock();
    pqxx::work txn{*_conn};
    txn.exec0("DELETE FROM category;");
    txn.exec0("DELETE FROM super_category;");
    txn.commit();
    _conn_mtx.unlock();
}

uint DatabaseHandler::insert_image(unsigned char* image, double elapsed_sec) {
    const std::string& stm = _prepared_statements[Prepareds::INSERT_IMAGE].name;

    pqxx::binarystring bytea_image(image, sizeof(image));

    _conn_mtx.lock();
    pqxx::work txn{*_conn};
    pqxx::result results{ txn.exec_prepared(stm, bytea_image, elapsed_sec) };
    txn.commit();
    _conn_mtx.unlock();

    return results[0][0].as<uint>();
}

void DatabaseHandler::insert_detection(int img_id, const std::string& name, float score, double x1, double y1, double x2, double y2) {
    const std::string& stm = _prepared_statements[Prepareds::INSERT_DETECTION].name;

    _conn_mtx.lock();
    pqxx::work txn{*_conn};
    txn.exec_prepared(stm, img_id, name, score, x1, y1, x2, y2);
    txn.commit();
    _conn_mtx.unlock();
}

void DatabaseHandler::insert_category(int sup_id, const std::string& name, const std::string& special_instruct) {
    const std::string& stm = _prepared_statements[Prepareds::INSERT_CATEGORY].name;

    _conn_mtx.lock();
    pqxx::work txn{*_conn};
    txn.exec_prepared(stm, sup_id, name, special_instruct);
    txn.commit();
    _conn_mtx.unlock();
}

void DatabaseHandler::insert_super_category(const std::string& name, bool is_recyclable) {
    const std::string& stm = _prepared_statements[Prepareds::INSERT_SUPER_CATEGORY].name;

    _conn_mtx.lock();
    pqxx::work txn{*_conn};
    txn.exec_prepared(stm, name, is_recyclable);
    txn.commit();
    _conn_mtx.unlock();
}
