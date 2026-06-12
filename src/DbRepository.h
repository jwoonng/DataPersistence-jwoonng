#pragma once
#include "IRepository.h"

// SQLite 활성화: 프로젝트 설정에서 ENABLE_SQLITE 전처리기 정의 후
//   third_party/sqlite/ 에 sqlite3.h, sqlite3.c 배치
#ifdef ENABLE_SQLITE
struct sqlite3;
#endif

class DbRepository : public IRepository {
public:
    explicit DbRepository(const std::string& path = "employees.db");
    ~DbRepository() override;

    bool                    create(Employee& emp)       override;
    std::optional<Employee> read(int id)                override;
    std::vector<Employee>   readAll()                   override;
    bool                    update(const Employee& emp) override;
    bool                    remove(int id)              override;
    std::string             storageType() const         override;

private:
    std::string m_path;
    bool        m_ready = false;

#ifdef ENABLE_SQLITE
    sqlite3* m_db = nullptr;
    void     initSchema();
#endif
};
