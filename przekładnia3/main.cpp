// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

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
using namespace std;
// stałe w programie
#define N 2 // rząd systemu
#define h 0.001 // krok obliczeń
#define L 2.5 // liczba okresów sygnału sinus w przedziale T
#define PI 3.14159265 // liczba PI

// zmienne globalne w programie

double T = 10.0; // całkowity czas symulacji – przedział [0 , T]
std::vector <float> us((1.0 * T / h) + 1); // sygnał wejściowy sinus
std::vector <float> I((1.0 * T / h) + 1); // sygnał wyjściowy
std::vector <float> W((1.0 * T / h) + 1);
double M = 8; // amplituda sygnału wejściowego
//Box z; // zmienna: pojedyncza wartość sygnału (u lub y)


// Main code
int main(int, char**)
{
    //variables
    int i, f = 0, total; //f pomocniczy do trójkątnej
    double w;
    char signal = 's';
    std::vector <float> x_data((1.0 * T / h) + 1); //os x wykresow
    double x = 0; //zmienna pomocnicza
    bool signal_panel = false, down = false; //dodatkowe panele GUI

    parameters param;
    bool showPlotWindow = false; //do okna GUI



    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(0L, wc.lpszClassName, L"Projekt", (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L), 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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
            
            ImGui::InputDouble("J1", &param.J1, 1);
            ImGui::InputDouble("J2", &param.J2, 1);
            ImGui::InputDouble("n1", &param.n1, 1);
            ImGui::InputDouble("n2", &param.n2, 1);
            ImGui::InputDouble("R1", &param.R1, 1);
            ImGui::InputDouble("L1", &param.L1, 1);
            ImGui::InputDouble("ke", &param.ke, 1);
            ImGui::InputDouble("kt", &param.kt, 1);
            if (ImGui::Button("Sinus"))
            {
                signal = 's';
                signal_panel = true;
            };
            if (ImGui::Button("Prostokatna"))
            {
                signal = 'p';
                signal_panel = true;

            };
            if (ImGui::Button("Trojkatna"))
            {
                signal = 't';
                signal_panel = true;
            };

            if (signal_panel)
            {
                for (int d = 0; d < 10; d++)
                {
                    ImGui::Spacing();
                }

                ImGui::InputDouble("Amplituda", &M, 1);
                ImGui::InputDouble("T", &T, 1);

            }
            int k = 0;
            if (ImGui::Button("Plot"))
            {
                showPlotWindow = true;
                create_param(param);
                //counting(param);
                total = 10000; // rozmiar wektorów danych
                w = 2.0 * PI * L / T; // częstotliwość sinusoidy
                for (i = 0; i < total; i++) // obliczenie pobudzenia – sinus lub fala prostokątna
                {

                    if (sin(w * i * h) * M >= M || i==9000) down = true;         // drugie warunki, żeby było działało od 7000 w górę, do zmiany
                    if (sin(w * i * h) * M <= -M || i == 7000) down = false;

                    if (!down) f++;
                    else f--;

                    if (signal == 's') us[i] = M * sin(w * i * h); // sygnał wejściowy sinus: u=M*sin(w*t) , t=i*h
                    else if (signal == 'p') us[i] = (sin(w * i * h) > 0 ? M : -M); // sygnał wejściowy fala prostokątna
                    else if (signal == 't') us[i] = f * h * M; // sygnał wejściowy trójkątny

                }

                counting(param, us, I, W, total, h);

                for (int i = 0; i < total ; i++) //przekazywanie wartosci na wykresy
                {
                    x_data[i] = i;
                    
                }



                f = 0;
                signal_panel = false;

            }


        }
        ImGui::End();

        //Rysowanie wykresów
        if (showPlotWindow)
        {

            ImGui::Begin("Wykresy", &showPlotWindow); // Pass the address of the boolean variable to control the window's visibility
            ImPlot::SetNextAxesLimits(0, (1.0 * T / h), -M, M);

            if (ImPlot::BeginPlot("Wykresy"))
            {
                ImPlot::PlotLine("Us", x_data.data(), us.data(), (1.0 * T / h));
                ImPlot::PlotLine("I", x_data.data(), I.data(), (1.0 * T / h));
                ImPlot::PlotLine("W", x_data.data(), W.data(), (1.0 * T / h));
                ImPlot::EndPlot();
            }

            /*if (ImPlot::BeginPlot("I"))
            {
                ImPlot::PlotLine("I", x_data, prad, 10000);
                ImPlot::EndPlot();
            }

            if (ImPlot::BeginPlot("W"))
            {
                ImPlot::PlotLine("W", x_data, omega, 10000);
                ImPlot::EndPlot();
            }*/

            ImGui::End();

        }



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
