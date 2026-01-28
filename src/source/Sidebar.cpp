#define RAYGUI_IMPLEMENTATION
#include "../headers/Sidebar.h"
#include <iostream>

Sidebar::Sidebar() {
    bounds_ = { 0, 0, 0, (float)GetScreenHeight() }; // Left side, full height
}

void Sidebar::update(float dt) {
    float targetWidth = isOpen_ ? expandedWidth_ : 0.0f;
    
    // Smooth slide animation (Linear Interpolation)
    currentWidth_ += (targetWidth - currentWidth_) * 10.0f * dt;
    
    bounds_.width = currentWidth_;
    bounds_.height = (float)GetScreenHeight(); // Handle window resizing
}

void Sidebar::render() {
    // Draw Background
    if (currentWidth_ < 1.0f) return; // Don't draw if closed

    DrawRectangleRec(bounds_, Fade(DARKGRAY, 0.95f));
    DrawRectangleLinesEx(bounds_, 2, LIGHTGRAY);

    // Draw Content only if expanded enough
    if (currentWidth_ > 200.0f) {
        
        // --- TABS  ---
        if (GuiButton((Rectangle){ 10, 10, 80, 30 }, "Inspector")) currentTab_ = SidebarTab::INSPECTOR;
        if (GuiButton((Rectangle){ 100, 10, 80, 30 }, "Settings")) currentTab_ = SidebarTab::SETTINGS;
        if (GuiButton((Rectangle){ 190, 10, 80, 30 }, "Debug")) currentTab_ = SidebarTab::DEBUG;

        if (currentTab_ == SidebarTab::INSPECTOR) {
            if (selectedBody_ != nullptr) {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Body Properties");

                // Mass Slider
                // We use a temp float because GuiSliderPro uses float pointers
                float massVal = (float)selectedBody_->getMass(); 
                if (GuiSlider((Rectangle){ 60, 90, 150, 20 }, "Mass", TextFormat("%.2e", massVal), &massVal, 0.1f, 100.0f)) {
                    selectedBody_->setMass((double)massVal);
                }

                // Radius Slider
                float radVal = (float)selectedBody_->getRadius();
                if (GuiSlider((Rectangle){ 60, 120, 150, 20 }, "Radius", TextFormat("%.2f", radVal), &radVal, 0.01f, 5.0f)) {
                    selectedBody_->setRadius((double)radVal);
                }

                // Delete Button
                if (GuiButton((Rectangle){ 10, 400, 220, 30 }, "DELETE BODY")) {
                    // Need a way to mark bodies for deletion
                    // selectedBody_->isDead = true; 
                    deselect();
                }
            } else {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "No body selected.");
            }
        }
    }
}

void Sidebar::selectBody(Body* body) {
    selectedBody_ = body;
    isOpen_ = true; // Auto open on click
}

void Sidebar::deselect() {
    selectedBody_ = nullptr;
    isOpen_ = false;
}

bool Sidebar::isMouseOver() {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, bounds_);
}