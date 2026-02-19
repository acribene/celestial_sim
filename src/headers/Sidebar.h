
#ifndef SIDEBAR_H
#define SIDEBAR_H
#include "raylib.h"
#include "raygui.h"
#include "Body.h"
#include "TimeManager.h"
#include "simulation.h"

// Enum for sidebar tabs
enum class SidebarTab {
    INSPECTOR,
    SETTINGS,
    INFO,
    CREATOR
};

class Sidebar {
private:
    // Layout
    Rectangle bounds_;
    float expandedWidth_ = 300.0f;
    float currentWidth_ = 0.0f;
    bool isOpen_ = false;
    
    // State
    Body* selectedBody_ = nullptr; // TODO: May change because of possible invalid pointers when bodies are removed
    SidebarTab currentTab_ = SidebarTab::INSPECTOR;
    Simulation& simulation_;
    TimeManager& timeManager_;

    Body tempBody_ = Body(); // Temporary body for creation tab

    // Save System State
    std::vector<std::string> saveFiles_;    // List of found files
    std::string saveFilesListCombined_;     // Required string format for RayGui (e.g. "save1;save2")
    int selectedSaveIndex_ = -1;            // Which file is selected in the list
    int listScrollIndex_ = 0;               // Scroll position for the list
    
    char saveNameBuffer_[64] = "my_save";   // Input buffer for new save name
    bool nameEditMode_ = false;             // Is the user typing right now?

    // Helper to scan the "saves" folder
    void refreshSaveList();

public:
    Sidebar(Simulation& sim, TimeManager& timeMgr);
    
    void applyTheme();
    // Core loop
    void update(float dt);
    void render();
    void openCreationMenu(Vec2 worldPos);

    // Selection logic
    void selectBody(Body* body);
    void deselect();
    bool isMouseOver(); // Helper to prevent clicking through the UI
    bool hasSelection() const { return selectedBody_ != nullptr; }
    bool isEditing() const { return nameEditMode_; }
    void toggleInfo();
};

#endif // SIDEBAR_H