#pragma once
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <json.hpp>

enum class ScreenMode {
	UNDEFINED,
	WINDOWED,
	FULLSCREEN
};

class ConfigManager
{
public:
	// Get the singleton instance
	static ConfigManager& GetInstance();
	// Set the config file path
	void SetConfigPath(const std::string& file_path);
	// Load the config file
	bool LoadConfig();
	// Set the screen mode
	void SetScreenMode(ScreenMode screen_mode);
	// Get the screen mode
	ScreenMode GetScreenMode() const;


private:
	ConfigManager();
	ConfigManager(const ConfigManager& other) = delete;
	ConfigManager& operator=(const ConfigManager& other) = delete;

	ScreenMode screen_mode_;
	std::string config_path_;
};