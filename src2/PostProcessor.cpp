#include "PostProcessor.h"

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height) : texture(), postProcessingShader(shader), width(width), height(height), confuse(false), chaos(false), shake(false), grayscale(false), blur(false), hasBeganRender(false), hasEndedRender(false), shakingStrength(0.01f), timeMultiplierForX(10.0f), timeMultiplierForY(15.0f), offset(offset){
    // initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &this->MSFBO);
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->RBO);
    // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // attach MS render buffer object to framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
    // also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->texture.Generate(width, height, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture.ID, 0); // attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // initialize render data and uniforms
    this->initRenderData();
    this->postProcessingShader.SetInteger("scene", 0, true);
    float offset = 1.0f / 300.0f;
    SetSampleOffsets(offset);
    int edge_kernel[25] = {
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,
        -1, -1, 24, -1, -1,
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->postProcessingShader.ID, "edge_kernel"), 25, edge_kernel);
    float blur_kernel[25] = {
        1.0f / 256.0f,  4.0f / 256.0f,  6.0f / 256.0f,  4.0f / 256.0f, 1.0f / 256.0f,
        4.0f / 256.0f, 16.0f / 256.0f, 24.0f / 256.0f, 16.0f / 256.0f, 4.0f / 256.0f,
        6.0f / 256.0f, 24.0f / 256.0f, 36.0f / 256.0f, 24.0f / 256.0f, 6.0f / 256.0f,
        4.0f / 256.0f, 16.0f / 256.0f, 24.0f / 256.0f, 16.0f / 256.0f, 4.0f / 256.0f,
        1.0f / 256.0f,  4.0f / 256.0f,  6.0f / 256.0f,  4.0f / 256.0f, 1.0f / 256.0f
    };
    glUniform1fv(glGetUniformLocation(this->postProcessingShader.ID, "blur_kernel"), 25, blur_kernel);
}

PostProcessor::~PostProcessor() {
	glDeleteFramebuffers(1, &this->MSFBO);
	glDeleteFramebuffers(1, &this->FBO);
	glDeleteRenderbuffers(1, &this->RBO);
}

void PostProcessor::SetShake(bool shake, float strength, float timeMultiplierForX, float timeMultiplierForY) {
	this->shake = shake;
    this->shakingStrength = strength;
	this->timeMultiplierForX = timeMultiplierForX;
	this->timeMultiplierForY = timeMultiplierForY;
}

void PostProcessor::SetConfuse(bool confuse) {
	this->confuse = confuse;
}

void PostProcessor::SetChaos(bool chaos) {
	this->chaos = chaos;
}

void PostProcessor::SetGrayscale(bool grayscale) {
	this->grayscale = grayscale;
}

void PostProcessor::SetBlur(bool blur) {
	this->blur = blur;
}

void PostProcessor::SetSampleOffsets(float offset) {
	this->postProcessingShader.Use();
	this->offset = offset;
    float offsets[25][2] = {
		{-2 * offset,  2 * offset}, // top-left corner
		{-1 * offset,  2 * offset}, // top-left
		{ 0 * offset,  2 * offset}, // top-center
		{ 1 * offset,  2 * offset}, // top-right
		{ 2 * offset,  2 * offset}, // top-right corner
		{-2 * offset,  1 * offset}, // middle-left
		{-1 * offset,  1 * offset},
		{ 0 * offset,  1 * offset},
		{ 1 * offset,  1 * offset},
		{ 2 * offset,  1 * offset}, // middle-right
		{-2 * offset,  0 * offset}, // center-left
		{-1 * offset,  0 * offset}, // center-left
		{ 0 * offset,  0 * offset}, // center-center
		{ 1 * offset,  0 * offset}, // center-right
		{ 2 * offset,  0 * offset}, // center-right
		{-2 * offset, -1 * offset}, // middle-left
		{-1 * offset, -1 * offset},
		{ 0 * offset, -1 * offset},
		{ 1 * offset, -1 * offset},
		{ 2 * offset, -1 * offset}, // middle-right
		{-2 * offset, -2 * offset}, // bottom-left corner
		{-1 * offset, -2 * offset}, // bottom-left
		{ 0 * offset, -2 * offset}, // bottom-center
		{ 1 * offset, -2 * offset}, // bottom-right
		{ 2 * offset, -2 * offset}  // bottom-right corner
	};
	glUniform2fv(glGetUniformLocation(this->postProcessingShader.ID, "offsets"), 25, (float*)offsets);
}

float PostProcessor::GetSampleOffsets() {
	return this->offset;
}

void PostProcessor::BeginRender() {
    assert(!this->hasBeganRender && !this->hasEndedRender);
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear all relevant buffers
	glClear(GL_COLOR_BUFFER_BIT);
    this->hasBeganRender = true;
}

void PostProcessor::EndRender() {
    assert(this->hasBeganRender && !this->hasEndedRender);
	// now resolve multisampled color-buffer into intermediate FBO to store to texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer
    this->hasEndedRender = true;
}

void PostProcessor::Render(float time) {
	assert(this->hasBeganRender && this->hasEndedRender);
	// set uniforms/options
	this->postProcessingShader.Use();
	this->postProcessingShader.SetFloat("time", time);
    this->postProcessingShader.SetFloat("timeMultiplierForX", this->timeMultiplierForX);
    this->postProcessingShader.SetFloat("timeMultiplierForY", this->timeMultiplierForY);
    this->postProcessingShader.SetFloat("shakingStrength", this->shakingStrength);
	this->postProcessingShader.SetInteger("confuse", this->confuse);
	this->postProcessingShader.SetInteger("chaos", this->chaos);
	this->postProcessingShader.SetInteger("shake", this->shake);
	this->postProcessingShader.SetInteger("grayscale", this->grayscale);
    this->postProcessingShader.SetInteger("blur", this->blur);
	// render textured quad
	glActiveTexture(GL_TEXTURE0);
	this->texture.Bind();
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
    this->hasBeganRender = this->hasEndedRender = false;
}

void PostProcessor::initRenderData() {
    // configure VAO/VBO
	unsigned int VBO;
    float vertices[] = {
		// pos        // tex
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}