#include "DbRepository.h"
#include <iostream>

#ifdef ENABLE_SQLITE
#include "sqlite3.h"
#endif

// ── 생성자 / 소멸자 ───────────────────────────────────────────────────────────

DbRepository::DbRepository(const std::string& path) : m_path(path) {
#ifdef ENABLE_SQLITE
    if (sqlite3_open(m_path.c_str(), &m_db) == SQLITE_OK) {
        initSchema();
        m_ready = true;
    } else {
        std::cerr << "[DB] 파일 열기 실패: " << m_path << "\n";
    }
#else
    std::cout << "\n  [DB] SQLite 미구성 상태입니다.\n"
              << "  download_sqlite.ps1 실행 후 ENABLE_SQLITE 를 전처리기에 추가하세요.\n";
#endif
}

DbRepository::~DbRepository() {
#ifdef ENABLE_SQLITE
    if (m_db) sqlite3_close(m_db);
#endif
}

std::string DbRepository::storageType() const {
#ifdef ENABLE_SQLITE
    return "SQLite DB  [" + m_path + "]";
#else
    return "SQLite DB  [미구성 — ENABLE_SQLITE 필요]";
#endif
}

// ── SQLite 활성화 시 구현 ─────────────────────────────────────────────────────

#ifdef ENABLE_SQLITE

void DbRepository::initSchema() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS employees ("
        "  id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name       TEXT    NOT NULL,"
        "  department TEXT    NOT NULL,"
        "  salary     REAL    NOT NULL"
        ");";
    sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr);
}

bool DbRepository::create(Employee& emp) {
    if (!m_ready) return false;
    const char* sql =
        "INSERT INTO employees (name, department, salary) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt,   1, emp.name.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   2, emp.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, emp.salary);

    const bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (ok) emp.id = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    sqlite3_finalize(stmt);
    return ok;
}

std::optional<Employee> DbRepository::read(int id) {
    if (!m_ready) return std::nullopt;
    const char* sql =
        "SELECT id, name, department, salary FROM employees WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_int(stmt, 1, id);
    std::optional<Employee> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Employee e;
        e.id         = sqlite3_column_int(stmt, 0);
        e.name       = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        e.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        e.salary     = sqlite3_column_double(stmt, 3);
        result = e;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Employee> DbRepository::readAll() {
    std::vector<Employee> result;
    if (!m_ready) return result;
    const char* sql =
        "SELECT id, name, department, salary FROM employees ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Employee e;
        e.id         = sqlite3_column_int(stmt, 0);
        e.name       = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        e.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        e.salary     = sqlite3_column_double(stmt, 3);
        result.push_back(e);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool DbRepository::update(const Employee& emp) {
    if (!m_ready) return false;
    const char* sql =
        "UPDATE employees SET name=?, department=?, salary=? WHERE id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt,   1, emp.name.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   2, emp.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, emp.salary);
    sqlite3_bind_int(stmt,    4, emp.id);

    const bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok && (sqlite3_changes(m_db) > 0);
}

bool DbRepository::remove(int id) {
    if (!m_ready) return false;
    const char* sql = "DELETE FROM employees WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, id);
    const bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok && (sqlite3_changes(m_db) > 0);
}

// ── SQLite 미구성 시 스텁 ─────────────────────────────────────────────────────

#else

bool                    DbRepository::create(Employee&)            { return false; }
std::optional<Employee> DbRepository::read(int)                    { return std::nullopt; }
std::vector<Employee>   DbRepository::readAll()                    { return {}; }
bool                    DbRepository::update(const Employee&)      { return false; }
bool                    DbRepository::remove(int)                  { return false; }

#endif
