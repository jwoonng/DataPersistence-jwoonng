#include "FileRepository.h"
#include <fstream>
#include <sstream>
#include <algorithm>

FileRepository::FileRepository(const std::string& path) : m_path(path) {}

std::string FileRepository::storageType() const {
    return "CSV File  [" + m_path + "]";
}

Employee FileRepository::parseLine(const std::string& line) {
    Employee e;
    std::istringstream ss(line);
    std::string token;
    try {
        if (std::getline(ss, token, ',')) e.id         = std::stoi(token);
        if (std::getline(ss, token, ',')) e.name       = token;
        if (std::getline(ss, token, ',')) e.department = token;
        if (std::getline(ss, token, ',')) e.salary     = std::stod(token);
    } catch (...) {}
    return e;
}

std::string FileRepository::formatLine(const Employee& e) {
    return std::to_string(e.id) + ","
         + e.name               + ","
         + e.department         + ","
         + std::to_string(e.salary);
}

std::vector<Employee> FileRepository::load() const {
    std::vector<Employee> records;
    std::ifstream file(m_path);
    if (!file.is_open()) return records;

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (!line.empty())
            records.push_back(parseLine(line));
    }
    return records;
}

void FileRepository::save(const std::vector<Employee>& records) const {
    const std::string tmp = m_path + ".tmp";
    {
        std::ofstream file(tmp, std::ios::trunc);
        if (!file.is_open()) return;
        file << "id,name,department,salary\n";
        for (const auto& e : records)
            file << formatLine(e) << "\n";
    }
    std::remove(m_path.c_str());
    std::rename(tmp.c_str(), m_path.c_str());
}

int FileRepository::generateId(const std::vector<Employee>& records) const {
    if (records.empty()) return 1;
    return std::max_element(records.begin(), records.end(),
        [](const Employee& a, const Employee& b) { return a.id < b.id; })->id + 1;
}

bool FileRepository::create(Employee& emp) {
    auto records = load();
    emp.id = generateId(records);
    records.push_back(emp);
    save(records);
    return true;
}

std::optional<Employee> FileRepository::read(int id) {
    for (const auto& e : load())
        if (e.id == id) return e;
    return std::nullopt;
}

std::vector<Employee> FileRepository::readAll() {
    return load();
}

bool FileRepository::update(const Employee& emp) {
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

bool FileRepository::remove(int id) {
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
