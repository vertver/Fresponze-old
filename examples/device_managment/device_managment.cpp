/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2020. All rights reserved.
* Fresponze - fast, simple and modern multimedia sound library
* Apache-2 License
**********************************************************************
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************/
#include <windows.h>
#include <stdio.h>
#include "Fresponze.h"
#include "FresponzeFileSystemWindows.h"
#include "FresponzeWavFile.h"
#include "FresponzeListener.h"
#include "FresponzeMixer.h"
#include "FresponzeMasterEmitter.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "spectrum.h"
#pragma comment(lib, "d3d11.lib")

IFresponze* pFresponze = nullptr;

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

fr_i32 OutputCount = 0;
fr_i32 InputCount = 0;
ListenersNode* listNode = nullptr;
PcmFormat format = {};
EndpointInformation OutputsLists = {};
EndpointInformation* OutputLists = {};
EndpointInformation InputsLists = {};
EndpointInformation* InputLists = {};
IBaseEmitter* pBaseEmitter = nullptr;
IBaseEmitter* pBaseEmitterSecond = nullptr;
IAudioHardware* pAudioHardware = nullptr;
IAdvancedMixer* pAdvancedMixer = nullptr;
IAudioCallback* pAudioCallback = nullptr;

const char* items[64] = { };
const char* items_delay_names[] = { "30", "50", "80", "100", "150", "200", "1000" };
float items_delay[] = { 30.f, 50.f, 80.f, 100.f, 150.f, 200.f, 1000.f };

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DrawImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		static int current_delay = 1;
		static int current_item = 0;
		static float volume = 1.0f;
		static int counter = 0;
		static bool is_already_runned = false;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("Fresponze Device enumerating", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::ListBox("Delays", &current_delay, items_delay_names, 7);
		if (ImGui::Button("Run", ImVec2(80, 35))) {
			if (is_already_runned) {
				pAudioHardware->Close();
				if (listNode) {
					pAdvancedMixer->DeleteListener(listNode);
					listNode = nullptr;
				}

				is_already_runned = false;
			}

			fr_i32 samples = OutputLists[current_item].EndpointFormat.SampleRate * (items_delay[current_delay] / 1000.f);
			OutputLists[current_item].EndpointFormat.Frames = samples;
			if (pAudioHardware->Open(RenderType, items_delay[current_delay], OutputLists[current_item].EndpointUUID)) {
				pAdvancedMixer->SetBufferSamples(samples);
				if (pAdvancedMixer->CreateListener((void*)"X:\\test.opus", listNode, OutputLists[current_item].EndpointFormat)) {
					pAdvancedMixer->AddEmitterToListener(listNode, pBaseEmitter);
					pBaseEmitter->SetState(eReplayState);
					pAdvancedMixer->SetMixFormat(OutputLists[current_item].EndpointFormat);
				}

				is_already_runned = true;
			}
		}

		if (ImGui::SliderFloat("Volume level", &volume, 0.0, 1.0f)) {
			pBaseEmitter->SetOption(0, &volume, sizeof(float));
		}

		ImGui::ListBox("Devices", &current_item, items, OutputCount);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	g_pSwapChain->Present(1, 0);
}

// Main code
int main(int, char**)
{
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Fresponze device enumerating"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Fresponze device enumerating"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 480, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Spectrum::StyleColorsSpectrum();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;
	ImFont* Fonts[16] = {};

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic
		0,
	};

	Fonts[0] = io.Fonts->AddFontFromFileTTF("Montserrat-Medium.ttf", 18.0f, &font_config, ranges);
	io.Fonts->Build();

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;

	if (FrInitializeInstance((void**)&pFresponze) != 0) {
		return -1;
	}

	pFresponze->GetMixerInterface(eMixerAdvancedType, (void**)&pAdvancedMixer);
	pAdvancedMixer->CreateEmitter(pBaseEmitter, 0);
	pAudioCallback = new CMixerAudioCallback(pAdvancedMixer);
	pFresponze->GetHardwareInterface(eEndpointWASAPIType, pAudioCallback, (void**)&pAudioHardware);

	if (!pAudioHardware->Enumerate()) {
		MessageBoxA(NULL, "Can't enumerate devices", "Fresponze error", MB_OK | MB_ICONERROR);
		return -1;
	}

	pAudioHardware->GetDevicesList(InputLists, OutputLists);
	pAudioHardware->GetDevicesCount(CaptureType, InputCount);
	pAudioHardware->GetDevicesCount(RenderType, OutputCount);
	for (size_t i = 0; i < OutputCount; i++) {
		items[i] = OutputLists[i].EndpointName;
	}

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		DrawImGui();
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Window_Flag_Resizeing = false;

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ENTERSIZEMOVE:
		/*
			HACK: with this timer, we can draw window with new size
			while this window in resizing state
		*/
		SetTimer(hWnd, 2, 4, NULL);
		Window_Flag_Resizeing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		KillTimer(hWnd, 2);
		Window_Flag_Resizeing = false;
		return 0;
	case WM_PAINT:
		if (g_pd3dDevice != NULL) {
			//if (Window_Flag_Resizeing) {
			DrawImGui();
			//}
		}
		break;
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_pd3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			CleanupRenderTarget();

			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		} else {
			Sleep(1);
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}


	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


