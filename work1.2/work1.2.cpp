// work1.1.cpp : Реализация класса SqlSelectQueryBuilder, 
// Расширьте класс из предыдущего задания методом

//SqlSelectQueryBuilder& AddWhere(const std::map<std::string, std::string>& kv) noexcept;
//С помощью которого можно будет добавить в запрос сразу несколько условий.
//
//Расширьте класс из предыдущего задания методом
//
//SqlSelectQueryBuilder& AddColumns(const std::vector<std::string>& columns) noexcept;
//С помощью этого метода можно добавить в запрос сразу несколько полей, по которым он будет строиться вместо одного, как в базовом варианте.

#include <iostream>
#include <string>
#include <vector>
#include <map>

class SqlSelectQueryBuilder {

public:
    SqlSelectQueryBuilder& AddColumn(std::string col)
    {
        if (select.empty())
        {
            select += "SELECT " + col;
        }
        else {
            select += ", " + col;
        }
        return *this;
    }

    SqlSelectQueryBuilder& AddColumns(const std::vector<std::string>& columns) noexcept
    {
        for (auto cols : columns) {
            if (select.empty())
            {
                select += "SELECT " + cols;
            }
            else {
                select += ", " + cols;
            }
        }
  
        return *this;
    }

    SqlSelectQueryBuilder& AddFrom(std::string table) {
        if (from.empty())
        {
            from += " FROM " + table;
        }
        else {
            std::cout << "Таблица уже выбрана!\n";
        }
        return *this;
    }
    SqlSelectQueryBuilder& AddWhere(std::string col, std::string condition) {
        if (where.empty())
        {
            where += " WHERE " + col + "=" + condition;
        }
        else {
            where += " AND " + col + "=" + condition;
        }
        return *this;
    }

    SqlSelectQueryBuilder& AddWhere(const std::map<std::string, std::string>& kv) noexcept
    {
        for (const auto k : kv) {
            if (where.empty())
            {
                where += " WHERE " + k.first + "=" + k.second;
            }
            else {
                where += " AND " + k.first + "=" + k.second;
            }
        }
  
        return *this;
    }

    std::string BuildQuery()
    {
        if (!query.empty()) {
            query.clear();
        }
        if (!select.empty()) {
            query = select + from + where + ";";
        }
        else {
            query = "SELECT *" + from + where + ";";
        }
        return query;
    }

private:
    std::string query;
    std::string select = "";
    std::string from = "";
    std::string where = "";
};

int main()
{
    setlocale(LC_ALL, "Russian");

    std::vector<std::string> cols = { "name", "phone" };
    std::map<std::string, std::string> where = {
        {"id","42"},
        {"name","John"}
    };

    SqlSelectQueryBuilder query_builder;

    query_builder.AddColumns(cols);
    query_builder.AddFrom("students");
    query_builder.AddWhere(where);
    const std::string result = query_builder.BuildQuery();
    const std::string example = "SELECT name, phone FROM students WHERE id=42 AND name=John;";
    if (result == example) {
        std::cout << "Корректный запрос\n";
    }
    else {
        std::cout << "Некорректный запрос\n";
        std::cout << result;
    }
}
