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
};

// const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
// const int SCREEN_WIDTH = (mode->width * 27) / 32;
// const int SCREEN_HEIGHT = (mode->height * 7) / 8;
// const glm::vec2 kWindowSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);

float screenScale = 1.0f;
float kBubbleRadius = kWindowSize.y / 42.f;
float kVelocityUnit = 2 * kBubbleRadius;
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

glm::vec2 rotateVector(glm::vec2 vector, float angle, glm::vec2 point) {
  // translate the vector so that the point is at the origin
  vector -= point;
  // rotate the vector using glm::rotate
  vector = glm::rotate(vector, angle);
  // translate the vector back to its original position
  vector += point;
  return vector;
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

// glm::vec2 getRelativePositionRatio(glm::vec2 object1Pos, glm::vec2
// object2Pos, glm::vec2 object1Size) {
//     return (object2Pos - object1Pos) / object1Size;
// }

// float generateRandomFloat(float min, float max) {
//     if (min == max) {
//         return min;
//     }
//     else if (min > max) {
//         std::swap(min, max);
//     }
//     std::random_device rd; // Obtain a random number from hardware
//     std::mt19937 eng(rd()); // Seed the generator
//     std::uniform_real_distribution<float> distr(min, max); // Define the
//     range return distr(eng);
// }
//
// int generateRandomInt(int min, int max) {
//     if (min == max) {
//		return min;
//	}
//     else if (min > max) {
//		std::swap(min, max);
//	}
//	std::random_device rd; // Obtain a random number from hardware
//	std::mt19937 eng(rd()); // Seed the generator
//	std::uniform_int_distribution<int> distr(min, max); // Define the range
//	return distr(eng);
// }

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

std::string u32StringToString(std::u32string u32str) {
    boost::locale::generator gen;
    std::locale loc = gen(""); // Create a system default locale
    std::locale::global(loc); // Make it the global locale
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
    std::cout << "width: " << width << " height: " << height
              << " newWidth: " << newWidth << " newHeight: " << newHeight
              << std::endl;
    std::cout << "padTop: " << padTop << " padBottom: " << padBottom
              << " padLeft: " << padLeft << " padRight: " << padRight
              << std::endl;
    std::cout << "standardWidth: " << standardWidth
              << " standardHeight: " << standardHeight << std::endl;
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
  std::lock_guard<std::mutex> lock(resourceMutex);
  Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
  if (Shaders.count(name) == 0)
    std::cout << "ERROR::RESOURCEMANAGER::SHADER_NOT_FOUND" << std::endl;
  assert(Shaders.count(name) > 0);
  return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha,
                                       std::string name) {
  std::lock_guard<std::mutex> lock(resourceMutex);
  Textures[name] = loadTextureFromFile(file, alpha);
  return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
  if (Textures.count(name) == 0)
    std::cout << "ERROR::RESOURCEMANAGER::TEXTURE_NOT_FOUND" << std::endl;
  assert(Textures.count(name) > 0);
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

// std::string ResourceManager::GetText(const std::vector<std::string> keys)
// const
//{
//     nlohmann::json res;
//	for(const auto& key : keys)
//	{
//		assert(texts.count(key) > 0 && "Key not found in texts");
//         res = texts.at(key);
//	}
//     assert(res.is_string() && "Value is not a string");
//     return res;
// }

// std::string ResourceManager::GetText(const std::string key) const
//{
//	return GetText(std::vector<std::string>{key});
// }

void ResourceManager::Clear() {
  for (auto iter : Shaders) glDeleteProgram(iter.second.ID);
  for (auto iter : Textures) glDeleteTextures(1, &iter.second.ID);
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
