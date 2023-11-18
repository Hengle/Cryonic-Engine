#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include <fstream>
#include <variant>
#include "Editor.h"
#include "FontManager.h"
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include <imgui_impl_raylib.h>
//#include "NewMaterial.h"
#include <fstream>
#include "Utilities.h"
#include <variant>

const float DEG = 180.0f / PI;
const float RAD = PI / 180.0f;

bool viewportOpen = true;
bool viewportFocused = false;
bool viewportHovered = false;
bool rmbDown = false;
RenderTexture ViewTexture;
Texture2D GridTexture = { 0 };

Vector4 viewportPosition;

std::variant<std::monostate, GameObject*, Material*> objectInProperties = std::monostate{}; // Make a struct or something that holds a Path and ifstream String. Not specific to material so prefabs and stuff can use
GameObject* selectedObject = nullptr;

bool explorerContextMenuOpen = false;
bool hierarchyContextMenuOpen = false;

bool resetPropertiesWin = true;
bool resetViewportWin = true;
bool resetFileExplorerWin = true;
bool resetHierarchy = true;

bool componentsWindowOpen = false;

std::filesystem::path fileExplorerPath;

std::unordered_map<std::string, Texture2D*> imageTextures;
std::vector<Texture2D*> tempTextures;
std::vector<RenderTexture2D*> tempRenderTextures;

Quaternion Orientation = QuaternionIdentity();

float cameraSpeed = 1;

RenderTexture2D* Editor::CreateModelPreview(std::filesystem::path modelPath, int textureSize)
{
    // Load the 3D model
    Model model = LoadModel(modelPath.string().c_str());

    // Set a basic camera to view the model
    Camera camera = { 0 };
    camera.position = { 0.0f, 0.0f, 6.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Create a render texture
    //RenderTexture2D target = LoadRenderTexture(textureSize, textureSize);
    tempRenderTextures.push_back(new RenderTexture2D(LoadRenderTexture(textureSize, textureSize)));

    // Render model to texture
    BeginTextureMode(*tempRenderTextures.back());

    // Clear the render texture
    ClearBackground(Color{ 63, 63, 63, 255 });

    // Set camera position and projection for rendering
    BeginMode3D(camera);

    // Draw the model
    DrawModel(model, { 0,0,0 }, 1.0f, WHITE);

    EndMode3D();

    EndTextureMode();

    std::cout << "Path: " << modelPath << ", Texture Size: " << textureSize << std::endl;

    // Unload the model (not needed anymore)
    UnloadModel(model);

    //tempTextures.push_back(new Texture2D(target.texture));

    //UnloadRenderTexture(target);

    // Return the generated texture
    return tempRenderTextures.back();
}

void Editor::RenderViewport()
{
    if (!viewportOpen) return;
    if (resetPropertiesWin)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
        //ImGui::SetNextWindowSize(ImVec2(1280, 720));
        ImGui::SetNextWindowSize(ImVec2(1366, 736));
        ImGui::SetNextWindowPos(ImVec2(277, 52));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // Todo: Use resize events
    viewportPosition = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y };

    if (ImGui::Begin("Viewport", &viewportOpen, ImGuiWindowFlags_NoScrollbar))
    {
        viewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
        rlImGuiImageRenderTextureFit(&ViewTexture, true);

        viewportHovered = ImGui::IsWindowHovered();

        // Camera Movement


        if (ImGui::IsWindowHovered() && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) // Todo: Maybe change to viewportFocused instead of IsWindowFocused
        {
            //    // Todo: Add SHIFT to speed up by x2, and scroll weel to change speed
            rmbDown = true;
            //HideCursor();
            //DisableCursor();
            UpdateCamera(&camera, CAMERA_PERSPECTIVE);
            //    UpdateCamera(&camera, CAMERA_FREE);
            //    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 forward = { -viewMatrix.m12, -viewMatrix.m13, -viewMatrix.m14 };
            //        camera.position.x += forward.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += forward.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += forward.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 left = { -viewMatrix.m0, -viewMatrix.m1, -viewMatrix.m2 };
            //        camera.position.x += left.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += left.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += left.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 backward = { viewMatrix.m12, viewMatrix.m13, viewMatrix.m14 };
            //        camera.position.x += backward.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += backward.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += backward.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 right = { viewMatrix.m0, viewMatrix.m1, viewMatrix.m2 };
            //        camera.position.x += right.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += right.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += right.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
        }
        else if (rmbDown && IsMouseButtonUp(MOUSE_BUTTON_RIGHT))
        {
            //ShowCursor();
            //EnableCursor();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::UpdateViewport()
{
    if (!viewportOpen) return;
    if (IsWindowResized())
    {
        UnloadRenderTexture(ViewTexture);
        ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    // Load new models/textures on drag&drop
    if (viewportHovered && IsFileDropped())
    {
        FilePathList droppedFiles = LoadDroppedFiles();

        if (droppedFiles.count == 1) // Only support one file dropped BUT ADD SUPPORT FOR MULTIPLE ---------------------------------------------------------------------
        {
            if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                IsFileExtension(droppedFiles.paths[0], ".glb") ||
                IsFileExtension(droppedFiles.paths[0], ".vox") ||
                IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
            {
                std::filesystem::path folderPath = projectData.path / "Assets" / "Models";
                if (!std::filesystem::exists(folderPath))
                    std::filesystem::create_directories(folderPath);

                fileExplorerPath = folderPath;
                std::filesystem::path filePath = droppedFiles.paths[0];

                std::string filename = filePath.filename().string();
                std::string extension = filePath.extension().string();

                int counter = 1;
                while (std::filesystem::exists(folderPath / filename)) {
                    // File with the same name exists, append a number to the filename
                    filename = filePath.stem().string() + std::to_string(counter) + extension;
                    counter++;
                }

                std::filesystem::copy_file(droppedFiles.paths[0], folderPath / filename);

                GameObject gameObject;
                Material material;
                //gameObject.SetModel(LoadModel((folderPath / filename).string().c_str()));
                gameObject.SetModelPath(folderPath / filename);
                gameObject.SetModel(LoadModel(gameObject.GetModelPath().string().c_str()));
                gameObject.transform.SetPosition({ 0,0,0 });
                //gameObject.SetRealSize(gameObject.GetModel().s);
                gameObject.transform.SetScale({ 1,1,1 });
                gameObject.transform.SetRotation(QuaternionIdentity());
                gameObject.SetName((folderPath / filename).stem().string());
                //material.SetDiffuseMap();
                // gameObject.GetModel().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture
                //gameObject.SetMaterial(.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                SceneManager::GetActiveScene()->AddGameObject(gameObject);

                selectedObject = &SceneManager::GetActiveScene()->GetGameObjects().back();
                objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects().back();

                // TODO: Move camera position from target enough distance to visualize model properly
            }
            else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
            {
                // Unload current model texture and load new one
                //UnloadTexture(texture);
                //texture = LoadTexture(droppedFiles.paths[0]);
                //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
            }
        }

        UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
    }

    //float period = 10;
    //float magnitude = 25;

    //camera.position.x = (float)(sinf((float)GetTime() / period) * magnitude);

    BeginTextureMode(ViewTexture);
    ClearBackground(SKYBLUE);

    BeginMode3D(camera);

    DrawGrid(100, 10.0f);

    for (GameObject& gameObject : SceneManager::GetActiveScene()->GetGameObjects())
    {
        if (!gameObject.IsActive()) continue;

        rlPushMatrix();

        // build up the transform
        Matrix transform = MatrixTranslate(gameObject.transform.GetPosition().x, gameObject.transform.GetPosition().y, gameObject.transform.GetPosition().z);

        transform = MatrixMultiply(QuaternionToMatrix(gameObject.transform.GetRotation()), transform);

        transform = MatrixMultiply(MatrixScale(gameObject.transform.GetScale().x, gameObject.transform.GetScale().y, gameObject.transform.GetScale().z), transform);

        // apply the transform
        rlMultMatrixf(MatrixToFloat(transform));

        // draw model
        if ((std::filesystem::exists(gameObject.GetModelPath())))
            DrawModel(gameObject.GetModel(), Vector3Zero(), 1, WHITE);
        else if (gameObject.GetModelPath() == "Cube")
            DrawCube(Vector3Zero(), 1, 1, 1, LIGHTGRAY);
        else if (gameObject.GetModelPath() == "Plane")
            DrawPlane(Vector3Zero(), Vector2{1,1}, LIGHTGRAY);
        else if (gameObject.GetModelPath() == "Sphere")
            DrawSphere(Vector3Zero(), 1, LIGHTGRAY);
        else
        {
            // Invalid model path or empty object
        }

        rlPopMatrix();
    }

    // grid of cube trees on a plane to make a "world"
    //DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE); // simple world plane
    //float spacing = 4;
    //int count = 5;

    //for (float x = -count * spacing; x <= count * spacing; x += spacing)
    //{
    //    for (float z = -count * spacing; z <= count * spacing; z += spacing)
    //    {
    //        Vector3 pos = { x, 0.5f, z };

    //        Vector3 min = { x - 0.5f,0,z - 0.5f };
    //        Vector3 max = { x + 0.5f,1,z + 0.5f };

    //        DrawCube(Vector3{ x, 1.5f, z }, 1, 1, 1, GREEN);
    //        DrawCube(Vector3{ x, 0.5f, z }, 0.25f, 1, 0.25f, BROWN);
    //    }
    //}

    EndMode3D();
    EndTextureMode();
}

void Editor::RenderFileExplorer() // Todo: Handle if path is in a now deleted folder.
{
    if (resetFileExplorerWin)
    {
        resetFileExplorerWin = false;
        ImGui::SetNextWindowSize(ImVec2(1920, 282));
        ImGui::SetNextWindowPos(ImVec2(0, 788));
        fileExplorerPath = projectData.path / "Assets"; // Todo: Make sure Assets path exists, if not then create it.
    }
    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;

    float nextX = 10;
    float nextY = 30;

    if (ImGui::Begin("File Explorer", nullptr, windowFlags))
    {
        // Back Folder/Button if not in Assets folder
        if (fileExplorerPath != projectData.path / "Assets")
        {
            ImGui::PushID(fileExplorerPath.string().c_str()); // set unique ID based on the path string
            // Creates back button
            ImGui::SetCursorPosY(nextY);
            ImGui::SetCursorPosX(nextX);
            if (rlImGuiImageButtonSize("##BackButton", imageTextures["FolderIcon"], ImVec2(32, 32)))
            {
                fileExplorerPath = fileExplorerPath.parent_path();
                ImGui::PopID();
                ImGui::End();
                return;
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
            ImGui::SetCursorPosX(nextX + 5);
            ImGui::Text("Back");             // Text to display below the image
            nextX += 60;
            ImGui::PopID(); // reset the unique ID
        }

        for (const auto& entry : std::filesystem::directory_iterator(fileExplorerPath))
        {
            ImGui::PushID(entry.path().string().c_str()); // set unique ID based on the path string
            std::string id = entry.path().string().c_str();
            std::string fileName = entry.path().filename().string();
            //ConsoleLogger::InfoLog("Found " + fileName);
            if (fileName.length() > 7) {
                fileName = fileName.substr(0, 8);
                fileName = fileName + "..";
            }
            ImGui::SetCursorPosY(nextY);
            ImGui::SetCursorPosX(nextX);
            ImVec2 pos = ImGui::GetCursorPos();
            if (entry.is_directory())
            {
                if (rlImGuiImageButtonSize(("##" + id).c_str(), imageTextures["FolderIcon"], ImVec2(32, 32)))
                {
                    fileExplorerPath = entry.path();
                    ImGui::PopID();
                    ImGui::End();
                    return;
                }
            }
            else if (entry.is_regular_file())
            {
                std::string extension = entry.path().extension().string();
                if (extension == ".cpp")
                {
                    if (rlImGuiImageButtonSize(("##" + id).c_str(), imageTextures["CppIcon"], ImVec2(32, 32)))
                    {
                        std::string command = "code " + entry.path().string();
                        std::system(command.c_str());
                    }
                }
                else if (extension == ".png" || extension == ".jpg" || extension == ".webp")
                {
                    tempTextures.push_back(new Texture2D(LoadTexture(entry.path().string().c_str())));
                    //imageTextures[path.stem().string()] = LoadTexture(path.string().c_str());

                    if (rlImGuiImageButtonSize(("##" + id).c_str(), tempTextures.back(), ImVec2(32, 32)))
                    {
                        std::string command = "start " + entry.path().string();
                        std::system(command.c_str());
                    }

                    //ImVec2 dropPos = ImGui::GetCursorScreenPos();
                    //ImVec2 dropSize = ImVec2(100, 100);
                    //if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                    //    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    //        draggingObject = true;
                    //        dragData.SetTexture(texture);
                    //        dragData.SetName(entry.path().stem().string());
                    //        dragData.SetPath(entry.path().string());
                    //        objectInProperties = nullptr;
                    //        selectedObject = nullptr;
                    //    }
                    //    else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    //    {
                    //        ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                    //        std::string command = "start " + PathToString(entry.path());
                    //        std::system(command.c_str());
                    //    }
                    //}
                    //if (isDragging && draggingObject) {
                    //    // Set the next window position, position, and background alpha

                    //    int width, height;
                    //    SDL_QueryTexture(folderIconTexture, nullptr, nullptr, &width, &height);
                    //    //ConsoleLogger::InfoLog("Width" + std::to_string(width) + " Height" + std::to_string(height));

                    //    ImGui::SetNextWindowSize(ImVec2(width, height));
                    //    ImGui::SetNextWindowPos(dragPos);
                    //    ImGui::SetNextWindowBgAlpha(0.0f);

                    //    // Set the padding and margins for the button and the window
                    //    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                    //    // Begin the window and draw the button
                    //    ImGui::Begin("DragSpriteWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
                    //    ImGui::Image((void*)dragData.GetTexture(), ImGui::GetWindowSize());
                    //    ImGui::End();
                    //    ImGui::PopStyleVar(4);
                    //}

                    //if (draggingObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    //    draggingObject = false;
                    //    isDragging = false;
                    //    ConsoleLogger::InfoLog("Dropped on file explorer");
                    //}
                }
                else if (extension == ".obj" || extension == ".gltf" || extension == ".glb" || extension == ".vox" || extension == ".iqm" || extension == ".m3d")
                {
                    if (rlImGuiImageButtonSize(("##" + id).c_str(), &CreateModelPreview(entry.path(), 32)->texture, ImVec2(32, 32)))
                    {
                    }
                }
                else
                {
                    ImGui::PopID();
                    continue;
                }
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
            ImGui::SetCursorPosX(nextX - 2);
            ImGui::Text(fileName.c_str());             // Text to display below the image

            nextX += 60;

            if (nextX > 1900)
            {
                nextX = 10;
                nextY = nextY + 75;
            }
            ImGui::PopID(); // reset the unique ID
        }
        // Add a scrollable text box
        //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        //ImGui::Text("Scrollable Text");
        //ImGui::EndChild();

        if (explorerContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
        {
            explorerContextMenuOpen = true;
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Create Script"))
                {
                    explorerContextMenuOpen = false;
                    //eventSheetEditor.eventSheetCreatorOpen = true;
                    //eventSheetEditor.SetPath(fileExplorerPath);
                }
                if (ImGui::MenuItem("Create Sprite"))
                {
                    explorerContextMenuOpen = false;
                    //spriteEditorWindow.spriteCreatorOpen = true;
                    //spriteEditorWindow.SetPath(fileExplorerPath);
                }
                if (ImGui::MenuItem("Create Material"))
                {
                    explorerContextMenuOpen = false;
                    std::ofstream outFile(fileExplorerPath / "NewMaterial.mat");
                    // Todo: In file explorere, show material as multi-color/rainbow square/sphere
                    if (outFile.is_open()) {
                        //outFile << "Hello, this is a text file!\n";
                        //outFile << "You can write more lines here.\n";

                        outFile.close();
                        //std::cout << "File created successfully.\n";
                    }
                    else {
                        //std::cerr << "Error opening the file.\n";
                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Open Folder In File Explorer"))
                {
                    explorerContextMenuOpen = false;
                    //ConsoleLogger::InfoLog("Opened Folder: " + PathToString(fileExplorerPath));
                    Utilities::OpenPathInExplorer(fileExplorerPath);
                }

                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
}

void Editor::RenderProperties()
{
    static bool show = true;

    if (resetPropertiesWin)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 736));
        ImGui::SetNextWindowPos(ImVec2(1643, 52));
        resetPropertiesWin = false;
    }
    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;
    if (ImGui::Begin("Properties", nullptr, windowFlags) && std::holds_alternative<GameObject*>(objectInProperties) && std::find_if(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [&](const auto& obj) { return &obj == std::get<GameObject*>(objectInProperties); }) != SceneManager::GetActiveScene()->GetGameObjects().end())
    {
        ImGui::BeginGroup();
        if (std::holds_alternative<GameObject*>(objectInProperties))
        {
            // Active checkbox
            ImGui::Checkbox("##Active" , &std::get<GameObject*>(objectInProperties)->active);
            // Name
            ImGui::SameLine();
            char nameBuffer[256] = {};
            strcpy_s(nameBuffer, std::get<GameObject*>(objectInProperties)->GetName().c_str());
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 67);
            if (ImGui::InputText("##ObjectNameText", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::get<GameObject*>(objectInProperties)->SetName(std::string(nameBuffer));
            }
            // Position
            ImGui::NewLine();
            ImGui::Text("Position:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            float xPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().x;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectXPos", &xPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ xPos, std::get<GameObject*>(objectInProperties)->transform.GetPosition().y, std::get<GameObject*>(objectInProperties)->transform.GetPosition().z });
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            float yPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().y;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectYPos", &yPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetPosition().x, yPos, std::get<GameObject*>(objectInProperties)->transform.GetPosition().z });
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            float zPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().z;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectZPos", &zPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetPosition().x, std::get<GameObject*>(objectInProperties)->transform.GetPosition().y, zPos });
            }
            // Scale
            ImGui::NewLine();
            ImGui::Text("Scale:      ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            //float xScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().x / std::get<GameObject*>(objectInProperties)->GetRealSize().x;
            float xScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().x;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectXScale", &xScale, 0, 0, "%.10g")) {
                //std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ xScale * std::get<GameObject*>(objectInProperties)->GetRealSize().x, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ xScale, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            //float yScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().y / std::get<GameObject*>(objectInProperties)->GetRealSize().y;
            float yScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().y;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectYScale", &yScale, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetScale().x, yScale, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            //float zScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().z / std::get<GameObject*>(objectInProperties)->GetRealSize().z;
            float zScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().z;
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputFloat("##ObjectZScale", &zScale, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetScale().x, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, zScale });
            }
            // Rotation
            //Vector3 rot = QuaternionToEuler(std::get<GameObject*>(objectInProperties)->transform.GetRotation());
            Vector3 rot = Vector3Scale(QuaternionToEuler(std::get<GameObject*>(objectInProperties)->transform.GetRotation()), DEG);
            ImGui::NewLine();
            ImGui::Text("Rotation:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            int xRot = static_cast<int>(rot.x);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputInt("##ObjectXRotation", &xRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(QuaternionFromEuler((float)xRot * RAD, rot.y * RAD, rot.z * RAD));
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            int yRot = static_cast<int>(rot.y);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputInt("##ObjectYRotation", &yRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(QuaternionFromEuler(rot.x * RAD, yRot * RAD, rot.z * RAD));
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            int zRot = static_cast<int>(rot.z);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputInt("##ObjectZRotation", &zRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(QuaternionFromEuler(rot.x * RAD, rot.y * RAD, zRot * RAD));
            }
            // Order
            //ImGui::NewLine();
            //ImGui::Text("Order:      ");
            //ImGui::SameLine();
            //char orderBuffer[256] = {};
            //sprintf_s(orderBuffer, sizeof(orderBuffer), "%d", std::get<GameObject*>(objectInProperties)->GetZOrder());
            //ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 15);
            //ImGui::SetNextItemWidth(50);
            //if (ImGui::InputText("##ObjectOrder", orderBuffer, sizeof(orderBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    std::get<GameObject*>(objectInProperties)->SetZOrder(std::stof(orderBuffer));
            //}
            // Tint
            //ImGui::NewLine();
            //ImGui::Text("Colour:     ");
            //ImGui::SameLine();
            //ImGui::Text("R");
            //ImGui::SameLine();
            //char rBufferTint[256] = {};
            //float tintValue = std::get<GameObject*>(objectInProperties)->GetTint().Value.x;
            //sprintf_s(rBufferTint, sizeof(rBufferTint), "%.0f", tintValue * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectRTint", rBufferTint, sizeof(rBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    std::get<GameObject*>(objectInProperties)->SetTint(ImColor(std::stof(rBufferTint) / 255, std::get<GameObject*>(objectInProperties)->GetTint().Value.y, std::get<GameObject*>(objectInProperties)->GetTint().Value.z, std::get<GameObject*>(objectInProperties)->GetTint().Value.w));
            //}
            //ImGui::SameLine();
            //ImGui::Text("G");
            //ImGui::SameLine();
            //char gBufferTint[256] = {};
            //sprintf_s(gBufferTint, sizeof(gBufferTint), "%.0f", std::get<GameObject*>(objectInProperties)->GetTint().Value.y * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectGTint", gBufferTint, sizeof(gBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    std::get<GameObject*>(objectInProperties)->SetTint(ImColor(std::get<GameObject*>(objectInProperties)->GetTint().Value.x, std::stof(gBufferTint) / 255, std::get<GameObject*>(objectInProperties)->GetTint().Value.z, std::get<GameObject*>(objectInProperties)->GetTint().Value.w));
            //}
            //ImGui::SameLine();
            //ImGui::Text("B");
            //ImGui::SameLine();
            //char bBufferTint[256] = {};
            //sprintf_s(bBufferTint, sizeof(bBufferTint), "%.0f", std::get<GameObject*>(objectInProperties)->GetTint().Value.z * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectBTint", bBufferTint, sizeof(bBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    std::get<GameObject*>(objectInProperties)->SetTint(ImColor(std::get<GameObject*>(objectInProperties)->GetTint().Value.x, std::get<GameObject*>(objectInProperties)->GetTint().Value.y, std::stof(bBufferTint) / 255, std::get<GameObject*>(objectInProperties)->GetTint().Value.w));
            //}
            //ImGui::SameLine();
            //ImGui::Text("A");
            //ImGui::SameLine();
            //char aBufferTint[256] = {};
            //sprintf_s(aBufferTint, sizeof(aBufferTint), "%.0f", std::get<GameObject*>(objectInProperties)->GetTint().Value.w * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectATint", aBufferTint, sizeof(aBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    std::get<GameObject*>(objectInProperties)->SetTint(ImColor(std::get<GameObject*>(objectInProperties)->GetTint().Value.x, std::get<GameObject*>(objectInProperties)->GetTint().Value.y, std::get<GameObject*>(objectInProperties)->GetTint().Value.z, std::stof(aBufferTint) / 255));
            //}
            // Components
            //int behaviourNum = 0;
            //for (Behaviour* behaviour : std::get<GameObject*>(objectInProperties)->GetBehaviours())
            //{
            //    behaviourNum++;
            //    float buttonWidth = ImGui::GetWindowWidth() - 15;
            //    float textWidth = ImGui::CalcTextSize(behaviour->GetName().c_str()).x;
            //    int numSpaces = (int)((buttonWidth - textWidth) / ImGui::CalcTextSize(" ").x / 2);
            //    std::string centeredText = std::string(numSpaces, ' ') + behaviour->GetName().c_str() + std::string(numSpaces, ' ');
            //    ImGui::Separator();
            //    //ImGui::NewLine();
            //    //ImGui::CollapseButton();
            //    ImGui::Text(centeredText.c_str());
            //    ImGui::SameLine();

            //    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Set button color to transparent
            //    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Set button hover color to transparent
            //    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Set button active color to transparent
            //    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
            //    if (ImGui::Button(("X##" + std::to_string(behaviourNum)).c_str()))
            //    {
            //        std::get<GameObject*>(objectInProperties)->RemoveBehaviour(behaviour);
            //    }
            //    ImGui::PopStyleColor(3);
            //    // Configeration properties here
            //}
            // Component button
            ImGui::NewLine();
            // Calculate the button width
            float buttonWidth = ImGui::GetWindowWidth() - 15;
            // Calculate the width of the button text
            float textWidth = ImGui::CalcTextSize("Add Behaviour").x;
            // Calculate the number of spaces needed to center the text
            int numSpaces = (int)((buttonWidth - textWidth) / ImGui::CalcTextSize(" ").x / 2);
            // Create the centered button text
            std::string centeredText = std::string(numSpaces, ' ') + "Add Behaviour " + std::string(numSpaces, ' ');

            // Create the button with the centered text
            if (ImGui::Button(centeredText.c_str(), ImVec2(buttonWidth, 0))) {
                componentsWindowOpen = true;
            }
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}

void Editor::RenderHierarchy()
{
    if (resetHierarchy)
    {
        ImGui::SetNextWindowSize(ImVec2(277, 736));
        ImGui::SetNextWindowPos(ImVec2(0, 52));
        resetHierarchy = false;
    }
    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;
    if (ImGui::Begin("Hierachy", nullptr, windowFlags))
    {
        for (int i = 0; i < SceneManager::GetActiveScene()->GetGameObjects().size(); i++) // Todo: I am doing this both in ShowProperties and ShowViewport, maybe merge them to use the same loop?
        {
            std::string objectName = SceneManager::GetActiveScene()->GetGameObjects()[i].GetName();
            if (objectName.length() > round(ImGui::GetWindowWidth() / 11)) {
                objectName = objectName.substr(0, round(ImGui::GetWindowWidth() / 11));
                objectName = objectName + "...";
            }
            if (ImGui::Button((objectName + "##" + std::to_string(SceneManager::GetActiveScene()->GetGameObjects()[i].GetId())).c_str(), ImVec2((ImGui::GetWindowWidth() - 15), 0)))
            {
                objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects()[i];
                selectedObject = &SceneManager::GetActiveScene()->GetGameObjects()[i];
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
        }
        // Add a scrollable text box
        //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        //ImGui::Text("Scrollable Text");
        //ImGui::EndChild();

        if (hierarchyContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
        {
            hierarchyContextMenuOpen = true;
            if (ImGui::BeginPopupContextWindow())
            {
                ImGui::Separator();

                std::string objectToCreate = "";

                if (ImGui::MenuItem("Create Empty"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Empty";
                }

                if (ImGui::MenuItem("Create Cube"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Cube";
                }

                if (ImGui::MenuItem("Create Capsule"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Capsule";
                }

                if (ImGui::MenuItem("Create Sphere"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Sphere";
                }

                if (ImGui::MenuItem("Create Plane"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Plane";
                }

                if (ImGui::MenuItem("Create Camera"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Camera";
                }

                if (objectToCreate != "")
                {
                    GameObject gameObject;
                    gameObject.SetModelPath(objectToCreate);
                    gameObject.transform.SetPosition({ 0,0,0 });
                    gameObject.transform.SetScale({ 1,1,1 });
                    gameObject.transform.SetRotation(QuaternionIdentity());
                    if (objectToCreate == "Empty") gameObject.SetName("GameObject");
                    else gameObject.SetName(objectToCreate);

                    SceneManager::GetActiveScene()->AddGameObject(gameObject);

                    selectedObject = &SceneManager::GetActiveScene()->GetGameObjects().back();
                    objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects().back();
                }

                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
}

void Editor::RenderTopbar()
{
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    ImGui::SetNextWindowSize(ImVec2(currentWidth, 25));
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    if (ImGui::Begin("Top Bar", nullptr, windowFlags))
    {
        // Play Button
        ImTextureID playIconTexture;
        //if (playing) playIconTexture = activePlayIcon;
        //else playIconTexture = playIcon;

        ImGui::SetCursorPos(ImVec2(currentWidth / 2 - 20, 2));

        if (rlImGuiImageButtonSize("##PlayButton", imageTextures["PlayIcon"], ImVec2(20, 20)))
        {
            //startStopPlaying = true;
            //paused = false;
        }

        // Pause Button
        ImTextureID pauseIconTexture;
        //if (paused) pauseIconTexture = activePauseIcon;
        //else if (playing) pauseIconTexture = pauseIcon;
        //else pauseIconTexture = grayedPauseIcon;

        ImGui::SetCursorPos(ImVec2(currentWidth / 2 + 20, 2));

        if (rlImGuiImageButtonSize("##PauseButton", imageTextures["GrayedPauseIcon"], ImVec2(20, 20)))
        {
            //if (playing) paused = !paused;
        }

    }
    ImGui::End();
}

void Editor::Render(void)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##EditorWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Project", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save Project", "Ctrl+S"))
            {
                ProjectManager::SaveProject();
            }
            if (ImGui::BeginMenu("Build Project"))
            {
                if (ImGui::MenuItem("Windows", ""))
                {
                    ProjectManager::BuildToWindows(projectData);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Project Settings", "")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Hierarchy", "")) {}
            if (ImGui::MenuItem("File Explorer", "")) {}
            if (ImGui::MenuItem("Properties", "")) {}
            if (ImGui::MenuItem("Sprite Editor", "")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

    RenderViewport();
    RenderHierarchy();
    RenderProperties();
    //RenderConsole();
    RenderFileExplorer();
    RenderTopbar();
}

void Editor::SetupViewport()
{
    ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    camera.fovy = 45;
    camera.up.y = 1;
    camera.position.y = 3;
    camera.position.z = -25;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = { 0.0f, 0.0f, 0.0f };

    //Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
    //GridTexture = LoadTextureFromImage(img);
    //UnloadImage(img);
    //GenTextureMipmaps(&GridTexture);
    //SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
    //SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);
}

void CloseViewport()
{
    UnloadRenderTexture(ViewTexture);
    UnloadTexture(GridTexture);
}

void Editor::InitFonts()
{
    //ImGuiIO& io = ImGui::GetIO();
    //CreateFont("Familiar-Pro-Bold", 30);
    //io.Fonts->AddFontDefault();
    //defaultFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Familiar-Pro-Bold.ttf", 30);
    //defaultFont15p = io.Fonts->AddFontFromFileTTF("resources/fonts/Familiar-Pro-Bold.ttf", 15);
    //markerBoldTitleFont = io.Fonts->AddFontFromFileTTF("resources/fonts/BoldMarker.ttf", 70);
    //Imgui_ImplRaylib_BuildFontAtlas();

    FontManager::CreateFonts("Familiar-Pro-Bold", { 15, 18, 20, 25, 30 });
    FontManager::CreateFont("BoldMarker", 90);
}

void Editor::InitStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 5;
}

void Editor::InitMisc()
{
    if (projectData.path.stem().string() != projectData.name)
        projectData.path = projectData.path / projectData.name;

    char* userProfile;
    size_t len;
    _dupenv_s(&userProfile, &len, "USERPROFILE");

    std::filesystem::path projectsPath = std::filesystem::path(userProfile) / "Documents" / "Cryonic Engine Projects";
    if (!std::filesystem::exists(projectsPath)) std::filesystem::create_directory(projectsPath);
}

void Editor::InitScenes()
{
    // Loads all scenes in /Assets/Scenes
    std::filesystem::path scenesPath = projectData.path / "Assets" / "Scenes";
    std::filesystem::path selectedScenePath;
    if (std::filesystem::exists(scenesPath))
    {
        for (const auto& file : std::filesystem::directory_iterator(scenesPath))
        {
            if (file.is_regular_file() && file.path().extension() == ".scene" && SceneManager::LoadScene(file.path()))
            {
                selectedScenePath = file.path();
            }
        }
    }
    // No scenes found, create a new one
    if (selectedScenePath.empty())
    {
        std::filesystem::create_directories(scenesPath);
        SceneManager::SetActiveScene(SceneManager::CreateScene());
        SceneManager::GetActiveScene()->SetPath(scenesPath / "Default.scene");
        SceneManager::SaveScene(SceneManager::GetActiveScene()); // Creates Default.scene
    }
    // Sets the last scene in the vector active
    else SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
}

void Editor::InitImages()
{
    for (const std::filesystem::path& path : std::filesystem::directory_iterator(std::filesystem::path(__FILE__).parent_path() / "resources" / "images"))
    {
        imageTextures[path.stem().string()] = new Texture2D(LoadTexture(path.string().c_str()));
    }
}

void Editor::Cleanup()
{
    for (auto& image : imageTextures)
    {
        UnloadTexture(*image.second);
        //delete image.second;
    }
    imageTextures.clear();

    for (auto& image : tempTextures)
    {
        UnloadTexture(*image);
        //delete image;
    }
    tempTextures.clear();

    for (auto& image : tempRenderTextures)
    {
        UnloadRenderTexture(*image);
        //delete image;
    }
    tempRenderTextures.clear();

    for (GameObject gameObject : SceneManager::GetActiveScene()->GetGameObjects())
    {
        if (std::filesystem::exists(gameObject.GetModelPath()))
            UnloadModel(gameObject.GetModel());
    }
    ImGui_ImplRaylib_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Init(ProjectData _projectData)
{
    projectData = _projectData;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI); // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    InitWindow(GetScreenWidth(), GetScreenHeight(), ("Cryonic Engine - v0.1-ALPHA - " + projectData.name).c_str());
    MaximizeWindow();
    SetWindowMinSize(100, 100);
    SetTargetFPS(144); // Todo: Set target FPS to monitor refresh rate and handle editor being moved across monitors or just take the higher refresh rate

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    InitStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplRaylib_Init();
    FontManager::InitFontManager();
    InitFonts();
    InitImages();
    InitMisc();
    InitScenes(); // Must go after InitMisc()

    SetupViewport();

    while (!WindowShouldClose())
    {
        FontManager::UpdateFonts();

        ImGui_ImplRaylib_ProcessEvents();

        ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();

        UpdateViewport();
        Render();

        // Rendering
        ImGui::Render();

        BeginDrawing();
        ClearBackground(DARKGRAY);

        ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
        EndDrawing();

        for (auto& image : tempTextures)
        {
            UnloadTexture(*image);
            delete image;
        }
        tempTextures.clear();

        for (auto& image : tempRenderTextures)
        {
            UnloadRenderTexture(*image);
        }
        tempRenderTextures.clear();
    }

    Cleanup();
    CloseWindow();
}
