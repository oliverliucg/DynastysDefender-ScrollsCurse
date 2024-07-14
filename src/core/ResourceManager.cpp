#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::unordered_map<Color, glm::vec3> colorMap = {
    {Color::Red, glm::vec3(1.0f, 0.0f, 0.0f)},
    {Color::Green, glm::vec3(0.0f, 1.0f, 0.0f)},
    {Color::Blue, glm::vec3(0.0f, 0.0f, 1.0f)},
    {Color::Yellow, glm::vec3(1.0f, 1.0f, 0.0f)},
    {Color::Pink, glm::vec3(0.949f, 0.4078f, 0.5725f)},
    {Color::Purple, glm::vec3(0.5f, 0.0f, 0.5f)},
    {Color::Orange, glm::vec3(1.0f, 0.65f, 0.0f)},
    {Color::Cyan, glm::vec3(0.0f, 1.0f, 1.0f)},
    {Color::White, glm::vec3(1.0f, 1.0f, 1.0f)},
    {Color::LightBlue, glm::vec3(0.678f, 0.847f, 0.902f)},
    {Color::Brown, glm::vec3(0.6471, 0.1647, 0.1647)},
    {Color::JadeGreen, glm::vec3(0.0, 0.6588, 0.4196)},
};

// const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
// const int SCREEN_WIDTH = (mode->width * 27) / 32;
// const int SCREEN_HEIGHT = (mode->height * 7) / 8;
// const glm::vec2 kWindowSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);

float screenScale = 1.0f;
float kBaseUnit = kWindowSize.y / 42.f;
float kBubbleRadius = kBaseUnit;
float kVelocityUnit = 2 * kBaseUnit;
glm::vec2 kBubbleSize = glm::vec2(kBubbleRadius * 2, kBubbleRadius * 2);
float kFontScale = 0.2f;
float kFontSize = kWindowSize.y * kFontScale;

bool areFloatsEqual(float a, float b, float epsilon) {
  return std::abs(a - b) < epsilon;
}

bool areFloatsEqual(const glm::vec2& a, const glm::vec2& b, float epsilon) {
  return areFloatsEqual(a.x, b.x, epsilon) && areFloatsEqual(a.y, b.y, epsilon);
}

bool areFloatsEqual(const glm::vec3& a, const glm::vec3& b, float epsilon) {
  return areFloatsEqual(a.r, b.r, epsilon) &&
         areFloatsEqual(a.g, b.g, epsilon) && areFloatsEqual(a.b, b.b, epsilon);
}

bool areFloatsLess(float a, float b, float epsilon) {
  return a < b && !areFloatsEqual(a, b, epsilon);
}

bool areFloatsGreater(float a, float b, float epsilon) {
  return a > b && !areFloatsEqual(a, b, epsilon);
}

float lawOfCosines(float a, float b, float cosgamma) {
  return std::sqrt(a * a + b * b - 2 * a * b * cosgamma);
}

std::pair<float, float> lawOfCosinesB(float a, float c, float cosgamma) {
  float firstPart = a * cosgamma;
  float secondPart = a * std::sqrt(cosgamma * cosgamma - 1 + (c * c) / (a * a));
  return std::make_pair(firstPart - secondPart, firstPart + secondPart);
}

float lawOfCosinesCos(float a, float b, float c) {
  return (a * a + b * b - c * c) / (2 * a * b);
}

float lawOfCosinesAngle(float a, float b, float c) {
  return glm::acos(lawOfCosinesCos(a, b, c));
}

float timeToTravel(float distance, glm::vec2 velocity) {
  return distance / glm::length(velocity);
}

glm::vec2 rotateVector(glm::vec2 point, float angle, glm::vec2 pivot) {
  // translate the vector so that the point is at the origin
  point -= pivot;
  // rotate the vector using glm::rotate
  point = glm::rotate(point, angle);
  // translate the vector back to its original position
  point += pivot;
  return point;
}

int getSignOfCrossProduct(glm::vec2 a, glm::vec2 b) {
  // Use glm::cross to calculate the cross product of a and b
  float crossProduct = glm::cross(glm::vec3(a, 0.0f), glm::vec3(b, 0.0f)).z;
  // return 1 if crossProduct is positive, -1 if crossProduct is negative, and 0
  // if crossProduct is 0
  if (crossProduct > 0) {
    return 1;
  } else if (crossProduct < 0) {
    return -1;
  } else {
    return 0;
  }
}

double generateGaussianRandom(double min, double max, double mean) {
  std::random_device rd;
  std::mt19937 gen(rd());
  if (mean == std::numeric_limits<double>::infinity()) {
    mean = (min + max) / 2.0;
  }
  double stddev = (max - min) / 6.0;
  std::normal_distribution<> distr(mean, stddev);
  double value = distr(gen);  // Generate a Gaussian-distributed value
  value = std::max(min, std::min(value, max));  // Clip the value to the range
  return value;
}

bool randomBool(float probability) {
  return generateRandom(0.0f, 1.0f) < probability;
}

bool isSameColor(glm::vec3 rgb1, glm::vec3 rgb2) {
  return areFloatsEqual(rgb1.r, rgb2.r) && areFloatsEqual(rgb1.g, rgb2.g) &&
         areFloatsEqual(rgb1.b, rgb2.b);
}

std::string colorToString(glm::vec4 color) {
  std::stringstream ss;
  ss << "(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a
     << ")";
  return ss.str();
}

Color colorToEnum(glm::vec3 rgb) {
  for (auto it : colorMap) {
    if (isSameColor(it.second, rgb)) {
      return it.first;
    }
  }
  return Color::Red;
}

bool isDeepColor(glm::vec3 rgb) {
  Color color = colorToEnum(rgb);
  return color == Color::Pink || color == Color::Purple ||
         color == Color::Blue || color == Color::Red || color == Color::Brown ||
         color == Color::JadeGreen;
}

glm::vec3 rgb2hsv(glm::vec3 rgb) {
  float r = rgb.r, g = rgb.g, b = rgb.b;
  float max = std::max({r, g, b});
  float min = std::min({r, g, b});
  float delta = max - min;

  float h, s, v = max;

  if (delta < 0.00001f) {
    s = 0;
    h = 0;  // Undefined, maybe nan?
  } else {
    if (max > 0.0f) {  // NOTE: if Max is == 0, this divide would cause a crash
      s = (delta / max);  // s
    } else {
      s = 0.0f;
      h = std::numeric_limits<float>::quiet_NaN();  // its now undefined
      return glm::vec3(h, s, v);
    }
    if (r >= max)           // > is bogus, just keeps compilor happy
      h = (g - b) / delta;  // between yellow & magenta
    else if (g >= max)
      h = 2.0f + (b - r) / delta;  // between cyan & yellow
    else
      h = 4.0f + (r - g) / delta;  // between magenta & cyan

    h *= 60.0f;  // degrees

    if (h < 0.0f) h += 360.0f;
  }
  return glm::vec3(h, s, v);
}

glm::vec3 hsv2rgb(glm::vec3 hsv) {
  float h = hsv.x, s = hsv.y, v = hsv.z;
  float r, g, b;

  int i = int(h / 60.0f) % 6;
  float f = (h / 60.0f) - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i) {
    case 0:
      r = v, g = t, b = p;
      break;
    case 1:
      r = q, g = v, b = p;
      break;
    case 2:
      r = p, g = v, b = t;
      break;
    case 3:
      r = p, g = q, b = v;
      break;
    case 4:
      r = t, g = p, b = v;
      break;
    case 5:
      r = v, g = p, b = q;
      break;
  }

  return glm::vec3(r, g, b);
}

glm::vec3 adjustColorForBrightBackground(glm::vec3 color) {
  // convert RGB to HSV
  glm::vec3 hsv = rgb2hsv(color);

  // Increase the saturation
  hsv.y = std::min(hsv.y * 1.5f, 1.0f);

  // Adjust the brightness
  hsv.z = std::min(hsv.z * 0.8f, 1.0f);

  // Convert back to RGB
  // std::cout << "hsv: " << hsv.x << " " << hsv.y << " " << hsv.z << std::endl;
  // auto newColor = hsv2rgb(hsv);
  // std::cout << "newColor: " << newColor.r << " " << newColor.g << " "
  //          << newColor.b << std::endl;
  return hsv2rgb(hsv);
}

int randomWeightedSelect(std::vector<float> weights) {
  // Calculate the sum of all weights
  float sum = 0.0f;
  for (float weight : weights) {
    sum += weight;
  }
  // Generate a random float between 0 and sum
  float randomFloat = generateRandom(0.0f, sum);
  // Find the index of the first weight that is greater than the random float
  float currentSum = 0.0f;
  for (int i = 0; i < weights.size(); ++i) {
    currentSum += weights[i];
    if (currentSum > randomFloat) {
      return i;
    }
  }
  // If no weight is greater than the random float, return the last index
  return weights.size() - 1;
}

bool isPointHigherYThanLine(glm::vec2 point, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  if (B == 0) {
    return false;
  } else if (B < 0) {
    A = -A;
    B = -B;
    C = -C;
  }
  return areFloatsGreater(A * point.x + B * point.y + C, 0.0f);
}

bool isPointLowerYThanLine(glm::vec2 point, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  if (B == 0) {
    return false;
  } else if (B < 0) {
    A = -A;
    B = -B;
    C = -C;
  }
  return areFloatsLess(A * point.x + B * point.y + C, 0.0f);
}

bool isPointHigherXThanLine(glm::vec2 point, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  if (A == 0) {
    return false;
  } else if (A < 0) {
    A = -A;
    B = -B;
    C = -C;
  }
  return areFloatsGreater(A * point.x + B * point.y + C, 0.0f);
}

bool isPointLowerXThanLine(glm::vec2 point, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  if (A == 0) {
    return false;
  } else if (A < 0) {
    A = -A;
    B = -B;
    C = -C;
  }
  return areFloatsLess(A * point.x + B * point.y + C, 0.0f);
}

bool isPointOnLine(glm::vec2 point, glm::vec3 lineParams) {
  return areFloatsEqual(
      lineParams.x * point.x + lineParams.y * point.y + lineParams.z, 0.0f);
}

std::optional<std::pair<float, float>> solveQuadratic(float a, float b,
                                                      float c) {
  float discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    // No real roots
    return std::nullopt;
  }
  float sqrtDiscriminant = std::sqrt(discriminant);
  float root1 = (-b + sqrtDiscriminant) / (2 * a);
  float root2 = (-b - sqrtDiscriminant) / (2 * a);
  return std::make_pair(root1, root2);
}

std::optional<std::pair<float, float>> solveTwoVariableLinear(
    float a1, float b1, float c1, float a2, float b2, float c2) {
  float determinant = a1 * b2 - a2 * b1;
  if (areFloatsEqual(determinant, 0.0f)) {
    // The two lines are parallel
    return std::nullopt;
  }
  float x = (c1 * b2 - c2 * b1) / determinant;
  float y = (a1 * c2 - a2 * c1) / determinant;
  return std::make_pair(x, y);
}

glm::vec3 solveLine(glm::vec2 point1, glm::vec2 point2) {
  float A = point2.y - point1.y;
  float B = point1.x - point2.x;
  float C = point1.y * point2.x - point1.x * point2.y;
  return glm::vec3(A, B, C);
}

float getYOfLine(float x, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  assert(B != 0 && "The line must not be vertical.");
  return (-A * x - C) / B;
}

float getXOfLine(float y, glm::vec3 lineParams) {
  float A = lineParams.x;
  float B = lineParams.y;
  float C = lineParams.z;
  assert(A != 0 && "The line must not be horizontal.");
  return (-B * y - C) / A;
}

glm::vec3 solveQuadratic(float y_intercept, glm::vec2 lowest_point) {
  float A = (y_intercept - lowest_point.y) / (lowest_point.x * lowest_point.x);
  float B = -2 * A * lowest_point.x;
  float C = y_intercept;
  return glm::vec3(A, B, C);
}

float getYOfQuadratic(float x, glm::vec3 quadraticParams) {
  float A = quadraticParams.x;
  float B = quadraticParams.y;
  float C = quadraticParams.z;
  return A * x * x + B * x + C;
}

std::optional<std::pair<float, float>> getXOfQuadratic(
    float y, glm::vec3 quadraticParams) {
  return solveQuadratic(quadraticParams.x, quadraticParams.y,
                        quadraticParams.z - y);
}

glm::vec2 calculateTravelDistanceVector(glm::vec2 velocity,
                                        glm::vec2 acceleration, float time) {
  return velocity * time + 0.5f * acceleration * time * time;
}

glm::vec2 calculateVelocity(glm::vec2 initialVelocity, glm::vec2 acceleration,
                            float time) {
  return initialVelocity + acceleration * time;
}

std::u32string intToU32String(int64_t num) {
  std::string str = std::to_string(num);
  return std::u32string(str.begin(), str.end());
}

int64_t u32StringToInt(std::u32string u32str) {
  for (size_t i = 0; i < u32str.size(); ++i) {
    if (u32str[i] == 0x002B || u32str[i] == 0x002D) {
      assert(i == 0 && "The sign must be at the beginning of the string.");
    } else {
      assert(u32str[i] >= 0x0030 && u32str[i] <= 0x0039 &&
             "The input string is not a number.");
    }
  }
  std::string str(u32str.begin(), u32str.end());
  return std::stoll(str);
}

std::string u32StringToString(std::u32string u32str) {
  boost::locale::generator gen;
  std::locale loc = gen("");  // Create a system default locale
  std::locale::global(loc);   // Make it the global locale
  // Convert from UTF-32 to UTF-8
  return boost::locale::conv::utf_to_utf<char>(u32str);
}

std::u32string stringToU32String(std::string str) {
  boost::locale::generator gen;
  std::locale loc = gen("");  // Create a system default locale
  std::locale::global(loc);   // Make it the global locale
  // Convert from UTF-8 to UTF-32
  return boost::locale::conv::utf_to_utf<char32_t>(str);
}

void printChar32(char32_t c) {
  std::cout << "0x" << std::hex << std::uppercase << std::setw(8)
            << std::setfill('0') << static_cast<unsigned int>(c) << std::endl;
  // Reset the output format
  std::cout << std::dec << std::nouppercase;
}

bool isControlChar(char32_t c) { return c < 0x0020 || c == 0x007F; }

bool isCharInU32String(char32_t c, std::u32string u32str) {
  return u32str.find(c) != std::u32string::npos;
}

std::pair<int, int> findWord(std::u32string text, int startIndex) {
  if (startIndex == text.size()) {
    return std::make_pair(startIndex, startIndex);
  }

  auto startIt = text.begin() + startIndex;
  // The word is end with a space or the end of the string or the end of the
  // line.
  auto it = std::find_if(startIt, text.end(),
                         [](char32_t c) { return c == U' ' || c == U'\n'; });
  return std::make_pair(startIndex, it - text.begin());
}

glm::vec4 mixBoundaries(glm::vec4 boundary1, glm::vec4 boundary2) {
  glm::vec4 result;
  result.x = std::max(boundary1.x, boundary2.x);
  result.y = std::max(boundary1.y, boundary2.y);
  result.z = std::min(boundary1.z, boundary2.z);
  result.w = std::min(boundary1.w, boundary2.w);
  return result;
}

SizePadding adjustToAspectRatio(int width, int height, int standardWidth,
                                int standardHeight) {
  //   float aspectRatio = (float)standardWidth / standardHeight;
  // float currentAspectRatio = (float)width / height;
  int newWidth = width;
  int newHeight = height;
  int padTop = 0, padBottom = 0, padLeft = 0, padRight = 0;
  if (width * standardHeight > height * standardWidth) {
    newWidth = height * standardWidth / standardHeight;
    padLeft = (width - newWidth) / 2;
    padRight = width - newWidth - padLeft;

  } else if (width * standardHeight < height * standardWidth) {
    newHeight = width * standardHeight / standardWidth;
    padTop = (height - newHeight) / 2;
    padBottom = height - newHeight - padTop;
  } else {
    // The aspect ratio is the same
    return SizePadding(width, height, 0, 0, 0, 0);
  }
  return SizePadding(newWidth, newHeight, padTop, padBottom, padLeft, padRight);
}

ResourceManager& ResourceManager::GetInstance() {
  static ResourceManager instance;
  return instance;
}

ResourceManager::ResourceManager() {
  // Initialize the queue with 100 consecutive integers
  this->maxID = 100;
  for (int i = 0; i <= this->maxID; ++i) {
    availableIDs.emplace(i);
  }
}

Shader ResourceManager::LoadShader(const char* vShaderFile,
                                   const char* fShaderFile,
                                   const char* gShaderFile, std::string name) {
  // std::lock_guard<std::mutex> lock(resourceMutex);
  Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return Shaders[name];
}

bool ResourceManager::HasShader(std::string name) {
  return Shaders.count(name) > 0;
}

Shader ResourceManager::GetShader(std::string name) {
  assert(Shaders.count(name) > 0 && "The shader does not exist.");
  return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha,
                                       std::string name) {
  /* std::lock_guard<std::mutex> lock(resourceMutex);*/
  Textures[name] = loadTextureFromFile(file, alpha);
  return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
  assert(Textures.count(name) > 0 && "The texture does not exist.");
  return Textures[name];
}

bool ResourceManager::LoadText(const char* jsonFile) {
  std::lock_guard<std::mutex> lock(resourceMutex);
  std::ifstream text_file(jsonFile, std::ios::in | std::ios::binary);
  if (!text_file.is_open()) {
    std::cerr << "Failed to open texts file: " << jsonFile << std::endl;
    return false;
  }

  text_file >> texts;
  return true;
}

void ResourceManager::Clear() {
  for (auto iter : Shaders) glDeleteProgram(iter.second.ID);
  for (auto iter : Textures) glDeleteTextures(1, &iter.second.ID);
  Shaders.clear();
  Textures.clear();
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile,
                                           const char* fShaderFile,
                                           const char* gShaderFile) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  try {
    // open files
    std::ifstream vertexShaderFile(vShaderFile);
    std::ifstream fragmentShaderFile(fShaderFile);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();
    // close file handlers
    vertexShaderFile.close();
    fragmentShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    // if geometry shader path is present, also load a geometry shader
    if (gShaderFile != nullptr) {
      std::ifstream geometryShaderFile(gShaderFile);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::exception e) {
    std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();
  const char* gShaderCode = geometryCode.c_str();
  // 2. now create shader object from source code
  Shader shader;
  shader.Compile(vShaderCode, fShaderCode,
                 gShaderFile != nullptr ? gShaderCode : nullptr);
  return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
  // create texture object
  Texture2D texture;
  if (alpha) {
    texture.Internal_Format = GL_RGBA;
    texture.Image_Format = GL_RGBA;
  }
  // load image
  int width, height, nrChannels;
  unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
  // now generate texture
  texture.Generate(width, height, data);
  // and finally free image data
  stbi_image_free(data);
  return texture;
}

int ResourceManager::GetAvailableID() {
  std::lock_guard<std::mutex> lock(resourceMutex);
  if (availableIDs.empty()) {
    int oldMaxID = this->maxID;
    this->maxID += 100;
    for (int i = oldMaxID + 1; i <= this->maxID; ++i) {
      availableIDs.emplace(i);
    }
  }
  int id = availableIDs.front();
  availableIDs.pop();
  return id;
}

void ResourceManager::ReturnID(int id) {
  std::lock_guard<std::mutex> lock(resourceMutex);
  availableIDs.emplace(id);
}

int ResourceManager::GetMaxAvailableID() {
  std::lock_guard<std::mutex> lock(resourceMutex);
  return maxID;
}
