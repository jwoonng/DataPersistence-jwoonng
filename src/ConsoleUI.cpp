#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#define NOMINMAX
#include <windows.h>

// ── 콘솔 색상 상수 ────────────────────────────────────────────────────────────
namespace Color {
    constexpr int DEFAULT = 7;
    constexpr int CYAN    = 11;
    constexpr int YELLOW  = 14;
    constexpr int GREEN   = 10;
    constexpr int RED     = 12;
    constexpr int WHITE   = 15;
    constexpr int GRAY    = 8;
}

static HANDLE g_hCon = INVALID_HANDLE_VALUE;

static HANDLE hcon() {
    if (g_hCon == INVALID_HANDLE_VALUE)
        g_hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    return g_hCon;
}

void ConsoleUI::setColor(int c) {
    SetConsoleTextAttribute(hcon(), static_cast<WORD>(c));
}
void ConsoleUI::resetColor() {
    SetConsoleTextAttribute(hcon(), Color::DEFAULT);
}

// ── 테이블 출력 ───────────────────────────────────────────────────────────────

void ConsoleUI::printSeparator() {
    setColor(Color::CYAN);
    std::cout << "  +------+----------------------+--------------------+--------------+\n";
    resetColor();
}

void ConsoleUI::printTableHeader() {
    printSeparator();
    setColor(Color::YELLOW);
    std::cout << "  | "
              << std::left  << std::setw(5)  << "ID"
              << "| " << std::setw(21) << "이름"
              << "| " << std::setw(19) << "부서"
              << "| " << std::setw(13) << "급여 (원)"
              << "|\n";
    resetColor();
    printSeparator();
}

void ConsoleUI::printRow(const Employee& e) {
    setColor(Color::WHITE);
    std::cout << "  | "
              << std::left  << std::setw(5)  << e.id
              << "| " << std::setw(21) << e.name
              << "| " << std::setw(19) << e.department
              << "| " << std::right << std::setw(10)
              << std::fixed << std::setprecision(0) << e.salary << "   |\n";
    resetColor();
}

void ConsoleUI::printTitle(const std::string& title) {
    setColor(Color::CYAN);
    std::cout << "\n  +--------------------------------------------------+\n";
    setColor(Color::YELLOW);
    std::cout << "  |  " << std::left << std::setw(48) << title << "|\n";
    setColor(Color::CYAN);
    std::cout << "  +--------------------------------------------------+\n\n";
    resetColor();
}

void ConsoleUI::waitKey() {
    setColor(Color::GRAY);
    std::cout << "\n  [Enter] 계속...";
    resetColor();
    std::cin.get();
}

// ── 직원 입력 헬퍼 ────────────────────────────────────────────────────────────

Employee ConsoleUI::inputEmployee(bool withId) {
    Employee e;
    if (withId) {
        std::cout << "  수정할 ID  : ";
        std::cin >> e.id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "  이름       : ";
    std::getline(std::cin, e.name);
    std::cout << "  부서       : ";
    std::getline(std::cin, e.department);
    std::cout << "  급여 (원)  : ";
    std::cin >> e.salary;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return e;
}

// ── CRUD 화면 ─────────────────────────────────────────────────────────────────

ConsoleUI::ConsoleUI(std::unique_ptr<IRepository> repo) : m_repo(std::move(repo)) {}

void ConsoleUI::run() {
    while (true) {
        system("cls");

        setColor(Color::CYAN);
        std::cout << "\n"
                  << "  +================================================+\n"
                  << "  |        Data Persistence PoC                    |\n";
        setColor(Color::YELLOW);
        std::cout << "  |  저장소: "
                  << std::left << std::setw(38) << m_repo->storageType() << "|\n";
        setColor(Color::CYAN);
        std::cout << "  +================================================+\n";
        setColor(Color::WHITE);
        std::cout << "  |  [1] 전체 조회  (Read All)                     |\n"
                  << "  |  [2] ID 조회    (Read by ID)                   |\n"
                  << "  |  [3] 추가       (Create)                       |\n"
                  << "  |  [4] 수정       (Update)                       |\n"
                  << "  |  [5] 삭제       (Delete)                       |\n";
        setColor(Color::GRAY);
        std::cout << "  |  [0] 저장소 변경 / 종료                        |\n";
        setColor(Color::CYAN);
        std::cout << "  +================================================+\n\n";
        resetColor();

        std::cout << "  선택: ";
        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: doReadAll(); break;
        case 2: doRead();    break;
        case 3: doCreate();  break;
        case 4: doUpdate();  break;
        case 5: doDelete();  break;
        case 0: return;
        default:
            setColor(Color::RED);
            std::cout << "\n  잘못된 선택입니다.\n";
            resetColor();
            waitKey();
            break;
        }
    }
}

void ConsoleUI::doReadAll() {
    system("cls");
    printTitle("전체 직원 조회  (Read All)");
    const auto records = m_repo->readAll();
    if (records.empty()) {
        setColor(Color::GRAY);
        std::cout << "  (저장된 데이터가 없습니다)\n";
        resetColor();
    } else {
        printTableHeader();
        for (const auto& e : records) printRow(e);
        printSeparator();
        setColor(Color::GRAY);
        std::cout << "  총 " << records.size() << "건\n";
        resetColor();
    }
    waitKey();
}

void ConsoleUI::doRead() {
    system("cls");
    printTitle("ID로 직원 조회  (Read by ID)");
    std::cout << "  조회할 ID: ";
    int id = 0;
    if (!(std::cin >> id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const auto result = m_repo->read(id);
    if (result) {
        printTableHeader();
        printRow(*result);
        printSeparator();
    } else {
        setColor(Color::RED);
        std::cout << "  ID " << id << " 에 해당하는 직원을 찾을 수 없습니다.\n";
        resetColor();
    }
    waitKey();
}

void ConsoleUI::doCreate() {
    system("cls");
    printTitle("직원 추가  (Create)");
    Employee e = inputEmployee(false);
    if (m_repo->create(e)) {
        setColor(Color::GREEN);
        std::cout << "\n  추가 완료.  새 ID = " << e.id << "\n";
        resetColor();
    } else {
        setColor(Color::RED);
        std::cout << "\n  추가 실패.\n";
        resetColor();
    }
    waitKey();
}

void ConsoleUI::doUpdate() {
    system("cls");
    printTitle("직원 수정  (Update)");

    std::cout << "  수정할 ID  : ";
    int id = 0;
    if (!(std::cin >> id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        waitKey();
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const auto existing = m_repo->read(id);
    if (!existing) {
        setColor(Color::RED);
        std::cout << "\n  ID " << id << " 에 해당하는 직원이 없습니다.\n";
        resetColor();
        waitKey();
        return;
    }

    setColor(Color::GRAY);
    std::cout << "\n  (빈 칸으로 엔터 시 기존 값 유지)\n\n";
    resetColor();

    Employee e = *existing;
    std::string input;

    std::cout << "  이름       [현재: " << e.name << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) e.name = input;

    std::cout << "  부서       [현재: " << e.department << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) e.department = input;

    std::cout << "  급여 (원)  [현재: "
              << std::fixed << std::setprecision(0) << e.salary << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { e.salary = std::stod(input); } catch (...) {}
    }

    if (m_repo->update(e)) {
        setColor(Color::GREEN);
        std::cout << "\n  수정 완료.\n";
        resetColor();
    } else {
        setColor(Color::RED);
        std::cout << "\n  수정 실패.  (ID " << e.id << " 없음)\n";
        resetColor();
    }
    waitKey();
}

void ConsoleUI::doDelete() {
    system("cls");
    printTitle("직원 삭제  (Delete)");
    std::cout << "  삭제할 ID: ";
    int id = 0;
    if (!(std::cin >> id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 해당 직원 먼저 출력
    const auto target = m_repo->read(id);
    if (!target) {
        setColor(Color::RED);
        std::cout << "\n  ID " << id << " 에 해당하는 직원이 없습니다.\n";
        resetColor();
        waitKey();
        return;
    }
    printTableHeader();
    printRow(*target);
    printSeparator();

    setColor(Color::YELLOW);
    std::cout << "\n  위 직원을 삭제하시겠습니까? (y/n): ";
    resetColor();
    char confirm = 'n';
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (confirm == 'y' || confirm == 'Y') {
        if (m_repo->remove(id)) {
            setColor(Color::GREEN);
            std::cout << "\n  삭제 완료.\n";
        } else {
            setColor(Color::RED);
            std::cout << "\n  삭제 실패.\n";
        }
    } else {
        setColor(Color::GRAY);
        std::cout << "\n  삭제 취소.\n";
    }
    resetColor();
    waitKey();
}
