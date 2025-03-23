#pragma once
#include <imgui.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Module.h"

struct SDL_Window;

namespace module::editor
{
// Forward declarations
class EditorPanel;
struct PanelState;
struct EditorTheme;
struct MenuItem;

class EditorPanel
{
public:
	virtual void render()				= 0;
	virtual ~EditorPanel()				= default;
	virtual const char* getName() const = 0;
};

struct PanelState
{
	bool isVisible = true;
	bool isDocked  = false;
	ImVec2 position{ 0, 0 };
	ImVec2 size{ 0, 0 };
	ImGuiID dockId = 0;
};

struct EditorTheme
{
	ImVec4 textColor{ 0, 0, 0, 1 };
	ImVec4 bgColor{ 0.94f, 0.94f, 0.94f, 1.0f };
	ImVec4 titlebarColor{ 0.85f, 0.85f, 0.85f, 1.0f };
	ImVec4 titlebarActiveColor{ 0.75f, 0.75f, 0.75f, 1.0f };
	ImVec4 accentColor{ 0.26f, 0.59f, 0.98f, 0.67f };
	ImVec4 accentActiveColor{ 0.26f, 0.59f, 0.98f, 1.0f };
	ImVec4 buttonColor{ 0.85f, 0.85f, 0.85f, 1.0f };
	ImVec4 buttonHoverColor{ 0.78f, 0.78f, 0.78f, 1.0f };
	ImVec4 buttonActiveColor{ 0.71f, 0.71f, 0.71f, 1.0f };
	float rounding	 = 3.0f;
	float borderSize = 1.0f;
};

struct MenuItem
{
	std::string name;
	std::function<void()> callback;
	std::vector<MenuItem> subItems;
};

class Editor : public module::Module
{
public:
	bool init() override;
	bool update() override;
	bool shutdown() override;

	// Theme Management
	void setTheme( const editor::EditorTheme& theme );
	void setDefaultTheme();

	const editor::EditorTheme& getCurrentTheme() const
	{
		return currentTheme;
	}

private:
	// Panel Management
	template <typename Panel>
	void addPanel()
	{
		static_assert( std::is_base_of_v<editor::EditorPanel, Panel>, "Panel must inherit from EditorPanel" );
		auto panel				 = std::make_unique<Panel>();
		panels[panel->getName()] = std::move( panel );
	}

	template <typename Panel, typename... Args>
	void addPanel( Args&&... args )
	{
		static_assert( std::is_base_of_v<editor::EditorPanel, Panel>, "Panel must inherit from EditorPanel" );
		auto panel				 = std::make_unique<Panel>( std::forward<Args>( args )... );
		panels[panel->getName()] = std::move( panel );
	}

	bool removePanel( const std::string& panelName );
	bool showPanel( const std::string& panelName, bool show );
	bool isPanelVisible( const std::string& panelName ) const;
	std::vector<std::string> getActivePanels() const;

	// Layout Management
	bool saveLayout( const std::string& filename = "" ) const;
	bool loadLayout( const std::string& filename = "" );
	void resetLayout();

	// Menu Management
	void addMenuItem( const std::string& menu, const std::string& item, std::function<void()> callback );
	void addSubMenuItem( const std::string& menu,
						 const std::string& item,
						 const std::string& subItem,
						 std::function<void()> callback );

	void renderMenuBar();

	SDL_Window* window = nullptr;  // owned by module window
	std::unordered_map<std::string, std::unique_ptr<EditorPanel>> panels;
	std::unordered_map<std::string, PanelState> panelStates;
	std::unordered_map<std::string, std::vector<MenuItem>> menuItems;
	EditorTheme currentTheme;
	bool isInitialized = false;
	void applyTheme() const;
};
}  // namespace module::editor