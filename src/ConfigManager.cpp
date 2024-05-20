#include "ConfigManager.h"

ConfigManager& ConfigManager::GetInstance() {
  static ConfigManager instance;
  return instance;
}

ConfigManager::ConfigManager(){};

void ConfigManager::SetConfigPath(const std::string& file_path) {
  config_path_ = file_path;
}

bool ConfigManager::LoadConfig() {
  std::ifstream config_file(config_path_);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open configuration file: " << config_path_
              << std::endl;
    return false;
  }

  config_file >> config_;

  return true;
}

void ConfigManager::SetScreenMode(ScreenMode screen_mode) {
  // Set the screen mode in the JSON object
  if (screen_mode == ScreenMode::FULLSCREEN) {
    config_["screenMode"] = "fullscreen";
  } else if (screen_mode == ScreenMode::WINDOWED) {
    config_["screenMode"] = "windowed";
  } else {
    std::cerr << "Invalid screen mode value: " << static_cast<int>(screen_mode)
              << std::endl;
    return;
  }
  // Write JSON object to file
  std::ofstream config_file(config_path_);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open configuration file for writing: "
              << config_path_ << std::endl;
    return;
  }
  config_file << config_.dump(4);  // 4 spaces for pretty print
}

ScreenMode ConfigManager::GetScreenMode() const {
  std::string screenMode = config_.at("screenMode");
  if (screenMode == "fullscreen") {
    return ScreenMode::FULLSCREEN;
  } else if (screenMode == "windowed") {
    return ScreenMode::WINDOWED;
  } else {
    std::cerr << "Invalid screen mode value in configuration file: "
              << screenMode << std::endl;
    return ScreenMode::FULLSCREEN;
  }
}

void ConfigManager::SetLanguage(Language language) {
  // Set the language in the JSON object
  config_["language"]["preference"] = language_map.at(language);
  // Write JSON object to file
  std::ofstream config_file(config_path_);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open configuration file for writing: "
              << config_path_ << std::endl;
    return;
  }
  config_file << config_.dump(4);  // 4 spaces for pretty print
}

Language ConfigManager::GetLanguage() const {
  std::string language = config_.at("language").at("preference");
  if (language == "en") {
    return Language::ENGLISH;
  } else if (language == "fr") {
    return Language::FRENCH;
  } else if (language == "ja") {
    return Language::JAPANESE;
  } else if (language == "ko") {
    return Language::KOREAN;
  } else if (language == "zh-Hant") {
    return Language::CHINESE_TRADITIONAL;
  } else if (language == "zh-Hans") {
    return Language::CHINESE_SIMPLIFIED;
  } else {
    std::cerr << "Invalid language value in configuration file: " << language
              << std::endl;
    return Language::ENGLISH;
  }
}

std::string ConfigManager::GetTextFilePath() const {
  std::string preference = config_.at("language").at("preference");
  return config_.at("language").at(preference);
}
