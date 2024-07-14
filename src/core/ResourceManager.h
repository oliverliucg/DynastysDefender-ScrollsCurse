#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <boost/locale.hpp>
#include <cmath>  // for std::abs
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <json.hpp>
#include <map>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>

#include "shader.h"
#include "texture.h"

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
  Brown,
  JadeGreen,
};

struct SizePadding {
  int width;
  int height;
  int padTop;
  int padBottom;
  int padLeft;
  int padRight;

  SizePadding()
      : width(3840),
        height(2160),
        padTop(0),
        padBottom(0),
        padLeft(0),
        padRight(0) {}
  SizePadding(int width, int height, int padTop, int padBottom, int padLeft,
              int padRight)
      : width(width),
        height(height),
        padTop(padTop),
        padBottom(padBottom),
        padLeft(padLeft),
        padRight(padRight) {}

  int GetPaddedWidth() const { return width + padLeft + padRight; }

  int GetPaddedHeight() const { return height + padTop + padBottom; }
};

struct ViewPortInfo {
  GLint x;
  GLint y;
  GLsizei width;
  GLsizei height;
  ViewPortInfo() : x(0), y(0), width(3840), height(2160) {}
  ViewPortInfo(GLint x, GLint y, GLsizei width, GLsizei height)
      : x(x), y(y), width(width), height(height) {}
};

extern std::unordered_map<Color, glm::vec3> colorMap;

constexpr glm::vec3 kScoreColorPink = glm::vec3(1.0f, 0.0f, 0.56471f);
constexpr glm::vec3 kScoreColorOrange = glm::vec3(1.0f, 0.65f, 0.0f);
constexpr float kScoreAlpha = 0.5f;

constexpr float kMouseCursorHeight = 150.f;
constexpr float kMouseCursorWidth = 150.f * 1209.f / 1148.f;

//// Small window for choosing game mode
// extern glm::vec2 kOptionWindowSize;

// Virtual screen size that the game is designed for
constexpr glm::vec2 kVirtualScreenSize = glm::vec2(3840, 2160);
const SizePadding kVirtualScreenSizePadding =
    SizePadding(3840, 2160, 0, 0, 0, 0);

// Full screen window size
extern glm::vec2 kFullScreenSize;
extern SizePadding kFullScreenSizePadding;

// Not full screen window size
extern glm::vec2 kWindowedModeSize;

// Set the opengl window width to 80% of the screen width and height to 90% of
// the screen height
extern glm::vec2 kWindowSize;

//// scale of width to make the proportion of game objects consistent
// const float kWidthScale = 27/28.0f;
extern float screenScale;

// Define the base unit for the game
extern float kBaseUnit;

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

bool areFloatsEqual(float a, float b, float epsilon = 1e-4);

bool areFloatsEqual(const glm::vec2& a, const glm::vec2& b,
                    float epsilon = 1e-4);

bool areFloatsEqual(const glm::vec3& a, const glm::vec3& b,
                    float epsilon = 1e-4);

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

// Calculate the amount of time to travel a distance with a given velocity
// glm::vec2
float timeToTravel(float distance, glm::vec2 velocity);

// Rotate a vector by a given angle around a point. clockwise is positive while
// counter-clockwise is negative
glm::vec2 rotateVector(glm::vec2 point, float angle,
                       glm::vec2 pivot = glm::vec2(0.0f, 0.0f));

// Get the sign of the cross product of two vectors
int getSignOfCrossProduct(glm::vec2 a, glm::vec2 b);

// Generate a random number with data type T between min and max
template <typename T>
T generateRandom(T min, T max) {
  if (min == max) {
    return min;
  } else if (min > max) {
    std::swap(min, max);
  }
  std::random_device rd;   // Obtain a random number from hardware
  std::mt19937 eng(rd());  // Seed the generator
  std::uniform_real_distribution<T> distr(min, max);  // Define the range
  return distr(eng);
}

// Generate a random integer between min and max
template <typename T>
T generateRandomInt(T min, T max) {
  if (min == max) {
    return min;
  } else if (min > max) {
    std::swap(min, max);
  }
  std::random_device rd;   // Obtain a random number from hardware
  std::mt19937 eng(rd());  // Seed the generator
  std::uniform_int_distribution<T> distr(min, max);  // Define the range
  return distr(eng);
}

// Generate a Gaussian random number between min and max.
double generateGaussianRandom(
    double min, double max,
    double mean = std::numeric_limits<double>::infinity());

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

// Color is deep or not
bool isDeepColor(glm::vec3 rgb);

// ggb to hsv
glm::vec3 rgb2hsv(glm::vec3 rgb);

// hsv to rgb
glm::vec3 hsv2rgb(glm::vec3 hsv);

// Adjust color for bright background
glm::vec3 adjustColorForBrightBackground(glm::vec3 color);

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
std::optional<std::pair<float, float>> solveQuadratic(float a, float b,
                                                      float c);

// Solve a two-variable linear equation
std::optional<std::pair<float, float>> solveTwoVariableLinear(
    float a1, float b1, float c1, float a2, float b2, float c2);

// Solve a line function given two points
glm::vec3 solveLine(glm::vec2 point1, glm::vec2 point2);

// Get the y value of a line given x
float getYOfLine(float x, glm::vec3 lineParams);

// Get the x value of a line given y
float getXOfLine(float y, glm::vec3 lineParams);

// Solve a quadratic function given the y-intercept (0, b) and the lowest
// point (x_0, y_0)
glm::vec3 solveQuadratic(float y_intercept, glm::vec2 lowest_point);

// Get the y value of a quadratic function given x
float getYOfQuadratic(float x, glm::vec3 quadraticParams);

// Get the x value of a quadratic function given y
std::optional<std::pair<float, float>> getXOfQuadratic(
    float y, glm::vec3 quadraticParams);

// Calculate the traval distance vector of a gameobject with a given velocity
// and acceleration in a given time
glm::vec2 calculateTravelDistanceVector(glm::vec2 velocity,
                                        glm::vec2 acceleration, float time);

// Calculate the velocity of a gameobject with a given initial velocity and
// acceleration in a given time
glm::vec2 calculateVelocity(glm::vec2 initialVelocity, glm::vec2 acceleration,
                            float time);

// convert int to u32string
std::u32string intToU32String(int64_t num);

// convert u32string to int
int64_t u32StringToInt(std::u32string u32str);

// convert u32string to string
std::string u32StringToString(std::u32string u32str);

// convert string to u32string
std::u32string stringToU32String(std::string str);

// print the hexadeciaml representation of a char32_t
void printChar32(char32_t c);

// check if a char32_t is a control character
bool isControlChar(char32_t c);

// check if a char32_t is contained in a u32string
bool isCharInU32String(char32_t c, std::u32string u32str);

// Find the start (inclusive) and end index (exclusive) of a word in a string.
std::pair<int, int> findWord(std::u32string text, int startIndex);

// Mix two boundaries.
glm::vec4 mixBoundaries(glm::vec4 boundary1, glm::vec4 boundary2);

// Adjust the size of a rectangle to fit the aspect ratio of a standard size
SizePadding adjustToAspectRatio(int width, int height, int standardWidth,
                                int standardHeight);

struct Circle {
  glm::vec2 center;
  float radius;
};

struct Line {
  float A, B, C;  // Line equation: Ax + By + C = 0
};

// A singleton resource manager class that hosts several functions to load
// Textures and Shaders. Each loaded texture and/or shader is also stored for
// future reference by string handles. All functions and resources are static
// and no public constructor is defined.
class ResourceManager {
 public:
  // get the singleton instance
  static ResourceManager& GetInstance();
  // loads (and generates) a shader program from file loading vertex, fragment
  // (and geometry) shader's source code. If gShaderFile is not nullptr, it also
  // loads a geometry shader
  Shader LoadShader(const char* vShaderFile, const char* fShaderFile,
                    const char* gShaderFile, std::string name);
  // Checks if a shader with the given name is loaded
  bool HasShader(std::string name);
  // retrieves a stored sader
  Shader GetShader(std::string name);
  // loads (and generates) a texture from file
  Texture2D LoadTexture(const char* file, bool alpha, std::string name);
  // retrieves a stored texture
  Texture2D GetTexture(std::string name);

  // Load text from a json file
  bool LoadText(const char* file);

  template <typename... Strings>
  std::u32string GetText(Strings... keys) const {
    std::vector<std::string> keyVec = {keys...};  // Convert arguments to vector
    nlohmann::json res = texts;
    for (const auto& key : keyVec) {
      assert(res.count(key) > 0 && "Key not found in texts");
      res = res.at(key);
    }
    assert(res.is_string() && "Value is not a string");
    // Get the string from json (UTF-8 string)
    std::string utf8_str = res.get<std::string>();

    return stringToU32String(utf8_str);
    stringToU32String(utf8_str);
  }

  int GetAvailableID();

  void ReturnID(int id);

  // Get maximum available unique ID
  int GetMaxAvailableID();

  // properly de-allocates all loaded resources
  void Clear();

 private:
  // resource storage
  std::unordered_map<std::string, Shader> Shaders;
  std::unordered_map<std::string, Texture2D> Textures;
  // text storage
  nlohmann::json texts;

  // maximum ID
  int maxID;
  // a static queue of all available unique IDs for GameObjects
  std::queue<int> availableIDs;
  // Colors

  std::mutex resourceMutex;  // Mutex for thread safety
  // loads and generates a shader from file
  Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile,
                            const char* gShaderFile = nullptr);
  // loads a single texture from file
  Texture2D loadTextureFromFile(const char* file, bool alpha);
  // private constructor
  ResourceManager();
  // Delete copy constructor and assignment operator to prevent copying
  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;
};