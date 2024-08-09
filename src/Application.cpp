//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#include <islay/Application.h>

#include <islay/imgui_apps.h>
#include <islay/ImageTexture.h>
#include "AppMsg.h"
#include <islay/Config.h>
#include <islay/Logger.h>
#include <islay/Utility.h>
#include <implot.h>

#include <opencv2/opencv.hpp>

#include "Engine.h"

Application::Application() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        printf("Error: %s\n", SDL_GetError());

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
//    const char* glsl_version = "#version 150";
    glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
//    const char* glsl_version = "#version 130";
    glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

// Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
#ifdef DEBUG
    window = SDL_CreateWindow("islay - debug", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 900, window_flags);
#else
    window = SDL_CreateWindow("islay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 900, window_flags);
#endif
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    }

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
}

Application::~Application(){
// Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Application::run(){

// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); // (void)io;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

// Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);    // Set background color

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

// Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());

// Load Fonts
// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
// - Read 'misc/fonts/README.txt' for more instructions and details.
// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//io.Fonts->AddFontDefault();
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
//IM_ASSERT(font != NULL);
#ifdef _MSC_VER
	ImFont* font = io.Fonts->AddFontFromFileTTF("../3rdparty/imgui/misc/fonts/mplus-1p-medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);
#else
	ImFont* font = io.Fonts->AddFontFromFileTTF("../3rdparty/imgui/misc/fonts/mplus-1p-medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);
#endif

// Window capture and recording
    bool requestedWindowCapture = false;
    enum WINDOW_RECORDING_STATUS {PAUSED = 0, REQUESTED = 1, RECORDING = 2};
    WINDOW_RECORDING_STATUS windowRecordingStatus = WINDOW_RECORDING_STATUS::PAUSED;
    cv::VideoWriter writer;
    std::string windowRecordingFileName;

// Initialize application config
    Config::get_instance();

// Setup logger
    ExampleAppLog my_log;
    Logger::get_instance().setExportDirectory(Config::get_instance().resultDirectory());

    AppMsgPtr appMsg = std::make_shared<AppMsg>();
    std::shared_ptr<Engine> engine(new Engine(appMsg));
    std::map<std::string, ImageTexture> texturePool;
    std::map<std::string, ImVec2> textureSizePool;
    auto clearTexturePool=[&](){
        texturePool.clear();
        textureSizePool.clear();
    };

    enum SHOW_IMAGE_MODE {IMGUI = 0, OPENCV = 1};
    static int selectedShowImageMode = SHOW_IMAGE_MODE::IMGUI;

// Main loop
    bool done = false;
    while (!done)
    {
        /// Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        /// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        /// GUI controller window
        static ImVec2 guiControllerWindowSize, guiControllerWindowPos;
        {
            const float DISTANCE = 10.0f;
            static float f = 0.0f;
            ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            ImGui::SetNextWindowSize(ImVec2(300,240), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            if (ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
            {
                {
                    ImGui::Text("GUI runs at %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                }
                {
                    ImGui::Text("Image Rendering Mode");
                    ImGui::Indent();
                    ImGui::RadioButton("ImGui", &selectedShowImageMode, SHOW_IMAGE_MODE::IMGUI); ImGui::SameLine();
                    ImGui::RadioButton("OpenCV", &selectedShowImageMode, SHOW_IMAGE_MODE::OPENCV);
                    ImGui::Unindent();
                }
                {
                    static float f = 0.0f;
                    ImGui::Text("Window Capture");
                    ImGui::Indent();
                    if (ImGui::Button("Capture")) {
                        requestedWindowCapture = true;
                    }
                    ImGui::Unindent();
                    ImGui::Text("Window Recording");
                    ImGui::Indent();
                    if (ImGui::Button("Start")) {
                        if (windowRecordingStatus == WINDOW_RECORDING_STATUS::PAUSED) {
                            int fps_encode = 60;
                            windowRecordingFileName = "recording_" + Util::now() + ".mp4";
                            SPDLOG_INFO("Video recording start: {}", windowRecordingFileName);
                            // FIXME: Framerate doesn't concide with the refresh rate, which can result in slow-mo video.
                            writer = cv::VideoWriter(
                                    Config::get_instance().resultDirectory() + "/" + windowRecordingFileName,
                                    cv::VideoWriter::fourcc('H', '2', '6', '4'), fps_encode,
                                    cv::Size((int) io.DisplaySize.x * (int) io.DisplayFramebufferScale.x,
                                             (int) io.DisplaySize.y * (int) io.DisplayFramebufferScale.y));
                            windowRecordingStatus = WINDOW_RECORDING_STATUS::REQUESTED;
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Stop")) {
                        SPDLOG_INFO("Video recording end");
                        windowRecordingStatus = WINDOW_RECORDING_STATUS::PAUSED;
                        writer.release();
                    }
                    ImGui::SameLine();
                    if (windowRecordingStatus == WINDOW_RECORDING_STATUS::PAUSED) {
                        ImGui::Text("PAUSED");
                    } else if (windowRecordingStatus == WINDOW_RECORDING_STATUS::REQUESTED) {
                        ImGui::Text("REQUESTED");
                    } else if (windowRecordingStatus == WINDOW_RECORDING_STATUS::RECORDING) {
                        ImGui::Text("%s", "RECORDING...");
                    }
                    ImGui::Unindent();
                }
                {
                    ImGui::Text("Exit program");
                    ImGui::Indent();
                    if (ImGui::Button("Exit")) {
                        done = true;
                    }
                }
            }
            guiControllerWindowSize = ImGui::GetWindowSize();
            guiControllerWindowPos = ImGui::GetWindowPos();
            ImGui::End();
        }

        /// Dear ImGui demo
        {
            ImGui::ShowDemoWindow();
        }

        /// Workers window
        static ImVec2 workerWindowSize, workerWindowPos;
        {
            const float DISTANCE = 10.0f;
            static float f = 0.0f;
            ImVec2 window_pos = ImVec2(DISTANCE, guiControllerWindowPos.y + guiControllerWindowSize.y + DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            ImGui::SetNextWindowSizeConstraints(ImVec2(300, 0), ImVec2(300, 600));
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            if(ImGui::Begin("Commands", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove ))
            {
                ImGui::Text("Workers:");
                {
                    ImGui::NewLine(); ImGui::SameLine();
                    ImGui::Text("WorkerSample");
                    ImGui::NewLine(); ImGui::SameLine();
                    if (ImGui::Button("Launch##WorkerSample")) {
                        engine->runWorkerSample();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Terminate##WorkerSample")) {
                        engine->terminateWorker("WorkerSample");
                    }
                }
                {
                    ImGui::NewLine(); ImGui::SameLine();
                    ImGui::Text("WorkerSample with CPU binding");
                    ImGui::NewLine(); ImGui::SameLine();
                    if (ImGui::Button("Launch##WorkerSampleWithCpuBinding")) {
                        engine->runWorkerSampleWithCpuBinding();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Terminate##WorkerSampleWithCpuBinding")) {
                        engine->terminateWorker("WorkerSampleWithCpuBinding");
                    }
                }
                {// Add your worker here as above

                }
                ImGui::NewLine(); ImGui::SameLine();
                ImGui::Text("Delete workers");
                ImGui::NewLine(); ImGui::SameLine();
                if (ImGui::Button("Delete workers")) {
                    engine->deleteAllWorker();
                }
                ImGui::Separator();
                {
                    ImGui::Text("Worker Status:");
                    ImVec2 child_size = ImVec2(0, ImGui::GetFontSize() * 5.0f);
                    ImGui::BeginChild("##ScrollingRegion_worker-status", child_size, false, ImGuiWindowFlags_HorizontalScrollbar);
                    for (auto &name: engine->getWorkerList()) {
                        ImGui::NewLine();
                        WORKER_STATUS observedWorkerStatus;
                        observedWorkerStatus = engine->getWorkerStatus(name);
                        if (observedWorkerStatus == WORKER_STATUS::JOINABLE) {
                            engine->resetWorker(name);
                        }
                        ImGui::SameLine();
                        if (observedWorkerStatus == WORKER_STATUS::IDLE) {
                            ImGui::Text("%s: idle", name.c_str());
                        } else if (observedWorkerStatus == WORKER_STATUS::RUNNING) {
                            ImGui::Text("%s: running", name.c_str());
                        } else if (observedWorkerStatus == WORKER_STATUS::TERMINATE_REQUESTED) {
                            ImGui::Text("%s: terminate requested", name.c_str());
                        } else if (observedWorkerStatus == WORKER_STATUS::JOINABLE) {
                            ImGui::Text("%s: joinable", name.c_str());
                        } else {
                            ImGui::Text("%s: unknown", name.c_str());
                        }
                        int puIndIfBinded = engine->getPuIfBinded(name);
                        if(puIndIfBinded != -1){
                            ImGui::SameLine();
                            ImGui::Text("(PU:%d)", puIndIfBinded) ;
                        }
                        ImGui::NextColumn();
                    }
                    ImGui::EndChild();

                }
                workerWindowPos = ImGui::GetWindowPos();
                workerWindowSize = ImGui::GetWindowSize();
                ImGui::End();
            }
        }

        /// Shown images window
        {
            const float DISTANCE = 10.0f;
            static float f = 0.0f;
            ImVec2 window_pos = ImVec2(DISTANCE, workerWindowPos.y + workerWindowSize.y + DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            ImGui::SetNextWindowSizeConstraints(ImVec2(300, 0), ImVec2(300, 600));
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            if(ImGui::Begin("Images", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove)) {
                ImGui::Text("Images:");
                ImGui::NewLine();
                ImGui::SameLine();
                // TODO: make items clickable and the clicked window be active
                if (ImGui::Button("Delete images")) {
                    if(selectedShowImageMode == SHOW_IMAGE_MODE::IMGUI){
                        clearTexturePool();
                        appMsg->ocvImageMsgCollection.clear();
                    } else if (selectedShowImageMode == SHOW_IMAGE_MODE::OPENCV){
                        cv::destroyAllWindows();
                    }
                }

                ImVec2 child_size = ImVec2(0, ImGui::GetFontSize() * 5.0f);
                ImGui::BeginChild("##ScrollingRegion_image", child_size, false, ImGuiWindowFlags_HorizontalScrollbar);
                static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                int id=0;
                for (const auto &texture: texturePool)
                {
                    ImGui::TreeNodeEx((void *) (intptr_t) id++, node_flags, "%s", texture.first.c_str());
                    if (ImGui::IsItemClicked()){
                        SPDLOG_INFO("{} pressed", texture.first.c_str());
                        ImGui::SetWindowFocus(texture.first.c_str());
                    }
                }
                ImGui::EndChild();
                ImGui::End();
            }
        }

        /// Display images
        {
            // Destroy OpenCV windows if exists
            if(selectedShowImageMode == SHOW_IMAGE_MODE::IMGUI) { /// Use ImGui
                cv::destroyAllWindows();
            }

            // Render images in texturePool
            for (auto &e: appMsg->ocvImageMsgCollection.pool) {
                auto msg = e.second->receive();
                if (msg != nullptr) {
                    if (selectedShowImageMode == SHOW_IMAGE_MODE::IMGUI) {
                        std::string winname = e.first;
                        if (textureSizePool.count(winname) == 0) {
                            ImGui::SetNextWindowSize(ImVec2(msg->img.cols, msg->img.rows));
                            textureSizePool[winname] = ImVec2(msg->img.cols, msg->img.rows);
                        } else {
                            ImGui::SetNextWindowSize(ImVec2(textureSizePool[winname].x, textureSizePool[winname].y));
                        }
                        if (ImGui::Begin(winname.c_str())) {
                            bool isWindowCollapsed = ImGui::IsWindowCollapsed();
                            texturePool[winname].setImage(&msg->img);
                            ImGui::Image(texturePool[winname].getOpenglTexture(),
                                         ImVec2(textureSizePool[winname].x - 20, textureSizePool[winname].y - 40),
                                         ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)
                            );
                            if (!isWindowCollapsed) {
                                float scale = std::min<float>(ImGui::GetWindowSize().x / textureSizePool[winname].x,
                                                              ImGui::GetWindowSize().y / textureSizePool[winname].y);
                                textureSizePool[winname] = ImVec2(textureSizePool[winname].x * scale,
                                                                  textureSizePool[winname].y * scale);
                            }
                        }
                        ImGui::End();
                    } else if (selectedShowImageMode == SHOW_IMAGE_MODE::OPENCV) {
                        std::string winname = e.first;
                        cv::namedWindow(winname, cv::WINDOW_NORMAL);
                        cv::imshow(winname, msg->img);
                    }
                } else {
                    if (selectedShowImageMode == SHOW_IMAGE_MODE::IMGUI) {
                        std::string winname = e.first;
                        ImGui::SetNextWindowSize(ImVec2(textureSizePool[winname].x, textureSizePool[winname].y));
                        if (ImGui::Begin(winname.c_str())) {
                            bool isWindowCollapsed = ImGui::IsWindowCollapsed();
                            ImGui::Image(texturePool[winname].getOpenglTexture(),
                                         ImVec2(textureSizePool[winname].x - 20, textureSizePool[winname].y - 40),
                                         ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f)
                            );
                            if (!isWindowCollapsed) {
                                float scale = std::min<float>(ImGui::GetWindowSize().x / textureSizePool[winname].x,
                                                              ImGui::GetWindowSize().y / textureSizePool[winname].y);
                                textureSizePool[winname] = ImVec2(textureSizePool[winname].x * scale,
                                                                  textureSizePool[winname].y * scale);
                            }
                        };
                        ImGui::End();
                    } else if (selectedShowImageMode == SHOW_IMAGE_MODE::OPENCV) {
                        cv::waitKey(1);
                    }
                }
            }
        }

        /// Plot window
        {
            static float xs1[1001], ys1[1001];
            double DEMO_TIME = ImGui::GetTime();
            for (int i = 0; i < 1001; ++i) {
                xs1[i] = i * 0.001f;
                ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)DEMO_TIME / 10));
            }
            static double xs2[11], ys2[11];
            for (int i = 0; i < 11; ++i) {
                xs2[i] = i * 0.1f;
                ys2[i] = xs2[i] * xs2[i];
            }
            ImGui::Begin("Plot");
            ImGui::BulletText("Anti-aliasing can be enabled from the plot's context menu (see Help).");
            if (ImPlot::BeginPlot("Line Plot", "x", "f(x)")) {
                ImPlot::PlotLine("sin(x)", xs1, ys1, 1001);
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                ImPlot::PlotLine("x^2", xs2, ys2, 11);
                ImPlot::EndPlot();
            }
            ImGui::End();
        }

        /// Config window
        float configHeight = 160;
        {
            const float DISTANCE = 10.0f;
            ImVec2 window_size = io.DisplaySize;
            ImVec2 window_pos = ImVec2(window_size.x/2, DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            ImGui::SetNextWindowSize(ImVec2(window_size.x/2-DISTANCE,configHeight), ImGuiCond_Once);
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            DrawJsonConfig("config", Config::get_instance().getDocument());
        }

        /// Logger window
        {
            Logger::get_instance().logger->flush();
            my_log.AddLog( "%s", Logger::get_instance().oss.str().c_str() );
            Logger::get_instance().oss.str("");
            Logger::get_instance().oss.clear();
            const float DISTANCE = 10.0f;
			ImVec2 window_size = io.DisplaySize;
			ImVec2 window_pos = ImVec2(window_size.x/2, DISTANCE*2+configHeight);
			ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowSize(ImVec2(window_size.x/2-DISTANCE,window_size.y-3*DISTANCE-configHeight), ImGuiCond_Once);
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
            my_log.Draw("Log");
        }

        /// Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        /// Screen capture and recording
        if (requestedWindowCapture) {
            glFinish();
            int width = (int) io.DisplaySize.x * (int) io.DisplayFramebufferScale.x;
            int height = (int) io.DisplaySize.y * (int) io.DisplayFramebufferScale.y;
            int type = CV_8UC3;
            int format = GL_BGR;
            glReadBuffer(GL_BACK);
            static cv::Mat out_img;
            out_img = cv::Mat(cv::Size(width, height), type);
            glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, out_img.data);
            cv::flip(out_img, out_img, 0);
            cv::imwrite((Config::get_instance().resultDirectory() + "/capture_" + Util::now() + ".png").c_str(),out_img,std::vector<int>(cv::IMWRITE_PNG_COMPRESSION));
            SPDLOG_INFO("Window captured: {}", "capture_" + Util::now() + ".png");
            requestedWindowCapture = false;
        }
        if (windowRecordingStatus >= WINDOW_RECORDING_STATUS::REQUESTED) {
            /// NOTE: For better color representation, consider using ffmpeg
            windowRecordingStatus = WINDOW_RECORDING_STATUS::RECORDING;
            glFinish();
            int width = (int) io.DisplaySize.x * (int) io.DisplayFramebufferScale.x;
            int height = (int) io.DisplaySize.y * (int) io.DisplayFramebufferScale.y;
            int type = CV_8UC3;
            int format = GL_BGR;
            glReadBuffer(GL_BACK);
            static cv::Mat out_img;
            out_img = cv::Mat(cv::Size(width, height), type);
            glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, out_img.data);
            cv::flip(out_img, out_img, 0);
            writer << out_img;
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(window);
    }

    engine->terminateAll(); // Request all workers to terminate
    engine->reset(); // Join all threads of workers

    SPDLOG_INFO("Program terminated successfully. See you!");

    return true;
}


