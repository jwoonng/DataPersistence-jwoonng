#include "JsonRepository.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

JsonRepository::JsonRepository(const std::string& path) : m_path(path) {}

std::string JsonRepository::storageType() const {
    return "JSON File [" + m_path + "]";
}

// ── minimal JSON escape/unescape ──────────────────────────────────────────────

std::string JsonRepository::escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
        case '"':  out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        default:   out += c;      break;
        }
    }
    return out;
}

// ── minimal JSON parser targeting [ { "id":N, "name":"...", ... }, ... ] ─────

std::vector<Employee> JsonRepository::parseJson(const std::string& json) {
    std::vector<Employee> result;

    // Helper: extract string field value  "key": "value"  (optional space after colon)
    auto strField = [&](const std::string& obj, const std::string& key) -> std::string {
        const std::string search = "\"" + key + "\":";
        size_t p = obj.find(search);
        if (p == std::string::npos) return "";
        p += search.size();
        while (p < obj.size() && obj[p] == ' ') ++p;
        if (p >= obj.size() || obj[p] != '"') return "";
        ++p;
        std::string val;
        for (; p < obj.size(); ++p) {
            if (obj[p] == '\\' && p + 1 < obj.size()) {
                ++p;
                switch (obj[p]) {
                case '"':  val += '"';  break;
                case '\\': val += '\\'; break;
                case 'n':  val += '\n'; break;
                case 'r':  val += '\r'; break;
                case 't':  val += '\t'; break;
                default:   val += obj[p]; break;
                }
            } else if (obj[p] == '"') {
                break;
            } else {
                val += obj[p];
            }
        }
        return val;
    };

    // Helper: extract numeric field value  "key": N
    auto numField = [&](const std::string& obj, const std::string& key) -> std::string {
        const std::string search = "\"" + key + "\":";
        size_t p = obj.find(search);
        if (p == std::string::npos) return "0";
        p += search.size();
        while (p < obj.size() && obj[p] == ' ') ++p;
        const size_t q = obj.find_first_of(",}", p);
        std::string val = (q == std::string::npos) ? obj.substr(p) : obj.substr(p, q - p);
        const size_t last = val.find_last_not_of(" \t\r\n");
        return (last == std::string::npos) ? "0" : val.substr(0, last + 1);
    };

    size_t pos = 0;
    while (pos < json.size()) {
        const size_t start = json.find('{', pos);
        if (start == std::string::npos) break;
        const size_t end = json.find('}', start);
        if (end == std::string::npos) break;

        const std::string obj = json.substr(start + 1, end - start - 1);
        try {
            Employee emp;
            emp.id         = std::stoi(numField(obj, "id"));
            emp.name       = strField(obj, "name");
            emp.department = strField(obj, "department");
            emp.salary     = std::stod(numField(obj, "salary"));
            if (emp.id > 0) result.push_back(emp);
        } catch (...) {}

        pos = end + 1;
    }
    return result;
}

std::string JsonRepository::serializeJson(const std::vector<Employee>& records) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "[\n";
    for (size_t i = 0; i < records.size(); ++i) {
        const auto& e = records[i];
        ss << "  {\n"
           << "    \"id\": "          << e.id                         << ",\n"
           << "    \"name\": \""      << escapeJson(e.name)           << "\",\n"
           << "    \"department\": \"" << escapeJson(e.department)   << "\",\n"
           << "    \"salary\": "      << e.salary                     << "\n"
           << "  }";
        if (i + 1 < records.size()) ss << ",";
        ss << "\n";
    }
    ss << "]\n";
    return ss.str();
}

// ── persistence helpers ───────────────────────────────────────────────────────

std::vector<Employee> JsonRepository::load() const {
    std::ifstream file(m_path);
    if (!file.is_open()) return {};
    std::ostringstream ss;
    ss << file.rdbuf();
    return parseJson(ss.str());
}

void JsonRepository::save(const std::vector<Employee>& records) const {
    std::ofstream file(m_path, std::ios::trunc);
    file << serializeJson(records);
}

int JsonRepository::generateId(const std::vector<Employee>& records) const {
    if (records.empty()) return 1;
    return std::max_element(records.begin(), records.end(),
        [](const Employee& a, const Employee& b) { return a.id < b.id; })->id + 1;
}

// ── CRUD ──────────────────────────────────────────────────────────────────────

bool JsonRepository::create(Employee& emp) {
    auto records = load();
    emp.id = generateId(records);
    records.push_back(emp);
    save(records);
    return true;
}

std::optional<Employee> JsonRepository::read(int id) {
    for (const auto& e : load())
        if (e.id == id) return e;
    return std::nullopt;
}

std::vector<Employee> JsonRepository::readAll() {
    return load();
}

bool JsonRepository::update(const Employee& emp) {
    auto records = load();
    for (auto& e : records) {
        if (e.id == emp.id) {
            e = emp;
            save(records);
            return true;
        }
    }
    return false;
}

bool JsonRepository::remove(int id) {
    auto records = load();
    const auto before = records.size();
    records.erase(
        std::remove_if(records.begin(), records.end(),
            [id](const Employee& e) { return e.id == id; }),
        records.end());
    if (records.size() == before) return false;
    save(records);
    return true;
}
