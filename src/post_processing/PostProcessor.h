#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "ResourceManager.h"
#include "Shader.h"
#include "SpriteRenderer.h"
#include "Texture.h"

// PostProcessor hosts all PostProcessing effects for the
// DynastysDefender-ScrollsCurse Game.
class PostProcessor {
 public:
  PostProcessor(Shader shader, unsigned int width, unsigned int height);
  ~PostProcessor();

  void BeginRender();
  void EndRender();
  void Render(float time);
  void SetConfuse(bool confuse);
  void SetChaos(bool chaos);
  bool IsChaos() const;
  void SetShake(bool shake, float strength = 0.01f,
                float timeMultiplierForX = 10.f,
                float timeMultiplierForY = 15.f);
  void SetGrayscale(bool grayscale);
  void SetBlur(bool blur);
  void SetSampleOffsets(float offset);
  float GetSampleOffsets() const;
  void SetIntensity(float intensity);
  float GetIntensity() const;
  void SetSrcViewPort(const ViewPortInfo& viewPortInfo);
  void SetDstViewPort(const ViewPortInfo& viewPortInfo);

  // Resize the framebuffer
  void Resize(SizePadding sizePadding);

 private:
  // state
  Texture2D texture;
  Shader postProcessingShader;
  unsigned int width{}, height{};
  // view port
  ViewPortInfo srcViewPort, dstViewPort;

  // options
  bool confuse{false}, chaos{false}, shake{false}, grayscale{false},
      blur{false};
  // paremeters
  float shakingStrength{0.01f}, timeMultiplierForX{10.0f},
      timeMultiplierForY{15.0f}, offset{1.0f / 300.0f}, intensity{1.0f};
  unsigned int MSFBO{}, FBO{};
  unsigned int RBO{};
  unsigned int VAO{};
  bool hasBeganRender{false}, hasEndedRender{false};

  void initRenderData();
};