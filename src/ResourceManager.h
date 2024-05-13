#pragma once
#include <random>
#include <map>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <cmath> // for std::abs
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "texture.h"
#include "shader.h"
#include <optional>

enum class Color {
    Red,
    Green,
    Blue,
    Yellow,
    Pink,
    Purple,
    Orange,
    Cyan,
    White,
    LightBlue,
};

struct SizePadding {
    int width;
    int height;
    int padTop;
    int padBottom;
    int padLeft;
    int padRight;

    SizePadding() : width(3840), height(2160), padTop(0), padBottom(0), padLeft(0), padRight(0) {}
    SizePadding(int width, int height, int padTop, int padBottom, int padLeft, int padRight) : width(width), height(height), padTop(padTop), padBottom(padBottom), padLeft(padLeft), padRight(padRight) {}
    
    int GetPaddedWidth() const {
		return width + padLeft + padRight;
	}

    int GetPaddedHeight() const {
		return height + padTop + padBottom;
	}
};

extern std::unordered_map<Color, glm::vec3> colorMap;

//// Small window for choosing game mode
//extern glm::vec2 kOptionWindowSize;

// Virtual screen size that the game is designed for
const glm::vec2 kVirtualScreenSize = glm::vec2(3840, 2160);
const SizePadding kVirtualScreenSizePadding = SizePadding(3840, 2160, 0, 0, 0, 0);

// Full screen window size
extern glm::vec2 kFullScreenSize;
extern SizePadding kFullScreenSizePadding;

// Not full screen window size
extern glm::vec2 kWindowedModeSize;

// Set the opengl window width to 80% of the screen width and height to 90% of the screen height
extern glm::vec2 kWindowSize;

//// scale of width to make the proportion of game objects consistent
//const float kWidthScale = 27/28.0f;
extern float screenScale;

// Regular bubbble's radius and size.
extern float kBubbleRadius;
extern glm::vec2 kBubbleSize;

// font size
extern float kFontScale;
extern float kFontSize;

// unit for velocity
extern float kVelocityUnit;

// number of directions of a neighbor bubble
const int kNumNeighborDirections = 12;

// File paths
const std::string kDisplaySettingsPath = "C:/Users/xiaod/resources/settings/display.txt";


// background story of the game
const std::string kBackgroundStoryPart1 = "During the Western Han Dynasty, Emperor Wu of Han, Liu Che, and his wife, Wei Zifu, were enjoying a banquet when the assassin Guo Jie, who had blended in with the servants, seized an opportunity to attempt an assassination on Liu Che. Amidst the merriment, Guo Jie suddenly loosed a cold arrow. Wei Zifu, who had been vigilant and noticed something amiss, timely pushed Liu Che out of the way, saving him from being struck by the arrow. Guo Jie then quickly moved to seize control, holding Liu Che and Wei Zifu hostage. Upon seeing this, Wei Qing rushed to their rescue.";
const std::string kBackgroundStoryPart2 = "Unbeknownst to them, Guo Jie produced a scroll from his sleeve, which was no ordinary scroll but one imbued with magical powers. Once unfurled, the scroll generated bubbles of various colors. Wei Qing was tasked with the challenge of bursting all these bubbles in a short amount of time to break the spell, thereby defeating Guo Jie and rescuing Emperor Wu and his sister, Wei Zifu.";

// control instructions
const std::string kControlInstruction1 = "{Shoot Bubble}: Press the {Space Bar} to fire a bubble.";
const std::string kControlInstruction2 = "{Aim Shooter}:"
                                            "\n\t{Left Rotation}: Press the {Left Arrow Key} to rotate the shooter counter-clockwise"
                                            "\n\t{Right Rotation}: Press the {Right Arrow Key} to rotate the shooter clockwise";
const std::string kControlInstruction3 = "{Precision Aiming}: Hold the {Ctrl Key} to enter slow mode, allowing for precise direction adjustments.";
const std::string kControlInstruction4 = "{Pause Game}: Press the {Q Key} to pause the game. This will display a menu with the following options:"
                                            "\n\t{Restart}: Start the game over from the beginning."
                                            "\n\t{Resume}: Resume your current game session."
                                            "\n\t{Stop}: Stop the game and return to the main menu.";

// Introduction for each character
const std::string kLiuCheIntroduction = "Emperor Wu of Han, Liu Che, was one of the most influential monarchs in Chinese history. His reign marked a period of great strength and prosperity for the Han Dynasty, particularly in terms of territorial expansion, economic development, and cultural flourishing. During a banquet at Princess Pingyang's residence, the assassin Guo Jie suddenly launched an attack and took control of the emperor and other guests.";
const std::string kWeiZiFuIntroduction = "Wei Zifu, the half-sister of Wei Qing and the beloved second empress of Emperor Wu, Liu Che, was deeply cherished by the emperor. During a banquet at Princess Pingyang's residence, when Guo Jie suddenly attacked, she alertly pushed Liu Che out of the way, saving him from an arrow. However, Guo Jie subsequently took her and Liu Che hostage.";
const std::string kWeiQingIntroduction = "Wei Qing, a renowned general during the reign of Emperor Wu of the Han Dynasty, played a crucial role in the early Han-Xiongnu Wars, achieving the ranks of Grand General and Grand Marshal. Wei Qing was the half-brother of Wei Zifu, the second empress of Emperor Wu, and the uncle of Crown Prince Liu Ju and the famous general Huo Qubing. When Guo Jie took control over Liu Che and Wei Zifu, Wei Qing quickly stepped forward to the rescue. He needed to break the enchantment of Guo Jie's magical scroll to defeat Guo Jie and rescue Liu Che and Wei Zifu.";
const std::string kGuoJieIntroduction = "Guo Jie, a knight-errant of the Western Han Dynasty, was directed by Liu Ling, the daughter of Prince Liu An of Huainan, to infiltrate the residence of Princess Pingyang, where Liu Che was attending a banquet, and attempt an assassination. Not only does Guo Jie employ a hidden arrow but also wields a magical scroll capable of producing challenging colorful bubbles.";

const std::string kPromptToMainMenuText = "Press Enter to proceed to the Main Menu";

bool areFloatsEqual(float a, float b, float epsilon = 1e-4);

bool areFloatsEqual(const glm::vec2& a, const glm::vec2& b, float epsilon = 1e-4);

bool areFloatsEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = 1e-4);

bool areFloatsLess(float a, float b, float epsilon = 1e-4);

bool areFloatsGreater(float a, float b, float epsilon = 1e-4);

// Calculate the length of C using the law of cosines
float lawOfCosines(float a, float b, float cosgamma);

// Calculate the lengthn of B using the law of cosines
std::pair<float, float> lawOfCosinesB(float a, float c, float cosgamma);

// Calculate the cosine of the angle using the law of cosines
float lawOfCosinesCos(float a, float b, float c);

// Calculate the angle using the law of cosines
float lawOfCosinesAngle(float a, float b, float c);

// Calculate the amount of time to travel a distance with a given velocity glm::vec2
float timeToTravel(float distance, glm::vec2 velocity);

// Rotate a vector by a given angle around a point. clockwise is positive while counter-clockwise is negative
glm::vec2 rotateVector(glm::vec2 vector, float angle, glm::vec2 point);

// Get the sign of the cross product of two vectors
int getSignOfCrossProduct(glm::vec2 a, glm::vec2 b);

//// Get the relative position ratio from object1 to object2
//glm::vec2 getRelativePositionRatio(glm::vec2 object1Pos, glm::vec2 object2Pos, glm::vec2 object1Size);

//// Generate a random float between min and max
//float generateRandomFloat(float min, float max);
//
//// Generate a random integer between min and max
//int generateRandomInt(int min, int max);

// Generate a random number with data type T between min and max
template <typename T>
T generateRandom(T min, T max) {
    if (min == max) {
        return min;
    }
    else if (min > max) {
        std::swap(min, max);
    }
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator
    std::uniform_real_distribution<T> distr(min, max); // Define the range
    return distr(eng);
}

// Decide true or false with a given probability
bool randomBool(float probability);

// Get the random index of a vector based on weights
int randomWeightedSelect(std::vector<float> weights);

// Convert color to string
std::string colorToString(glm::vec4 color);

// Check if two colors are the same
bool isSameColor(glm::vec3 rgb1, glm::vec3 rgb2);

// Convert a color to a Color enum
Color colorToEnum(glm::vec3 rgb);

// Checks if a char is a descender.
bool isDescender(char c);

// Check if a point has higher y value than a line
bool isPointHigherYThanLine(glm::vec2 point, glm::vec3 lineParams);

// Check if a point has lower y value than a line
bool isPointLowerYThanLine(glm::vec2 point, glm::vec3 lineParams);

// Check if a point has higher x value than a line
bool isPointHigherXThanLine(glm::vec2 point, glm::vec3 lineParams);

// Check if a point has lower x value than a line
bool isPointLowerXThanLine(glm::vec2 point, glm::vec3 lineParams);

// Check if a point is on a line
bool isPointOnLine(glm::vec2 point, glm::vec3 lineParams);

// Solve a quadratic equation
std::optional<std::pair<float, float>> solveQuadratic(float a, float b, float c);

// Solve a two-variable linear equation
std::optional<std::pair<float, float>> solveTwoVariableLinear(float a1, float b1, float c1, float a2, float b2, float c2);

// Solve a line equation given two points
glm::vec3 solveLine(glm::vec2 point1, glm::vec2 point2);

// Get the y value of a line given x
float getYOfLine(float x, glm::vec3 lineParams);

// Get the x value of a line given y
float getXOfLine(float y, glm::vec3 lineParams);

// Calculate the traval distance vector of a gameobject with a given velocity and acceleration in a given time
glm::vec2 calculateTravelDistanceVector(glm::vec2 velocity, glm::vec2 acceleration, float time);

// Calculate the velocity of a gameobject with a given initial velocity and acceleration in a given time
glm::vec2 calculateVelocity(glm::vec2 initialVelocity, glm::vec2 acceleration, float time);

// Find the start (inclusive) and end index (exclusive) of a word in a string.
std::pair<int, int> findWord(std::string text, int startIndex);

// Mix two boundaries.
glm::vec4 mixBoundaries(glm::vec4 boundary1, glm::vec4 boundary2);

// Adjust the size of a rectangle to fit the aspect ratio of a standard size
SizePadding adjustToAspectRatio(int width, int height, int standardWidth, int standardHeight);

struct Circle {
    glm::vec2 center;
    float radius;
};

struct Line {
    float A, B, C; // Line equation: Ax + By + C = 0
};

// A singleton resource manager class that hosts several functions to load Textures and Shaders. Each loaded texture and/or shader is also stored for future reference by string handles. All functions and resources are static and no public constructor is defined.
class ResourceManager {
public:

    // get the singleton instance
    static ResourceManager& GetInstance();
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    // retrieves a stored sader
    Shader    GetShader(std::string name);
    // loads (and generates) a texture from file
    Texture2D LoadTexture(const char* file, bool alpha, std::string name);
    // retrieves a stored texture
    Texture2D GetTexture(std::string name);

    int GetAvailableID();

    void ReturnID(int id);

    // Get maximum available unique ID
    int GetMaxAvailableID();

    // properly de-allocates all loaded resources
    void  Clear();

private:
    // resource storage
    std::unordered_map<std::string, Shader>    Shaders; 
    std::unordered_map<std::string, Texture2D> Textures;
    
    // maximum ID
    int maxID;
    // a static queue of all available unique IDs for GameObjects
    std::queue<int> availableIDs;
    // Colors
    
    std::mutex resourceMutex; // Mutex for thread safety
    // loads and generates a shader from file
    Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
    // loads a single texture from file
    Texture2D loadTextureFromFile(const char* file, bool alpha);
    // private constructor
    ResourceManager();
    // Delete copy constructor and assignment operator to prevent copying
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};