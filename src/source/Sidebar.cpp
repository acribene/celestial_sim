#define RAYGUI_IMPLEMENTATION
#include "../headers/Sidebar.h"
#include <iostream>

Sidebar::Sidebar( Simulation& sim, TimeManager& timeMgr ) : simulation_(sim), timeManager_(timeMgr) {
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
        if (GuiButton((Rectangle){ 100, 10, 60, 30 }, "Create")) currentTab_ = SidebarTab::CREATOR;
        if (GuiButton((Rectangle){ 190, 10, 80, 30 }, "Settings")) currentTab_ = SidebarTab::SETTINGS;
        if (GuiButton((Rectangle){ 280, 10, 80, 30 }, "Info")) currentTab_ = SidebarTab::INFO;

        if (currentTab_ == SidebarTab::INSPECTOR) {
            if (selectedBody_ != nullptr) {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Body Properties");

                // Mass Slider (Logarithmic Control)
                // We control the 'exponent' from -8.0 to 1.0
                double currentMass = selectedBody_->getMass();
                float oldLogMass = (float)log10(currentMass); 
                float logMass = oldLogMass;
                GuiSlider((Rectangle){ 60, 90, 150, 20 }, "Mass", TextFormat("%.2e", currentMass), &logMass, -8.0f, 1.0f);
                if (logMass != oldLogMass) {
                    selectedBody_->setMass(pow(10.0, logMass));
                }

                // Radius Slider
                // float radVal = (float)selectedBody_->getRadius();
                // if (GuiSlider((Rectangle){ 60, 120, 150, 20 }, "Radius", TextFormat("%.2f", radVal), &radVal, 0.01f, 5.0f)) {
                //     selectedBody_->setRadius((double)radVal);
                // }

                // Delete Button
                if (GuiButton((Rectangle){ 10, 400, 220, 30 }, "DELETE BODY")) {
                    // Find and remove the selected body from the simulation
                    simulation_.DeleteBodyAt( selectedBody_->getPos() );
                    deselect();
                }
            } else {
                GuiLabel((Rectangle){ 10, 50, 200, 20 }, "No body selected.");
            }
        }
        else if(currentTab_ == SidebarTab::SETTINGS) {
            GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Simulation Settings");
            // Add settings controls here
        }
        else if(currentTab_ == SidebarTab::INFO) {            
            GuiLabel((Rectangle){ 10, 50, 200, 20 }, "2D Physics Simulator");
            
            GuiLabel((Rectangle){ 10, 80, 200, 20 }, TextFormat("FPS: %d", GetFPS()));
            GuiLabel((Rectangle){ 10, 100, 200, 20 }, TextFormat("Time Scale: %.2f", timeManager_.getTimeScale()));

            if (timeManager_.getPauseState()) { 
                DrawText("PAUSED", 10, 130, 20, RED);
                GuiLabel((Rectangle){ 10, 150, 200, 20 }, "(Press ENTER to Step)");
            }

            int startY = 200;
            GuiLabel((Rectangle){ 10.0f, (float)startY, 250, 20 }, "CONTROLS:");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 25, 250, 20 }, "- Arrow keys: Pan Camera");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 45, 250, 20 }, "- Scroll / A / Z: Zoom");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 65, 250, 20 }, "- +/- : Change Time Scale");
            GuiLabel((Rectangle){ 10.0f, (float)startY + 85, 250, 20 }, "- Space : Pause/Resume");
        }
        else if(currentTab_ == SidebarTab::CREATOR) {
           GuiLabel((Rectangle){ 10, 50, 200, 20 }, "Create New Body");
            GuiLabel((Rectangle){ 10, 70, 200, 20 }, TextFormat("Pos: %.2f, %.2f AU", tempBody_.getPos().getX(), tempBody_.getPos().getY()));

            // --- MASS (Log Scale) ---
            GuiLabel((Rectangle){ 10, 100, 200, 20 }, "Mass (Log Scale)");
            
            // 1. Get current log value from the real mass
            float currentLogMass = (float)log10(tempBody_.getMass());
            
            // 2. Draw Slider using the log value
            GuiSlider((Rectangle){ 60, 120, 150, 20 }, "Mass", TextFormat("%.2e Msun", tempBody_.getMass()), &currentLogMass, -8.0f, -4.0f);
            
            // 3. Convert back to real mass and update body
            tempBody_.setMass(pow(10.0, currentLogMass));

            // --- RADIUS (Auto-calculated) ---
            // Formula: same as in rendering for consistency
            double calculatedRadius = 0.02 + 0.005 * (currentLogMass + 8.0);
            
            // Update the temp body immediately
            tempBody_.setRadius(calculatedRadius);

            // Display as Read-Only Label instead of Slider
            GuiLabel((Rectangle){ 10, 150, 200, 20 }, "Radius (Auto-Scaled)");
            // Draw a disabled slider or just a value box to visualize it
            GuiStatusBar((Rectangle){ 60, 170, 150, 20 }, TextFormat("%.4f AU", calculatedRadius));

            // --- VELOCITY ---
            Vec2 currentVel = tempBody_.getVel();
            GuiLabel((Rectangle){ 10, 200, 200, 20 }, "Velocity X (AU/yr)");
            float tempVelX = (float)currentVel.getX();
            GuiSlider((Rectangle){ 60, 220, 150, 20 }, "VX", TextFormat("%.2f", tempVelX), &tempVelX, -10.0f, 10.0f);
            currentVel.setX(tempVelX);

            GuiLabel((Rectangle){ 10, 250, 200, 20 }, "Velocity Y (AU/yr)");
            float tempVelY = (float)currentVel.getY();
            GuiSlider((Rectangle){ 60, 270, 150, 20 }, "VY", TextFormat("%.2f", tempVelY), &tempVelY, -10.0f, 10.0f);
            currentVel.setY(tempVelY);

            tempBody_.setVel(currentVel);

            // Create Button
            if (GuiButton((Rectangle){ 10, 320, 220, 40 }, "SPAWN BODY")) {
                Body newBody( tempBody_ );
                selectedBody_ = simulation_.addBody( newBody );                
                currentTab_ = SidebarTab::INSPECTOR;
                timeManager_.togglePause(); // Unpause on creation
            }
        }
    }
}

void Sidebar::openCreationMenu(Vec2 worldPos) {
    tempBody_ = Body(); // Reset temporary body
    tempBody_.setPos(worldPos);
    currentTab_ = SidebarTab::CREATOR;
    isOpen_ = true;
    
    // Store REAL mass (10^-6), not the log value (-6.0)
    tempBody_.setMass(pow(10.0, -6.0)); 
    
    // Auto-calculate initial radius based on the mass
    double logMass = -6.0;
    double autoRadius = 0.02 + 0.005 * (logMass + 8.0);
    tempBody_.setRadius(autoRadius);
    
    tempBody_.setVel(Vec2(0.0, 0.0));
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