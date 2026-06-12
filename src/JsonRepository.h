#pragma once
#include "IRepository.h"

class JsonRepository : public IRepository {
public:
    explicit JsonRepository(const std::string& path = "employees.json");

    bool                    create(Employee& emp)       override;
    std::optional<Employee> read(int id)                override;
    std::vector<Employee>   readAll()                   override;
    bool                    update(const Employee& emp) override;
    bool                    remove(int id)              override;
    std::string             storageType() const         override;

private:
    std::string           m_path;

    std::vector<Employee> load() const;
    void                  save(const std::vector<Employee>& records) const;
    int                   generateId(const std::vector<Employee>& records) const;

    static std::vector<Employee> parseJson(const std::string& json);
    static std::string           serializeJson(const std::vector<Employee>& records);
    static std::string           escapeJson(const std::string& s);
};
