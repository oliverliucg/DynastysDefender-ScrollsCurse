#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"

// PostProcessor hosts all PostProcessing effects for the Puzzle Bobble Game.
class PostProcessor{
public:
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	~PostProcessor();

	void BeginRender();
	void EndRender();
	void Render(float time);
	void SetConfuse(bool confuse);
	void SetChaos(bool chaos);
	void SetShake(bool shake, float strength = 0.01f, float timeMultiplierForX = 10.f, float timeMultiplierForY = 15.f);
	void SetGrayscale(bool grayscale);
	void SetBlur(bool blur);
	void SetSampleOffsets(float offset);
	float GetSampleOffsets();
	bool HasBeganRender();
	bool HasEndedRender();
	// Resize the framebuffer
	void Resize(unsigned int width, unsigned int height);
private:
	// state
	Texture2D texture;
	Shader postProcessingShader;
	unsigned int width, height;
	// options
	bool confuse, chaos, shake, grayscale, blur;
	// paremeters
	float shakingStrength, timeMultiplierForX, timeMultiplierForY, offset;
	unsigned int MSFBO, FBO;
	unsigned int RBO;
	unsigned int VAO;
	bool hasBeganRender, hasEndedRender;

	void initRenderData();

};