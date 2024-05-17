#pragma once
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <json.hpp>

enum class ScreenMode {
	UNDEFINED,
	WINDOWED,
	FULLSCREEN
};

enum class Language {
	ENGLISH,
	FRENCH,
	JAPANESE,
	KOREAN,
	CHINESE_SIMPLIFIED,
	CHINESE_TRADITIONAL
};


// Specialize std::hash for Language enum
namespace std {
	template <>
	struct hash<Language> {
		std::size_t operator()(const Language& lang) const noexcept {
			return std::hash<int>()(static_cast<int>(lang));
		}
	};
}

// Define the language map
inline std::unordered_map<Language, std::string> language_map = {
	{Language::ENGLISH, "en"},
	{Language::FRENCH, "fr"},
	{Language::JAPANESE, "ja"},
	{Language::KOREAN, "ko"},
	{Language::CHINESE_SIMPLIFIED, "zh-Hans"},
	{Language::CHINESE_TRADITIONAL, "zh-Hant"}
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
	// Get the language preference
	Language GetLanguage() const;


private:
	ConfigManager();
	ConfigManager(const ConfigManager& other) = delete;
	ConfigManager& operator=(const ConfigManager& other) = delete;

	std::string config_path_;
	nlohmann::json config_;
};
