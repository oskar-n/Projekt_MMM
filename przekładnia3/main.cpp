
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "implot.h"
#include "implot_internal.h"
#include <d3d9.h>
#include <tchar.h>

#include <derivative.h>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};



// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// deklaracje zasięgów


// stałe w programie
#define N 2 // rząd systemu
#define PI 3.14159265 // liczba PI

// zmienne globalne w programie

//double T = 10.0; // całkowity czas symulacji – przedział [0 , T]
//double h = 0.001; // krok obliczeń
//double M = 8; // amplituda sygnału wejściowego
//double w = 1; //okres wygnalu wejsciowego
//std::vector <double> us((1.0 * T / h) + 1); // sygnał wejściowy 
//std::vector <double> I((1.0 * T / h) + 1); // natężenie prądu
//std::vector <double> W((1.0 * T / h) + 1); // prędkość kątowa
//



// Main code
int main(int, char**)
{
    //variables

    double T = 10.0; // całkowity czas symulacji – przedział [0 , T]
    double h = 0.001; // krok obliczeń
    double M = 8; // amplituda sygnału wejściowego
    double w = 1; //okres wygnalu wejsciowego
    std::vector <double> us((1.0 * T / h) + 1); // sygnał wejściowy 
    std::vector <double> I((1.0 * T / h) + 1); // natężenie prądu
    std::vector <double> W((1.0 * T / h) + 1); // prędkość kątowa

    double i, f = 0, total; //f pomocniczy do trójkątnej
    char signal = 's';
    std::vector <double> x_data((1.0 * T / h) + 1); //os x wykresow
    double x = 0; //zmienna pomocnicza
    bool signal_panel = false, down = false; //dodatkowe panele GUI

    parameters param;
    bool showPlotWindow = false; //do okna GUI



    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(0L, wc.lpszClassName, L"Projekt", (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L), 100, 100, 1280, 1200, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        // Część wyświetlająca ImGui
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        if (ImGui::Begin("Przekładnia", nullptr, ImGuiWindowFlags_NoDecoration))
        {
            ImGui::PushItemWidth(100);
            ImGui::InputDouble("J1", &param.J1, 1);
            ImGui::InputDouble("J2", &param.J2, 1);
            ImGui::InputDouble("n1", &param.n1, 1);
            ImGui::InputDouble("n2", &param.n2, 1);
            ImGui::InputDouble("R1", &param.R1, 1);
            ImGui::InputDouble("L1", &param.L1, 1);
            ImGui::InputDouble("ke", &param.ke, 1);
            ImGui::InputDouble("kt", &param.kt, 1);
            ImGui::PopItemWidth();
            ImGui::Text("Krok obliczen: %f", h);
            
            if (ImGui::Button("0.01"))
            {
                
                h = 0.01;
            }
            ImGui::SameLine();
            if (ImGui::Button("0.001"))
			{
				h = 0.001;
			}
            ImGui::SameLine();
            if (ImGui::Button("0.0001"))
            {
                h = 0.0001;
            }

            ImGui::Text("Sygnal wejsciowy: %c", signal);

            if (ImGui::Button("Sinus"))
            {
                signal = 's';
                signal_panel = true;
            };
            ImGui::SameLine();
            if (ImGui::Button("Prostokatna"))
            {
                signal = 'p';
                signal_panel = true;

            };
            ImGui::SameLine();
            if (ImGui::Button("Trojkatna"))
            {
                signal = 't';
                signal_panel = true;
            };
            ImGui::SameLine();
            if (ImGui::Button("Heavyside"))
            {
                signal = 'h';
                signal_panel = true;
            };
            ImGui::SameLine();
            if (ImGui::Button("Dirac"))
            {
                signal = 'd';
                signal_panel = true;
            }
            

            ImGui::PushItemWidth(200);
            if (signal_panel)
            {
                for (int d = 0; d < 10; d++)
                {
                    ImGui::Spacing();
                }

                if (signal == 's' || signal == 'p' || signal == 't')
                {
                    ImGui::InputDouble("Amplituda", &M, 0.5);
                    ImGui::InputDouble("Czas symulacji", &T, 0.5);
                    ImGui::InputDouble("Okres", &w, 0.5);
                }
                else
                {
                    
                    ImGui::InputDouble("Wartosc", &M, 0.5);
                    ImGui::InputDouble("Czas symulacji", &T, 0.5);
                }

            }
            ImGui::PopItemWidth();


            us.resize((1.0 * T / h) + 1);
            
            for(int i = 0; i < 10; i++)
			{
				ImGui::Spacing();
			}
   
            if (ImGui::Button("Symulacja",ImVec2(200, 50)))
            {
                showPlotWindow = true;
                create_param(param);
                
                total = (1.0 * T / h) + 1; // rozmiar wektorów danych
              
                us.clear();
                us.resize(total);
                
                double slope = (4.0 * M) / w; // Slope dla sygnału trójkątnego
                double current_value = M; // start od -M
                bool increasing = true;

                for (i = 0; i < total; i++) // obliczenie pobudzenia – sinus lub fala prostokątna
                {
                    if (signal == 's') 
                        us[i] = M * sin((2*PI/w) * i * h); // sygnał wejściowy sinus: u=M*sin(w*t) , t=i*h

                    else if (signal == 'p') 
                        us[i] = (sin((2*PI/w) *i * h) > 0 ? M : -M); // sygnał wejściowy fala prostokątna

                    else if (signal == 't') {
                        if (increasing) {
                            current_value += slope * h;
                            if (current_value >= M) {
                                current_value = M;
                                increasing = false;
                            }
                        }
                        else {
                            current_value -= slope * h;
                            if (current_value <= -M) {
                                current_value = -M;
                                increasing = true;
                            }
                        }
                        us[i] = current_value; // sygnał wejściowy trójkątny
                    }

                    else if (signal == 'h')
                    {
                        us[i] = M;
                    }

                    else if (signal == 'd')
                    {
                        us[0] = M;
                    }
                }

                I.clear(); W.clear();
                I.resize(total); W.resize(total); // zmiana rozmiaru wektorów danych
      
                counting(param, us, I, W, total, h);

                x_data.clear();
                x_data.resize(total);

                for (int i = 0; i < total-1 ; i++) //przekazywanie wartosci na wykresy
                {
                    x_data[i] = i/(total);                   
                }

                f = 0;
                signal_panel = false;

            }

        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)) && showPlotWindow==false)
        {
            
            done = true;
        }

       

        //Rysowanie wykresów
        if (showPlotWindow)
        {
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
            {
                showPlotWindow = false;
            }

            ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::Begin("Wykresy", &showPlotWindow); // 

            // Wykres dla 'Us'
            ImPlot::SetNextAxesToFit();
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // czerwnony kolor
            if (ImPlot::BeginPlot("Us"))
            {
                ImPlot::PlotLine("Us", x_data.data(), us.data(), (1.0 * T / h));
                ImPlot::EndPlot();
            }

            ImGui::Spacing(); 

            // Wykres dla 'I'
            ImPlot::SetNextAxesToFit();
            ImPlot::SetNextLineStyle(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // zielony kolor
            if (ImPlot::BeginPlot("I"))
            {
                ImPlot::PlotLine("I", x_data.data(), I.data(), (1.0 * T / h));
                ImPlot::EndPlot();
            }

            ImGui::Spacing(); 

            // Wykres dla 'W'
            ImPlot::SetNextAxesToFit();
            ImPlot::SetNextLineStyle(ImVec4(0.0f, 0.0f, 1.0f, 1.0f)); // Niebieski kolor
            if (ImPlot::BeginPlot("W"))
            {
                ImPlot::PlotLine("W", x_data.data(), W.data(), (1.0 * T / h));
                ImPlot::EndPlot();
            }

            ImGui::End();
        }

        ImGui::End();

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
