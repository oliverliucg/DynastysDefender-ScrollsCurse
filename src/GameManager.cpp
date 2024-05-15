#include "GameManager.h"
#include <cstring>

GameManager::GameManager(unsigned int width, unsigned int height)
	: state(GameState::INITIAL), lastState(GameState::UNDEFINED), targetState(GameState::UNDEFINED), transitionState(TransitionState::START), 
    screenMode(ConfigManager::GetInstance().GetScreenMode()), targetScreenMode(ScreenMode::UNDEFINED), width(static_cast<float>(width)), height(static_cast<float>(height)), level(1), activePage("")
{
    // Initialize all key states to false
    memset(keys, false, sizeof(keys));
    memset(keysLocked, false, sizeof(keysLocked));

    // Set the scroll offset to 0
    scrollYOffset = 0.f;
    scrollSensitivity = 25.0f;

    // Initialize the mouse states
    leftMousePressed = false;
    isReadyToDrag = true;
    isDragging = false;
    gameArenaShaking = false;
}

GameManager::~GameManager() {
}
void GameManager::Init() {
    std::cout << "bubble radius: " << kBubbleRadius << std::endl;
    std::cout << "bubble size: " << kBubbleSize.x << " x " << kBubbleSize.y << std::endl;
    std::cout << "game window size: " << this->width << " x " << this->height << std::endl;
    std::cout << "full window size: " << kFullScreenSize.x << " x " << kFullScreenSize.y << std::endl;
    std::cout << "windowed mode size: " << kWindowedModeSize.x << " x " << kWindowedModeSize.y << std::endl;
    std::cout << "full window paddings: " << kFullScreenSizePadding.padLeft << ", " << kFullScreenSizePadding.padRight << ", " << kFullScreenSizePadding.padTop << ", " << kFullScreenSizePadding.padBottom << std::endl;
  
    //if (this->state == GameState::OPTION) {
    //    ResourceManager& resourceManager = ResourceManager::GetInstance();
    //    /*resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/sprite.vs.txt", "C:/Users/xiaod/resources/shaders/sprite.fs.txt", nullptr, "sprite");*/
    //    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/pure_color.vs.txt", "C:/Users/xiaod/resources/shaders/pure_color.fs.txt", nullptr, "purecolor");
    //    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/text_2d.vs.txt", "C:/Users/xiaod/resources/shaders/text_2d.fs.txt", nullptr, "text");
    //    // configure shaders
    //    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
    //        static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
    //    resourceManager.GetShader("purecolor").Use().SetMatrix4("projection", projection);
    //    colorRenderer = std::make_shared<ColorRenderer>(resourceManager.GetShader("purecolor"));
    //    circleRenderer = std::make_shared<CircleRenderer>(resourceManager.GetShader("purecolor"), 0.5f);
    //    textRenderer2 = std::make_shared<TextRenderer>(resourceManager.GetShader("text"), this->width, this->height);
    //    // Initialize text renderer
    //    textRenderer = std::make_shared<TextRenderer>(resourceManager.GetShader("text"), this->width, this->height);
    //    textRenderer->Load("C:/Users/xiaod/resources/fonts/Prompt-MediumItalic.TTF", this->height * 0.03f);
    //    textRenderer2 = std::make_shared<TextRenderer>(resourceManager.GetShader("text"), this->width, this->height);
    //    textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Regular.TTF", kFontSize, CharStyle::Regular);
    //    //textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Bold.TTF", kFontSize, CharStyle::Bold);
    //    //textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Italic.TTF", kFontSize, CharStyle::Italic);
    //    //textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-BoldItalic.TTF", kFontSize, CharStyle::BoldItalic);

    //    buttons["fullscreen"] = std::make_shared<Button>(glm::vec2(this->width * 0.15f, this->height * 0.18f), glm::vec2(this->width * 0.7f, this->height * 0.25f), "Full-Screen");
    //    buttons["windowed"] = std::make_shared<Button>(glm::vec2(this->width * 0.15f, this->height * 0.48f), glm::vec2(this->width * 0.7f, this->height * 0.25f), "Windowed");
    //    buttons["exit"] = std::make_shared<Button>(glm::vec2(this->width * 0.82f + this->height * 0.075f - this->height * 0.01f, this->height * 0.85f - this->height * 0.01f), glm::vec2(this->width * 0.18f, this->height * 0.15f), "Exit");
    //    buttons["fullscreen"]->SetState(ButtonState::kNormal);
    //    buttons["windowed"]->SetState(ButtonState::kNormal);
    //    buttons["exit"]->SetState(ButtonState::kNormal);

    //    return;
    //}
    this->SetState(GameState::INITIAL);
    this->GoToState(GameState::STORY);

    // load shaders
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/sprite.vs.txt", "C:/Users/xiaod/resources/shaders/sprite.fs.txt", nullptr, "sprite");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/post_processing.vs.txt", "C:/Users/xiaod/resources/shaders/post_processing.fs.txt", nullptr, "postprocessing");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/pure_color.vs.txt", "C:/Users/xiaod/resources/shaders/pure_color.fs.txt", nullptr, "purecolor");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/ray.vs.txt", "C:/Users/xiaod/resources/shaders/ray.fs.txt", nullptr, "ray");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/particle.vs.txt", "C:/Users/xiaod/resources/shaders/particle.fs.txt", nullptr, "particle");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/text_2d.vs.txt", "C:/Users/xiaod/resources/shaders/text_2d.fs.txt", nullptr, "text");
    resourceManager.LoadShader("C:/Users/xiaod/resources/shaders/discard.vs.txt", "C:/Users/xiaod/resources/shaders/discard.fs.txt", nullptr, "discard");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
        static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
    resourceManager.GetShader("sprite").Use().SetInteger("image", 0);
    resourceManager.GetShader("sprite").SetMatrix4("projection", projection);
    resourceManager.GetShader("purecolor").Use().SetMatrix4("projection", projection);
    resourceManager.GetShader("ray").Use().SetMatrix4("projection", projection);
    resourceManager.GetShader("particle").Use().SetInteger("sprite", 0);
    resourceManager.GetShader("particle").SetMatrix4("projection", projection);
    resourceManager.GetShader("discard").Use().SetInteger("image", 0);
    resourceManager.GetShader("discard").SetMatrix4("projection", projection);

    // set render-specific controls
    spriteRenderer = std::make_shared<SpriteRenderer>(resourceManager.GetShader("sprite"));
    spriteDynamicRenderer = std::make_shared<SpriteDynamicRenderer>(resourceManager.GetShader("sprite"));
    partialTextureRenderer = std::make_shared<PartialTextureRenderer>(resourceManager.GetShader("discard"));
    colorRenderer = std::make_shared<ColorRenderer>(resourceManager.GetShader("purecolor"));
    circleRenderer = std::make_shared<CircleRenderer>(resourceManager.GetShader("purecolor"), 0.5f);
    rayRenderer = std::make_shared<RayRenderer>(resourceManager.GetShader("ray"));
    lineRenderer = std::make_shared<LineRenderer>(resourceManager.GetShader("ray"));
    
 //   int paddedWidth = this->width, paddedHeight = this->height;
 //   if (this->screenMode == ScreenMode::FULLSCREEN) {
 //       paddedWidth += kFullScreenSizePadding.padLeft + kFullScreenSizePadding.padRight;
	//	paddedHeight += kFullScreenSizePadding.padTop + kFullScreenSizePadding.padBottom;
	//}
 //   std::cout << "padded width: " << paddedWidth << ", padded height: " << paddedHeight << std::endl;
    postProcessor = std::make_shared<PostProcessor>(resourceManager.GetShader("postprocessing"), this->width, this->height);

    // load textures
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/handynastry2.png", false, "background");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/arenagray.png", true, "gameboard");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/graybubble.png", true, "bubble");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow.png", true, "shooter");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow1.png", true, "shooter1");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow_h.png", true, "arrow");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow_h_1.png", true, "arrow1");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow_h_2.png", true, "arrow2");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/ancient_arrow_h_3.png", true, "arrow3");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/particle.png", true, "particle");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/particle1.png", true, "particle1");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/cracks.png", true, "cracks");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/liuchesad.png", true, "liuchesad");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/liuchehappy.png", true, "liuchehappy");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/liucheangry.png", true, "liucheangry");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weizifusad.png", true, "weizifusad");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weizifuhappy.png", true, "weizifuhappy");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/guojiefight.png", true, "guojiefight");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/guojiesad.png", true, "guojiesad");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weiqingfight2.png", true, "weiqingfight");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weiqingsad.png", true, "weiqingsad");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weiqinghappy.png", true, "weiqinghappy");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/weiqingwin.png", true, "weiqingwin");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/scroll_paper1.png", true, "scrollpaper");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/scroll_upper.png", true, "scrollupper");
    resourceManager.LoadTexture("C:/Users/xiaod/resources/textures/scroll_lower.png", true, "scrolllower");
    // Create game board
    gameBoard = std::make_unique<GameBoard>(glm::vec2(this->width / 3, this->height*0.09), glm::vec2(this->width / 3, this->height*0.82), glm::vec4(GameBoardColorMap[GameBoardColor::ORIGINAL], 0.9f), resourceManager.GetTexture("scrollpaper"));

    // Create scroll    
    scroll = std::make_unique<Scroll>(glm::vec2(
        this->width / 2.0f, this->height / 2.0f), 
        glm::vec2(this->width / 1.8f, this->width / 3 / 11.7772021), 
        glm::vec2(this->gameBoard->GetSize().x, 0.f),
        resourceManager.GetTexture("scrollupper"));
    // Set the scroll's y position to be out of the top of the screen
    scroll->SetTargetCenter("outofscreen", glm::vec2(this->width / 2.0f, -scroll->GetRollerSize().y / 2.0f));
    scroll->SetCenter(scroll->GetTargetCenter("outofscreen"));

    // Create characters.
    // Liu Che
    std::unordered_map<GameCharacterState, glm::vec2> positions = {
    	{GameCharacterState::SAD, glm::vec2(this->width * 0.01, this->height * 0.7)},
		{GameCharacterState::HAPPY, glm::vec2(this->width * 0.01, this->height * 0.7)},
		{GameCharacterState::ANGRY, glm::vec2(this->width * 0.01, this->height * 0.7)}
    };
    std::unordered_map<GameCharacterState, glm::vec2> sizes = {
        {GameCharacterState::SAD, glm::vec2(this->height / 1.6893 / 4.5, this->height / 4.5)},
        {GameCharacterState::HAPPY, glm::vec2(this->height / 1.74 / 4.5, this->height / 4.5)},
        {GameCharacterState::ANGRY, glm::vec2(this->height / 1.6871 / 4.5, this->height / 4.5)}
    };
    std::unordered_map<GameCharacterState, Texture2D> textures = {
        {GameCharacterState::SAD, resourceManager.GetTexture("liuchesad")},
        {GameCharacterState::HAPPY, resourceManager.GetTexture("liuchehappy")},
        {GameCharacterState::ANGRY, resourceManager.GetTexture("liucheangry")}
    };
    gameCharacters["liuche"] = std::make_shared<GameCharacter>("liuche", GameCharacterState::HAPPY, positions, sizes, 0.0f, glm::vec2(0.5f, 0.5f), textures);
    gameCharacters["liuche"]->SetFaceDirection(false);

    // Wei Zi Fu
    positions.clear();
    positions = {
        {GameCharacterState::SAD, glm::vec2(this->width * 0.05, this->height * 0.65)},
        {GameCharacterState::HAPPY, glm::vec2(this->width * 0.045, this->height * 0.65)}
    };
    sizes.clear();
    sizes = {
		{GameCharacterState::SAD, glm::vec2(this->height / 2.15196 / 4.2, this->height / 4.2)},
		{GameCharacterState::HAPPY, glm::vec2(this->height / 1.98169 / 4.2, this->height / 4.2)}
	};
    textures.clear();
    textures = {
		{GameCharacterState::SAD, resourceManager.GetTexture("weizifusad")},
		{GameCharacterState::HAPPY, resourceManager.GetTexture("weizifuhappy")}
	};
    gameCharacters["weizifu"] = std::make_shared<GameCharacter>("weizifu", GameCharacterState::HAPPY, positions, sizes, 0.0f, glm::vec2(0.5f, 0.5f), textures);
    gameCharacters["weizifu"]->SetFaceDirection(false);
    
    // Guo Jie
    positions.clear();
    positions = {
		{GameCharacterState::FIGHTING, glm::vec2(this->width * 0.15, this->height * 0.67)},
        {GameCharacterState::SAD, glm::vec2(this->width * 0.155, this->height * 0.665)}
	};
    sizes.clear();
    sizes = {
        {GameCharacterState::FIGHTING, glm::vec2(this->height / 1.7053 / 4, this->height / 4)},
        {GameCharacterState::SAD, glm::vec2(this->height / 2.0662 / 4, this->height / 4)}
    };
    textures.clear();
    textures = {
        {GameCharacterState::FIGHTING, resourceManager.GetTexture("guojiefight")},
        {GameCharacterState::SAD, resourceManager.GetTexture("guojiesad")}
    };
    gameCharacters["guojie"] = std::make_shared<GameCharacter>("guojie", GameCharacterState::FIGHTING, positions, sizes, 0.0f, glm::vec2(0.5f, 0.5f), textures);
    gameCharacters["guojie"]->SetFaceDirection(false);

    // Set health for characters weiqing and guojie
    glm::vec2 totalHealthBarPos = glm::vec2(positions[GameCharacterState::FIGHTING].x - kBubbleRadius * 1.0f, positions[GameCharacterState::FIGHTING].y - kBubbleRadius * 0.9f);
    glm::vec2 totalHealthBarSize = glm::vec2(kBubbleRadius * 8.f, kBubbleRadius * 0.7f);
    gameCharacters["guojie"]->GetHealth().SetTotalHealth(this->numGameLevels);
    gameCharacters["guojie"]->GetHealth().SetTotalHealthBar(totalHealthBarPos, totalHealthBarSize, glm::vec4(1.f, 1.f, 1.f, 1.f));
    gameCharacters["guojie"]->GetHealth().SetCurrentHealth(this->numGameLevels);
    gameCharacters["guojie"]->GetHealth().SetDamagePopOutDirection(false);
    gameCharacters["guojie"]->SetHealthBarRelativeCenterRatios();
    // Set the rotation pivot for the characters
    gameCharacters["guojie"]->SetRotationPivot(glm::vec2(0.5f, 0.9f));
    // Set the target position "landing"
    gameCharacters["guojie"]->SetTargetPosition("landing", gameCharacters["guojie"]->GetPosition());
    // Set the target position "out of the screen"
    gameCharacters["guojie"]->SetTargetPosition("outofscreen", gameCharacters["guojie"]->GetPosition() - glm::vec2(0, 40 * kBubbleRadius));

    // Wei Qing
    positions.clear();
    positions = {
        {GameCharacterState::FIGHTING, glm::vec2(this->width * 0.74, this->height * 0.665)},
        {GameCharacterState::SAD, glm::vec2(this->width * 0.775, this->height * 0.665)},
        {GameCharacterState::HAPPY, glm::vec2(this->width * 0.775, this->height * 0.665)},
        {GameCharacterState::WINNING, glm::vec2(this->width * 0.757, this->height * 0.665)}
    };
    sizes.clear();
    sizes = {
		{GameCharacterState::FIGHTING, glm::vec2(this->height / 1.222 / 4, this->height / 4)},
		{GameCharacterState::SAD, glm::vec2(this->height / 1.6902 / 4, this->height / 4)},
		{GameCharacterState::HAPPY, glm::vec2(this->height / 1.757 / 4, this->height / 4)},
		{GameCharacterState::WINNING, glm::vec2(this->height / 1.4359 / 4, this->height / 4)}
	};
    textures.clear();
    textures = {
		{GameCharacterState::FIGHTING, resourceManager.GetTexture("weiqingfight")},
		{GameCharacterState::SAD, resourceManager.GetTexture("weiqingsad")},
		{GameCharacterState::HAPPY, resourceManager.GetTexture("weiqinghappy")},
		{GameCharacterState::WINNING, resourceManager.GetTexture("weiqingwin")}
	};
    gameCharacters["weiqing"] = std::make_shared<GameCharacter>("weiqing", GameCharacterState::HAPPY, positions, sizes, 0.0f, glm::vec2(0.5f, 0.5f), textures);

    // Set health for characters weiqing and guojie
    totalHealthBarPos = glm::vec2(positions[GameCharacterState::FIGHTING].x + kBubbleRadius * 1.0f, positions[GameCharacterState::FIGHTING].y-kBubbleRadius * 0.9f);
    gameCharacters["weiqing"]->GetHealth().SetTotalHealth(1);
    gameCharacters["weiqing"]->GetHealth().SetTotalHealthBar(totalHealthBarPos, totalHealthBarSize, glm::vec4(1.f, 1.f, 1.f, 1.f));
    gameCharacters["weiqing"]->GetHealth().SetCurrentHealth(1);
    gameCharacters["weiqing"]->SetHealthBarRelativeCenterRatios();

    // Set the rotation pivot for the characters
    gameCharacters["weiqing"]->SetRotationPivot(glm::vec2(0.5f, 0.9f));

    // Create shooter
    shooter = std::make_unique<Shooter>(glm::vec2(this->width / 2 - gameBoard->GetSize().x * 14.f / 135, gameBoard->GetPosition().y + gameBoard->GetSize().y * 0.800117578f), glm::vec2(gameBoard->GetSize().x*7.f/135, gameBoard->GetSize().y * 300.f/1701), glm::vec2(0.5f, 0.53f), glm::vec2(0.5f, 0.8f), resourceManager.GetTexture("shooter1"));
    shooter->GenerateBubble();
    shooter->SetPosition(glm::vec2(this->width / 2 - shooter->GetCarriedBubble().GetRadius(), gameBoard->GetPosition().y + gameBoard->GetSize().y * 0.800117578f));

    // Initialize particles
    shadowTrailSystem = std::make_unique<ShadowTrailSystem>(resourceManager.GetShader("particle"), resourceManager.GetTexture("particle"), 6000);
    explosionSystem = std::make_unique<ExplosionSystem>(resourceManager.GetShader("particle"), resourceManager.GetTexture("particle1"), 3000);

    // Initialize text renderer
    textRenderer = std::make_shared<TextRenderer>(resourceManager.GetShader("text"), this->width, this->height);
    textRenderer->Load("C:/Users/xiaod/resources/fonts/Prompt-MediumItalic.TTF", this->height * 0.03f);
    textRenderer2 = std::make_shared<TextRenderer>(resourceManager.GetShader("text"), this->width, this->height);
    textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Regular.TTF", kFontSize, CharStyle::Regular);
    textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Bold.TTF", kFontSize, CharStyle::Bold);
    textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-Italic.TTF", kFontSize, CharStyle::Italic);
    textRenderer2->Load("C:/Users/xiaod/resources/fonts/Prompt-BoldItalic.TTF", kFontSize, CharStyle::BoldItalic);

    // Initialize text box
    texts["story"] = std::make_shared<Text>(/*pos=*/glm::vec2(gameBoard->GetPosition().x + kBubbleRadius / 2.0f, gameBoard->GetPosition().y + kBubbleRadius / 2.0f), 
        /*lineWidth=*/gameBoard->GetSize().x - 2 * kBubbleRadius);
    texts["story"]->AddParagraph(kBackgroundStoryPart1);
    texts["story"]->AddParagraph(kBackgroundStoryPart2);
    texts["story"]->SetScale(0.03f/kFontScale);

    texts["control"] = std::make_shared<Text>(/*pos=*/glm::vec2(gameBoard->GetPosition().x + kBubbleRadius / 2.0f, gameBoard->GetPosition().y + kBubbleRadius / 2.0f),
        		/*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts["control"]->AddParagraph(kControlInstruction1);
    texts["control"]->AddParagraph(kControlInstruction2);
    texts["control"]->AddParagraph(kControlInstruction3);
    texts["control"]->AddParagraph(kControlInstruction4);
    texts["control"]->SetScale(0.0216f/kFontScale);

    texts["victory"] = std::make_shared<Text>(/*pos=*/glm::vec2(0,0),
        		/*lineWidth=*/std::numeric_limits<float>::max(), /*boxBounds=*/glm::vec4(0.f, 0.f, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    texts["victory"]->AddParagraph("{[VICTORY]}");
    texts["victory"]->SetColor(glm::vec3(1.f, 0.f, 0.f));
    texts["victory"]->SetTargetScale("max", 2.f / kFontScale);
    texts["victory"]->SetScale(texts["victory"]->GetTargetScale("max"));
    texts["victory"]->SetCenter(glm::vec2(this->width / 2.0f, this->height / 2.0f));

    texts["defeated"] = std::make_shared<Text>(/*pos=*/glm::vec2(0, 0),
        				/*lineWidth=*/std::numeric_limits<float>::max(), /*boxBounds=*/glm::vec4(0.f, 0.f, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    texts["defeated"]->AddParagraph("{[DEFEATED]}");
    texts["defeated"]->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
    texts["defeated"]->SetTargetScale("max", 2.f / kFontScale);
    texts["defeated"]->SetScale(texts["defeated"]->GetTargetScale("max"));
    texts["defeated"]->SetCenter(glm::vec2(this->width / 2.0f, this->height / 2.0f));

    texts["prompttomainmenu"] = std::make_shared<Text>(/*pos=*/glm::vec2(this->width * 0.4f, this->height * 0.65f), /*lineWidth=*/std::numeric_limits<float>::max());
    texts["prompttomainmenu"]->AddParagraph(kPromptToMainMenuText);
    texts["prompttomainmenu"]->SetColor(glm::vec3(1.f, 1.f, 1.f));
    texts["prompttomainmenu"]->SetScale(0.025f / kFontScale);
    texts["prompttomainmenu"]->SetCenter(glm::vec2(this->width / 2.0f, this->height * 0.65f));

    texts["time"] = std::make_shared<Text>(/*pos=*/glm::vec2(gameBoard->GetPosition().x + gameBoard->GetSize().x - 2*kBubbleRadius, gameBoard->GetPosition().y - kBubbleRadius),
        /*lineWidth=*/gameBoard->GetSize().x);
    texts["time"]->AddParagraph("30");
    texts["time"]->SetScale(0.025f / kFontScale);
    // Create buttons
   /*Button menuButton = Button(glm::vec2(this->width / 2.0f - kBubbleRadius * 4.0f, this->height * 0.84f), glm::vec2(kBubbleRadius * 8.f, kBubbleRadius * 3.0f), "Menu");*/
   //glm::vec2 commonButtionPosition = glm::vec2(this->width / 2.0f - kBubbleRadius * 4.5f, this->height * 0.84f);
   //glm::vec2 commonButtonSize = glm::vec2(gameBoard->GetSize().x-2*(commonButtionPosition.x-gameBoard->GetPosition().x), kBubbleRadius * 3.0f);
    buttons["back"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 4.5f, this->height * 0.84f), glm::vec2(kBubbleRadius * 6.5f, kBubbleRadius * 3.0f), "Back");
    buttons["control"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 4.5f, this->height * 0.84f), glm::vec2(kBubbleRadius * 9.5f, kBubbleRadius * 3.0f), "Control");
    buttons["start"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 11.0f, this->height * 0.84f), glm::vec2(kBubbleRadius * 6.5f, kBubbleRadius * 3.0f), "Start");
    buttons["exit"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f + kBubbleRadius * 5.0f, this->height * 0.84f), glm::vec2(kBubbleRadius * 5.5f, kBubbleRadius * 3.0f), "Exit");
    buttons["restart"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 11.5f, this->height * 0.84f), glm::vec2(kBubbleRadius * 9.0f, kBubbleRadius * 3.0f), "Restart");
    buttons["resume"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 2.5f, this->height * 0.84f), glm::vec2(kBubbleRadius * 10.0f, kBubbleRadius * 3.0f), "Resume");
    buttons["stop"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f + kBubbleRadius * 7.0f, this->height * 0.84f), glm::vec2(kBubbleRadius * 6.0f, kBubbleRadius * 3.0f), "Stop");
    buttons["displaysettings"] = std::make_shared<Button>(glm::vec2(this->width / 2.0f - kBubbleRadius * 4.5f, this->height * 0.84f), glm::vec2(kBubbleRadius * 18.f, kBubbleRadius * 3.0f), "Display");

    // Create page "story" / "Main Menu"
    auto textUnit = std::make_shared<TextUnit>("storytextunit", texts["story"], textRenderer2);
    auto startButtonUnit = std::make_shared<ButtonUnit>("startbuttonunit", buttons["start"], textRenderer2, colorRenderer);
    auto controlButtonUnit = std::make_shared<ButtonUnit>("controlbuttonunit", buttons["control"], textRenderer2, colorRenderer);
    auto displaySettingsButtonUnit = std::make_shared<ButtonUnit>("displaysettingsbuttonunit", buttons["displaysettings"], textRenderer2, colorRenderer);
    auto exitButtonUnit = std::make_shared<ButtonUnit>("exitbuttonunit", buttons["exit"], textRenderer2, colorRenderer);
    // Create character introduction units
    texts["liucheintro"] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts["liucheintro"]->AddParagraph(kLiuCheIntroduction);
    texts["liucheintro"]->SetScale(0.0216f / kFontScale);
    texts["weizifuintro"] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts["weizifuintro"]->AddParagraph(kWeiZiFuIntroduction);
    texts["weizifuintro"]->SetScale(0.0216f / kFontScale);
    texts["weiqingintro"] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts["weiqingintro"]->AddParagraph(kWeiQingIntroduction);
    texts["weiqingintro"]->SetScale(0.0216f / kFontScale);
    texts["guojieintro"] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts["guojieintro"]->AddParagraph(kGuoJieIntroduction);
    texts["guojieintro"]->SetScale(0.0216f / kFontScale);

    auto liucheIconUnit = std::make_shared<ImageUnit>("liucheiconunit", 
        /*pos=*/glm::vec2(0.f), /*size=*/gameCharacters.at("liuche")->GetGeneralSize(GameCharacterState::HAPPY)*0.75f, 
        /*texture=*/gameCharacters.at("liuche")->GetGeneralTexture(GameCharacterState::HAPPY), spriteRenderer);
    auto liucheTextUnit = std::make_shared<TextUnit>("liuchetextunit", texts["liucheintro"], textRenderer2);
    auto liucheIconTextUnit = std::make_shared<OptionUnit>("liucheicontextunit", liucheIconUnit, liucheTextUnit);
    // Get the icon size of liucheiconunit
    glm::vec2 liucheIconSize = liucheIconUnit->GetSize();
    auto weizifuIconUnit = std::make_shared<ImageUnit>("weizifuiconunit", 
        		/*pos=*/glm::vec2(0.f), /*size=*/gameCharacters.at("weizifu")->GetGeneralSize(GameCharacterState::HAPPY)*0.75f, 
        		/*texture=*/gameCharacters.at("weizifu")->GetGeneralTexture(GameCharacterState::HAPPY), spriteRenderer);
    auto weizifuTextUnit = std::make_shared<TextUnit>("weizifutextunit", texts["weizifuintro"], textRenderer2);
    auto weizifuIconTextUnit = std::make_shared<OptionUnit>("weizifuicontextunit", weizifuIconUnit, weizifuTextUnit);
    // Set the horizontal spacing between the icon and text in weizifuicontextunit
    float horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() + liucheIconSize.x - weizifuIconUnit->GetSize().x;
    weizifuIconTextUnit->SetHorizontalSpacing(horizontalSpacing);
    const float weiqingIconOffset = -0.75f * kBubbleRadius;
    auto weiqingIconUnit = std::make_shared<ImageUnit>("weiqingiconunit", 
        				/*pos=*/glm::vec2(0.f), /*size=*/gameCharacters.at("weiqing")->GetGeneralSize(GameCharacterState::HAPPY)*0.75f, 
        				/*texture=*/gameCharacters.at("weiqing")->GetGeneralTexture(GameCharacterState::HAPPY), spriteRenderer);
    weiqingIconUnit->SetTextureRenderingMode(TextureRenderingMode::FlipHorizontally);
    auto weiqingTextUnit = std::make_shared<TextUnit>("weiqingtextunit", texts["weiqingintro"], textRenderer2);
    auto weiqingIconTextUnit = std::make_shared<OptionUnit>("weiqingicontextunit", weiqingIconUnit, weiqingTextUnit);
    // Set the horizontal spacing between the icon and text in weiqingicontextunit
    horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() + liucheIconSize.x - weiqingIconUnit->GetSize().x - weiqingIconOffset;
    weiqingIconTextUnit->SetHorizontalSpacing(horizontalSpacing);
    const float guojieIconOffset = -0.3f * kBubbleRadius;
    auto guojieIconUnit = std::make_shared<ImageUnit>("guojieiconunit", 
        						/*pos=*/glm::vec2(0.f), /*size=*/gameCharacters.at("guojie")->GetGeneralSize(GameCharacterState::FIGHTING)*0.75f, 
        						/*texture=*/gameCharacters.at("guojie")->GetGeneralTexture(GameCharacterState::FIGHTING), spriteRenderer);
    auto guojieTextUnit = std::make_shared<TextUnit>("guojiestextunit", texts["guojieintro"], textRenderer2);
    auto guojieIconTextUnit = std::make_shared<OptionUnit>("guojieicontextunit", guojieIconUnit, guojieTextUnit);
    // Set the horizontal spacing between the icon and text in guojieicontextunit
    horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() + liucheIconSize.x - guojieIconUnit->GetSize().x - guojieIconOffset;
    guojieIconTextUnit->SetHorizontalSpacing(horizontalSpacing);


    auto textSection = std::make_shared<PageSection>("storytextsection");
    textSection->AddContent(textUnit);
    textSection->AddContent(liucheIconTextUnit);
    textSection->AddContent(weizifuIconTextUnit);
    textSection->AddContent(weiqingIconTextUnit);
    textSection->AddContent(guojieIconTextUnit);
    textSection->SetScrollIconAllowed(false);
    auto buttonSection = std::make_shared<PageSection>("storybuttonsection");
    buttonSection->AddContent(startButtonUnit);
    buttonSection->AddContent(controlButtonUnit);
    buttonSection->AddContent(displaySettingsButtonUnit);
    buttonSection->AddContent(exitButtonUnit);
    buttonSection->SetInterUnitSpacing("startbuttonunit", "controlbuttonunit", 0.1f * kBubbleRadius);
    buttonSection->SetInterUnitSpacing("controlbuttonunit", "displaysettingsbuttonunit", 0.1f * kBubbleRadius);
    buttonSection->SetInterUnitSpacing("displaysettingsbuttonunit", "exitbuttonunit", 0.1f * kBubbleRadius);
    pages["story"] = std::make_unique<Page>("story");
    pages["story"]->AddSection(textSection);
    pages["story"]->AddSection(buttonSection);
    // Set the top, bottom and left spacing of the page "story".
    pages["story"]->SetTopSpacing(0.5f * kBubbleRadius);
    pages["story"]->SetBottomSpacing(0.5f * kBubbleRadius);
    pages["story"]->SetLeftSpacing(0.5f * kBubbleRadius);
    // Set the inter spacing between the sections of the page "story".
    pages["story"]->SetInterSectionSpacing("storytextsection", "storybuttonsection", 0.5f * kBubbleRadius);

    glm::vec2 commonButtionPosition = glm::vec2(pages["story"]->GetPosition().x + pages["story"]->GetLeftSpacing(), this->height * 0.84f);
    glm::vec2 commonButtonSize = glm::vec2(gameBoard->GetSize().x - 2 * (commonButtionPosition.x - pages["story"]->GetPosition().x), kBubbleRadius * 2.0f);
    for (const auto& buttonName : buttonSection->GetOrder()) {
		std::shared_ptr<ButtonUnit> buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(buttonSection->GetContent(buttonName));
		buttonUnit->SetPosition(commonButtionPosition);
		buttonUnit->SetSize(commonButtonSize);
		auto button = buttonUnit->GetButton();
		button->SetTextOnCenter(true);
	}
    float interspacingBetweenTextAndButton = pages["story"]->GetInterSectionSpacing("storytextsection", "storybuttonsection");
    float maxHeightForTextSection = gameBoard->GetSize().y - buttonSection->GetHeight() - pages["story"]->GetBottomSpacing() - pages["story"]->GetTopSpacing() - interspacingBetweenTextAndButton;
    textSection->SetInterUnitSpacing("storytextunit", "liucheicontextunit", 3.5f * kBubbleRadius);
    textSection->SetInterUnitSpacing("liucheicontextunit", "weizifuicontextunit", 2.f * kBubbleRadius);
    textSection->SetInterUnitSpacing("weizifuicontextunit", "weiqingicontextunit", 2.f * kBubbleRadius);
    textSection->SetInterUnitSpacing("weiqingicontextunit", "guojieicontextunit", 2.f * kBubbleRadius);
    textSection->SetUnitHorizontalOffset("weiqingicontextunit", weiqingIconOffset);
    textSection->SetUnitHorizontalOffset("guojieicontextunit", guojieIconOffset);
    textSection->SetMaxHeight(maxHeightForTextSection);
    textSection->SetMaxWidth(gameBoard->GetSize().x - pages["story"]->GetLeftSpacing() - 0.5*kBubbleRadius);
    pages["story"]->SetPosition(glm::vec2(this->gameBoard->GetPosition().x, std::max(this->gameBoard->GetCenter().y - pages["story"]->GetHeight() * 0.5f, this->gameBoard->GetPosition().y)));
    if (textSection->NeedScrollIcon()) {
		// Adjust the line width of text content
		textSection->SetMaxWidth(textSection->GetMaxWidth() - PageSection::kScrollIconWidth);
		// Create scroll icon
		textSection->InitScrollIcon(colorRenderer, circleRenderer, lineRenderer,
            			this->gameBoard->GetPosition().x + this->gameBoard->GetSize().x - Scroll::kSilkEdgeWidth - 0.5f * PageSection::kScrollIconWidth);
	}

    pages.at("story")->UpdatePosition();

    // Create page "control"
    textUnit = std::make_shared<TextUnit>("controltextunit", texts["control"], textRenderer2);
    /*auto startButtonUnit = std::make_shared<ButtonUnit>("startbuttonunit", buttons["controlstart"], textRenderer2, colorRenderer);*/
    auto backButtonUnit = std::make_shared<ButtonUnit>("backbuttonunit", buttons["back"], textRenderer2, colorRenderer);
    /*auto exitButtonUnit = std::make_shared<ButtonUnit>("exitbuttonunit", buttons["controlexit"], textRenderer2, colorRenderer);*/
    textSection = std::make_shared<PageSection>("controltextsection");
    textSection->AddContent(textUnit);
    buttonSection = std::make_shared<PageSection>("controlbuttonsection");
    buttonSection->AddContent(backButtonUnit);
    buttonSection->SetInterUnitSpacing("startbuttonunit", "backbuttonunit", 0.1f * kBubbleRadius);
    buttonSection->SetInterUnitSpacing("backbuttonunit", "exitbuttonunit", 0.1f * kBubbleRadius);
    pages["control"] = std::make_unique<Page>("control");
    pages["control"]->AddSection(textSection);
    pages["control"]->AddSection(buttonSection);
    // Set the top, bottom and left spacing of the page "control".
    pages["control"]->SetTopSpacing(0.5f * kBubbleRadius);
    pages["control"]->SetBottomSpacing(0.5f * kBubbleRadius);
    pages["control"]->SetLeftSpacing(0.5f * kBubbleRadius);
    // Set the inter spacing between the sections of the page "control".
    pages["control"]->SetInterSectionSpacing("controltextsection", "controlbuttonsection", 2.0f * kBubbleRadius);

    commonButtionPosition = glm::vec2(pages["control"]->GetPosition().x + pages["control"]->GetLeftSpacing(), this->height * 0.84f);
    commonButtonSize = glm::vec2(gameBoard->GetSize().x - 2 * (commonButtionPosition.x - pages["control"]->GetPosition().x), kBubbleRadius * 2.0f);
    for (const auto& buttonName : buttonSection->GetOrder()) {
        std::shared_ptr<ButtonUnit> buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(buttonSection->GetContent(buttonName));
        buttonUnit->SetPosition(commonButtionPosition);
        buttonUnit->SetSize(commonButtonSize);
        auto button = buttonUnit->GetButton();
        button->SetTextOnCenter(true);
    }
    interspacingBetweenTextAndButton = pages["control"]->GetInterSectionSpacing("controltextsection", "controlbuttonsection");
    maxHeightForTextSection = gameBoard->GetSize().y - buttonSection->GetHeight() - pages["control"]->GetBottomSpacing() - pages["control"]->GetTopSpacing() - interspacingBetweenTextAndButton;
    textSection->SetMaxHeight(maxHeightForTextSection);
    textSection->SetMaxWidth(gameBoard->GetSize().x - pages["control"]->GetLeftSpacing() - 0.5*kBubbleRadius);
    pages["control"]->SetPosition(glm::vec2(this->gameBoard->GetPosition().x, std::max(this->gameBoard->GetCenter().y-pages["control"]->GetHeight()*0.5f, this->gameBoard->GetPosition().y)));
    if (textSection->NeedScrollIcon()) {
        // Adjust the line width of text content
        textSection->SetMaxWidth(textSection->GetMaxWidth() - PageSection::kScrollIconWidth);
        // Create scroll icon
        textSection->InitScrollIcon(colorRenderer, circleRenderer, lineRenderer,
            this->gameBoard->GetPosition().x + this->gameBoard->GetSize().x - Scroll::kSilkEdgeWidth - 0.5f * PageSection::kScrollIconWidth);
    }

    // Create page "Display Settings"
    // Create option texts.
    //const std::string windowedModeName = "windowedmode";
    //const std::string fullScreenModeName = "fullscreenmode";
    //texts[windowedModeName] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    //texts[windowedModeName]->AddParagraph("Windowed Mode");
    //texts[windowedModeName]->SetScale(0.0216f / kFontScale);
    auto displaysetting1 = CreateClickableOptionUnit("fullscreenmode", "Full Screen Mode");
    auto displaysetting2 = CreateClickableOptionUnit("windowedmode", "Windowed Mode");
    if(this->GetScreenMode() == ScreenMode::FULLSCREEN)
		displaysetting1->SetState(OptionState::kClicked);
    else
        displaysetting2->SetState(OptionState::kClicked);
    textSection = std::make_shared<PageSection>("displaysettingtextsection");
    buttonSection = std::make_shared<PageSection>("displaysettingbuttonsection");
    textSection->AddContent(displaysetting1);
    textSection->AddContent(displaysetting2);
    textSection->SetInterUnitSpacing("fullscreenmode", "windowedmode", 0.5f * kBubbleRadius);
    buttonSection->AddContent(backButtonUnit);
    pages["displaysettings"] = std::make_unique<Page>("displaysettings");
    pages["displaysettings"]->AddSection(textSection);
    pages["displaysettings"]->AddSection(buttonSection);
    // Set the top, bottom and left spacing of the page "Display Settings".
    pages["displaysettings"]->SetTopSpacing(0.5f * kBubbleRadius);
    pages["displaysettings"]->SetBottomSpacing(0.5f * kBubbleRadius);
    pages["displaysettings"]->SetLeftSpacing(0.5f * kBubbleRadius);
    // Set the inter spacing between the sections of the page "Display Settings".
    pages["displaysettings"]->SetInterSectionSpacing("displaysettingtextsection", "displaysettingbuttonsection", 2.0f * kBubbleRadius);
    interspacingBetweenTextAndButton = pages["displaysettings"]->GetInterSectionSpacing("displaysettingtextsection", "displaysettingbuttonsection");
    maxHeightForTextSection = gameBoard->GetSize().y - buttonSection->GetHeight() - pages["displaysettings"]->GetBottomSpacing() - pages["displaysettings"]->GetTopSpacing() - interspacingBetweenTextAndButton;
    textSection->SetMaxHeight(maxHeightForTextSection);
    textSection->SetMaxWidth(gameBoard->GetSize().x - pages["displaysettings"]->GetLeftSpacing() - 0.5*kBubbleRadius);
    pages["displaysettings"]->SetPosition(glm::vec2(this->gameBoard->GetPosition().x, std::max(this->gameBoard->GetCenter().y - pages["displaysettings"]->GetHeight() * 0.5f, this->gameBoard->GetPosition().y)));

    // Initialize the timer
    timer = std::make_shared<Timer>();

    // Set the target silk length for closing and opening the scroll
    scroll->SetTargetSilkLenForClosing(0.f);
    scroll->SetTargetSilkLenForOpening(gameBoard->GetSize().y);
    scroll->SetTargetSilkLenForNarrowing(scroll->GetTargetSilkLenForOpening());
    scroll->SetTargetSilkLenForNarrowing(scroll->GetTargetSilkLenForNarrowing());
    // Set the narrowing, closing, and opening velocity of the scroll
    scroll->SetVelocityForNarrowing(kBubbleRadius);
    scroll->SetVelocityForClosing(48*kBubbleRadius);
    scroll->SetVelocityForOpening(96*kBubbleRadius);
    // Set the target position for scroll retracting, deploying, and attacking
    scroll->SetTargetPositionForRetracting(glm::vec2(this->width * 0.16f, this->height * 0.83f));
    scroll->SetTargetPositionForDeploying(glm::vec2(this->width / 2.0f, this->height / 2.0f));
    scroll->SetTargetPositionForAttacking(glm::vec2(this->width * 0.785f, this->height * 0.73f));

}

void GameManager::ProcessInput(float dt) {
    // If 'F' is pressed, then we toggle the full screen mode.
    if (this->keys[GLFW_KEY_F] && this->keysLocked[GLFW_KEY_F] == false) {
		this->keysLocked[GLFW_KEY_F] = true;
		if (this->GetScreenMode() != ScreenMode::FULLSCREEN) {
			this->GoToScreenMode(ScreenMode::FULLSCREEN);
		}
	}
    // If 'W' is pressed, then we toggle the windowed mode.
    if (this->keys[GLFW_KEY_W] && this->keysLocked[GLFW_KEY_W] == false) {
        this->keysLocked[GLFW_KEY_W] = true;
        if (this->GetScreenMode() != ScreenMode::WINDOWED) {
			this->GoToScreenMode(ScreenMode::WINDOWED);
		}
    }

    if (this->state == GameState::ACTIVE) {
        // move shooter
        if (this->keys[GLFW_KEY_LEFT])
        {
            // if the key 'ctrl' is hold, then we rotate the shooter in a smaller angle.
            if (this->keys[GLFW_KEY_LEFT_CONTROL]) {
                shooter->SetRoll(shooter->GetRoll() - 0.001f);
            }
            else {
                shooter->SetRoll(shooter->GetRoll() - 0.01f);
            }
            shooter->GetRay().UpdatePath(this->gameBoard->GetValidBoundaries(), this->statics);
        }
        else if (this->keys[GLFW_KEY_RIGHT])
        {
            // if the key 'ctrl' is hold, then we rotate the shooter in a smaller angle.
            if (this->keys[GLFW_KEY_LEFT_CONTROL]) {
                shooter->SetRoll(shooter->GetRoll() + 0.001f);
            }
            else {
                shooter->SetRoll(shooter->GetRoll() + 0.01f);
            }
            shooter->GetRay().UpdatePath(this->gameBoard-> GetValidBoundaries(), this->statics);
        }
        else if (this->keys[GLFW_KEY_Q] && this->keysLocked[GLFW_KEY_Q] == false) {
            this->keysLocked[GLFW_KEY_Q] = true;
            if (this->targetState == GameState::UNDEFINED) {
                // if Q is pressed, then we go to the state 'Control' of the game.
                this->GoToState(GameState::CONTROL);
                // Set scroll state to be CLOSING
                this->scroll->SetState(ScrollState::CLOSING);
                // pause timer of related events if it is not paused yet.
                if (!this->timer->IsPaused("beforenarrowing")) {
                    this->timer->PauseEventTimer("beforenarrowing");
                }
            }
        }

        if (this->keys[GLFW_KEY_SPACE] && this->keysLocked[GLFW_KEY_SPACE] == false) {
            this->keysLocked[GLFW_KEY_SPACE] = true;
            std::unique_ptr<Bubble> bubble = shooter->ShootBubble(GetNextBubbleColor());
            moves.emplace(bubble->GetID(), std::move(bubble));
            //// update the gameboard color based on the color of the ray.
            //gameBoard->UpdateColor(shooter->GetRay().GetColorWithoutAlpha());
        }
    }
    else if (this->state == GameState::MENU) {
        //if (this->keys[GLFW_KEY_ENTER] && !this->keysLocked[GLFW_KEY_ENTER])
        //{
        //    this->SetState(GameState::PREPARING);
        //    this->keysLocked[GLFW_KEY_ENTER] = true;
        //}
    }
    else if (this->state == GameState::STORY) {
    }
	else if (this->state == GameState::CONTROL) {
    }
    else if (this->state == GameState::WIN || this->state == GameState::LOSE) {
        if (this->keys[GLFW_KEY_ENTER] && this->keysLocked[GLFW_KEY_ENTER] == false) {
            this->keysLocked[GLFW_KEY_ENTER] = true;
            //postProcessor->SetBlur(false);
            postProcessor->SetGrayscale(false);
            // gradually fading out the blur effect
            timer->SetEventTimer("deblurring", 2.f);
            // Set game character to be transparent gradually in its old state and be opaque in its new state.
            const float transparency = 0.4f, opaque = 0.3f;       
            auto guojieCopied = std::make_shared<GameCharacter>(*gameCharacters["guojie"]);
            graduallyTransparentObjects["guojie"] = std::make_pair(guojieCopied, transparency);
            // Clear all carried objects of guojie
            gameCharacters["guojie"]->ClearCarriedObjects();            
            if (gameCharacters["weiqing"]->GetState() != GameCharacterState::HAPPY) {
				auto weiqingCopied = std::make_shared<GameCharacter>(*gameCharacters["weiqing"]);
                graduallyTransparentObjects["weiqing"] = std::make_pair(weiqingCopied, transparency);
                gameCharacters["weiqing"]->SetState(GameCharacterState::HAPPY);
                gameCharacters["weiqing"]->SetAlpha(0.f);
                graduallyOpaqueObjects["weiqing"] = std::make_pair(gameCharacters["weiqing"], opaque);
			}
            if (gameCharacters["weizifu"]->GetState() != GameCharacterState::HAPPY) {
                auto weizifuCopied = std::make_shared<GameCharacter>(*gameCharacters["weizifu"]);
                // Add 0 before "Weizifu" and 1 before "Liuche" to make sure that Weizifu is drawn before Liuche.
                graduallyTransparentObjects["0weizifu"] = std::make_pair(weizifuCopied, transparency);
                gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);
                gameCharacters["weizifu"]->SetAlpha(0.f);
                graduallyOpaqueObjects["0weizifu"] = std::make_pair(gameCharacters["weizifu"], opaque);
            }
            if (gameCharacters["liuche"]->GetState() != GameCharacterState::HAPPY) {
                auto liucheCopied = std::make_shared<GameCharacter>(*gameCharacters["liuche"]);
                graduallyTransparentObjects["1liuche"] = std::make_pair(liucheCopied, transparency);
                gameCharacters["liuche"]->SetState(GameCharacterState::HAPPY);
                gameCharacters["liuche"]->SetAlpha(0.f);
                graduallyOpaqueObjects["1liuche"] = std::make_pair(gameCharacters["liuche"], opaque);
            }

            gameCharacters["guojie"]->SetState(GameCharacterState::FIGHTING);
            // Reset the health of characters
            int totalHealth = gameCharacters["guojie"]->GetHealth().GetTotalHealth();
            gameCharacters["guojie"]->GetHealth().SetCurrentHealth(totalHealth);
            totalHealth = gameCharacters["weiqing"]->GetHealth().GetTotalHealth();
            gameCharacters["weiqing"]->GetHealth().SetCurrentHealth(totalHealth);

            gameCharacters["guojie"]->SetPosition(gameCharacters["guojie"]->GetTargetPosition("outofscreen"));

            // Delete all the arrows
            arrows.clear();

            // Delete all the static bubbles.
            statics.clear();

            // Reset the game level to 1.
            this->level = 1;

            // Reset the text scale of the victory and defeated texts
            texts["victory"]->SetScale(texts["victory"]->GetTargetScale("max"));
            texts["defeated"]->SetScale(texts["defeated"]->GetTargetScale("max"));
            
            // Clean the timer for prompt to main menu
            timer->CleanEvent("prompttomainmenu");
            timer->CleanEvent("hideprompttomainmenu");
            
            // Set the scroll's y position to be out of the top of the screen
            scroll->Reset();
            this->SetState(GameState::INITIAL);
            this->GoToState(GameState::STORY);
        }
    }
    else if (this->state == GameState::OPTION) {
        glm::vec2 mousePosition = glm::vec2(this->mouseX, this->mouseY);
        // check if the mouse is hovering over the buttons that are active.
        auto it = buttons.begin();
        while (it != buttons.end()) {
            auto& [content, button] = *it++;
            // continue if the button is not active
            if (button->GetState() == ButtonState::kInactive) {
                continue;
            }
            if (button->IsPositionInside(mousePosition)) {
                // Check if the button is pressed
                if (this->leftMousePressed) {
                    button->SetState(ButtonState::kPressed);
                    if (content == "windowed") {
                        this->screenMode = ScreenMode::WINDOWED;
                        this->SetState(GameState::EXIT);
                        this->GoToState(GameState::INITIAL);
                        this->buttons.clear();
                    }else if(content == "fullscreen") {
						this->screenMode = ScreenMode::FULLSCREEN;
                        this->SetState(GameState::EXIT);
                        this->GoToState(GameState::INITIAL);
                        this->buttons.clear();
					}else if (content == "exit") {
                        this->SetState(GameState::EXIT);
					}
     
                    // Deactivate all the buttons.
                    for (auto& [content, button] : buttons) {
                        button->SetState(ButtonState::kInactive);
                    }
                    // assume that only one button is pressed at a time, so we break the loop.
                    break;
                }
                else {
                    button->SetState(ButtonState::kHovered);
                }
            }
            else {
                button->SetState(ButtonState::kNormal);
            }
        }
    }

    if (!activePage.empty()) {
        // Move the text content of the page "control" based on the scroll offset.
        if (this->scrollYOffset != 0.f) {
            // Iterate over the sections of the page "control"
            for (auto sectionName : pages.at(activePage)->GetOrder()) {
                auto section = pages.at(activePage)->GetSection(sectionName);
                // Skip if the scroll icon is not allowed.
                if (!section->IsScrollIconAllowed()) {
					continue;
				}
                // Skip if there's no scroll icon is not initialized.
                if (!section->IsScrollIconInitialized()) {
                    continue;
                }
                // Skip if the mouse cursor is not in the box.
                if (!section->IsMouseInBox(glm::vec2(this->mouseX, this->mouseY))) {
                    continue;
                }
                section->MoveScrollIcon(-this->scrollYOffset * this->scrollSensitivity);
                // assume that only one section is scrolled at a time, so we break the loop.
                break;
            }
            // reset the scroll offset
            this->scrollYOffset = 0.f;
        }
        else if (this->leftMousePressed) {
            glm::vec2 mousePosition = glm::vec2(this->mouseX, this->mouseY);
            for (auto sectionName : pages.at(activePage)->GetOrder()) {
                auto section = pages.at(activePage)->GetSection(sectionName);
                // Skip if there's no scroll icon is not initialized.
                if (!section->IsScrollIconInitialized()) {
                    continue;
                }
                Capsule& icon = section->GetScrollIcon();
                auto sectionboundingBox = section->GetBoundingBox();
                sectionboundingBox.z += 20 * icon.GetSize().x;
                bool mouseIsCustomBox = mousePosition.x > sectionboundingBox.x && mousePosition.x < sectionboundingBox.z && mousePosition.y > sectionboundingBox.y && mousePosition.y < sectionboundingBox.w;
                if (mouseIsCustomBox && (icon.IsPositionInside(mousePosition) || isReadyToDrag && isDragging)) {
                    if (isReadyToDrag) {
                        isDragging = true;
                        // move the story text scroll based on the mouse cursor postion offset.
                        float mouseYOffset = this->mouseY - this->mouseLastY;
                        section->MoveScrollIcon(mouseYOffset);
                        this->mouseLastX = this->mouseX;
                        this->mouseLastY = this->mouseY;
                    }
                }
                else {
                    isReadyToDrag = false;
                    isDragging = false;
                    // If the mouse cursor is not in the scroll, we reset the mouse last position to the current position.
                    this->mouseLastX = this->mouseX;
                    this->mouseLastY = this->mouseY;
                }
            }
        }

        glm::vec2 mousePosition = glm::vec2(this->mouseX, this->mouseY);
        if (this->targetState == GameState::UNDEFINED) {
            // check if the mouse is hovering over the buttons that are active.
            auto it = buttons.begin();
            while (it != buttons.end()) {
                auto& [content, button] = *it++;
                // continue if the button is not active
                if (button->GetState() == ButtonState::kInactive) {
                    continue;
                }
                if (button->IsPositionInside(mousePosition)) {
                    // Check if the button is pressed
                    if (this->leftMousePressed) {
                        if (button->GetState() == ButtonState::kHovered) {
                            button->SetState(ButtonState::kPressed);
                            if (content == "control") {
                                this->GoToState(GameState::CONTROL);
                            }
                            else if (content == "displaysettings") {
								this->GoToState(GameState::DISPLAY_SETTINGS);
							}
                            else if (content == "start") {
                                // print the page story position and all its sections' positions
                                auto textSection = pages["story"]->GetSection("storytextsection");
                                auto buttonSection = pages["story"]->GetSection("storybuttonsection");

                                this->GoToState(GameState::PREPARING);
                                if (gameCharacters["guojie"]->GetState() != GameCharacterState::FIGHTING) {
                                    gameCharacters["guojie"]->SetState(GameCharacterState::FIGHTING);
                                }
                                // Set the position of guojie to be out of the screen.
                                gameCharacters["guojie"]->SetPosition(gameCharacters["guojie"]->GetTargetPosition("outofscreen"));
                                // Set the target position of guojie to be landing
                                gameCharacters["guojie"]->SetAndMoveToTargetPosition(gameCharacters["guojie"]->GetTargetPosition("landing"));
                                // Set the initial velocity and acceleration of guojie.
                                gameCharacters["guojie"]->SetVelocity(glm::vec2(0.0f, 10.0f * kBubbleRadius));
                                gameCharacters["guojie"]->SetAcceleration(glm::vec2(0.0f, 10.0f * 9.8f * kBubbleRadius));

                                //// Set the velocity of guojie.
                                //gameCharacters["guojie"]->SetVelocity(glm::vec2(0.0f, 10.0f*kBubbleRadius));
                            }
                            else if (content == "exit") {
                                this->SetState(GameState::EXIT);
                            }
                            else if (content == "back") {
                                // Go to the last state
                                this->GoToState(this->lastState);
                            }

                            // Set scroll state to be CLOSING
                            this->scroll->SetState(ScrollState::CLOSING);

                            // assume that only one button is pressed at a time, so we break the loop.
                            break;
                        }
                    }
                    else {
                        button->SetState(ButtonState::kHovered);
                    }
                }
                else {
                    button->SetState(ButtonState::kNormal);
                }
            }

            // Iterate options in the current active page
            if (!activePage.empty()) {
                for (size_t sectionIdx = 0; sectionIdx < pages.at(activePage)->GetOrder().size(); ++sectionIdx) {
					auto sectionName = pages.at(activePage)->GetOrder()[sectionIdx];
					auto section = pages.at(activePage)->GetSection(sectionName);
                    std::string optionToBeClicked = "";
                    std::string optionAlreadyClicked = "";
                    for (size_t unitIdx = 0; unitIdx < section->GetOrder().size(); ++unitIdx) {
						auto unitName = section->GetOrder()[unitIdx];
						auto contentUnit = section->GetContent(unitName);
                        auto option = std::dynamic_pointer_cast<OptionUnit>(contentUnit);
                        if (option == nullptr) {
							continue;
						}
                        assert(option->GetType() == ContentType::kOption && "The content type should be an option.");
                        if (option->GetState() == OptionState::kUnclickable) {
                            continue;
                        }
                        else if (option->GetState() == OptionState::kClicked) {
							optionAlreadyClicked = option->GetName();
						}
                        if (option->IsPositionInsideIcon(mousePosition) && option->GetState() != OptionState::kClicked) {
                            if (this->leftMousePressed) {
                                optionToBeClicked = option->GetName();
                                if (option->GetState() == OptionState::kHovered) {
                                    option->SetState(OptionState::kClicked);
                                    if (activePage == "displaysettings") {
                                        if (optionToBeClicked == "fullscreenmode") {
                                            this->GoToScreenMode(ScreenMode::FULLSCREEN);
                                        }
                                        else if (optionToBeClicked == "windowedmode") {
                                            this->GoToScreenMode(ScreenMode::WINDOWED);
                                        }
									}
                                    continue;
								}
							}
                            else {
                                if (option->GetState() != OptionState::kClicked) {
									option->SetState(OptionState::kHovered);
								}
							}
						}
                        else {
                            if (option->GetState() != OptionState::kClicked) {
                                option->SetState(OptionState::kNormal);
                            }
						}
					}
					// Set the state of the unclicked options to be kNormal if there's any option been clicked.
                    if (!optionToBeClicked.empty()) {
                        auto option = std::dynamic_pointer_cast<OptionUnit>(section->GetContent(optionAlreadyClicked));
                        assert(option != nullptr && "The option should not be nullptr.");
                        assert(option->GetType() == ContentType::kOption && "The content type should be an option.");
                        assert(option->GetState() != OptionState::kUnclickable && "The option should be clickable.");
                        if (option->GetState() == OptionState::kClicked) {
                            option->SetState(OptionState::kNormal);
                        }
                    }
				}
            }

        }
    }
}

void GameManager::Update(float dt)
 {  
    // Adjust the length of scroll based on the length of the page "control".
    if (!this->activePage.empty()) {
    	this->scroll->SetTargetSilkLenForOpening(std::min(pages.at(this->activePage)->GetHeight(), gameBoard->GetSize().y));
    }

    // silk boundary before scrolling
    glm::vec4 silkBoundsBefore = this->scroll->GetSilkBounds();

    if (this->scroll->GetState() == ScrollState::CLOSING) {
        this->scroll->Close(dt, this->scroll->GetTargetSilkLenForClosing());
    }
    else if (this->scroll->GetState() == ScrollState::OPENING) {
        if (this->lastState != GameState::PREPARING && this->state == GameState::ACTIVE) {
            this->scroll->Open(dt, this->scroll->GetCurrentSilkLenForNarrowing());
            if (this->scroll->GetState() == ScrollState::OPENED) {
                if (this->scroll->GetCurrentSilkLenForNarrowing() > this->scroll->GetTargetSilkLenForNarrowing()) {
                    this->scroll->SetState(ScrollState::NARROWING);
                    silkBoundsBefore = this->scroll->GetSilkBounds();
                }
			}
        }
        else {
            this->scroll->Open(dt, this->scroll->GetTargetSilkLenForOpening());
        }
    }
    else if (this->scroll->GetState() == ScrollState::NARROWING) {
        this->scroll->Narrow(dt, this->scroll->GetTargetSilkLenForNarrowing());
    }
    else if (this->scroll->GetState() == ScrollState::RETRACTING) {
    	this->scroll->Retract(dt);
        if (this->scroll->GetState() == ScrollState::RETRACTED) {
            if (this->level <= this->numGameLevels && gameCharacters["weiqing"]->GetHealth().GetCurrentHealth() > 0) {
                this->timer->SetEventTimer("scrollInSleeve", 0.8f);
                this->timer->StartEventTimer("scrollInSleeve");
                this->scroll->SetState(ScrollState::DEPLOYING);
            }
            else {
                assert(this->targetState == GameState::LOSE && "The target state should be LOSE.");
                this->SetToTargetState();
                // Disable the scroll
                this->scroll->SetState(ScrollState::DISABLED);
            }
        }
    }
    else if (this->scroll->GetState() == ScrollState::DEPLOYING) {
        if (this->timer->IsEventTimerExpired("scrollInSleeve")) {
            this->scroll->SetAlpha(1.f);
            this->scroll->Deploy(dt);
        }
    }
    else if (this->scroll->GetState() == ScrollState::ATTACKING) {
		this->scroll->Attack(dt);
        if (this->scroll->GetState() == ScrollState::ATTACKED) {
            this->timer->SetEventTimer("scrollHitting", 0.1f);
            this->timer->StartEventTimer("scrollHitting");
			this->scroll->SetState(ScrollState::RETURNING);
            gameCharacters["weiqing"]->SetTargetRoll(glm::pi<float>() / 6.f);
            gameCharacters["weiqing"]->ActivateStun();
            // Decrease the health of Weiqing by 1.
            gameCharacters["weiqing"]->GetHealth().DecreaseHealth(1);

		}
	}
    else if (this->scroll->GetState() == ScrollState::RETURNING) {
        if (this->timer->IsEventTimerExpired("scrollHitting")) {
            this->scroll->Return(dt);
        }
        if (gameCharacters["weiqing"]->IsStunned()) {
            gameCharacters["weiqing"]->RotateTo(gameCharacters["weiqing"]->GetTargetRoll(), 2.7f*glm::pi<float>(), dt);
            if (!gameCharacters["weiqing"]->IsRotating()) {
                gameCharacters["weiqing"]->SetTargetRoll(0.f);
                gameCharacters["weiqing"]->DeactivateStun();
            }
        }
        else if (gameCharacters["weiqing"]->IsRotating()) {
            gameCharacters["weiqing"]->RotateTo(gameCharacters["weiqing"]->GetTargetRoll(), -3.0f * glm::pi<float>(), dt);
        }
        else if (gameCharacters["weiqing"]->GetHealth().GetCurrentHealth() == 0) {
            gameCharacters["weiqing"]->SetState(GameCharacterState::SAD);
            // Go to the state of LOSE
            this->GoToState(GameState::LOSE);
        }
	}

    // Scroll should only shake when the scroll is OPENED.
    this->gameArenaShaking &= this->scroll->GetState() == ScrollState::OPENED;

    // Update arrows
    bool hasNewArrowHit = false;
    for (auto& arrow : arrows) {
        if (!arrow->IsStopped()) {
            arrow->Update(dt);
            if (arrow->IsStopped()) {
                gameCharacters["guojie"]->SetTargetRoll(-glm::pi<float>() / 6.f);
                gameCharacters["guojie"]->ActivateStun();
                gameCharacters["guojie"]->GetHealth().DecreaseHealth(1);
            }
            if (arrow->IsPenetrating() || arrow->IsStopped()) {
                if (gameCharacters["guojie"]->GetCarriedObjects().count(arrow->GetID()) == 0) {
                    gameCharacters["guojie"]->AddCarriedObject(arrow);
                    //gameCharacters["guojie"]->GetCarriedObjects()[arrow->GetID()] = arrow;
                    hasNewArrowHit = true;
                    // sort the arrows based on the x and y penetration position of the arrow. If the x position is the same, then we sort based on the y position.
                    std::sort(arrows.begin(), arrows.end(), [](const std::shared_ptr<Arrow>& a, const std::shared_ptr<Arrow>& b) {
                        if (a->GetPenetrationPosition().x == b->GetPenetrationPosition().x) {
							return a->GetPenetrationPosition().y > b->GetPenetrationPosition().y;
						}
                        return a->GetPenetrationPosition().x < b->GetPenetrationPosition().x;
					});
                }
                //if (arrow->IsStopped() && this->level > this->numGameLevels) {
                //    gameCharacters["guojie"]->SetState(GameCharacterState::SAD);
                //}
            }
        }
	}
    if (!arrows.empty() && !hasNewArrowHit) {
        if (gameCharacters["guojie"]->IsStunned()) {
            gameCharacters["guojie"]->RotateTo(gameCharacters["guojie"]->GetTargetRoll(), -2.7f * glm::pi<float>(), dt);
            if (!gameCharacters["guojie"]->IsRotating()) {
                gameCharacters["guojie"]->SetTargetRoll(0.f);
                gameCharacters["guojie"]->DeactivateStun();
            }
        }
        else if (gameCharacters["guojie"]->IsRotating()) {
            gameCharacters["guojie"]->RotateTo(gameCharacters["guojie"]->GetTargetRoll(), 3.0f * glm::pi<float>(), dt);
        }
        else if (gameCharacters["guojie"]->GetHealth().GetCurrentHealth() == 0 && gameCharacters["guojie"]->GetState() != GameCharacterState::SAD && (this->state == GameState::ACTIVE || this->state == GameState::PREPARING)) {
            gameCharacters["guojie"]->SetState(GameCharacterState::SAD);
            gameCharacters["weiqing"]->SetState(GameCharacterState::WINNING);
            gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);
            gameCharacters["liuche"]->SetState(GameCharacterState::HAPPY);

            // Get the current center of the scroll
            glm::vec2 scrollCenter = this->scroll->GetCenter();
            // Exploding the scroll as guojie is defeated.
            std::vector<ExplosionInfo> explosionInfo;
            // 0.388235f, 0.174671f, 0.16863f, 1.0f
            explosionInfo.emplace_back(this->scroll->GetCenter(), glm::vec4(0.388235f, 0.174671f, 0.16863f, 1.0f), 2000, this->scroll->GetSilkWidth() * 0.5f, this->scroll->GetSilkLen()*0.5f, glm::vec2(2*kBubbleRadius/15.f, 4*kBubbleRadius/15.f));
            explosionSystem->CreateExplosions(explosionInfo);

            // Set the scroll to DISABLED state.
            this->scroll->SetState(ScrollState::DISABLED);

            // Set the state to be WINNING
            this->SetState(GameState::WIN);
		}
    }

    // silk boundary after scrolling
    glm::vec4 silkBoundsAfter = this->scroll->GetSilkBounds();
    
    // Update health damage texts.
    gameCharacters["guojie"]->GetHealth().UpdateDamageTexts(dt);
    gameCharacters["weiqing"]->GetHealth().UpdateDamageTexts(dt);

    if (this->state == GameState::ACTIVE) {

        // Get the game board position and size
        glm::vec2 gameBoardPostition = gameBoard->GetValidPosition();
        glm::vec2 gameBoardSize = gameBoard->GetValidSize();
        glm::vec4 gameBoardBoundaries = gameBoard->GetValidBoundaries();

        // If the target state is UNDEFINED and the scroll is narrowing or narrowed, then we get real boundaries by mixing the game board boundaries and the scroll boundaries.
        if (this->targetState == GameState::UNDEFINED && (this->scroll->GetState() == ScrollState::NARROWING || this->scroll->GetState() == ScrollState::NARROWED)) {
            float offsetY = silkBoundsAfter.y - silkBoundsBefore.y; 
            // Reset the valid boundaries of the game board.
            gameBoard->SetValidPosition(glm::vec2(silkBoundsAfter.x, silkBoundsAfter.y));
            gameBoard->SetValidSize(glm::vec2(silkBoundsAfter.z - silkBoundsAfter.x, silkBoundsAfter.w - silkBoundsAfter.y));
            gameBoardSize = gameBoard->GetValidSize();
            gameBoardPostition = gameBoard->GetValidPosition();
            gameBoardBoundaries = gameBoard->GetValidBoundaries();

            // move all the static bubbles downwards by the offset.
            for (auto& [id, bubble] : statics) {
                bubble->SetPosition(bubble->GetPosition() + glm::vec2(0.f, offsetY));
			}

            // Check and adjust the postisions of the moving bubbles if they are hitting with the top wall or with the bottom wall.
            for(auto& [id, bubble] : moves){
                if (bubble->GetPosition().y < gameBoardBoundaries.y) {
					bubble->SetPosition(glm::vec2(bubble->GetPosition().x, gameBoardBoundaries.y));
				}
                else if (bubble->GetPosition().y + bubble->GetSize().y > gameBoardBoundaries.w) {
					bubble->SetPosition(glm::vec2(bubble->GetPosition().x, gameBoardBoundaries.w - bubble->GetSize().y));
				}
            }

            // Move the shooter upwards by the offset.
            shooter->SetPosition(shooter->GetPosition() - glm::vec2(0.f, offsetY));
            shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);

            // Move the timer's text downwards by the offset.
            texts["time"]->SetPosition(texts["time"]->GetPosition() + glm::vec2(0.f, offsetY));

            if(this->scroll->GetState() == ScrollState::NARROWED) {
				// Reset the scroll state to OPENED.
                this->scroll->SetState(ScrollState::OPENED);
                // Resume the timer.
                this->timer->ResumeEventTimer("beforenarrowing");
                // Set the time to be displayed on the screen.
                texts["time"]->SetParagraph(0, std::to_string(static_cast<int>(std::ceil(this->timer->GetEventTimer("beforenarrowing")))));
			}
		}
		
        // Update all moving bubbles
        auto it = moves.begin();
        while (it != moves.end()) {
            auto& [id_ref, bubble] = *it++;
            int id = id_ref;
            bool isPenetrating = bubble->Move(dt, gameBoardBoundaries, statics);
            if (!isPenetrating) {
                continue;
            }

            // We would like to adjust the postion of the bubble to make it have at least two static neighbors if possible.
            std::vector<int> candidateIds;
            std::vector<int> neighborIds = GetNeighborIds(bubble);

            // If the new bubble only has 1 neighbor, then we try to adjust the position of the bubble to make it have more neighbors.
            if (neighborIds.size() == 1) {
                // Fine tuning the position of the collidef moving bubbles to the best free slot.
                int staticBubbleId = neighborIds[0];
                bool done = FineTuneToNeighbor(id, staticBubbleId);
                if (!done) {
                    done = FineTuneToClose(id, staticBubbleId);
                }
            }
            else if (neighborIds.size() == 0) {
                // If the bubble has no neighbor, it must be at the top of the game board.
                if (!IsAtUpperBoundary(bubble->GetPosition())) {
					std::cout << "The bubble is not at the top of the game board." << std::endl;
                    std::cout << "Bubble position: " << bubble->GetPosition().x << " " << bubble->GetPosition().y << std::endl;
                }
                else {
                    // Fine tuning the position of the collidef moving bubbles to the best free slot at the upper boundary of the game board.
					bool done = FineTuneToCloseUpper(id); (void)done;
                }
            }

            //Add the bubble to the static bubbles
            statics[id] = std::move(bubble);
            ++colorCount[statics[id]->GetColorEnum()];
            // Remove the bubble from the moving bubbles
            moves[id] = nullptr;
            moves.erase(id);
            shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
            // Check if the bubble connect to a group of bubbles of the same color. And if they together form a group of more than 2 bubbles, then we remove them.
            std::vector<int> connectedBubbleIds = FindConnectedBubblesOfSameColor(id);

            if (connectedBubbleIds.size() > 2) {
                // Remove the connected bubbles from the static bubbles and push them into explodings.
                for (int connectedBubbleId : connectedBubbleIds) {
                    --colorCount[statics[connectedBubbleId]->GetColorEnum()];
                    explodings[connectedBubbleId] = std::move(statics[connectedBubbleId]);
                    statics[connectedBubbleId] = nullptr;
                    statics.erase(connectedBubbleId);
                    shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
                }

                // Find all the bubbles that are falling after the explosion. A bubble is falling if it is not connected to the top wall or the static bubbles.
                std::vector<int> fallingIds = FindAllFallingBubbles();

                // Remove the falling bubbles from the static bubbles and push them into falling.
                for (int fallingId : fallingIds) {
                    // the falling bubble should be in the statics.
                    assert(statics.count(fallingId) > 0 && "Failed to find the bubble by ID.");
                    --colorCount[statics[fallingId]->GetColorEnum()];
                    fallings.emplace(fallingId, std::move(statics[fallingId]));
                    // Remove the falling bubble from the static bubbles.
                    statics[fallingId] = nullptr;
                    statics.erase(fallingId);
                    shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
                }

                // clear all the exploding bubbles.
                std::vector<ExplosionInfo> explosionInfo;
                for (auto& [id, bubble] : explodings) {
                    explosionInfo.emplace_back(bubble->GetCenter(), bubble->GetColor(), 150, bubble->GetRadius()*0.6f);
                }
                explosionSystem->CreateExplosions(explosionInfo);
                explodings.clear();
            }

            // If all statuc bubbles are removed, we set the GameState to PREPAREING and switch to the next level.
            if (statics.empty()) {
                // Clear all the moving bubbles.
                moves.clear();

                // Initialize an arrow firing by Weiqing towards Guojie.
                // Get target position on the charactor guojie.
                glm::vec2 targetPostion = glm::vec2(gameCharacters["guojie"]->GetPosition().x + gameCharacters["guojie"]->GetSize().x/2.0f, gameCharacters["guojie"]->GetPosition().y + gameCharacters["guojie"]->GetSize().y * 0.53f);
                // Randomly add offset to the target position within a circle of radius kBubbleRadius.
                float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 * glm::pi<float>();
                float radius = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * kBubbleRadius * 0.65f;
                glm::vec2 offset = glm::vec2(radius * std::cos(angle), radius * std::sin(angle));
                targetPostion += offset;

                arrows.emplace_back(std::make_shared<Arrow>(
                    glm::vec2(gameCharacters["weiqing"]->GetPosition().x - 2 * kBubbleRadius, gameCharacters["weiqing"]->GetPosition().y + 5.5 * kBubbleRadius),
                    glm::vec2(this->width / 13.6708861f, this->height / 70.f),
                    ResourceManager::GetInstance().GetTexture("arrow3")
                ));
                /*arrows.emplace_back(glm::vec2(gameCharacters["guojie"]->GetPosition().x + 3 * kBubbleRadius, gameCharacters["guojie"]->GetPosition().y + 5.5 * kBubbleRadius), glm::vec2(this->width / 13.6708861f, this->height / 70.f), ResourceManager::GetInstance().GetTexture("arrow3"));*/
                arrows.back()->Fire(targetPostion, 65.0f*kBubbleRadius);

                this->GoToState(GameState::PREPARING);
                this->scroll->SetState(ScrollState::CLOSING);
                ++(this->level);
                /*if (this->level < numGameLevels) {
                    ++(this->level);
                }*/
 /*               if (this->level < numGameLevels) {
                    this->GoToState(GameState::PREPARING);
                    this->scroll->SetState(ScrollState::CLOSING);
                    ++(this->level);
                }
                else {
                    this->GoToState(GameState::WIN);
                    this->scroll->SetState(ScrollState::CLOSING);
                }*/
                break;
            }
        }

        // Check if the current level is failed.
        if (this->IsLevelFailed()) {
            // If weiqing's health is 0, then lose the game.
            if (gameCharacters["weiqing"]->GetHealth().GetCurrentHealth() == 0) {
                this->GoToState(GameState::LOSE);
            }
            else {
                // If the current level is failed, then we restart the current level.
                this->GoToState(GameState::PREPARING);
                this->scroll->SetState(ScrollState::CLOSING);
            }
        }

        // check if the event timer for narrowing the scroll is triggered.
        if (this->scroll->GetState() == ScrollState::OPENED) {
            if (this->timer->GetEventStartTime("beforenarrowing") < 0.f) {
                this->timer->ResumeEventTimer("beforenarrowing");
            }

            if (this->timer->IsEventTimerExpired("beforenarrowing")) {
                // Stop the shake effect
                this->gameArenaShaking = false;
                // Narrow the scroll
                this->scroll->SetState(ScrollState::NARROWING);
                this->scroll->SetTargetSilkLenForNarrowing(gameBoardSize.y - 2 * kBubbleRadius);
                // Restart the event timer for narrowing the scroll.
                this->timer->StartEventTimer("beforenarrowing");
                // Pause the event timer for narrowing the scroll.
                this->timer->PauseEventTimer("beforenarrowing");
            }
            else {
                // Update the time to be displayed on the screen.
                texts["time"]->SetParagraph(0, std::to_string(static_cast<int>(std::ceil(this->timer->GetEventRemainingTime("beforenarrowing")))));
                // Shaking the whole scroll if the time is less than 1.5s.
                if (this->timer->GetEventRemainingTime("beforenarrowing") < 1.5f) {
                    this->gameArenaShaking = true;
                }
            }
        }

        if (this->targetState != GameState::UNDEFINED && this->scroll->GetState() == ScrollState::CLOSED) {
            this->SetToTargetState();
            this->SetTransitionState(TransitionState::TRANSITION);
            // Set the state of the gameboard to be ACTIVE if the new state is not PREPARING.
            if (this->state != GameState::PREPARING) {
                gameBoard->SetState(GameBoardState::ACTIVE);
                this->scroll->SetState(ScrollState::OPENING);
            }
        }
    }
    else if (this->state == GameState::PREPARING) {
        this->scroll->SetTargetSilkLenForNarrowing(this->scroll->GetTargetSilkLenForOpening());
        this->scroll->SetCurrentSilkLenForNarrowing(this->scroll->GetTargetSilkLenForOpening());
        if (this->scroll->GetState() == ScrollState::CLOSED) {
            moves.clear();
            // If the static bubbles are empty, then retract the scroll, else the scroll would be attacking the player.
            if (statics.empty()) {
				this->scroll->SetState(ScrollState::RETRACTING);
			}
            else {
                this->scroll->SetState(ScrollState::ATTACKING);		
            }
            statics.clear();
            colorCount.clear();
            /*this->scroll->SetState(ScrollState::OPENING);*/
        }
        else if (this->scroll->GetState() == ScrollState::DEPLOYED || this->scroll->GetState() == ScrollState::RETURNED) {
            if (this->targetState != GameState::LOSE) {
                this->scroll->SetState(ScrollState::OPENING);
            }
            else {
                this->scroll->SetState(ScrollState::RETRACTING);
            }
        }
        if (statics.empty()) {
            // Generate random static bubbles
            GenerateRandomStaticBubbles();
            // refresh the color of the carried bubble and the next bubble based the existing colors of all static bubbles if it is the first level.
            if (this->level == 1) {
				shooter->RefreshCarriedBubbleColor(GetNextBubbleColor());
				shooter->RefreshNextBubbleColor(GetNextBubbleColor());
                //// update the gameboard color based on the color of the ray.
                //gameBoard->UpdateColor(shooter->GetRay().GetColorWithoutAlpha());
			}
            // Get half total offset of the scroll narrowing
            glm::vec2 halfOffset = gameBoard->GetValidPosition() - gameBoard->GetPosition();

            // Set the valid boundaries of the game board.
            gameBoard->SetValidPosition(gameBoard->GetPosition());
            gameBoard->SetValidSize(gameBoard->GetSize());
            // reset the time text position
            texts["time"]->SetPosition(texts["time"]->GetPosition() - halfOffset);
            // reset the position of the shooter
            shooter->SetPosition(shooter->GetPosition() + halfOffset);
            shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(), this->statics);
        }
        else {
            if (scroll->GetState() == ScrollState::OPENED) {
                this->SetState(GameState::ACTIVE);
                this->targetState = GameState::UNDEFINED;
                float duration = GetNarrowingTimeInterval();
                this->timer->SetEventTimer("beforenarrowing", duration);
                this->timer->StartEventTimer("beforenarrowing");
            }
        }
    }
    else if (this->state == GameState::WIN || this->state == GameState::LOSE) {
        //if (this->scroll->GetState() == ScrollState::CLOSED) {
        //    this->scroll->SetState(ScrollState::RETRACTING);
        //}
        const float targetFontScale = 0.2f / kFontScale;
        std::string topic = this->state == GameState::WIN ? "victory" : "defeated";
        if (texts[topic]->GetScale() > targetFontScale) {
           /* 0.25 * kFontScale * dt;*/
            float newScale = texts[topic]->GetScale() - 1.f / kFontScale * dt;
            if (newScale < targetFontScale) {
				newScale = targetFontScale;

                // Make the whole screen blur gradually.
                postProcessor->SetBlur(true);
                postProcessor->SetSampleOffsets(0.0005f);
			}
            texts[topic]->SetScale(newScale);
  /*          texts["victory"]->SetPosition(texts["victory"]->GetPosition() + newScale);*/
        }
        else {
            // If the game is lost, then grayscale the screen.
            if (this->state == GameState::LOSE) {
				postProcessor->SetGrayscale(true);
			}
            // Make the whole screen blur gradually.
            float targetSampleOffsets = 1.f / 300;
            if (postProcessor->GetSampleOffsets() < targetSampleOffsets) {
                float newSampleOffsets = postProcessor->GetSampleOffsets() + 0.0015f * dt;
                if (newSampleOffsets > targetSampleOffsets) {
					newSampleOffsets = targetSampleOffsets;
                    this->timer->SetEventTimer("prompttomainmenu", 1.5f);
                    this->timer->SetEventTimer("hideprompttomainmenu", 0.5f);
                    this->timer->StartEventTimer("prompttomainmenu");
                    this->timer->StartEventTimer("hideprompttomainmenu");
                    this->timer->PauseEventTimer("hideprompttomainmenu");
				}
				postProcessor->SetSampleOffsets(newSampleOffsets);
			}
            if (this->timer->HasEvent("prompttomainmenu") && !this->timer->IsPaused("prompttomainmenu")) {
                if (this->timer->IsEventTimerExpired("prompttomainmenu")) {
                    this->timer->StartEventTimer("prompttomainmenu");
                    this->timer->PauseEventTimer("prompttomainmenu");
                    this->timer->StartEventTimer("hideprompttomainmenu");
                }
            }
            else if (this->timer->HasEvent("hideprompttomainmenu") && !this->timer->IsPaused("hideprompttomainmenu")) {
                if (this->timer->IsEventTimerExpired("hideprompttomainmenu")) {
                    this->timer->StartEventTimer("hideprompttomainmenu");
                    this->timer->PauseEventTimer("hideprompttomainmenu");
                    this->timer->StartEventTimer("prompttomainmenu");
                }
            }
        }
    }
    else if (this->state == GameState::INITIAL) {
        if (this->transitionState == TransitionState::START) {
            float scrollCenterX = scroll->GetCenter().x;
            float scrollCenterY = scroll->GetCenter().y;
            // If the scroll is out of the top of the screen and its velocity is 0.f, then we give it a velocity and a acceleration to make it gradually move down.
            if (scrollCenterY < 0.f && scroll->GetVelocity() == glm::vec2(0.f, 0.f)) {
                glm::vec2 scrollCenter = scroll->GetCenter();
                scroll->SetVelocity(glm::vec2(0.0f, 26 * kBubbleRadius));
                float stopPoint = this->height * 0.545f;
                float timeToStop = 2.f*(stopPoint-scrollCenter.y)/scroll->GetVelocity().y;
                glm::vec2 acceleration = glm::vec2(0,-scroll->GetVelocity().y / timeToStop);
                scroll->SetAcceleration(acceleration);
            }
            else {
                // Move the scroll with a gradually decreasing speed.
                scroll->Move(dt);

                if (scroll->GetVelocity().y <= 0.f && areFloatsEqual(scrollCenterY, this->height * 0.5f, kBubbleRadius * 0.04f)) {
                    // Set the center of the scroll to be at the center of the screen.
                    scroll->SetCenter(glm::vec2(this->width / 2.0f, this->height / 2.0f));
                    // Set the velocity and acceleration of the scroll to be 0.
                    scroll->SetVelocity(glm::vec2(0.f, 0.f));
                    scroll->SetAcceleration(glm::vec2(0.f, 0.f));
                    // Set the transition state to be END.
                    this->SetTransitionState(TransitionState::TRANSITION);
                    GameState lastLastState = this->lastState;
                    // Set the game state to the target state.
                    this->SetToTargetState();
                    // Set the gameboard state to ACTIVE.
                    gameBoard->SetState(GameBoardState::ACTIVE);
                    // Set the scoll state to be OPENING.
                    scroll->SetState(ScrollState::OPENING);
                    if (lastLastState == GameState::EXIT) {
                        // Set the offset of the text "Story".
                        glm::vec2 buttonSectionPos = this->pages.at("story")->GetSection("storybuttonsection")->GetPosition();
                        glm::vec2 textSectionPos = this->pages.at("story")->GetSection("storytextsection")->GetPosition();
                        float initialOffset = buttonSectionPos.y - textSectionPos.y;
                        pages.at("story")->GetSection("storytextsection")->SetOffset(initialOffset);
                    }
                }
            }
        }
        else if(this->transitionState == TransitionState::TRANSITION){
        }
    }
    else if (this->state == GameState::STORY) {
        auto textSection = this->pages.at("story")->GetSection("storytextsection");
        float targetOffset = 0.95f * textSection->GetMaxHeight() - textSection->GetHeight();
        if ((this->transitionState == TransitionState::TRANSITION && this->targetState == GameState::UNDEFINED 
            || this->targetState != GameState::UNDEFINED && this->scroll->GetState() == ScrollState::CLOSING) && !textSection->IsScrollIconAllowed()) {
            // If the offset of the text section is greater than 0, then we decrease the offset to move the text section upwards.
            float offset = textSection->GetOffset();
            targetOffset = std::min(0.f, targetOffset);
            if (offset > targetOffset) {
                offset = std::max(offset - 5 * kBubbleRadius * dt, targetOffset);
				this->pages.at("story")->GetSection("storytextsection")->SetOffset(offset);
			}
            else {
				// Finish the transition to GameState::STORY
				this->SetTransitionState(TransitionState::END);
                // Enable the scroll icon
                textSection->SetScrollIconAllowed(true);
                // Set the position of the scroll icon
                textSection->ResetSrcollIconPosition();
			}
        }
        if (this->targetState != GameState::UNDEFINED && this->scroll->GetState() == ScrollState::CLOSED) {
			this->SetToTargetState();
            this->SetTransitionState(TransitionState::TRANSITION);
            if (this->state != GameState::PREPARING) {
                this->scroll->SetState(ScrollState::OPENING);
            }
            // If the new state is Preparing, then we set the gameboard state to be INGAME.
            if (this->state == GameState::PREPARING) {
				gameBoard->SetState(GameBoardState::INGAME);
			}
            if (!textSection->IsScrollIconAllowed()) {
				textSection->SetOffset(targetOffset);
                // Enable the scroll icon
                textSection->SetScrollIconAllowed(true);
                // Set the position of the scroll icon
                textSection->ResetSrcollIconPosition();
			}
            
		}
		
    }
    if (this->state == GameState::CONTROL) {
        if (this->transitionState == TransitionState::TRANSITION) {

			// Set the button "Start", "Story", and "Exit" to from kInactive to kNormal.
            if (this->lastState == GameState::STORY && buttons["start"]->GetState() == ButtonState::kInactive) {
    //            auto buttonSection = pages["control"]->GetSection("controlbuttonsection");
    //            for (const auto& buttonName : buttonSection->GetOrder()) {
    //                std::shared_ptr<ButtonUnit> buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(buttonSection->GetContent(buttonName));
				//	buttonUnit->GetButton()->SetState(ButtonState::kNormal);

				//}

                /*AdjustButtonsHorizontalPosition({ "back", "exit" });*/
            } else if (this->lastState == GameState::ACTIVE && buttons["restart"]->GetState() == ButtonState::kInactive) {
                    buttons["restart"]->SetState(ButtonState::kNormal);
                    buttons["resume"]->SetState(ButtonState::kNormal);
                    buttons["stop"]->SetState(ButtonState::kNormal);
                    /*AdjustButtonsHorizontalPosition({ "restart", "resume", "stop" });*/
            }else{
                if (this->scroll->GetState() == ScrollState::OPENED && (buttons["back"]->GetState() != ButtonState::kInactive || buttons["restart"]->GetState() != ButtonState::kInactive)) {
					this->SetTransitionState(TransitionState::END);
				}
				
            }
            activePage = "control";
        }
        if (this->targetState != GameState::UNDEFINED && this->scroll->GetState() == ScrollState::CLOSED) {
            this->SetToTargetState();
            this->SetTransitionState(TransitionState::TRANSITION);
            if (this->state != GameState::PREPARING) {
                this->scroll->SetState(ScrollState::OPENING);
            }

            // If the new state is PREPARING or ACTIVE, then we set the gameboard state to be INGAME.
            if (this->state == GameState::PREPARING || this->state == GameState::ACTIVE) {
                gameBoard->SetState(GameBoardState::INGAME);	
            }
        }
    }
    else if (this->state == GameState::DISPLAY_SETTINGS) {
        if (this->targetState != GameState::UNDEFINED && this->scroll->GetState() == ScrollState::CLOSED) {
			this->SetToTargetState();
			this->SetTransitionState(TransitionState::TRANSITION);
            this->scroll->SetState(ScrollState::OPENING);
        }
    }
    // Check if the characters are moving to the target position.
    for (auto& [name, character] : gameCharacters) {
        if (character->IsMoving()) {
            /*float characterVelocity = 30 * kBubbleRadius;*/
            float characterVelocity = glm::length(character->GetVelocity() + character->GetAcceleration() * dt * 0.5f);
            glm::vec2 characterDirection = character->GetCurrentTargetPosition() - character->GetPosition();
            glm::vec2 characterVelocityVec = glm::normalize(characterDirection) * characterVelocity;

            // Create shadow as the character moves.
      /*      std::cout << "frame count while doing respawn: " << this->frameCount << std::endl;*/
            shadowTrailSystem->respawnParticles(*character, 40, characterVelocityVec*0.1f, glm::vec2(kBubbleRadius*0.2f, kBubbleRadius*0.4f), glm::vec2(character->GetSize().x*0.46f, character->GetSize().y*0.48f));
            
 /*           if (name == "guojie") {
                character->SetAlpha(0.2f);
            }*/;
    /*        glm::vec2 avgVelocity = character->GetVelocity() + character->GetAcceleration() * dt * 0.5f;*/
			character->MoveTo(character->GetCurrentTargetPosition(), characterVelocity, dt);
            character->SetVelocity(character->GetVelocity() + character->GetAcceleration() * dt);
            if (!character->IsMoving()) {
                character->StopMoving();
            }
            if (name == "guojie" && !character->IsMoving()) {
                // Shake the screen when guojie lands on the target position.
                postProcessor->SetShake(true, /*strength=*/0.0081f, /*timeMultiplierForX=*/30.f, /*timeMultiplierForY=*/80.f);
                postProcessor->SetBlur(true);
                postProcessor->SetSampleOffsets(0.0005f);
                // Set time for shaking the screen.
                this->timer->SetEventTimer("guojieshaking", 0.95f);
                this->timer->StartEventTimer("guojieshaking");
                // Set the time for cracks on the ground.
                this->timer->SetEventTimer("cracks", 1.5f);
                this->timer->StartEventTimer("cracks");
            }
		}
        // Set the state of the game characters after guojie arrives at the target position.
        if (this->timer->HasEvent("guojieshaking") && this->timer->GetEventUsedTime("guojieshaking") > 0.085f) {
            gameCharacters["liuche"]->SetState(GameCharacterState::SAD);
            gameCharacters["weizifu"]->SetState(GameCharacterState::SAD);
            gameCharacters["weiqing"]->SetState(GameCharacterState::FIGHTING);
        }
	}

    // Update all the particles
    /*std::cout << "frame count while updating: " << this->frameCount << std::endl;*/
    shadowTrailSystem->Update(dt);
    explosionSystem->Update(dt);

    // Update all the falling bubbles
    std::vector<int> toRemove;
    for (auto& [id, bubble] : fallings) {
        // update the position of the falling bubble
        bubble->Move(dt);
        // Apply gravity to the falling bubble
        bubble->ApplyGravity(dt);
        // If the falling bubble pass the bottom of the game board, then we remove it from the falling bubbles.
        if (bubble->GetPosition().y > gameBoard->GetPosition().y + gameBoard->GetSize().y) {
            toRemove.emplace_back(id);
        }
    }
    for (const int id : toRemove) {
        fallings.erase(id);
    }

    if (!graduallyTransparentObjects.empty()) {
        // gruadully make the old-state characters invisible.
        std::vector<std::string> objectToRemove;
        for (auto& item : graduallyTransparentObjects) {
            const std::string& objectName = item.first;
            auto& gameObject = item.second.first;
            float transparentSpeed = item.second.second;
            // If the object is still visible, then we make it gradually transparent.
            if (gameObject->GetAlpha() > 0.f) {
                float newAlpha = gameObject->GetAlpha() - transparentSpeed * dt;
                if (newAlpha < 0.f) {
                    newAlpha = 0.f;
                }
                gameObject->SetAlpha(newAlpha);
            }
            else {
                // Add the object name to the remove list.
                objectToRemove.emplace_back(objectName);
            }
        }
        // Remove the objects that are already invisible.
        for (const std::string& objectName : objectToRemove) {
            graduallyTransparentObjects.erase(objectName);
        }
    }

    if (!graduallyOpaqueObjects.empty()) {
        // gruadully make the old-state characters invisible.
        std::vector<std::string> objectToRemove;
        // gruadully make the new-state characters visible.
        for (auto& item : graduallyOpaqueObjects) {
            const std::string& objectName = item.first;
            auto& gameObject = item.second.first;
            float opaqueSpeed = item.second.second;
            // If the object is still invisible, then we make it gradually opaque.
            if (gameObject->GetAlpha() < 1.f) {
                float newAlpha = gameObject->GetAlpha() + opaqueSpeed * dt;
                if (newAlpha > 1.f) {
                    newAlpha = 1.f;
                }
                gameObject->SetAlpha(newAlpha);
            }
            else {
                // Add the object name to the remove list.
                objectToRemove.emplace_back(objectName);
            }
        }
        // Remove the objects that are already visible.
        for (const std::string& objectName : objectToRemove) {
            graduallyOpaqueObjects.erase(objectName);
        }
    }

    // Gradually fading out the blur if the event "deblurring" exists.
    if (this->timer->HasEvent("deblurring")) {
        float targetSampleOffsets = 0.f;
        if (postProcessor->GetSampleOffsets() > targetSampleOffsets) {
            float newSampleOffsets = postProcessor->GetSampleOffsets() - 0.002f * dt;
            if (newSampleOffsets < targetSampleOffsets) {
                newSampleOffsets = targetSampleOffsets;
                this->timer->CleanEvent("deblurring");
                postProcessor->SetBlur(false);   
            }
            postProcessor->SetSampleOffsets(newSampleOffsets);
        }
    }
}

void GameManager::Render(){

    if (this->state == GameState::EXIT) {
        return;
    }

    //if (this->state == GameState::OPTION) {
    //    // Render the buttons that is active
    //    auto it = buttons.begin();
    //    while (it != buttons.end())
    //    {
    //        if (it->second->GetState() != ButtonState::kInactive) {
    //            it->second->Draw(textRenderer2, colorRenderer);
    //        }
    //        ++it;
    //    }
    //   /* texts["prompttomainmenu"]->Draw(textRenderer2, true);*/
    //    return;
    //}

    // Background
    ResourceManager& resourceManager = ResourceManager::GetInstance();


    postProcessor->BeginRender();
    
    //if (!areFloatsEqual(glm::vec2(this->width, this->height), kFullScreenSize)) {
    //    std::string str = "";
    //}

    spriteRenderer->DrawSprite(resourceManager.GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Draw shadow particles
    shadowTrailSystem->Draw(/*isDarkBackGround=*/true);

    if (this->state == GameState::ACTIVE || this->state == GameState::PREPARING || this->state == GameState::WIN || this->state == GameState::LOSE) {
        // Create cracks on the ground when guojie lands on the target position.
        if (this->timer->HasEvent("cracks") || this->timer->HasEvent("cracksFading")) {
            glm::vec2 cracksPosition = gameCharacters["guojie"]->GetPosition() + glm::vec2(gameCharacters["guojie"]->GetSize().x * 0.0f, gameCharacters["guojie"]->GetSize().y * 0.84f);
            float crackswidth = gameCharacters["guojie"]->GetSize().x * 1.5f;
            float cracksheight = crackswidth * 0.56479f;
            /*spriteRenderer->DrawSprite(resourceManager.GetTexture("cracks"), cracksPosition, glm::vec2(crackswidth, cracksheight));*/
            // Set the alpha channel range based on the remaining time of the event timer.
            glm::vec2 alphaChannelRange = glm::vec2(0.0f, 1.0f);
            if (this->timer->HasEvent("cracksFading")) {
                alphaChannelRange = glm::vec2(0.0f, this->timer->GetEventRemainingTime("cracksFading") / this->timer->GetEventTimer("cracksFading"));
            }
            partialTextureRenderer->DrawPartialTexture(resourceManager.GetTexture("cracks"), 
                /*position=*/cracksPosition, 
                /*size=*/glm::vec2(crackswidth, cracksheight),
                /*redChannelRange=*/glm::vec2(0.f, 0.15686f),
                /*greenChannelRange=*/glm::vec2(0.f, 0.07843f),
                /*blueChannelRange=*/glm::vec2(0.f, 0.03922f),
                /*alphaChannelRange=*/alphaChannelRange);
            if (this->timer->HasEvent("cracks") && this->timer->IsEventTimerExpired("cracks")) {
				this->timer->CleanEvent("cracks");
                this->timer->SetEventTimer("cracksFading", 2.f);
                this->timer->StartEventTimer("cracksFading");
            }
            else if (this->timer->HasEvent("cracksFading") && this->timer->IsEventTimerExpired("cracksFading")) {
				this->timer->CleanEvent("cracksFading");
			}
        }


        // Draw all sad and happy state of wei zi fu
      /*  gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);*/
        gameCharacters["weizifu"]->Draw(spriteRenderer);

        gameCharacters["liuche"]->Draw(spriteRenderer);

        if(gameCharacters["guojie"]->IsMoving()){
            // Do not draw health bar when guojie is moving.
            gameCharacters["guojie"]->Draw(spriteRenderer);
            gameCharacters["weiqing"]->Draw(spriteRenderer);
        }
        else {
            gameCharacters["guojie"]->DrawGameCharacter(spriteRenderer, colorRenderer, circleRenderer, textRenderer2);
            gameCharacters["weiqing"]->DrawGameCharacter(spriteRenderer, colorRenderer, circleRenderer, textRenderer2);
        }

        // Check if the event timer for shaking the screen has expired.
        if (this->timer->HasEvent("guojieshaking") && this->timer->IsEventTimerExpired("guojieshaking")) {
			postProcessor->SetShake(false);
            postProcessor->SetBlur(false);
            this->timer->CleanEvent("guojieshaking");
		}

  //       Render the arrows
  //      for (auto& [id, carriedObject] : gameCharacters["guojie"]->GetCarriedObjects()) {
  //          if (auto arrow = std::dynamic_pointer_cast<Arrow>(carriedObject)) {
  //              if (arrow->IsPenetrating() || arrow->IsStopped()) {
  //                  arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
  //              }
  //              else {
  //                  arrow->Draw(spriteRenderer);
  //              }
		//	}
		//}
        for (auto& arrow : arrows) {
            if (arrow->IsPenetrating() || arrow->IsStopped()) {
                arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
            }
            else {
                arrow->Draw(spriteRenderer);
            }
        }
      
        
        if (this->state == GameState::ACTIVE || this->state == GameState::PREPARING) {
           
            if (scroll->GetState() != ScrollState::DISABLED) {
                if (this->gameArenaShaking) {
                    // Memorize the current positions for each shaking object.
                    originalPositionsForShaking.clear();
                    originalPositionsForShaking["scroll"] = this->scroll->GetCenter();
                    originalPositionsForShaking["gameboard"] = gameBoard->GetPosition();
                    originalPositionsForShaking["shooter"] = shooter->GetPosition();
                    for (auto& [id, bubble] : moves) {
                        originalPositionsForShaking[std::to_string(id)] = bubble->GetPosition();
                    }
                    for (auto& [id, bubble] : statics) {
                        originalPositionsForShaking[std::to_string(id)] = bubble->GetPosition();
                    }
                    originalPositionsForShaking["time"] = texts["time"]->GetPosition();

                    // start shake effect
                    float shakingStrengthForX = kBubbleRadius*0.6048f; // Shaking intensity for the X axis
                    float shakingStrengthForY = kBubbleRadius*0.3402f; // Shaking intensity for the Y axis
                    float timeMultiplierForX = 60.0f; // Controls the frequency of shaking along the X axis
                    float timeMultiplierForY = 160.0f; // Controls the frequency of shaking along the Y axis
                    float currentTime = glfwGetTime();
                    // Calculate the shake offset based on the current time
                    float shakeOffsetX = cos(currentTime * timeMultiplierForX) * shakingStrengthForX;
                    float shakeOffsetY = cos(currentTime * timeMultiplierForY) * shakingStrengthForY;
                    // shake the scroll
                    scroll->SetCenter(scroll->GetCenter() + glm::vec2(shakeOffsetX, shakeOffsetY));
                    // shake the game board
                    gameBoard->SetPosition(gameBoard->GetPosition() + glm::vec2(shakeOffsetX, shakeOffsetY));
                    // shake the shooter
                    shooter->SetPosition(shooter->GetPosition() + glm::vec2(shakeOffsetX, shakeOffsetY));
                    // Update the path of the ray
                    shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(), this->statics);
                    // shake the moving bubbles
                    for (auto& [id, bubble] : moves) {
                        bubble->SetPosition(bubble->GetPosition() + glm::vec2(shakeOffsetX, shakeOffsetY));
                    }
                    // shake the static bubbles
                    for (auto& [id, bubble] : statics) {
                        bubble->SetPosition(bubble->GetPosition() + glm::vec2(shakeOffsetX, shakeOffsetY));
                    }
                    // shake the time text
                    texts["time"]->SetPosition(texts["time"]->GetPosition() + glm::vec2(shakeOffsetX, shakeOffsetY));
                }

                scroll->Draw(spriteRenderer);

                // Enable scissor test
                ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
                handler.EnableScissorTest();

                // Set the scissor box based on the upper and lower scroll
                glm::vec4 silkBounds = scroll->GetSilkBounds();
                handler.SetScissorBox(silkBounds[0], this->height - silkBounds[3], scroll->GetSilkWidth(), scroll->GetSilkLen());
               /* glScissor(silkBounds[0], this->height - silkBounds[3], scroll->GetSilkWidth(), scroll->GetSilkLen());*/

                // GameBoard
                gameBoard->Draw(spriteRenderer);

                // Shooter
                shooter->Draw(spriteRenderer);

                //// Ray
              /*  shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(), this->statics);*/
                shooter->GetRay().Draw(rayRenderer);

                // Particles
                explosionSystem->Draw();

                // Draw all moving bubbles
                for (auto& bubble : moves) {
                    bubble.second->Draw(spriteRenderer);
                }

                // Draw all static bubbles
                for (auto& bubble : statics) {
                    bubble.second->Draw(spriteRenderer);
                }

                //// Draw all the free slots
                //for (auto slot : this->freeSlots) {
                //    spriteRenderer->DrawSprite(resourceManager.GetTexture("bubble"), glm::vec2(slot.x-kBubbleRadius, slot.y-kBubbleRadius), glm::vec2(2*kBubbleRadius, 2 * kBubbleRadius), 0.f, glm::vec2(0.5,0.5), glm::vec4(0.5,0.5,0.5,0.7));
                //}

                // Draw all falling bubbles
                for (auto& bubble : fallings) {
                    bubble.second->Draw(spriteRenderer);
                }

                // Disable scissor test
                /*glDisable(GL_SCISSOR_TEST);*/
                handler.DisableScissorTest();

                // Draw the time when the scroll is opened
                if (this->scroll->GetState() == ScrollState::OPENED) {
                    texts["time"]->Draw(textRenderer2);
                }

                // Restore the original positions for each shaking object
                if (this->gameArenaShaking) {
                    this->scroll->SetCenter(originalPositionsForShaking["scroll"]);
					gameBoard->SetPosition(originalPositionsForShaking["gameboard"]);
					shooter->SetPosition(originalPositionsForShaking["shooter"]);
                    shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(), this->statics);
                    for (auto& [id, bubble] : moves) {
						bubble->SetPosition(originalPositionsForShaking[std::to_string(id)]);
					}
                    for (auto& [id, bubble] : statics) {
						bubble->SetPosition(originalPositionsForShaking[std::to_string(id)]);
					}
					texts["time"]->SetPosition(originalPositionsForShaking["time"]);
                }
            }
        }
        else {
            if (scroll->GetState() != ScrollState::DISABLED) {
                scroll->Draw(spriteRenderer);
            }
            // Particles
            explosionSystem->Draw();
        }
    }
    else if (this->state == GameState::MENU || this->state == GameState::INITIAL || this->state == GameState::STORY || this->state == GameState::CONTROL ||
        this->state == GameState::DISPLAY_SETTINGS) {
        gameCharacters["weizifu"]->Draw(spriteRenderer);
        gameCharacters["liuche"]->Draw(spriteRenderer);
        gameCharacters["weiqing"]->Draw(spriteRenderer);
        if (this->targetState == GameState::PREPARING) {
            gameCharacters["guojie"]->Draw(spriteRenderer);
        }
        // Draw scroll
        scroll->Draw(spriteRenderer);
        // Enable scissor test
       /* glEnable(GL_SCISSOR_TEST);*/
        ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
        handler.EnableScissorTest();

        // Set the scissor box based on the upper and lower scroll
        glm::vec4 silkBounds = scroll->GetSilkBounds();
        /*glScissor(silkBounds[0], this->height - silkBounds[3], scroll->GetSilkWidth(), scroll->GetSilkLen());*/
        handler.SetScissorBox(silkBounds[0], this->height - silkBounds[3], scroll->GetSilkWidth(), scroll->GetSilkLen());
        // Draw the game board if it is ACTIVE
        if (gameBoard->GetState() == GameBoardState::ACTIVE) {
			gameBoard->Draw(spriteRenderer);
		}    
        
        if (!activePage.empty()) {
			pages.at(activePage)->Draw();
		}

  //      //// Render the buttons that is active
  //      //if (this->state != GameState::CONTROL) {
  //      //    auto it = buttons.begin();
  //      //    while (it != buttons.end())
  //      //    {
  //      //        if (it->second->GetState() != ButtonState::kInactive) {
  //      //            it->second->Draw(textRenderer2, colorRenderer);
  //      //        }
  //      //        ++it;
  //      //    }
  //      //}

        // Disable scissor test
   /*     glDisable(GL_SCISSOR_TEST);*/
        handler.DisableScissorTest();
	}

    if (!this->graduallyTransparentObjects.empty()) {
        for (auto& item : graduallyTransparentObjects) {
            item.second.first->Draw(spriteRenderer);
            // if it's a game character, then draw the carried objects.
            if (std::shared_ptr<GameCharacter> character = std::dynamic_pointer_cast<GameCharacter>(item.second.first)) {
                auto carriedObjects = character->GetCarriedObjects();
                for (auto& [id, carriedObject] : carriedObjects) {
                    if (std::shared_ptr<Arrow> arrow = std::dynamic_pointer_cast<Arrow>(carriedObject)) {
                        arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
                    }
                    else {
                    	carriedObject->Draw(spriteRenderer);
                    }
				}
            }
        }
    }
   
    postProcessor->EndRender();
    postProcessor->Render(glfwGetTime());
  
    if (this->state == GameState::WIN || this->state == GameState::LOSE) {
        if (this->state == GameState::WIN) {
            texts["victory"]->Draw(textRenderer2, true);
        }
        else {
            texts["defeated"]->Draw(textRenderer2, true);
        }
        if (this->timer->HasEvent("prompttomainmenu") && !this->timer->IsPaused("prompttomainmenu")) {
            texts["prompttomainmenu"]->Draw(textRenderer2, true);
        }
    }
}

std::string GameManager::GetPageName(GameState gameState) {
    switch (gameState) {
		case GameState::STORY:
			return "story";
		case GameState::CONTROL:
			return "control";
        case GameState::DISPLAY_SETTINGS:
			return "displaysettings";
		default:
			return "";
	}
}

void GameManager::SetState(GameState newState) {
    this->lastState = this->state;
	this->state = newState;
    // setting the active page based on the new state
    std::string lastPage = GetPageName(this->lastState);
    activePage = GetPageName(newState);
    if (!lastPage.empty()) {
        // Disable the buttons in the last page.
        for(const auto& sectionName : pages.at(lastPage)->GetOrder()){
		    auto section = pages.at(lastPage)->GetSection(sectionName);
			for (const auto& contentName : section->GetOrder()) {
				if (auto buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(section->GetContent(contentName))) {
					buttonUnit->GetButton()->SetState(ButtonState::kInactive);
				}
			}
		}
    }
    if (!activePage.empty()) {
        // Enable the buttons in the last page.
        for (const auto& sectionName : pages.at(activePage)->GetOrder()) {
            auto section = pages.at(activePage)->GetSection(sectionName);
            for (const auto& contentName : section->GetOrder()) {
                if (auto buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(section->GetContent(contentName))) {
                    buttonUnit->GetButton()->SetState(ButtonState::kNormal);
                }
            }
        }
        // Refresh the position of each units in the active page.
        pages.at(activePage)->UpdatePosition();
    }
               
}

void GameManager::SetToTargetState() {
	SetState(this->targetState);
    this->targetState = GameState::UNDEFINED;
}

GameState GameManager::GetState() {
    return this->state;
}

void GameManager::GoToState(GameState newState) {
    this->targetState = newState;
    this->transitionState = TransitionState::START;
}

void GameManager::GoToScreenMode(ScreenMode newScreenMode) {
	this->targetScreenMode = newScreenMode;
}

void GameManager::SetScreenMode(ScreenMode newScreenMode) {
    assert(newScreenMode != ScreenMode::UNDEFINED && "The screen mode is not defined.");
	this->screenMode = newScreenMode;
    // Store the screen mode to the global config.
    ConfigManager& configManager = ConfigManager::GetInstance();
    configManager.SetScreenMode(this->screenMode);
}

ScreenMode GameManager::GetScreenMode() {
	return this->screenMode;
}

void GameManager::SetToTargetScreenMode() {
	SetScreenMode(this->targetScreenMode);
	this->targetScreenMode = ScreenMode::UNDEFINED;
}

void GameManager::SetTransitionState(TransitionState newTransitionState) {
	this->transitionState = newTransitionState;
}

TransitionState GameManager::GetTransitionState() {
	return this->transitionState;
}

void GameManager::SetScrollState(ScrollState newScrollState) {
	this->scroll->SetState(newScrollState);
}

ScrollState GameManager::GetScrollState() {
	return this->scroll->GetState();
}


std::vector<int> GameManager::GetNeighborIds(std::unique_ptr<Bubble>& bubble, float absError) {
    std::vector<int> neighborIds;
    for (auto& [id, staticBubble] : statics) {
        if (IsNeighbor(staticBubble->GetCenter(), bubble->GetCenter(), absError)) {
            neighborIds.push_back(id);
        }
    }
    return neighborIds;
}

std::vector<int> GameManager::GetNeighborIds(std::vector<Bubble*>& bubbles) {
    std::vector<int> neighborIds;
    for (auto& bubble : bubbles) {
        for (auto& [id, staticBubble] : statics) {
            if (IsNeighbor(staticBubble->GetCenter(), bubble->GetCenter())) {
				neighborIds.push_back(id);
			}
		}
	}
	return neighborIds;
}

std::vector<int> GameManager::GetNeighborIds(std::unordered_map<int, std::unique_ptr<Bubble> >& bubbles) {
    std::unordered_set<int> neighborIds;
    for (auto& [id, bubble] : bubbles) {
        for (auto& [staticId, staticBubble] : statics) {
            if (IsNeighbor(staticBubble->GetCenter(), bubble->GetCenter())) {
				neighborIds.insert(staticId);
			}
		}
	}
    return std::vector<int>(neighborIds.begin(), neighborIds.end());
}
bool GameManager::IsAtUpperBoundary(glm::vec2 pos) {
	return areFloatsEqual(pos.y, gameBoard->GetValidPosition().y, 1e-2);
}

bool GameManager::IsConnectedToTopHelper(std::unique_ptr<Bubble>& bubble, bool* visited, bool* isConnectToTop) {
    if (visited[bubble->GetID()]) {
        return isConnectToTop[bubble->GetID()];
    }
    
    if (IsAtUpperBoundary(bubble->GetPosition())) {
        return isConnectToTop[bubble->GetID()] = true;
    }
    visited[bubble->GetID()] = true;
    std::vector<int> neighborIds = GetNeighborIds(bubble);
    for (auto& neighborId : neighborIds) {
        if (statics[neighborId] == nullptr) {
            std::string str = "";
        }
        if (IsConnectedToTopHelper(statics[neighborId], visited, isConnectToTop)) {
			isConnectToTop[bubble->GetID()] = true;
		}
	}

    return isConnectToTop[bubble->GetID()];
}
    

bool GameManager::IsConnectedToTop(std::unique_ptr<Bubble>& bubble) {
    const int maxAvailableID = ResourceManager::GetInstance().GetMaxAvailableID();
    bool* visited = new bool[maxAvailableID + 1];
    bool* isConnectToTop = new bool[maxAvailableID + 1];
    for (int i = 0; i <= maxAvailableID; ++i) {
		visited[i] = false;
		isConnectToTop[i] = false;
	}
    
    bool isConnected = IsConnectedToTopHelper(bubble, visited, isConnectToTop);

    delete[] visited;
    delete[] isConnectToTop;
    visited = nullptr;
    isConnectToTop = nullptr;

    return isConnected;
}

std::vector<int> GameManager::FindConnectedBubbles(std::vector<int>& bubbleIds) {
    std::vector<int> unvisited;
    std::vector<int> connectedBubbles;
    // push all the static bubbles ids into the unvisited set.
    for (auto& [id, bubble] : statics) {
		unvisited.emplace_back(id);
	}
    // push all the given bubble ids into the connectedBubblesSet.
    for (auto& id : bubbleIds) {
        connectedBubbles.emplace_back(id);
    }
    // Traverse all the unvisited bubbles and find the bubbles that are connected to the given bubble.
    size_t i = 0;
    while (i < unvisited.size()) {
        int unvisitedId = unvisited[i++];
        for (size_t j = 0; j < connectedBubbles.size(); ++j) {
            if (IsNeighbor(statics[unvisitedId]->GetCenter(), statics[connectedBubbles[j]]->GetCenter())) {
                connectedBubbles.emplace_back(unvisitedId);
                std::swap(unvisited[i-1], unvisited.back());
                unvisited.pop_back();
                if (unvisited.empty()) {
                    return connectedBubbles;
                }
                else {
                    i = 0;
                    break;
                }
            }
        }
    }
    return connectedBubbles;
}

std::vector<int> GameManager::FindAllFallingBubbles() {
    const int maxAvailableID = ResourceManager::GetInstance().GetMaxAvailableID();

    std::vector<int> topIds;
    std::vector<int> fallingIds;


    // First get the ids of all the bubbles that are on the top of the game board.
    for (auto& [id, bubble] : statics) {
        if (IsAtUpperBoundary(bubble->GetPosition())) {
            topIds.emplace_back(id);
		}
	}

    // Find all bubbles that are connected to those top bubbles.
    std::vector<int> connectedIds = FindConnectedBubbles(topIds);

    // Find all the statics bubbles that are not connected to the top of the game board and add them to the falling bubbles.
    for (auto& [id, bubble] : statics) {
        if (std::find(connectedIds.begin(), connectedIds.end(), id) == connectedIds.end()) {
			fallingIds.emplace_back(id);
		}
	}

    return fallingIds;
}

std::vector<int> GameManager::FindCloseUpperBubbles(std::unique_ptr<Bubble>& bubble) {
    std::vector<int> closeUpperIds;
    for (auto& [id, staticBubble] : statics) {
        if (IsAtUpperBoundary(staticBubble->GetPosition()) && glm::distance(staticBubble->GetCenter(), bubble->GetCenter()) < (1.5f * bubble->GetRadius() + staticBubble->GetRadius())) {
            closeUpperIds.emplace_back(id);
        }
    }
    return closeUpperIds;
}

bool GameManager::FineTuneToCloseUpper(int bubbleId) {
    // Get all the static bubbles that are connected to the top of the game board and close to the current bubble.
    auto& bubble = moves[bubbleId];
    std::vector<int> closeUpperIds = FindCloseUpperBubbles(bubble);
    if (closeUpperIds.empty()) {
		return false;
	}
    // Iterate through all the close upper bubbles and find the best free slot center.
    float highestWeight = 0.0f;
    glm::vec2 bestFreeSlotCenter;
    for (const auto& id : closeUpperIds) {
        // Check if the bubble is to the left or to the right of the static bubble.
        float deltaX = statics[id]->GetCenter().x - bubble->GetCenter().x;
        bool isLeft = deltaX < 0.f;
        // If the static bubble is to the left of the moving bubble,then the free slot center should be to the right of the static bubble.
        glm::vec2 freeSlotCenter;
        if (isLeft) {
            freeSlotCenter = statics[id]->GetCenter() + glm::vec2(statics[id]->GetRadius()+bubble->GetRadius(), 0.0f);
        }
        else {
            freeSlotCenter = statics[id]->GetCenter() - glm::vec2(statics[id]->GetRadius() + bubble->GetRadius(), 0.0f);	
        }
        // Get the weight of the free slot
        float weight = GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

        // If the weight is greater than the highest weight, then we update the highest weight and the free slot center.
        if (weight > highestWeight) {
			highestWeight = weight;
			bestFreeSlotCenter = freeSlotCenter;
		}
    }
    // If the highest weight is greater than 0, then we move the bubble to the best free slot center.
    if (highestWeight > 0.0f) {
		bubble->SetPosition(bestFreeSlotCenter - glm::vec2(kBubbleRadius, kBubbleRadius));
		return true;
	}
    // No fine tuning happened.
	return false;

}

bool GameManager::FineTuneToNeighbor(int bubbleId, int staticBubbleId) {
    assert(moves.count(bubbleId) > 0 && statics.count(staticBubbleId) > 0 && "Failed to find the bubble by ID.");
    // bubbles.
    auto& bubble = moves[bubbleId];
    auto& staticBubble = statics[staticBubbleId];
    float highestWeight = 0.0f;
    glm::vec2 bestFreeSlotCenter;
    // Get the normalized vector from the static bubble to the bubble
    glm::vec2 bubbleDirection = glm::normalize(bubble->GetCenter() - staticBubble->GetCenter());
    for (auto& [id, neighborBubble] : statics) {
        if (id == staticBubbleId) {
            continue;
        }
        if (IsNeighbor(neighborBubble->GetCenter(), staticBubble->GetCenter())) {
            // Get the normalized vector from the static bubble to the neighbor
            glm::vec2 neighborDirection = glm::normalize(neighborBubble->GetCenter() - staticBubble->GetCenter());

            // Get the cosine of the angle between the two vectors
            float cosAngle = glm::dot(neighborDirection, bubbleDirection);
            // If the angle is less than 60 degrees or greater than 100 degrees, then we won't consider it.
            if (cosAngle < 0.0f || cosAngle > 0.5f) {
                continue;
            }
            // Get the free slot center. It should form a 60 degree angle with the static bubble and the neighbor bubble.
            glm::vec2 freeSlotCenter;
            int a = getSignOfCrossProduct(neighborDirection, bubbleDirection);
            // if a is 0, then the two vectors are parallel. We don't consider this case.
            if (a == 0) {
                continue;
            }
            else if (a > 0) {
                freeSlotCenter = rotateVector(staticBubble->GetCenter() + 2 * kBubbleRadius * neighborDirection, glm::pi<float>() / 3, staticBubble->GetCenter());
            }
            else {
                freeSlotCenter = rotateVector(staticBubble->GetCenter() + 2 * kBubbleRadius * neighborDirection, -glm::pi<float>() / 3, staticBubble->GetCenter());
            }

            // Get the weight of the free slot
            float weight = GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

            // If the weight is greater than the highest weight, then we update the highest weight and the free slot center.
            if (weight > highestWeight) {
                highestWeight = weight;
                bestFreeSlotCenter = freeSlotCenter;
            }
        }
    }
    // If the highest weight is greater than 0, then we move the bubble to the best free slot center.
    if (highestWeight > 0.0f) {
        bubble->SetPosition(bestFreeSlotCenter - glm::vec2(kBubbleRadius, kBubbleRadius));
        return true;
    }

    // No fine tuning happened.
    return false;
}

bool GameManager::FineTuneToClose(int bubbleId, int staticBubbleId) {
    // bubbles.
    auto& bubble = moves[bubbleId];
    auto& staticBubble = statics[staticBubbleId];
    float highestWeight = 0.0f;
    glm::vec2 bestFreeSlotCenter;
    // Get the normalized vector from the static bubble to the bubble
    glm::vec2 bubbleDirection = glm::normalize(bubble->GetCenter() - staticBubble->GetCenter());
    // If the highest weight is 0, we get the closest static bubbles that are not neighbors of the static bubble, but its distance to the static bubble is less than or equal to 4*kBubbleRadius.
    for (auto& [id, closeBubble] : statics) {
        if (id == staticBubbleId) {
            continue;
        }
        // If it is a neighbor of the static bubble, then we don't consider it.
        if (IsNeighbor(closeBubble->GetCenter(), staticBubble->GetCenter())) {
            continue;
        }
        // If the distance between the two bubbles is greater than 4*kBubbleRadius, then we don't consider it.
        float distance = glm::distance(closeBubble->GetCenter(), staticBubble->GetCenter());
        if (distance > 4 * kBubbleRadius) {
            continue;
        }
        // Get the normalized vector from the static bubble to the close bubble
        glm::vec2 closeBubbleDirection = glm::normalize(closeBubble->GetCenter() - staticBubble->GetCenter());
        // Get the cosine of the angle between the two vectors
        float cosAngle = glm::dot(closeBubbleDirection, bubbleDirection);
        // If the angle is less than 60 degrees or greater than 90 degrees, then we won't consider it.
        if (cosAngle < 0.0f || cosAngle > 0.866f) {
            continue;
        }
        // calculate the angle from the cosAngle using glm::acos
        float rotationAngle = lawOfCosinesAngle(distance, 2 * kBubbleRadius, 2 * kBubbleRadius);
        // Get the free slot center. This free slot should be a circle neighboring both the static bubble and the close bubble.
        glm::vec2 freeSlotCenter;
        int a = getSignOfCrossProduct(closeBubbleDirection, bubbleDirection);
        // if a is 0, then the two vectors are parallel. We don't consider this case.
        if (a == 0) {
            continue;
        }
        else if (a > 0) {
            freeSlotCenter = rotateVector(staticBubble->GetCenter() + 2 * kBubbleRadius * closeBubbleDirection, rotationAngle, staticBubble->GetCenter());
        }
        else {
            freeSlotCenter = rotateVector(staticBubble->GetCenter() + 2 * kBubbleRadius * closeBubbleDirection, -rotationAngle, staticBubble->GetCenter());
        }
        // Get the weight of the free slot
        float weight = GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

        // If the weight is greater than the highest weight, then we update the highest weight and the free slot center.
        if (weight > highestWeight) {
            highestWeight = weight;
            bestFreeSlotCenter = freeSlotCenter;
        }
    }

    // If the highest weight is greater than 0, then we move the bubble to the best free slot center.
    if (highestWeight > 0.0f) {
        bubble->SetPosition(bestFreeSlotCenter - glm::vec2(kBubbleRadius, kBubbleRadius));
        return true;
    }

    // No fine tuning happaned.
    return false;
}

std::vector<int> GameManager::FindConnectedBubblesOfSameColor(int bubbleId) {
    // bubble should be in the statics.
    assert(statics.count(bubbleId) > 0 && "Failed to find the bubble by ID.");
	std::vector<int> connectedBubbleIds;
	// Get the color of the bubble
	glm::vec3 color = statics[bubbleId]->GetColorWithoutAlpha();
	// Use BFS to find all connected bubbles of the same color
	std::queue<int> q;
	q.emplace(bubbleId);
    while (!q.empty()) {
		int currentId = q.front();
		q.pop();
        // If the current id already exists in the connected bubble ids, then we skip it.
        if (std::find(connectedBubbleIds.begin(), connectedBubbleIds.end(), currentId) != connectedBubbleIds.end()) {
            continue;
        }
		connectedBubbleIds.push_back(currentId);
		// Get the neighbors of the current bubble
		std::vector<int> neighborIds = GetNeighborIds(statics[currentId], 0.1f);
        for (auto& neighborId : neighborIds) {
			// If the neighbor has the same color as the bubble and it is not in the connected bubble ids, then we add it to the queue.
            if (isSameColor(color, statics[neighborId]->GetColorWithoutAlpha())) {
				q.emplace(neighborId);
			}
		}
	}
	return connectedBubbleIds;
}

std::vector<int> GameManager::IsCollidingWithStaticBubbles(std::unique_ptr<Bubble>& bubble) {
    std::vector<int> ids;
    for (auto& other : statics) {
        if (areFloatsEqual(glm::distance(bubble->GetCenter(), other.second->GetCenter()), 2 * kBubbleRadius)) {
            ids.push_back(other.first);
		}
	}
	return ids;
}

bool GameManager::IsNeighbor(glm::vec2 bubbleCenter, glm::vec2 slotCenter, float absError) {
    return areFloatsEqual(glm::distance(bubbleCenter, slotCenter), 2 * kBubbleRadius, absError);
}

std::vector<glm::vec2> GameManager::GetCommonFreeSlots(std::vector<std::unique_ptr<Bubble>>& bubbles, std::vector<glm::vec2> candidateFreeSlots) {
	std::vector<glm::vec2> commonFreeSlots;
    for (auto& slot : candidateFreeSlots) {
		bool common = true;
        for (auto& bubble : bubbles) {
            if (!IsNeighbor(bubble->GetCenter(), slot)) {
				common = false;
				break;
			}
		}
        if (common) {
			commonFreeSlots.push_back(slot);
		}
	}
	return commonFreeSlots;
}

std::vector<glm::vec2> GameManager::GetCommonFreeSlots(std::vector<int>& bubbleIds, std::vector<glm::vec2> candidateFreeSlots) {
    std::vector<glm::vec2> commonFreeSlots;
    for (auto& slot : candidateFreeSlots) {
	    bool common = true;
        for (auto& id : bubbleIds) {
            if (!IsNeighbor(statics[id]->GetCenter(), slot)) {
			    common = false;
			    break;
		    }
	    }
        if (common) {
		    commonFreeSlots.push_back(slot);
	    }
	}
	return commonFreeSlots;
}

float GameManager::GetFreeSlotWeight(glm::vec3 color, glm::vec2 slotCenter) {
    // Check if the free slot is outside the boundaries of the game board.
    auto boundaries = gameBoard->GetBoundaries();
    if (areFloatsLess(slotCenter.x - kBubbleRadius, boundaries[0]) || areFloatsGreater(slotCenter.x + kBubbleRadius, boundaries[2]) || areFloatsLess(slotCenter.y - kBubbleRadius, boundaries[1]) || areFloatsGreater(slotCenter.y + kBubbleRadius, boundaries[3])) {
		return -1.0f;
	}

    // First check if the slot is already occupied by a static bubble. That case, the distance between the slot center and the center of the static bubble is less than 2*kBubbleRadius.
    for (auto& other : statics) {
        if (glm::distance(slotCenter, other.second->GetCenter()) < 2 * kBubbleRadius - 1.0f) {
            // return -1.0f to indicate that the slot is not free.
            return -1.0f;
        }
    }

    float weight = 0.0f;
    for (auto& other : statics) {
        if (IsNeighbor(other.second->GetCenter(), slotCenter)) {
            if (isSameColor(other.second->GetColorWithoutAlpha(), color)) {
				weight += 1.0f;
			}
            else {
				weight += 0.1f;
			}
		}
	}
	return weight;
}

std::vector<glm::vec2> GameManager::GetPotentialNeighborFreeSlots(glm::vec2 bubbleCenter) {
    std::vector<glm::vec2> neighborCenters;
    // The angle between two neighbors is 60 degrees.
    float angle = glm::radians(60.0f);
    // The first neighbor is the right neighbor.
    glm::vec2 rightNeighbor = bubbleCenter + glm::vec2(2 * kBubbleRadius, 0.0f);
    neighborCenters.push_back(rightNeighbor);

    // Get the other 5 neighbors
    for (size_t neighborNum = 1; neighborNum < 6; ++neighborNum) {
        glm::vec2 neighborCenter = rotateVector(rightNeighbor, angle * neighborNum, bubbleCenter);
        neighborCenters.push_back(neighborCenter);
    }

    // Remove the neighbors that are outside the boundaries
    auto boundaries = gameBoard->GetBoundaries();
    for (auto it = neighborCenters.begin(); it != neighborCenters.end();) {
        bool lessThenXBegin = areFloatsLess(it->x - kBubbleRadius, boundaries[0]);
        bool greaterThenXEnd = areFloatsGreater(it->x + kBubbleRadius, boundaries[2]);
        bool lessThenYBegin = areFloatsLess(it->y - kBubbleRadius, boundaries[1]);
        bool greaterThenYEnd = areFloatsGreater(it->y + kBubbleRadius, boundaries[3]);
        if (lessThenXBegin || greaterThenXEnd || lessThenYBegin || greaterThenYEnd) {
            it = neighborCenters.erase(it);
        }
        else {
            ++it;
        }
    }

    // Remove the neighbors that are overlapping with the static bubbles
    for (auto it = neighborCenters.begin(); it != neighborCenters.end();) {
		bool overlap = false;
		for (auto& [id, bubble] : statics) {
			if (areFloatsLess(glm::distance(*it, bubble->GetCenter()), 2 * kBubbleRadius)) {
				overlap = true;
				break;
			}
		}
		if (overlap) {
			it = neighborCenters.erase(it);
		}
		else {
			++it;
		}
	}

    return neighborCenters;
}

void GameManager::UpdateFreeSlots(std::unique_ptr<Bubble>& bubble) {
    // Get the center of the bubble
    glm::vec2 bubbleCenter = bubble->GetCenter();
    // Remove the free slots whose distance to the bubble center is less than 2*kBubbleRadius.
    freeSlots.erase(std::remove_if(freeSlots.begin(), freeSlots.end(), [&](glm::vec2 slotCenter) {
        return glm::distance(slotCenter, bubbleCenter) < 2 * kBubbleRadius;
	}), freeSlots.end());

    // Get potential free slots
    std::vector<glm::vec2> potentialFreeSlots = GetPotentialNeighborFreeSlots(bubble->GetCenter());

    // Add the potential free slots to the free slots if they are not in the free slots.
    for (auto& center1 : potentialFreeSlots) {
        bool alreadyInFreeSlots = false;
        for (auto& center2 : freeSlots) {
            if (areFloatsEqual(glm::distance(center1, center2), 0.0f)) {
                alreadyInFreeSlots = true;
				break;
			}
        }
        if (!alreadyInFreeSlots) {
            freeSlots.emplace_back(center1);
        }
    }
}

void GameManager::GenerateRandomStaticBubblesHelper(GameLevel gameLevel) {
    // Randomly get N colors from the color map
    std::vector<int> colorSet;
    for(auto &[colorEnum, colorVal] : colorMap) {
        colorSet.push_back(static_cast<int>(colorEnum));
    }
    std::vector<glm::vec3> colorPool;
    for (int i = 0; i < gameLevel.numColors; ++i) {
        int randomIndex = rand() % colorSet.size();
        colorPool.push_back(colorMap[static_cast<Color>(colorSet[randomIndex])]);
        colorSet.erase(colorSet.begin() + randomIndex);
	}

    glm::vec2 centerForNewBubble(-1.f, -1.f);
    std::vector<int> addedBubbleIds;
    int lastAddedBubbleID = -1;

    //// Record the number of bubbles of each color. The key is the index of the color in the color pool and the value is the number of bubbles of that color.
    //std::unordered_map<int, int> colorCount;
   
    // Generate bubble one by one
    for (int i = 0; i < gameLevel.numInitialBubbles; ++i) {
        bool hasExistingBubble = !statics.empty();
        bool toBeNeighborOfBubble = randomBool(gameLevel.probabilityNewBubbleIsNeighborOfBubble) && hasExistingBubble;
        bool toBeNeighborOfLastAdded = randomBool(gameLevel.probabilityNewBubbleIsNeighborOfLastAdded) && toBeNeighborOfBubble;
        bool toBeNeighborOfBubbleOfSameColor = randomBool(gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor) && toBeNeighborOfBubble;
        //toBeNeighborOfBubble = false;
        //toBeNeighborOfLastAdded = false;
        //toBeNeighborOfBubbleOfSameColor = false;
        if (toBeNeighborOfLastAdded) {
            assert(lastAddedBubbleID != -1 && "Have not added any bubble yet.");
            // Get potential neighbors of the last added bubble
            std::vector<glm::vec2> lastAddedFreeSlots;
            lastAddedFreeSlots = GetPotentialNeighborFreeSlots(statics[lastAddedBubbleID]->GetCenter());
            while (lastAddedFreeSlots.empty()) {
                addedBubbleIds.pop_back();
                assert(!addedBubbleIds.empty() && "There should always be a static bubble that has available neighbor free slots.");
                lastAddedBubbleID = addedBubbleIds.back();
				lastAddedFreeSlots = GetPotentialNeighborFreeSlots(statics[lastAddedBubbleID]->GetCenter());
			}
            if (i == 0) {
                std::string str = ""; (void)str;
            }
            //// Get the common free slots of the last added free slots and the provided free slots.
            //std::vector<glm::vec2> commonFreeSlots = GetCommonFreeSlots(lastAddedFreeSlots, freeSlots);
            // Randomly select a free slot from the common free slots.
            int randomIndex = rand() % lastAddedFreeSlots.size();
            centerForNewBubble = lastAddedFreeSlots[randomIndex];
        }
        else if (toBeNeighborOfBubble) {
            // Randomly select a free slot that is a neighbor of an existing static bubble.
            do{
				int randomIndex = rand() % freeSlots.size();
                centerForNewBubble = freeSlots[randomIndex];
            } while (!IsNeighborOfStaticBubbles(centerForNewBubble));
        }
        else {
			// Randomly select a free slot from the provided free slots.
			int randomIndex = rand() % freeSlots.size();
			centerForNewBubble = freeSlots[randomIndex];
		}

        // Create a new bubble
        std::unique_ptr<Bubble> newBubble = std::make_unique<Bubble>(
			centerForNewBubble - glm::vec2(kBubbleRadius, kBubbleRadius), kBubbleRadius,
			glm::vec2(0.0f, 0.0f),
			glm::vec4(colorPool[rand() % colorPool.size()], 0.8),
			ResourceManager::GetInstance().GetTexture("bubble"));
        
        if (toBeNeighborOfBubbleOfSameColor) {
        	// Get the ids of the bubbles that are neighbors of the new bubble.
			std::vector<int> neighborIds = GetNeighborIds(newBubble);
            assert(!neighborIds.empty() && "Failed to get neighbor ids");
            // Randomly select a neighbor and set the color of the new bubble to the color of the selected neighbor.
            int randomIndex = rand() % neighborIds.size();
            // Get the color of the selected neighbor.
            glm::vec4 color = statics[neighborIds[randomIndex]]->GetColor();
            // Set the color of the new bubble to the color of the selected neighbor.
            newBubble->SetColor(color);
        }
        // Set the last added bubble id to the id of the new bubble.
        lastAddedBubbleID = newBubble->GetID();

        // Add the new bubble to the static bubbles.
        statics[lastAddedBubbleID] = std::move(newBubble);

        // Update the free slots
        UpdateFreeSlots(statics[lastAddedBubbleID]);

        // Memorize the id of the new bubble
        addedBubbleIds.emplace_back(lastAddedBubbleID);

        // Update the color count
        ++colorCount[statics[lastAddedBubbleID]->GetColorEnum()];
    }
}

void GameManager::GenerateRandomStaticBubbles() {
    // Get the boundaries of the game board (left, upper, right, lower)
    glm::vec4 boundaries = glm::vec4(gameBoard->GetPosition().x, gameBoard->GetPosition().y, gameBoard->GetPosition().x + gameBoard->GetSize().x, gameBoard->GetPosition().y + gameBoard->GetSize().y);
    
    // generate random static bubble either on the top or as the neighbor of an existing static bubble.
    freeSlots.clear();

    // First get all the free slots on the top of the game board.
    for (float x = boundaries.x + kBubbleRadius; !areFloatsGreater(x, boundaries.z-kBubbleRadius); x += kBubbleRadius) {
		freeSlots.emplace_back(x, boundaries.y + kBubbleRadius);
	}

    // Use a random number engine with a seed based on the current time
    auto rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

    // Shuffle the free slots
    std::shuffle(freeSlots.begin(), freeSlots.end(), rng);

    GameLevel gameLevel;
    if (level == 1) {
        gameLevel.numColors = 1;
        gameLevel.numInitialBubbles = 10;
        gameLevel.maxInitialBubbleDepth = 22*kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 1.f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 1.f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 1.f;
        GenerateRandomStaticBubblesHelper(gameLevel);
	}
	else if (level == 2) {
        gameLevel.numColors = 3;
        gameLevel.numInitialBubbles = 30;
        gameLevel.maxInitialBubbleDepth = 24 * kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 0.85f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 0.85f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 0.85f;
        GenerateRandomStaticBubblesHelper(gameLevel);
	}
	else if (level == 3) {
        gameLevel.numColors = 5;
        gameLevel.numInitialBubbles = 50;
        gameLevel.maxInitialBubbleDepth = 26 * kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 0.7f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 0.7f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 0.7f;
        GenerateRandomStaticBubblesHelper(gameLevel);
    }
    else if (level == 4) {
        gameLevel.numColors = 7;
        gameLevel.numInitialBubbles = 70;
        gameLevel.maxInitialBubbleDepth = 28 * kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 0.55f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 0.55f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 0.55f;
        GenerateRandomStaticBubblesHelper(gameLevel);
    }
    else if (level == 5) {
        gameLevel.numColors = 9;
        gameLevel.numInitialBubbles = 90;
        gameLevel.maxInitialBubbleDepth = 32 * kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 0.4f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 0.4f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 0.4f;
        GenerateRandomStaticBubblesHelper(gameLevel);
    }
    else if (level == 6) {
        gameLevel.numColors = 10;
        gameLevel.numInitialBubbles = 110;
        gameLevel.maxInitialBubbleDepth = 32 * kBubbleRadius;
        gameLevel.probabilityNewBubbleIsNeighborOfLastAdded = 0.25f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubble = 0.25f;
        gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor = 0.25f;
        GenerateRandomStaticBubblesHelper(gameLevel);
    }
}

float GameManager::GetNarrowingTimeInterval() {
    float baseTime = 6.f;

    float decreasingFactor = 0.9f;

    float time = baseTime * pow(decreasingFactor, level - 1); // level >= 1

    // minimum time interval is 6 seconds
    if (time < 6.f) {
		time = 6.f;
	}
    return time;
}


std::vector<glm::vec2> GameManager::GetCommonFreeSlots(std::vector<glm::vec2> freeSlots1, std::vector<glm::vec2> freeSlots2) {
    std::vector<glm::vec2> commonFreeSlots;
    for (auto& slot1 : freeSlots1) {
        for (auto& slot2 : freeSlots2) {
            if (areFloatsEqual(slot1.x, slot2.x) && areFloatsEqual(slot1.y, slot2.y)) {
				commonFreeSlots.push_back(slot2);
			}
		}
	}
	return commonFreeSlots;
}

bool GameManager::IsNeighborOfStaticBubbles(glm::vec2 freeSlotCenter) {
    for (auto& [id, bubble] : statics) {
        if (IsNeighbor(bubble->GetCenter(), freeSlotCenter)) {
			return true;
		}
	}
	return false;
}

glm::vec4 GameManager::GetNextBubbleColor() {
    // If statics is empty, then we randomly select a color from the color map.
    if (statics.empty()) {
        // Get a random color from Color predefined in the resource manager
        static std::random_device rd;  // Initialize a random device
        static std::mt19937 gen(rd()); // Seed the generator
        static std::uniform_int_distribution<> dis(0, static_cast<int>(colorMap.size()) - 1); // Distribution for enum values

        Color randomColor = static_cast<Color>(dis(gen)); // Get a random color enum value
        // Set the color
        return glm::vec4(colorMap[randomColor], 0.8);
	}
    // If statics is not empty, then we determine the new color based on the count of bubbles of each color.
    else {
        std::vector<Color> colors;
        std::vector<float> weights;
        int sum = 0;
        for (const auto& [color, count] : colorCount) {
            sum += count;
            assert(count >= 0 && "Color count should be non-negative.");
            colors.emplace_back(color);
            weights.emplace_back(static_cast<float>(count));
        }
        // Determine the new color based on the counts of bubbles of each color using random weighted selection.
        int randomIndex = randomWeightedSelect(weights);
        return glm::vec4(colorMap[colors[randomIndex]], 0.8);
	}
}

void GameManager::AdjustButtonsHorizontalPosition(const std::vector<std::string>& buttonName, float interval) {
    if (buttonName.empty()) {
        return;
    }
    assert(buttons.count(buttonName[0]) == 1 && "Button name does not exist.");
    glm::vec2 pos = buttons[buttonName[0]]->GetPosition();
    glm::vec2 size = buttons[buttonName[0]]->GetSize();
    glm::vec2 intervalVec = glm::vec2(interval+size.x, 0);
    glm::vec2 nextPos = pos + intervalVec;
    // Iterate through the rest of buttons and adjust their positions.
    for(size_t i = 1; i < buttonName.size(); ++i) {
		assert(buttons.count(buttonName[i]) == 1 && "Button name does not exist.");
		pos = buttons[buttonName[i]]->GetPosition();
        size = buttons[buttonName[i]]->GetSize();
        if (pos != nextPos) {
            buttons[buttonName[i]]->SetPosition(nextPos);
        }
        pos = nextPos;
        intervalVec = glm::vec2(interval + size.x, 0);
        nextPos = pos + intervalVec;
	}
}

bool GameManager::IsLevelFailed() {
    if (this->state != GameState::ACTIVE || this->targetState != GameState::UNDEFINED) {
		return false;
	}
	// Get the shooter's carried bubble
    const Bubble& carriedBubble = shooter->GetCarriedBubble();
    // Get the radius and center of the carried bubble
    float radius = carriedBubble.GetRadius();
    glm::vec2 center = carriedBubble.GetCenter();
    // If part of the carried bubble is outside the actual upper boundary of the game board, then the current level is failed.
    if (areFloatsLess(center.y - radius, gameBoard->GetValidPosition().y)) {
        return true;
    }
    // If the carried bubble is overlapping with the static bubbles, then the current level is failed.
    for (auto& [id, bubble] : statics) {
        if (glm::distance(center, bubble->GetCenter()) < radius + bubble->GetRadius()) {
			return true;
		}
	}
    return false;
}

std::shared_ptr<OptionUnit> GameManager::CreateClickableOptionUnit(const std::string& name, const std::string& text) {
    //auto liucheIconUnit = std::make_shared<ImageUnit>("liucheiconunit",
    //    /*pos=*/glm::vec2(0.f), /*size=*/gameCharacters.at("liuche")->GetGeneralSize(GameCharacterState::HAPPY) * 0.75f,
    //    /*texture=*/gameCharacters.at("liuche")->GetGeneralTexture(GameCharacterState::HAPPY), spriteRenderer);
    //auto liucheTextUnit = std::make_shared<TextUnit>("liuchetextunit", texts["liucheintro"], textRenderer2);
    //auto liucheIconTextUnit = std::make_shared<OptionUnit>("liucheicontextunit", liucheIconUnit, liucheTextUnit);
    // Create the image unit
    std::shared_ptr<ImageUnit> imageUnit = std::make_shared<ImageUnit>("clickableiconunit",
            /*pos=*/glm::vec2(0.f), /*size=*/kBubbleSize,
            /*texture=*/ResourceManager::GetInstance().GetTexture("bubble"), this->spriteRenderer);
    // Create the text unit
    texts[name] = std::make_shared<Text>(/*pos=*/glm::vec2(0.f), /*lineWidth=*/gameBoard->GetSize().x - kBubbleRadius);
    texts[name]->AddParagraph(text);
    texts[name]->SetScale(0.0018f * imageUnit->GetHeight());
    std::cout << "imageUnit->GetHeight(): " << imageUnit->GetHeight() << std::endl;
    std::shared_ptr<TextUnit> textUnit = std::make_shared<TextUnit>(name, texts.at(name), this->textRenderer2);
    // Create the option unit
    std::shared_ptr<OptionUnit> optionUnit = std::make_shared<OptionUnit>(name, imageUnit, textUnit, OptionState::kNormal);
    optionUnit->SetTextOnCenter(true);
    optionUnit->SetPosition(optionUnit->GetPosition());

    return optionUnit;
}
