#pragma once
#include <vector>
#include <optional>
#include <string>
#include "Employee.h"

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool                    create(Employee& emp)       = 0;
    virtual std::optional<Employee> read(int id)                = 0;
    virtual std::vector<Employee>   readAll()                   = 0;
    virtual bool                    update(const Employee& emp) = 0;
    virtual bool                    remove(int id)              = 0;
    virtual std::string             storageType() const         = 0;
};
