#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Table.hpp"
#include "Column.hpp"
#include <imgui_node_editor.h>
#include <string>
#include "Schema.hpp"
#include <algorithm>
#include "SqlGenerator.hpp"
#include "ProjectSerializer.hpp"
#include "GLFW/glfw3native.h"
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <map>
#include "SqlTypes.hpp"
namespace ed = ax::NodeEditor;

// Otwarcie okienka Windows do zapisywania pliku
std::string openSaveFileDialog() {
    char filename[MAX_PATH] = {0};
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Pliki JSON (*.json)\0*.json\0Wszystkie pliki (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "json";

    if (GetSaveFileNameA(&ofn)) return std::string(filename);
    return "";
}

// Otwarcie okienka Windows do wczytania pliku
std::string openLoadFileDialog() {
    char filename[MAX_PATH] = {0};
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Pliki JSON (*.json)\0*.json\0Wszystkie pliki (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "json";

    if (GetOpenFileNameA(&ofn)) return std::string(filename);
    return "";
}

// Struktura przechowująca ,,zdjęcie" całego stanu aplikacji w danym momenie. Służy do funkcji Cofnij
struct AppStateSnapshot {
    Schema schema;
    std::vector<Link> links;
    int tableCounter;
    int nextTableId;
    int nextLinkId;
};


int main() {
    // Inicjalizacja GLFW (biblioteka tworząca okno w systemie OS)
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "VisualSQL - Start", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync

    // Inicjalizacja ImGui (silnik interfejsu graficznego)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 1.3f;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Inicjalizacja Node Editor (rozszerzenie do rysowania płótna i węzłów)
    ed::Config config;
    config.SettingsFile = "VisualSQL_NodeEditor.json";
    ed::EditorContext* m_Context = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Context);

    Schema mySchema("Mój projekt VisualSQL"); // Główny obiekt przechowujący wszystkie tabele
    int tableCounter = 1; // Licznik do tworzenia domyślnych nazw tabel (np. Tabela_1, Tabela_2 itd.)
    int nextTableId = 1; // Unikalne ID tabeli
    ed::NodeId selectedNodeId = 0; // Przechowuje ID klikniętej przez użytkownika tabeli
    int newTableIdToPlace = 0; // Zmienna pomocnicza wymuszająca pozycję nowej tabeli na środku

    std::vector<Link> wszystkieLinki; // Wektor przechowujący wszystkie narysowane strzałki (relacje)
    int nextLinkId = 1; // Licznik dla unikalnych id linków

    std::vector<AppStateSnapshot> undoStack; // Stos ,,zdjęć" aplikacji do funkcji Cofnij

    // Słownik grupujący typy danych po nazwie kategorii
    std::map<std::string, std::vector<SqlTypeDefinition>> availableSqlTypes;

    std::ifstream typesFile("sql_types.json");
    if (typesFile.is_open()) {
        nlohmann::json j;
        typesFile >> j;

        if (j.contains("types") && j["types"].is_array()) {
            for (const auto& item : j["types"]) {
                std::string cat = item["category"].get<std::string>();

                availableSqlTypes[cat].push_back({
                item["name"].get<std::string>(),
                    cat,
                    item["hasLength"].get<bool>()
                });
            }
        }
        typesFile.close();
    } else {
        std::cerr << "Nie znaleziono pliku sql_types.json." << std::endl;
        availableSqlTypes["Awaryjne"] = {
            {"INT", "Awaryjne", false},
            {"VARCHAR", "Awaryjne", true}
        };
    }

    auto ZapiszStan = [&]() {
        undoStack.push_back({mySchema, wszystkieLinki, tableCounter, nextTableId, nextLinkId});
    };

    std::string generatedSql = "";
    bool showSqlModal = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Pasek zadań u góry
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("Zapisz", "Ctrl+S")) {
                    std::string filepath = openSaveFileDialog();
                    if (!filepath.empty()) {
                        bool success = ProjectSerializer::saveToFile(mySchema, wszystkieLinki, filepath);
                        if (success) {
                            std::cout << "Zapisano do: " << filepath << std::endl;
                        }
                    }
                }

                if (ImGui::MenuItem("Wczytaj", "Ctrl+O")) {
                    std::string filepath = openLoadFileDialog();
                    if (!filepath.empty()) {
                        bool success = ProjectSerializer::loadFromFile(mySchema, wszystkieLinki, filepath, nextTableId);
                        if (success) {
                            std::cout << "Wczytano z: " << filepath << std::endl;
                        }
                    }
                }

                if (ImGui::MenuItem("Zakoncz", "Alt+F4")) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                };
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Cofnij", "Ctrl+Z", false, !undoStack.empty())) {
                    // Wyciągnięcie ostatniego zdjęcia ze stosu
                    AppStateSnapshot lastState = undoStack.back();

                    // Usunięcie tego zdjęcia ze stosu
                    undoStack.pop_back();

                    // Nadpis obecnego stanu aplikacji danymi ze zdjęcia
                    mySchema = lastState.schema;
                    wszystkieLinki = lastState.links;
                    tableCounter = lastState.tableCounter;
                    nextTableId = lastState.nextTableId;
                    nextLinkId = lastState.nextLinkId;
                    selectedNodeId = 0;
                };
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Dopasuj widok (Zoom to Fit)")) {
                    ed::NavigateToContent();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Generate")) {
                if (ImGui::MenuItem("Generate SQL Code")) {
                    // Wywołanie zewnętrznej klasy do tłumaczenia grafu na język bazy danych
                    generatedSql = SqlGenerator::generateSchemaSql(mySchema, wszystkieLinki, availableSqlTypes);
                    showSqlModal = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Obliczanie szerokości paneli
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        float screenWidth = viewport->WorkSize.x;
        float screenHeight = viewport->WorkSize.y;

        float leftPanelWidth = 100.0f;
        float rightPanelWidth = 350.0f;


        // Flagi blokujące możliwość przesuwania paneli myszką (przypinamy je an stałe)
        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, screenHeight));

        // Lewy panel - Narzędzia
        ImGui::Begin("LeftPanel", nullptr, panelFlags);
        ImGui::Text("Narzedzia");
        ImGui::Separator();
        if (ImGui::Button("+", ImVec2(40, 40))) {
            ZapiszStan(); // Zdjęcie przed zmianą
            std::string nowaNazwa = "Tabela_" + std::to_string(tableCounter);
            tableCounter++;

            Table newTable(nextTableId++, nowaNazwa);
            newTable.addColumn(Column("id", "INT"));

            mySchema.addTable(newTable);
            newTableIdToPlace = newTable.getId();
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + screenWidth - rightPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, screenHeight));

        // Prawy panel - Właściwości
        ImGui::Begin("RightPanel", nullptr, panelFlags);
        ImGui::Text("Wlasciwosci");
        ImGui::Separator();

        // Jeśli kliknięto jakąś tabelę na płótnie
        if (selectedNodeId.Get() != 0) {
            // Wyszukanie w pamięci oryginalnej tabeli po jej ID
            auto it = std::find_if(mySchema.getTablesMutable().begin(), mySchema.getTablesMutable().end(),
                [selectedNodeId](const Table& t) { return t.getId() == selectedNodeId.Get(); });

            if (it != mySchema.getTablesMutable().end()) {
                Table& selectedTable = *it;

                ImGui::Text("Wybrana tabela:");

                char nameBuffer[256];
                strcpy(nameBuffer, selectedTable.getName().c_str());

                // Jeśli użytkownik wpisze nową nazwę, od razu zapisz ją w obiekcie Table
                if (ImGui::InputText("##TableName", nameBuffer, sizeof(nameBuffer))) {
                    selectedTable.setName(nameBuffer);
                }

                ImGui::Separator();
                ImGui::Text("Kolumny:");
                ImGui::SameLine();
                if (ImGui::Button("+ Dodaj kolumne")) {
                    selectedTable.addColumn(Column("nowa_kolumna", "VARCHAR"));
                }

                ImGui::Separator();

                // Pętla renderująca interfejs do edycji kolumn
                for (size_t i = 0; i < selectedTable.getColumns().size(); i++) {
                    Column& col = selectedTable.getColumnsMutable()[i]; // Używamy Mutable, żeby móc edytować!

                    ImGui::PushID(i); // Dodane 'i', żeby ImGui wiedziało, która to kolumna

                    ImGui::SetNextItemWidth(100.0f);
                    char colNameBuf[256];
                    strcpy(colNameBuf, col.getName().c_str());

                    if (ImGui::InputText("##Nazwa", colNameBuf, sizeof(colNameBuf))) {
                        col.setName(colNameBuf);
                    }

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    std::string currentType = col.getType();

                    // Lista rozwijana z dynamicznym ładowaniem typów danych
                    if (ImGui::BeginCombo("##Typ", currentType.c_str())) {

                        // Przechodzimy przez wszystkie kategorie w naszym Słowniku
                        for (const auto& [category, typesList] : availableSqlTypes) {

                            ImGui::TextDisabled("%s", category.c_str());
                            ImGui::Separator();

                            for (const auto& t : typesList) {
                                bool is_selected = (currentType == t.name);

                                ImGui::Indent(15.0f); // Wcięcie wizualne dla estetyki
                                if (ImGui::Selectable(t.name.c_str(), is_selected)) {
                                    col.setType(t.name);
                                }
                                ImGui::Unindent(15.0f);

                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // Sprawdzanie flagi hasLength dla nowego formatu danych
                    bool showLengthField = false;
                    for (const auto& [category, typesList] : availableSqlTypes) {
                        for (const auto& t : typesList) {
                            if (t.name == col.getType()) {
                                showLengthField = t.hasLength;
                                break; // Szybkie przerwanie pętli jeśli znaleźliśmy
                            }
                        }
                    }

                    // Wyświetlenie pola długości
                    if (showLengthField) {
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(50.0f);
                        int currentLen = col.getLength();
                        if (ImGui::InputInt("##Len", &currentLen, 0)) {
                            if (currentLen < 0) currentLen = 0;
                            col.setLength(currentLen);
                        }
                    }

                    ImGui::PopID();

                    ImGui::SameLine();
                    bool isPK = col.getIsPrimaryKey();
                    if (ImGui::Checkbox("PK", &isPK)) {
                        col.setIsPrimaryKey(isPK);
                    }
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Kliknij w wezel na plotnie.");
        }

        ImGui::End();

        // Środek - Płótno
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + leftPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(screenWidth - leftPanelWidth - rightPanelWidth, screenHeight));

        ImGui::Begin("Workspace", nullptr, panelFlags);

        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        if (newTableIdToPlace != 0) {
            // Obliczenie fizyczny środek okna roboczego
            ImVec2 windowCenter = ImVec2(
                ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f,
                ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f
            );

            // Tłumaczenie współrzędne ekranu na współrzędne nieskończonego płótna
            ImVec2 canvasPos = ed::ScreenToCanvas(windowCenter);

            // Wymuszenie pozycji węzła
            ed::SetNodePosition(newTableIdToPlace, canvasPos);

            // Reset zmiennej, żeby operacja wykonała się tylko raz
            newTableIdToPlace = 0;
        }

        std::vector<Table>& wszystkieTabele = mySchema.getTablesMutable();

        // Rysowanie tabel i kolumn jako węzłów w grafie
        for (size_t i = 0; i < wszystkieTabele.size(); i++) {
            ed::BeginNode(wszystkieTabele[i].getId());

            ImGui::Text("Tabela: %s", wszystkieTabele[i].getName().c_str());
            ImGui::Separator();

            for (size_t j = 0; j < wszystkieTabele[i].getColumns().size(); j++) {
                Column& col = wszystkieTabele[i].getColumnsMutable()[j];

                // Lewy pin - do tworzenia relacji wchodzących (klucz obcy)
                ed::BeginPin(col.getInputPin().ID, ed::PinKind::Input);
                ImGui::Text("->");
                ed::EndPin();

                ImGui::SameLine();

                std::string pkText = col.getIsPrimaryKey() ? " (PK)" : "";
                ImGui::Text("%s%s : %s", col.getName().c_str(), pkText.c_str(), col.getType().c_str());

                ImGui::SameLine();

                // Prawy pin - do tworzenia relacji wychodzących (klucz główny)
                ed::BeginPin(col.getOutputPin().ID, ed::PinKind::Output);
                ImGui::Text("->");
                ed::EndPin();
            }

            ed::EndNode();
        }

        // Rysowanie strzałek (relacji zapisanych w pamięci)
        for (auto& link : wszystkieLinki) {
            ed::Link(link.ID, link.StartPinID, link.EndPinID);
        }

        // Logika obsługi rysowania nowych relacji myszką
        if (ed::BeginCreate()) {
            ed::PinId startPinId, endPinId;

            // Użytkownik przeciągnął myszką pomiędzy dwoma punktami
            if (ed::QueryNewLink(&startPinId, &endPinId)) {
                if (startPinId && endPinId) {
                    // Funkcja anonimowa tłumacząca suche int ID na oryginalny obiekt Pin
                    auto FindPin = [&wszystkieTabele](ed::PinId id) -> Pin* {
                        for (auto& table : wszystkieTabele) {
                            for (auto& col : table.getColumnsMutable()) {
                                if (col.getInputPin().ID == id) return &col.getInputPin();
                                if (col.getOutputPin().ID == id) return &col.getOutputPin();
                            }
                        }
                        return nullptr;
                    };

                    Pin* startPin = FindPin(startPinId);
                    Pin* endPin = FindPin(endPinId);

                    if (startPin && endPin) {
                        bool linkAlreadyExists = false;
                        for (const auto& link : wszystkieLinki) {
                            if ((link.StartPinID == startPinId && link.EndPinID == endPinId) ||
                                (link.StartPinID == endPinId && link.EndPinID == startPinId)) {
                                linkAlreadyExists = true;
                                break;
                            }
                        }

                        // Walidacja relacji bazy danych
                        if (startPin == endPin) {
                            ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
                        }
                        else if (startPin -> Type == endPin -> Type) {
                            ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
                        }
                        else if (linkAlreadyExists) {
                            ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
                        }
                        else {
                            if (ed::AcceptNewItem(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 2.0f)) {
                                ZapiszStan();
                                wszystkieLinki.push_back(Link(nextLinkId++, startPinId, endPinId));
                            }
                        }
                    }
                }
            }
        }
        ed::EndCreate();

        // Logika obsługi usuwania myszką (klawisz Del)
        if (ed::BeginDelete()) {
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId)) {
                if (ed::AcceptDeletedItem()) {
                    ZapiszStan();
                    wszystkieLinki.erase(
                        std::remove_if(wszystkieLinki.begin(), wszystkieLinki.end(),
                            [deletedLinkId](const Link& link) { return link.ID == deletedLinkId; }),
                        wszystkieLinki.end()
                    );
                }
            }

            ed::NodeId deletedNodeId;
            while (ed::QueryDeletedNode(&deletedNodeId)) {
                if (ed::AcceptDeletedItem()) {
                    ZapiszStan();
                    auto it = std::find_if(wszystkieTabele.begin(), wszystkieTabele.end(),
                        [deletedNodeId](const Table& t) { return t.getId() == deletedNodeId.Get(); });

                    if (it != wszystkieTabele.end()) {
                        std::vector<ed::PinId> pinsToDelete;
                        for (auto& col : it->getColumnsMutable()) {
                            pinsToDelete.push_back(col.getInputPin().ID);
                            pinsToDelete.push_back(col.getOutputPin().ID);
                        }

                        // Jeśli usuwamy tabelę, musimy usunąć wszystkie podpięte do niej strzałki
                        wszystkieLinki.erase(
                            std::remove_if(wszystkieLinki.begin(), wszystkieLinki.end(),
                                [&pinsToDelete](const Link& link) {
                                    bool startKilled = std::find(pinsToDelete.begin(), pinsToDelete.end(), link.StartPinID) != pinsToDelete.end();
                                    bool endKilled = std::find(pinsToDelete.begin(), pinsToDelete.end(), link.EndPinID) != pinsToDelete.end();
                                    return startKilled || endKilled;
                                }),
                            wszystkieLinki.end()
                        );

                        wszystkieTabele.erase(it);

                        if (selectedNodeId == deletedNodeId) {
                            selectedNodeId = 0;
                        }
                    }
                }
            }
        }
        ed::EndDelete();

        // Podświetlenie tabeli w prawym panelu jeśli na nią kliknięto
        ed::NodeId selectedNodes[1];
        int selectedCount = ed::GetSelectedNodes(selectedNodes, 1);

        if (selectedCount > 0) {
            selectedNodeId = selectedNodes[0];
        } else {
            selectedNodeId = 0;
        }

        ed::End();
        ImGui::End();

        // Okienko z wygenerowanym kodem SQL
        if (showSqlModal) {
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Wygenerowany kod SQL", &showSqlModal)) {
                ImGui::InputTextMultiline("sql_output", (char*)generatedSql.c_str(), generatedSql.size(),
                    ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_ReadOnly);

                if (ImGui::Button("Zamknij")) {
                    showSqlModal = false;
                }
            }
            ImGui::End();
        }

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Sprzątanie pamięci po wyłączeniu aplikacji
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ed::DestroyEditor(m_Context);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}