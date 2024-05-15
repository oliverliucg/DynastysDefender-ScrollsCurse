#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"
#include "ResourceManager.h"

// PostProcessor hosts all PostProcessing effects for the DynastysDefender-ScrollsCurse Game.
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
	void SetSrcViewPort(const ViewPortInfo& viewPortInfo);
	void SetDstViewPort(const ViewPortInfo& viewPortInfo);
	// Resize the framebuffer
	void Resize(SizePadding sizePadding);
private:
	// state
	Texture2D texture;
	Shader postProcessingShader;
	unsigned int width, height;
	// view port
	ViewPortInfo srcViewPort, dstViewPort;

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