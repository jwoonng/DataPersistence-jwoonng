#pragma once
#include "IRepository.h"

class FileRepository : public IRepository {
public:
    explicit FileRepository(const std::string& path = "employees.csv");

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

    static Employee       parseLine(const std::string& line);
    static std::string    formatLine(const Employee& e);
};
