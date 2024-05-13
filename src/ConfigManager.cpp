#include "ConfigManager.h"

ConfigManager& ConfigManager::GetInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {};

void ConfigManager::SetConfigPath(const std::string& file_path) {
	config_path_ = file_path;
}

bool ConfigManager::LoadConfig() {
    std::ifstream file(config_path_);
    std::string line;

    if (file.is_open() && std::getline(file, line)) {
        file.close();  // Close the file early

        if (line == "F") {
            screen_mode_ = ScreenMode::FULLSCREEN;
            return true;
        }
        else if (line == "W") {
            screen_mode_ = ScreenMode::WINDOWED;
            return true;
        }
        else {
            std::cerr << "Invalid screen mode flag in file. Defaulting to windowed mode." << std::endl;
        }
    }
    else {
        std::cerr << "Unable to open the file or file is empty. Defaulting to windowed mode." << std::endl;
    }

    return false; // Default to windowed mode
}

void ConfigManager::SetScreenMode(ScreenMode screen_mode) {
    screen_mode_ = screen_mode;
	std::ofstream file(config_path_);
    assert(file.is_open() && "Unable to open the file to save screen mode.");
    switch (screen_mode_) {
    case ScreenMode::WINDOWED:
        file << "W";
        break;
    case ScreenMode::FULLSCREEN:
        file << "F";
        break;
    default:
        break;
    }
    file.close();
}

ScreenMode ConfigManager::GetScreenMode() const {
    assert(screen_mode_ != ScreenMode::UNDEFINED && "Screen mode is not set.");
	return screen_mode_;
}
