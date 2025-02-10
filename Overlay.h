#pragma once
#include "framework.h"

namespace Overlay
{
	Present oPresent;
	HWND window = NULL;
	WNDPROC oWndProc;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* mainRenderTargetView;

	void SetupImGuiStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	}

	void InitImGui()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// Remove the NoMouseCursorChange flag to allow mouse cursor change
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);

		SetupImGuiStyle();
	}

	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	void Watermark()
	{
		static bool p_open = true;
		const float DISTANCE = 15.0f;
		static int corner = 2;
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		}
		ImGui::SetNextWindowBgAlpha(0.725f);

		if (ImGui::Begin("Watermark", &p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text("NX-Finder - Beta");
			ImGui::Separator();
			ImGui::Text("Developed by @nax1800");
			ImGui::Text("F4 to toggle menu visibility.");
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			}
		}
		ImGui::End();
	}

	static bool bOpenMenu = true;

	void Menu()
	{
		ImGuiIO& io = ImGui::GetIO();

		if (bOpenMenu)
		{
			// Capture the mouse when the menu is open
			io.WantCaptureMouse = true;

			// Set the mouse cursor to visible
			if (!io.MouseDrawCursor)
			{
				io.MouseDrawCursor = true;
			}

			ImGui::Begin("NX-Finder");
			ImGui::SetWindowSize("NX-Finder", { 900, 750 }, ImGuiWindowFlags_NoResize);
			ImGui::BeginTabBar("TabBar");
			if (ImGui::BeginTabItem("Game"))
			{
				ImGui::BeginTabBar("GameTabBar");
				if (ImGui::BeginTabItem("Live Actors"))
				{
					static vector<UObject*> LiveActors{};
					static bool bActorsLoaded = !LiveActors.empty();
					if (!bActorsLoaded && ImGui::Button("Load Actors"))
					{
						UObject* World = Globals::GetWorld();
						LogInfo("World: {}", World->GetFullName());
						auto Actors = Globals::GetAllActors();
						if (Actors.IsValid())
						{
							for (int i = 0; i < Actors.Num(); i++)
							{
								UObject* Actor = Actors[i];
								if (!Actor)
									continue;

								LiveActors.push_back(Actor);
							}
						}

						bActorsLoaded = !LiveActors.empty();
					}
					if (bActorsLoaded && ImGui::Button("Refresh Actors"))
					{
						UObject* World = Globals::GetWorld();
						LogInfo("World: {}", World->GetFullName());
						auto Actors = Globals::GetAllActors();
						if (Actors.IsValid())
						{
							for (int i = 0; i < Actors.Num(); i++)
							{
								UObject* Actor = Actors[i];
								if (!Actor)
									continue;

								LiveActors.push_back(Actor);
							}
						}

						bActorsLoaded = !LiveActors.empty();
					}
					if (bActorsLoaded)
					{
						for (UObject* Actor : LiveActors)
						{
							if (ImGui::TreeNode(format("{}", Actor->GetFullName()).c_str()))
							{
								ImGui::Text(format("Class: {}", Actor->Class->GetFullName()).c_str());
								ImGui::Separator();
								vector<UProperty*> ActorProperties = Actor->GetProperties();
								if (!ActorProperties.empty())
								{
									for (UProperty* ActorProp : ActorProperties)
									{
										if (!ActorProp)
											continue;

										if (ActorProp->GetFullName().starts_with("ObjectProperty "))
										{
											auto PropertyYuh = *(UObject**)(__int64(Actor) + ActorProp->Offset);
											ImGui::Text(format("ObjectProperty: {}: {}", ActorProp->GetName(), PropertyYuh->GetFullName()).c_str());
										}
										else if (ActorProp->GetFullName().starts_with("IntProperty "))
										{
											auto PropertyYuh = *(uint8*)(__int64(Actor) + ActorProp->Offset);
											ImGui::Text(format("IntProperty: {}: {}", ActorProp->GetName(), PropertyYuh).c_str());
										}
										else if (ActorProp->GetFullName().starts_with("BoolProperty "))
										{
											auto PropertyYuh = *(bool*)(__int64(Actor) + ActorProp->Offset);
											ImGui::Text(format("BoolProperty: {}: {}", ActorProp->GetName(), PropertyYuh).c_str());
										}
										else if (ActorProp->GetFullName().starts_with("NameProperty "))
										{
											auto PropertyYuh = *(FName*)(__int64(Actor) + ActorProp->Offset);
											ImGui::Text(format("NameProperty: {}: {}", ActorProp->GetName(), PropertyYuh.ToString()).c_str());
										}
										else if (ActorProp->GetFullName().starts_with("FloatProperty "))
										{
											auto PropertyYuh = *(float*)(__int64(Actor) + ActorProp->Offset);
											ImGui::Text(format("FloatProperty: {}: {}", ActorProp->GetName(), PropertyYuh).c_str());
										}
									}
								}
								ImGui::TreePop();
							}
						}
					}
				}
				ImGui::EndTabItem();
				ImGui::EndTabBar();
			}
			if (ImGui::BeginTabItem("Class Inspector"))
			{
				static bool bFailed = false;
				static char buffer[128] = "";
				static string ClassName = "";
				static vector<UProperty*> ClassProps{};
				static vector<UFunction*> ClassFuncs{};
				ImGui::InputText("Class Name", buffer, sizeof(buffer));
				if (bFailed) ImGui::Text(format("{} is an invalid class.", ClassName).c_str());
				if (ImGui::Button("Inspect"))
				{
					ClassName = buffer;
					auto Class = UE4::Utils::GetDefaultObj(buffer);

					if (Class)
					{
						ClassName = Class->GetName();
						ClassProps.clear();
						ClassFuncs.clear();
						for (UProperty* Property : Class->GetProperties())
						{
							if (Property)
								ClassProps.push_back(Property);
						}

						for (UFunction* Function : Class->GetFunctions())
						{
							if (Function)
								ClassFuncs.push_back(Function);
						}

						bFailed = false;
					}
					else
						bFailed = true;
				}

				ImGui::Text(format("{}", ClassName).c_str());
				ImGui::Separator();
				if (!ClassProps.empty() && ImGui::TreeNode("Properties"))
				{
					for (UProperty* Prop : ClassProps)
					{
						ImGui::Separator();
						if (ImGui::TreeNode(format("{}", Prop->GetFullName()).c_str()))
						{
							ImGui::Text(format("Offset: 0x{:x}", Prop->Offset).c_str());
							ImGui::Text(format("ArrayDim: 0x{:x}", Prop->ArrayDim).c_str());
							ImGui::Text(format("ElementSize: 0x{:x}", Prop->ElementSize).c_str());
							ImGui::Text(format("PropertyFlags: {}", Prop->PropertyFlags).c_str());
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				if (!ClassFuncs.empty() && ImGui::TreeNode("Functions"))
				{
					for (UFunction* Func : ClassFuncs)
					{
						if (!Func)
							continue;
						ImGui::Separator();
						if (ImGui::TreeNode(format("{}", Func->GetFullName()).c_str()))
						{
							ImGui::Text(format("Exec: 0x{:x}", Memory::GetOffset(Func->ExecFunction)).c_str());
							vector<UProperty*> FuncProps = Func->GetFuncProperties();
							for (UProperty* Prop : FuncProps)
							{
								if (!Prop)
									continue;

								if (ImGui::TreeNode(format("{}", Prop->GetFullName()).c_str()))
								{
									ImGui::Text(format("Offset: 0x{:x}", Prop->Offset).c_str());
									ImGui::Text(format("ArrayDim: 0x{:x}", Prop->ArrayDim).c_str());
									ImGui::Text(format("ElementSize: 0x{:x}", Prop->ElementSize).c_str());
									ImGui::Text(format("PropertyFlags: {}", Prop->PropertyFlags).c_str());
									ImGui::TreePop();
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				if (!ClassProps.empty() && ImGui::Button("Dump"))
				{
					UE4::Utils::DumpClass(Globals::GamePath, ClassName, ClassProps, ClassFuncs);
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Offsets"))
			{
				ImGui::Text(format("GObjects: 0x{:x}", Offsets::GObjects).c_str());
				ImGui::Text(format("Append String: 0x{:x}", Offsets::AppendString).c_str());
				ImGui::Text(format("GWorld: 0x{:x}", Offsets::GWorld).c_str());
				ImGui::Text(format("Process Event: 0x{:x}", Offsets::ProcessEvent).c_str());
				ImGui::Text(format("Process Event Idx: 0x{:x}", Offsets::ProcessEventIdx).c_str());
				ImGui::Separator();
				ImGui::Text(format("SetWorld: 0x{:x}", Offsets::SetWorld).c_str());
				ImGui::Text(format("InitListen: 0x{:x}", Offsets::InitListen).c_str());
				ImGui::Separator();
				if (ImGui::Button("Dump"))
				{
					MessageBoxA(0, "Not available yet.", "NX-Finder", 0);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Dump"))
			{
				ImGui::Text(format("Dump Location: {}", Globals::GamePath).c_str());
				ImGui::Separator();
				if (ImGui::Button("Dump Objects"))
				{
					UE4::Utils::DumpObjects(Globals::GamePath);
				}

				if (ImGui::Button("Dump Classes"))
				{
					UE4::Utils::DumpClasses(Globals::GamePath);
				}

				if (ImGui::Button("Dump Functions"))
				{
					UE4::Utils::DumpFunctions(Globals::GamePath);
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Memory Search (WIP)"))
			{
				ImGui::Text("Use this if you know what you're doing. (THIS IS VERY WIP)");
				ImGui::Separator();

				static vector<uint8_t> opcodeList{};
				static char membuffer[128] = "";
				static char optbuffer[16] = "";
				static bool bFirstFind = false;
				static bool bForward = false;
				static uintptr_t StringOffset = 0;
				static uintptr_t FuncOffset = 0;
				ImGui::InputText("String to search", membuffer, sizeof(membuffer));
				ImGui::SameLine();
				ImGui::Checkbox("First Find", &bFirstFind);
				ImGui::Checkbox("Forward", &bForward);
				ImGui::InputText("Opcode (Hex)", optbuffer, sizeof(optbuffer));
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					int opcode;
					std::stringstream ss;
					ss << std::hex << optbuffer;
					if (ss >> opcode)  // Convert hex string to int
					{
						opcodeList.push_back(opcode);
					}
					optbuffer[0] = '\0';  // Clear input after adding
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear"))
				{
					opcodeList.clear();
				}
				ImGui::Separator();
				ImGui::Text("Opcodes:");
				for (uint8_t opcode : opcodeList)
				{
					ImGui::SameLine();
					ImGui::Text(format("0x{:x}, ", opcode).c_str());
				}
				ImGui::Separator();
				if (ImGui::Button("Find"))
				{
					string str = string(membuffer);
					std::wstring widestr = std::wstring(str.begin(), str.end());
					const wchar_t* widecstr = widestr.c_str();

					auto StringRefAddr = Memcury::Scanner::FindStringRef(widecstr, bFirstFind);
					if (StringRefAddr.IsValid())
					{
						StringOffset = Memory::GetOffset(StringRefAddr.GetAs<void*>());
						FuncOffset = Memory::GetOffset(StringRefAddr.ScanFor(opcodeList, bForward).GetAs<void*>());
					}
				}
				ImGui::Separator();
				ImGui::Text("Results:");
				ImGui::Text(format("String at: 0x{:x}", StringOffset).c_str());
				ImGui::Text(format("In Function: 0x{:x}", FuncOffset).c_str());

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Settings"))
			{
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
			ImGui::End();
		}
		else
		{
			// Release mouse capture when the menu is closed
			io.WantCaptureMouse = false;
			io.MouseDrawCursor = false;  // Hide the mouse cursor when the menu is closed
		}
	}

	bool init = false;
	HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!init)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);
				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				window = sd.OutputWindow;
				ID3D11Texture2D* pBackBuffer;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
				pBackBuffer->Release();
				oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
				InitImGui();
				init = true;
				LogInfo("ImGui Initialized.");
			}

			else
				return oPresent(pSwapChain, SyncInterval, Flags);
		}


		if (GetAsyncKeyState(VK_F4) & 0x01)
		{
			bOpenMenu = !bOpenMenu;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Watermark();
		Menu();

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	DWORD WINAPI MainThread(LPVOID lpReserved)
	{
		bool init_hook = false;
		do
		{
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				kiero::bind(8, (void**)&oPresent, hkPresent);
				init_hook = true;
			}
		} while (!init_hook);
		return TRUE;
	}
}