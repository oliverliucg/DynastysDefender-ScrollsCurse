#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <string>
#include <unordered_map>

enum class Difficulty { UNDEFINED, EASY, MEDIUM, HARD, EXPERT };

enum class ScreenMode { UNDEFINED, WINDOWED, FULLSCREEN };

enum class CharStyle { UNDEFINED, REGULAR, BOLD, ITALIC, BOLD_ITALIC };

enum class Language {
  UNDEFINED,
  GERMAN,
  ENGLISH,
  SPANISH,
  ITALIAN,
  FRENCH,
  JAPANESE,
  KOREAN,
  PORTUGUESE_BR,
  PORTUGUESE_PT,
  RUSSIAN,
  CHINESE_SIMPLIFIED,
  CHINESE_TRADITIONAL
};

namespace std {

// Specilize std::hash for Difficulty enum
template <>
struct hash<Difficulty> {
  std::size_t operator()(const Difficulty& diff) const noexcept {
    return std::hash<int>()(static_cast<int>(diff));
  }
};
// Specialize std::hash for Language enum
template <>
struct hash<Language> {
  std::size_t operator()(const Language& lang) const noexcept {
    return std::hash<int>()(static_cast<int>(lang));
  }
};

// Specialize std::hash for CharStyle enum
template <>
struct hash<CharStyle> {
  std::size_t operator()(const CharStyle& style) const noexcept {
    return std::hash<int>()(static_cast<int>(style));
  }
};

}  // namespace std

// Define the difficulty map
inline std::unordered_map<Difficulty, std::string> difficulty_map = {
    {Difficulty::EASY, "easy"},
    {Difficulty::MEDIUM, "medium"},
    {Difficulty::HARD, "hard"},
    {Difficulty::EXPERT, "expert"}};

// Define the language map
inline std::unordered_map<Language, std::string> language_map = {

    {Language::GERMAN, "de"},
    {Language::ENGLISH, "en"},
    {Language::SPANISH, "es"},
    {Language::ITALIAN, "it"},
    {Language::FRENCH, "fr"},
    {Language::JAPANESE, "ja"},
    {Language::KOREAN, "ko"},
    {Language::PORTUGUESE_BR, "pt-BR"},
    {Language::PORTUGUESE_PT, "pt-PT"},
    {Language::RUSSIAN, "ru"},
    {Language::CHINESE_SIMPLIFIED, "zh-Hans"},
    {Language::CHINESE_TRADITIONAL, "zh-Hant"}};

// Define language enum map
inline std::unordered_map<std::string, Language> language_enum_map = {
    {"de", Language::GERMAN},
    {"en", Language::ENGLISH},
    {"es", Language::SPANISH},
    {"it", Language::ITALIAN},
    {"fr", Language::FRENCH},
    {"ja", Language::JAPANESE},
    {"ko", Language::KOREAN},
    {"pt-BR", Language::PORTUGUESE_BR},
    {"pt-PT", Language::PORTUGUESE_PT},
    {"ru", Language::RUSSIAN},
    {"zh-Hans", Language::CHINESE_SIMPLIFIED},
    {"zh-Hant", Language::CHINESE_TRADITIONAL}};

// Define bench mark character for each language's font
inline std::unordered_map<Language, char32_t> benchmark_char_map = {
    {Language::GERMAN, U'f'},
    {Language::ENGLISH, U'f'},
    {Language::SPANISH, U'f'},
    {Language::ITALIAN, U'f'},
    {Language::FRENCH, U'f'},
    {Language::JAPANESE, U'宴'},
    {Language::KOREAN, U'힣'},
    {Language::PORTUGUESE_BR, U'f'},
    {Language::PORTUGUESE_PT, U'f'},
    {Language::RUSSIAN, U'б'},
    {Language::CHINESE_SIMPLIFIED, U'宴'},
    {Language::CHINESE_TRADITIONAL, U'宴'}};

// Define the char style map
inline std::unordered_map<CharStyle, std::string> char_style_map = {
    {CharStyle::REGULAR, "regular"},
    {CharStyle::BOLD, "bold"},
    {CharStyle::ITALIC, "italic"},
    {CharStyle::BOLD_ITALIC, "boldItalic"}};

class ConfigManager {
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
  // Set the language preference
  void SetLanguage(Language language);
  // Get the language preference
  Language GetLanguage() const;
  // Set the difficulty
  void SetDifficulty(Difficulty difficulty);
  // Get the difficulty
  Difficulty GetDifficulty() const;
  // Set the score.
  void SetScore(int64_t score);
  // Get the score.
  int64_t GetScore() const;
  // Get path to font file for a certain character
  std::pair<char32_t, std::string> GetFontFilePath(char32_t character,
                                                   CharStyle style) const;
  // Get path to font file for the given language
  std::string GetFontFilePath(CharStyle style) const;
  // Get path to text file for the given language
  std::string GetTextFilePath() const;

 private:
  ConfigManager();
  ConfigManager(const ConfigManager& other) = delete;
  ConfigManager& operator=(const ConfigManager& other) = delete;

  std::string config_path_;
  nlohmann::json config_;
};
