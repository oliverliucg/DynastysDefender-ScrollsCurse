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
  switch (screen_mode) {
    case ScreenMode::FULLSCREEN:
      config_["screenMode"] = "fullscreen";
      break;
    case ScreenMode::WINDOWED_BORDERLESS:
      config_["screenMode"] = "windowedborderless";
      break;
    case ScreenMode::WINDOWED:
      config_["screenMode"] = "windowed";
      break;
    default:
      std::cerr << "Invalid screen mode value: "
                << static_cast<int>(screen_mode) << std::endl;
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
  } else if (screenMode == "windowedborderless") {
    return ScreenMode::WINDOWED_BORDERLESS;
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
  if (language == "de") {
    return Language::GERMAN;
  } else if (language == "en") {
    return Language::ENGLISH;
  } else if (language == "es") {
    return Language::SPANISH;
  } else if (language == "it") {
    return Language::ITALIAN;
  } else if (language == "fr") {
    return Language::FRENCH;
  } else if (language == "ja") {
    return Language::JAPANESE;
  } else if (language == "ko") {
    return Language::KOREAN;
  } else if (language == "pt-BR") {
    return Language::PORTUGUESE_BR;
  } else if (language == "pt-PT") {
    return Language::PORTUGUESE_PT;
  } else if (language == "ru") {
    return Language::RUSSIAN;
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

void ConfigManager::SetDifficulty(Difficulty difficulty) {
  // Set the difficulty in the JSON object
  switch (difficulty) {
    case Difficulty::EASY:
      config_["difficulty"] = "easy";
      break;
    case Difficulty::MEDIUM:
      config_["difficulty"] = "medium";
      break;
    case Difficulty::HARD:
      config_["difficulty"] = "hard";
      break;
    case Difficulty::EXPERT:
      config_["difficulty"] = "expert";
      break;
    default:
      std::cerr << "Invalid difficulty value: " << static_cast<int>(difficulty)
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

Difficulty ConfigManager::GetDifficulty() const {
  std::string difficulty = config_.at("difficulty");
  if (difficulty == "easy") {
    return Difficulty::EASY;
  } else if (difficulty == "medium") {
    return Difficulty::MEDIUM;
  } else if (difficulty == "hard") {
    return Difficulty::HARD;
  } else if (difficulty == "expert") {
    return Difficulty::EXPERT;
  } else {
    std::cerr << "Invalid difficulty value in configuration file: "
              << difficulty << std::endl;
    return Difficulty::MEDIUM;
  }
}

void ConfigManager::SetScore(int64_t score) {
  // Set the score in the JSON object
  config_["score"] = score;
  // Write JSON object to file
  std::ofstream config_file(config_path_);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open configuration file for writing: "
              << config_path_ << std::endl;
    return;
  }
  config_file << config_.dump(4);  // 4 spaces for pretty print
}

int64_t ConfigManager::GetScore() const {
  return config_.at("score").get<int64_t>();
}

std::pair<char32_t, std::string> ConfigManager::GetFontFilePath(
    char32_t character, CharStyle style) const {
  std::string style_str = char_style_map.at(style);
  if (0x0020 <= character && character <= 0x017F) {
    // If character is from 0 to 00FF, it is for English and French, and we
    // would load the latin font
    return std::make_pair(
        0, config_.at("language").at("en").at("font").at(style_str));
  } else if (0x4E00 <= character && character <= 0x9FFF) {
    // If character is from 4E00 to 9FFF, it is for simplified Chinese or
    // traditional chinese, and we would load the chinese font
    return std::make_pair(
        0x4E00, config_.at("language").at("zh-Hans").at("font").at(style_str));
  } else if (0x3000 <= character && character <= 0x303F ||
             0x3040 <= character && character <= 0x309F ||
             0x30A0 <= character && character <= 0x30FF ||
             0xFF00 <= character && character <= 0xFFEF) {
    // If character is from 3040 to 309F, it is for Japanese, and we would
    // load the japanese font (Hiragana).
    return std::make_pair(
        0x3040, config_.at("language").at("ja").at("font").at(style_str));
  } else if (0x1100 <= character && character <= 0x11FF ||
             0xAC00 <= character && character <= 0xD7AF) {
    // If character is from 1100 to 11FF, it is for Korean, and we would
    // load the korean font (Hangul).
    return std::make_pair(
        0x1100, config_.at("language").at("ko").at("font").at(style_str));
  } else if (0x0400 <= character && character <= 0x04FF) {
    // If character is from 0400 to 04FF, it is for Russian, and we would
    // load the russian font.
    return std::make_pair(
        0x0400, config_.at("language").at("ru").at("font").at(style_str));
  } else {
    return std::make_pair(character, "");
  }
}

std::string ConfigManager::GetFontFilePath(CharStyle style) const {
  std::string preference = config_.at("language").at("preference");
  std::string style_str = char_style_map.at(style);
  return config_.at("language").at(preference).at("font").at(style_str);
}

std::string ConfigManager::GetTextFilePath() const {
  std::string preference = config_.at("language").at("preference");
  return config_.at("language").at(preference).at("text");
}
