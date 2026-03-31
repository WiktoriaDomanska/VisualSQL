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
namespace ed = ax::NodeEditor;

int main() {
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 1.3f;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ed::Config config;
    config.SettingsFile = "VisualSQL_NodeEditor.json";
    ed::EditorContext* m_Context = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Context);

    Schema mySchema("Mój projekt VisualSQL");
    int tableCounter = 1;
    ed::NodeId selectedNodeId = 0;

    std::vector<Link> wszystkieLinki; // Wektor przechowujący otworzone relacje
    int nextLinkId = 1; // Licznik dla unikalnych id linków

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Zapisz", "Ctrl+S");
                ImGui::MenuItem("Zakoncz");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Cofnij");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Dopasuj widok (Zoom to Fit)")) {
                    ed::NavigateToContent();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        float screenWidth = viewport->WorkSize.x;
        float screenHeight = viewport->WorkSize.y;

        float leftPanelWidth = 100.0f;
        float rightPanelWidth = 350.0f;

        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, screenHeight));

        ImGui::Begin("LeftPanel", nullptr, panelFlags);
        ImGui::Text("Narzedzia");
        ImGui::Separator();
        if (ImGui::Button("+", ImVec2(40, 40))) {
            std::string nowaNazwa = "Tabela_" + std::to_string(tableCounter);
            tableCounter++;

            Table newTable(nowaNazwa);
            newTable.addColumn(Column("id", "INT"));

            mySchema.addTable(newTable);
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + screenWidth - rightPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, screenHeight));

        ImGui::Begin("RightPanel", nullptr, panelFlags);
        ImGui::Text("Wlasciwosci");
        ImGui::Separator();

        if (selectedNodeId.Get() != 0) {
            int tableIndex = selectedNodeId.Get() - 1;

            if (tableIndex >= 0 && tableIndex < mySchema.getTables().size()) {
                Table& selectedTable = mySchema.getTablesMutable()[tableIndex];

                ImGui::Text("Wybrana tabela:");

                char nameBuffer[256];
                strcpy(nameBuffer, selectedTable.getName().c_str());

                if (ImGui::InputText("##TableName", nameBuffer, sizeof(nameBuffer))) {
                    selectedTable.setName(nameBuffer);
                }

                ImGui::Separator();
                ImGui::Text("Kolumny:");
                ImGui::SameLine();
                if (ImGui::Button("+ Dodaj kolumne")) {
                    selectedTable.addColumn(Column("nowa_kolumna", "VARCHAR(255"));
                }

                ImGui::Separator();

                const char* sqlTypes[] = {"INT", "VARCHAR(255)", "TEXT", "FLOAT", "BOOLEAN", "TIMESTAMP", "DATE"};

                for (size_t i = 0; i < selectedTable.getColumns().size(); i++) {
                    Column& col = selectedTable.getColumnsMutable()[i];

                    ImGui::PushID(i);

                    ImGui::SetNextItemWidth(120.0f);
                    char colNameBuf[256];
                    strcpy(colNameBuf, col.getName().c_str());

                    if (ImGui::InputText("##Nazwa", colNameBuf, sizeof(colNameBuf))) {
                        col.setName(colNameBuf);
                    }

                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(120.0f);

                    int currentTypeIdx = -1;
                    for (int n = 0; n < IM_ARRAYSIZE(sqlTypes); n++) {
                        if (col.getType() == sqlTypes[n]) {
                            currentTypeIdx = n;
                            break;
                        }
                    }

                    if (ImGui::Combo("##Typ", &currentTypeIdx, sqlTypes, IM_ARRAYSIZE(sqlTypes))) {
                        if (currentTypeIdx >= 0) {
                            col.setType(sqlTypes[currentTypeIdx]);
                        }
                    }
                    ImGui::PopID();
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Kliknij w wezel na plotnie.");
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + leftPanelWidth, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(screenWidth - leftPanelWidth - rightPanelWidth, screenHeight));

        ImGui::Begin("Workspace", nullptr, panelFlags);

        ed::Begin("My Editor", ImVec2(0.0, 0.0f));

        std::vector<Table>& wszystkieTabele = mySchema.getTablesMutable();

        for (size_t i = 0; i < wszystkieTabele.size(); i++) {
            ed::BeginNode(i + 1);

            ImGui::Text("Tabela: %s", wszystkieTabele[i].getName().c_str());
            ImGui::Separator();

            for (size_t j = 0; j < wszystkieTabele[i].getColumns().size(); j++) {
                Column& col = wszystkieTabele[i].getColumnsMutable()[j];

                ed::BeginPin(col.getInputPin().ID, ed::PinKind::Input);
                ImGui::Text("->");
                ed::EndPin();

                ImGui::SameLine();

                ImGui::Text("%s : %s", col.getName().c_str(), col.getType().c_str());

                ImGui::SameLine();

                ed::BeginPin(col.getOutputPin().ID, ed::PinKind::Output);
                ImGui::Text("->");
                ed::EndPin();
            }

            ed::EndNode();
        }

        for (auto& link : wszystkieLinki) {
            ed::Link(link.ID, link.StartPinID, link.EndPinID);
        }

        if (ed::BeginCreate()) {
            ed::PinId startPinId, endPinId;

            if (ed::QueryNewLink(&startPinId, &endPinId)) {
                if (startPinId && endPinId) {
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
                        if (startPin == endPin) {
                            ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
                        }
                        else if (startPin->Type == endPin->Type) {
                            ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
                        }
                        else {
                            if (ed::AcceptNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f)) {
                                wszystkieLinki.push_back(Link(nextLinkId++, startPinId, endPinId));
                            }
                        }
                    }
                }
            }
        }
        ed::EndCreate();

        if (ed::BeginDelete()) {
            ed::LinkId deletedLinkId;

            while (ed::QueryDeletedLink(&deletedLinkId)) {
                if (ed::AcceptDeletedItem()) {
                    wszystkieLinki.erase(
                        std::remove_if(wszystkieLinki.begin(), wszystkieLinki.end(),
                            [deletedLinkId](const Link& link) { return link.ID == deletedLinkId; }),
                            wszystkieLinki.end()
                            );
                }
            }
        }
        ed::EndDelete();

        ed::NodeId selectedNodes[1];
        int selectedCount = ed::GetSelectedNodes(selectedNodes, 1);

        if (selectedCount > 0) {
            selectedNodeId = selectedNodes[0];
        } else {
            selectedNodeId = 0;
        }

        ed::End();
        ImGui::End();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ed::DestroyEditor(m_Context);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}