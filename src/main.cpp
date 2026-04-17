#include "raylib.h"
#include "Body.hpp"
#include <vector>
#include <cmath>

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Simulador de Mecanica Orbital - Curvatura Suave");

    // configuracion de camara
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 15.0f, 100.0f }; 
    camera.target = { 0.0f, 0.0f, 0.0f };      
    camera.up = { 0.0f, 1.0f, 0.0f };          
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    double scale = 1e-6; 
    double dt = 150.0; 

    // tierra y satelite
    Phys::Body earth("Tierra", 5.972e24, Phys::Vector3(0, 0, 0), Phys::Vector3(0, 0, 0));
    Phys::Body sat("Sputnik", 1000.0, Phys::Vector3(10000000, 0, 0), Phys::Vector3(0, 0, 6500));
    Phys::Body moon("Luna", 7.347e22, Phys::Vector3(40000000, 0, 0), Phys::Vector3(0, 0, 3100)); // se añade la luna LOLXD

    std::vector<Vector3> trail;
    const int maxTrailPoints = 2000; 

    SetTargetFPS(60);
    DisableCursor(); // cursor oculto asi no moleste

    while (!WindowShouldClose()) {
        
        // rotar camara
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        // fisica
        // A- fuerza sobre el satelite (tierra + luna)
        Phys::Vector3 fEarthToSat = sat.calculateGravitationalForce(earth);
        Phys::Vector3 fMoonToSat = sat.calculateGravitationalForce(moon);
        sat.applyForce(fEarthToSat + fMoonToSat);

        // B- fuerza sobre la Luna solo la tierra
        Phys::Vector3 fEarthToMoon = moon.calculateGravitationalForce(earth);
        moon.applyForce(fEarthToMoon);

        sat.update(dt);
        moon.update(dt);

        // logica de la estela de rastro
        Vector3 currentSatPos = { 
            static_cast<float>(sat.position.x * scale), 
            static_cast<float>(sat.position.y * scale), 
            static_cast<float>(sat.position.z * scale) 
        };
        Vector3 currentMoonPos = { // logica de luna
            static_cast<float>(moon.position.x * scale),
            static_cast<float>(moon.position.y * scale),
            static_cast<float>(moon.position.z * scale)
        };

        trail.push_back(currentSatPos);
        if (trail.size() > maxTrailPoints) {
            trail.erase(trail.begin());
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                
                // grilla distorsionada | espacio-tiempo
                const int gridCount = 81;
                const float gridSpacing = 2.0f; 
                const float visualDepthExaggeration = 8.0f; 

                static std::vector<std::vector<Vector3>> gridPoints(gridCount, std::vector<Vector3>(gridCount));

                for (int i = 0; i < gridCount; i++) {
                    for (int j = 0; j < gridCount; j++) {
                        float x = (i - gridCount/2) * gridSpacing;
                        float z = (j - gridCount/2) * gridSpacing;
                        float y = 0.0f;

                        // distorsion tierra
                        float dE2 = x*x + z*z + 8.0f;
                        y -= (10.0f * visualDepthExaggeration) / std::sqrt(dE2);

			            //distrosion luna
                        float dxM = x - currentMoonPos.x;
                        float dzM = z - currentMoonPos.z;
                        float dM2 = dxM*dxM + dzM*dzM + 2.0f;
                        y -= (3.5f * visualDepthExaggeration) / std::sqrt(dM2);

			            //distorsion satelite
                        float dxS = x - currentSatPos.x;
                        float dzS = z - currentSatPos.z;
                        float dS2 = dxS*dxS + dzS*dzS + 0.5f;
                        y -= (1.0f * visualDepthExaggeration) / std::sqrt(dS2);

                        gridPoints[i][j] = { x, y, z };
                    }
                }

                for (int i = 0; i < gridCount; i++) {
                    for (int j = 0; j < gridCount; j++) {
                        if (i < gridCount - 1) 
                            DrawLine3D(gridPoints[i][j], gridPoints[i+1][j], Fade(DARKGRAY, 0.4f));
                        if (j < gridCount - 1) 
                            DrawLine3D(gridPoints[i][j], gridPoints[i][j+1], Fade(DARKGRAY, 0.4f));
                    }
                }

                //tierra
                DrawSphere({0, -1.5f, 0}, 2.0f, BLUE); 
                
                // luna
                DrawSphere(currentMoonPos, 1.2f, GRAY);
                DrawSphereWires(currentMoonPos, 1.3f, 10, 10, Fade(LIGHTGRAY, 0.3f));

                // satelite y estela
                DrawSphere(currentSatPos, 0.4f, WHITE);
                if (trail.size() >= 2) {
                    for (size_t i = 0; i < trail.size() - 1; i++) {
                        DrawLine3D(trail[i], trail[i+1], RED);
                    }
                }
                
            EndMode3D();

            DrawRectangle(10, 10, 350, 120, Fade(BLACK, 0.6f));
            DrawText("ULTRA-HIGH FIDELITY SPACETIME", 20, 20, 15, YELLOW);
            DrawText(TextFormat("Grid: %d x %d vertices", gridCount, gridCount), 20, 45, 12, GREEN);
            DrawText(TextFormat("Dist. Luna: %.0f km", (moon.position - earth.position).magnitude() / 1000), 20, 65, 12, RAYWHITE);
            DrawText(TextFormat("FPS: %d", GetFPS()), 20, 85, 20, SKYBLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}