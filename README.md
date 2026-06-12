# DataPersistence

직원(Employee) 데이터를 **JSON / CSV / SQLite** 3가지 저장 방식으로 관리하는 C++ 콘솔 애플리케이션입니다.  
Repository 패턴을 적용하여 저장소 간 전환이 자유롭고, 동일한 CRUD 인터페이스로 세 가지 저장 방식을 모두 지원합니다.

---

## 요구 사항

| 항목 | 버전 |
|------|------|
| Visual Studio | 2022 (v145 toolset) |
| C++ 표준 | C++20 |
| OS | Windows 10/11 (x64) |

> SQLite는 소스 코드(`third_party/sqlite/`)로 포함되어 있어 별도 설치가 필요 없습니다.

---

## 빌드 방법

1. `DataPersistence.sln`을 Visual Studio 2022에서 엽니다.
2. 구성을 **Debug | x64** 또는 **Release | x64**로 선택합니다.
3. **빌드 > 솔루션 빌드** (`Ctrl+Shift+B`)를 실행합니다.
4. 빌드 완료 후 실행 파일 위치:
   ```
   x64/Debug/DataPersistence.exe
   x64/Release/DataPersistence.exe
   ```

---

## 실행 방법

빌드 후 실행 파일을 직접 실행하거나, Visual Studio에서 `F5`(디버그) / `Ctrl+F5`(실행)로 시작합니다.

```
x64\Debug\DataPersistence.exe
```

---

## 사용법

### 1. 저장소 선택

프로그램 시작 시 사용할 저장소를 선택합니다.

```
╔══════════════════════════════════╗
║     DataPersistence PoC          ║
╚══════════════════════════════════╝
 저장소를 선택하세요.
  [1] JSON 파일
  [2] CSV 파일
  [3] SQLite DB
  [0] 종료
```

| 선택 | 저장소 | 데이터 파일 |
|------|--------|------------|
| 1 | JSON | `employees.json` |
| 2 | CSV | `employees.csv` |
| 3 | SQLite | `x64/Debug/employees.db` |

### 2. CRUD 메뉴

저장소 선택 후 아래 메뉴가 표시됩니다.

```
  [1] 전체 조회
  [2] ID 조회
  [3] 추가
  [4] 수정
  [5] 삭제
  [0] 저장소 변경
```

#### [1] 전체 조회
저장된 모든 직원 목록을 테이블 형식으로 출력합니다.

```
ID  | 이름       | 부서         | 급여
----+------------+--------------+------------
  1 | 홍길동     | 개발팀       | 5,000,000
  2 | 김영희     | 인사팀       | 4,200,000
```

#### [2] ID 조회
특정 직원 ID를 입력하면 해당 직원 정보를 조회합니다.

#### [3] 추가
이름, 부서, 급여를 입력하면 자동으로 ID가 부여되고 저장됩니다.

```
이름     : 홍길동
부서     : 개발팀
급여     : 5000000
```

#### [4] 수정
수정할 직원 ID를 입력한 뒤 변경할 항목을 입력합니다.  
입력값 없이 Enter를 누르면 기존 값이 유지됩니다.

#### [5] 삭제
삭제할 직원 ID를 입력하면 확인 후 삭제됩니다.

#### [0] 저장소 변경
현재 저장소 세션을 종료하고 저장소 선택 화면으로 돌아갑니다.

---

## 프로젝트 구조

```
DataPersistence/
├── src/
│   ├── main.cpp              # 진입점 및 저장소 선택 UI
│   ├── Employee.h            # 직원 데이터 구조체
│   ├── IRepository.h         # 저장소 추상 인터페이스
│   ├── ConsoleUI.h/cpp       # 콘솔 CRUD 인터페이스
│   ├── FileRepository.h/cpp  # CSV 저장소 구현
│   ├── JsonRepository.h/cpp  # JSON 저장소 구현
│   └── DbRepository.h/cpp    # SQLite 저장소 구현
├── third_party/sqlite/
│   ├── sqlite3.h
│   └── sqlite3.c
├── employees.json            # JSON 데이터 파일
├── employees.csv             # CSV 데이터 파일
├── DataPersistence.sln
└── DataPersistence.vcxproj
```

---

## 데이터 파일 형식

**employees.json**
```json
[
  {"id": 1, "name": "홍길동", "department": "개발팀", "salary": 5000000.00}
]
```

**employees.csv**
```
id,name,department,salary
1,홍길동,개발팀,5000000.000000
```

**employees.db** — SQLite 데이터베이스 (`x64/Debug/employees.db`)

---

## 설계 구조

```
main.cpp
  └── selectRepository()  ─── IRepository (인터페이스)
                               ├── JsonRepository  → employees.json
                               ├── FileRepository  → employees.csv
                               └── DbRepository    → employees.db

ConsoleUI ──────────────────── IRepository 주입받아 CRUD 수행
```

저장소를 교체해도 `ConsoleUI`와 `main.cpp` 코드는 변경 없이 동작합니다.

---

## 참고

- SQLite는 `ENABLE_SQLITE` 전처리기 매크로로 활성화됩니다. 프로젝트 파일에 기본 정의되어 있어 별도 설정 불필요합니다.
- 파일 저장소(JSON, CSV)는 저장 시 `.tmp` 임시 파일을 사용한 후 교체하는 방식으로 데이터 무결성을 보장합니다.
- SQLite 저장소는 Prepared Statement를 사용하여 SQL Injection을 방지합니다.
