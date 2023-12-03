// work1.1.cpp : Реализация класса SqlSelectQueryBuilder, 
// который можно использовать для построения простых SELECT-запросов.

#include <iostream>
#include <string>

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

    SqlSelectQueryBuilder& AddFrom(std::string table) {
        if (from.empty())
        {
            from += " FROM " + table;
        }
        else {
            std::cout<<"Таблица уже выбрана!\n";
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

    std::string BuildQuery() {
        if (!query.empty()) {
            query.clear();
        }
        query = select + from + where + ";";
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

    SqlSelectQueryBuilder query_builder;

    query_builder.AddColumn("name").AddColumn("phone");
    query_builder.AddFrom("students");
    query_builder.AddWhere("id", "42").AddWhere("name", "John");
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
