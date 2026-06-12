#pragma once
#include <memory>
#include "IRepository.h"

class ConsoleUI {
public:
    explicit ConsoleUI(std::unique_ptr<IRepository> repo);
    void run();

private:
    std::unique_ptr<IRepository> m_repo;

    void doReadAll();
    void doRead();
    void doCreate();
    void doUpdate();
    void doDelete();

    static void        printTableHeader();
    static void        printRow(const Employee& e);
    static void        printSeparator();
    static void        printTitle(const std::string& title);
    static void        setColor(int color);
    static void        resetColor();
    static void        waitKey();
    static Employee    inputEmployee(bool withId);
};
