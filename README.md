# VisualSQL

**VisualSQL** to autorskie narzędzie desktopowe umożliwiające wizualne projektowanie relacyjnych baz danych oraz automatyczne generowanie gotowego kodu SQL.

Aplikacja rozwiązuje problem żmudnego pisania zapytań DDL (Data Definition Language) poprzez intuicyjny interfejs typu "Drag & Drop".

## Główne funkcjonalności

Projekt realizuje pełen cykl projektowania schematu bazy danych:

*   **Interaktywne płótno węzłów (Canvas):** Swobodne tworzenie, przesuwanie i usuwanie tabel na nieskończonej siatce z obsługą nawigacji i przybliżania (Zoom to Fit).
*   **Wizualizacja relacji:** Łączenie tabel strzałkami reprezentującymi klucze obce (Foreign Keys).
*   **Edytor właściwości:** Szczegółowe definiowanie nazw kolumn, typów danych (np. INT, VARCHAR, DATE) oraz zaawansowanych atrybutów takich jak Primary Key, Auto Increment, Unique czy Not Null.
*   **Wielosilnikowy Generator SQL:** Tłumaczenie narysowanego diagramu na poprawny składniowo skrypt SQL z obsługą dialektów **MySQL** oraz **PostgreSQL**.
*   **Serializacja projektu:** Możliwość zapisu bieżącego stanu pracy do pliku `.json` i jego późniejszego odtworzenia.

### Moduł ,,Magiczna Różdżka" (Auto-Linker)
Autorski algorytm oparty na heurystyce tekstowej. Narzędzie automatycznie analizuje nazwy tabel oraz kolumn, a następnie samo rysuje odpowiednie relacje pomiędzy kluczami obcymi i głównymi na podstawie przyjętej konwencji nazewniczej.

## Stos technologiczny

Aplikacja została napisana w nowoczesnym standardzie **C++17** z wykorzystaniem paradygmatu programowania obiektowego (OOP).

*   **Interfejs graficzny:** [Dear ImGui](https://github.com/ocornut/imgui) – ultra-szybki framework typu Immediate Mode GUI.
*   **System okien:** **GLFW** + **OpenGL3**.
*   **Zarządzanie grafami:** [imgui-node-editor](https://github.com/thedmd/imgui-node-editor).
*   **Zapis i odczyt (JSON):** [nlohmann/json](https://github.com/nlohmann/json).
*   **System budowania:** CMake.

## Kompilacja i uruchomienie

Do zbudowania projektu wymagane jest środowisko wspierające `CMake` (np. CLion, Visual Studio) oraz kompilator C++17. Zależności (takie jak GLFW czy nlohmann/json) są automatycznie pobierane przez narzędzie `FetchContent` wbudowane w CMake.

1. Sklonuj repozytorium:
```bash
   git clone [https://github.com/WiktoriaDomanska/VisualSQL.git](https://github.com/WiktoriaDomanska/VisualSQL.git)
```
2. Otwórz folder w swoim środowisku IDE (np. CLion).
3. Przeładuj projekt CMake.
4. Skompiluj i uruchom plik wykonywalny VisualSQL.

## Krótka instrukcja obsługi
Wbudowany w aplikację system pomocy znajdziesz w górnym menu: Help -> Instrukcja obsługi.
Aplikacja została zaprojektowana tak, aby wybaczać błędy - system relacji blokuje tworzenie cykli czy łączenie niezgodnych typów danych, a wbudowana historia akcji pozwala cofnąć zmiany.