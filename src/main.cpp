#include <iostream>
#include <iomanip>
#include <memory>
#include <limits>
#define NOMINMAX
#include <windows.h>

#include "ConsoleUI.h"
#include "FileRepository.h"
#include "JsonRepository.h"
#include "DbRepository.h"

namespace Color {
    constexpr int DEFAULT = 7;
    constexpr int CYAN    = 11;
    constexpr int YELLOW  = 14;
    constexpr int GREEN   = 10;
    constexpr int WHITE   = 15;
    constexpr int GRAY    = 8;
}

static void setColor(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(c));
}
static void resetColor() { setColor(Color::DEFAULT); }

static std::unique_ptr<IRepository> selectRepository() {
    while (true) {
        system("cls");

        setColor(Color::CYAN);
        std::cout << "\n"
                  << "  +================================================+\n"
                  << "  |        Data Persistence PoC                    |\n"
                  << "  |        저장 방식을 선택하세요                  |\n"
                  << "  +================================================+\n";
        setColor(Color::WHITE);
        std::cout << "  |  [1] JSON 저장소 조회                          |\n"
                  << "  |  [2] CSV 저장소 조회                           |\n"
                  << "  |  [3] SQLite 저장소 조회                        |\n";
        setColor(Color::GRAY);
        std::cout << "  |  [0] 종료                                      |\n";
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
        case 1: return std::make_unique<JsonRepository>();
        case 2: return std::make_unique<FileRepository>();
        case 3: return std::make_unique<DbRepository>();
        case 0: return nullptr;
        default:
            setColor(Color::GRAY);
            std::cout << "\n  잘못된 선택입니다. 다시 시도하세요.\n";
            resetColor();
            Sleep(1000);
            break;
        }
    }
}

int main() {
    // 콘솔 UTF-8 출력 설정 (한글 포함)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    while (true) {
        auto repo = selectRepository();
        if (!repo) break;               // [0] 종료

        ConsoleUI ui(std::move(repo));
        ui.run();                       // [0] 누르면 저장소 선택 화면으로 복귀
    }

    system("cls");
    setColor(Color::CYAN);
    std::cout << "\n  프로그램을 종료합니다.\n\n";
    resetColor();
    return 0;
}
