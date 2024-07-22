#include "GameManager.h"

#include <cstring>

GameManager::GameManager(unsigned int width, unsigned int height)
    : state(GameState::INITIAL),
      lastState(GameState::UNDEFINED),
      targetState(GameState::UNDEFINED),
      transitionState(TransitionState::START),
      difficulty(ConfigManager::GetInstance().GetDifficulty()),
      screenMode(ConfigManager::GetInstance().GetScreenMode()),
      targetScreenMode(ScreenMode::UNDEFINED),
      language(ConfigManager::GetInstance().GetLanguage()),
      width(static_cast<float>(width)),
      height(static_cast<float>(height)),
      level(1),
      score(ConfigManager::GetInstance().GetScore()),
      hideDefaultMouseCursor(false),
      numOfScoreIncrementsReady(0),
      activePage("") {
  // Initialize all key states to false
  memset(keys, false, sizeof(keys));
  memset(keysLocked, false, sizeof(keysLocked));

  // Set the scroll offset to 0
  scrollYOffset = 0.f;
  scrollSensitivity = 25.0f;

  // Initailze the focus state
  focused = true;
  // Initialize the mouse states
  leftMousePressed = false;
  isReadyToDrag = true;
  isDragging = false;
  gameArenaShaking = false;
}

GameManager::~GameManager() {
  // Clear all resources
  this->ClearResources();
}

void GameManager::PreLoad() {
  this->SetState(GameState::PRELOAD);

  // load logo
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  resourceManager.LoadTexture("textures/oliverliulogo4k.png", false, "logo");
  resourceManager.LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr,
                             "sprite");
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(this->width),
                 static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
  resourceManager.GetShader("sprite").Use().SetInteger("image", 0);
  resourceManager.GetShader("sprite").SetMatrix4("projection", projection);
  spriteRenderer =
      std::make_shared<SpriteRenderer>(resourceManager.GetShader("sprite"));
}

void GameManager::Init() {
  // load shaders
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  resourceManager.LoadShader("shaders/post_processing.vs",
                             "shaders/post_processing.fs", nullptr,
                             "postprocessing");
  resourceManager.LoadShader("shaders/pure_color.vs", "shaders/pure_color.fs",
                             nullptr, "purecolor");
  resourceManager.LoadShader("shaders/ray.vs", "shaders/ray.fs", nullptr,
                             "ray");
  resourceManager.LoadShader("shaders/particle.vs", "shaders/particle.fs",
                             nullptr, "particle");
  resourceManager.LoadShader("shaders/text_2d.vs", "shaders/text_2d.fs",
                             nullptr, "text");
  resourceManager.LoadShader("shaders/discard.vs", "shaders/discard.fs",
                             nullptr, "discard");

  // configure shaders
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(this->width),
                 static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
  if (!resourceManager.HasShader("sprite")) {
    resourceManager.LoadShader("shaders/sprite.vs", "shaders/sprite.fs",
                               nullptr, "sprite");
    resourceManager.GetShader("sprite").Use().SetInteger("image", 0);
    resourceManager.GetShader("sprite").SetMatrix4("projection", projection);
  }
  // resourceManager.GetShader("sprite").Use().SetInteger("image", 0);
  // resourceManager.GetShader("sprite").SetMatrix4("projection", projection);
  resourceManager.GetShader("purecolor")
      .Use()
      .SetMatrix4("projection", projection);
  resourceManager.GetShader("ray").Use().SetMatrix4("projection", projection);
  resourceManager.GetShader("particle").Use().SetInteger("sprite", 0);
  resourceManager.GetShader("particle").SetMatrix4("projection", projection);
  resourceManager.GetShader("discard").Use().SetInteger("image", 0);
  resourceManager.GetShader("discard").SetMatrix4("projection", projection);

  // set render-specific controls
  if (spriteRenderer == nullptr) {
    spriteRenderer =
        std::make_shared<SpriteRenderer>(resourceManager.GetShader("sprite"));
  }
  spriteDynamicRenderer = std::make_shared<SpriteDynamicRenderer>(
      resourceManager.GetShader("sprite"));
  partialTextureRenderer = std::make_shared<PartialTextureRenderer>(
      resourceManager.GetShader("discard"));
  colorRenderer =
      std::make_shared<ColorRenderer>(resourceManager.GetShader("purecolor"));
  circleRenderer = std::make_shared<CircleRenderer>(
      resourceManager.GetShader("purecolor"), 0.5f);
  rayRenderer = std::make_shared<RayRenderer>(resourceManager.GetShader("ray"));
  lineRenderer =
      std::make_shared<LineRenderer>(resourceManager.GetShader("ray"));

  postProcessor = std::make_shared<PostProcessor>(
      resourceManager.GetShader("postprocessing"), this->width, this->height);

  // load textures
  resourceManager.LoadTexture("textures/brush2.png", true, "mouse");
  resourceManager.LoadTexture("textures/splash2.png", false, "splash");
  resourceManager.LoadTexture("textures/handynastry4.png", true, "background");
  resourceManager.LoadTexture("textures/graybubble.png", true, "bubble");
  resourceManager.LoadTexture("textures/ancient_arrow1.png", true, "shooter1");
  resourceManager.LoadTexture("textures/ancient_arrow_h_3.png", true, "arrow3");
  resourceManager.LoadTexture("textures/particle.png", true, "particle");
  resourceManager.LoadTexture("textures/particle1.png", true, "particle1");
  resourceManager.LoadTexture("textures/cracks.png", true, "cracks");
  resourceManager.LoadTexture("textures/liuchesad2.png", true, "liuchesad");
  resourceManager.LoadTexture("textures/liuchehappy3.png", true, "liuchehappy");
  resourceManager.LoadTexture("textures/liucheangry.png", true, "liucheangry");
  resourceManager.LoadTexture("textures/weizifusad3.png", true, "weizifusad");
  resourceManager.LoadTexture("textures/weizifuhappy3.png", true,
                              "weizifuhappy");
  resourceManager.LoadTexture("textures/guojiefight2.png", true, "guojiefight");
  resourceManager.LoadTexture("textures/guojiesad2.png", true, "guojiesad");
  resourceManager.LoadTexture("textures/weiqingfight2.png", true,
                              "weiqingfight");
  resourceManager.LoadTexture("textures/weiqingsad2.png", true, "weiqingsad");
  resourceManager.LoadTexture("textures/weiqinghappy2.png", true,
                              "weiqinghappy");
  resourceManager.LoadTexture("textures/weiqingwin2.png", true, "weiqingwin");
  resourceManager.LoadTexture("textures/scroll_paper1.png", true,
                              "scrollpaper");
  resourceManager.LoadTexture("textures/scroll_upper.png", true, "scrollupper");

  // Create game board
  gameBoard = std::make_unique<GameBoard>(
      glm::vec2(this->width / 3, this->height * 0.09),
      glm::vec2(this->width / 3, this->height * 0.82),
      glm::vec4(GameBoardColorMap[GameBoardColor::ORIGINAL], 0.9f),
      resourceManager.GetTexture("scrollpaper"));

  // Create scroll
  scroll = std::make_unique<Scroll>(
      glm::vec2(this->width / 2.0f, this->height / 2.0f),
      glm::vec2(this->width / 1.8f, this->width / 3 / 11.7772021),
      glm::vec2(this->gameBoard->GetSize().x, 0.f),
      resourceManager.GetTexture("scrollupper"));
  // Set the scroll's y position to be out of the top of the screen
  scroll->SetTargetCenter(
      "outofscreen",
      glm::vec2(this->width / 2.0f, -scroll->GetRollerSize().y / 2.0f));
  scroll->SetCenter(scroll->GetTargetCenter("outofscreen"));

  // Create characters.
  // Liu Che
  std::unordered_map<GameCharacterState, glm::vec2> positions = {
      {GameCharacterState::SAD,
       glm::vec2(this->width * 0.01, this->height * 0.7)},
      {GameCharacterState::HAPPY,
       glm::vec2(this->width * 0.01, this->height * 0.7)},
      {GameCharacterState::ANGRY,
       glm::vec2(this->width * 0.01, this->height * 0.7)}};
  std::unordered_map<GameCharacterState, glm::vec2> sizes = {
      {GameCharacterState::SAD,
       glm::vec2(this->height / 1.6893 / 4.5, this->height / 4.5)},
      {GameCharacterState::HAPPY,
       glm::vec2(this->height / 1.74 / 4.5, this->height / 4.5)},
      {GameCharacterState::ANGRY,
       glm::vec2(this->height / 1.6871 / 4.5, this->height / 4.5)}};
  std::unordered_map<GameCharacterState, Texture2D> textures = {
      {GameCharacterState::SAD, resourceManager.GetTexture("liuchesad")},
      {GameCharacterState::HAPPY, resourceManager.GetTexture("liuchehappy")},
      {GameCharacterState::ANGRY, resourceManager.GetTexture("liucheangry")}};
  gameCharacters["liuche"] = std::make_shared<GameCharacter>(
      "liuche", GameCharacterState::HAPPY, positions, sizes, 0.0f,
      glm::vec2(0.5f, 0.5f), textures);
  gameCharacters["liuche"]->SetFaceDirection(false);

  // Wei Zi Fu
  positions.clear();
  positions = {{GameCharacterState::SAD,
                glm::vec2(this->width * 0.05, this->height * 0.65)},
               {GameCharacterState::HAPPY,
                glm::vec2(this->width * 0.045, this->height * 0.65)}};
  sizes.clear();
  sizes = {{GameCharacterState::SAD,
            glm::vec2(this->height / 2.15196 / 4.2, this->height / 4.2)},
           {GameCharacterState::HAPPY,
            glm::vec2(this->height / 1.98169 / 4.2, this->height / 4.2)}};
  textures.clear();
  textures = {
      {GameCharacterState::SAD, resourceManager.GetTexture("weizifusad")},
      {GameCharacterState::HAPPY, resourceManager.GetTexture("weizifuhappy")}};
  gameCharacters["weizifu"] = std::make_shared<GameCharacter>(
      "weizifu", GameCharacterState::HAPPY, positions, sizes, 0.0f,
      glm::vec2(0.5f, 0.5f), textures);
  gameCharacters["weizifu"]->SetFaceDirection(false);

  // Guo Jie
  positions.clear();
  positions = {{GameCharacterState::FIGHTING,
                glm::vec2(this->width * 0.15, this->height * 0.67)},
               {GameCharacterState::SAD,
                glm::vec2(this->width * 0.155, this->height * 0.665)}};
  sizes.clear();
  sizes = {{GameCharacterState::FIGHTING,
            glm::vec2(this->height / 1.7053 / 4, this->height / 4)},
           {GameCharacterState::SAD,
            glm::vec2(this->height / 2.0662 / 4, this->height / 4)}};
  textures.clear();
  textures = {
      {GameCharacterState::FIGHTING, resourceManager.GetTexture("guojiefight")},
      {GameCharacterState::SAD, resourceManager.GetTexture("guojiesad")}};
  gameCharacters["guojie"] = std::make_shared<GameCharacter>(
      "guojie", GameCharacterState::FIGHTING, positions, sizes, 0.0f,
      glm::vec2(0.5f, 0.5f), textures);
  gameCharacters["guojie"]->SetFaceDirection(false);

  // Set health for characters weiqing and guojie
  glm::vec2 totalHealthBarPos =
      glm::vec2(positions[GameCharacterState::FIGHTING].x - kBaseUnit * 1.0f,
                positions[GameCharacterState::FIGHTING].y - kBaseUnit * 0.9f);
  glm::vec2 totalHealthBarSize = glm::vec2(kBaseUnit * 8.f, kBaseUnit * 0.7f);
  gameCharacters["guojie"]->GetHealth().SetTotalHealth(
      this->GetNumGameLevels());
  gameCharacters["guojie"]->GetHealth().SetTotalHealthBar(
      totalHealthBarPos, totalHealthBarSize, glm::vec4(1.f, 1.f, 1.f, 1.f));
  gameCharacters["guojie"]->GetHealth().SetCurrentHealth(
      this->GetNumGameLevels());
  gameCharacters["guojie"]->GetHealth().SetDamagePopOutDirection(false);
  gameCharacters["guojie"]->SetHealthBarRelativeCenterRatios();
  // Set the rotation pivot for the characters
  gameCharacters["guojie"]->SetRotationPivot(glm::vec2(0.5f, 0.9f));
  // Set the target position "landing"
  gameCharacters["guojie"]->SetTargetPosition(
      "landing", gameCharacters["guojie"]->GetPosition());
  // Set the target position "out of the screen"
  gameCharacters["guojie"]->SetTargetPosition(
      "outofscreen",
      gameCharacters["guojie"]->GetPosition() - glm::vec2(0, 40 * kBaseUnit));

  // Wei Qing
  positions.clear();
  glm::vec2 weiqingfightingPos =
      glm::vec2(this->width * 0.8, this->height * 0.665);
  positions = {{GameCharacterState::FIGHTING, weiqingfightingPos},
               {GameCharacterState::SAD,
                weiqingfightingPos + glm::vec2(this->width * 0.035, 0)},
               {GameCharacterState::HAPPY,
                weiqingfightingPos + glm::vec2(this->width * 0.035, 0)},
               {GameCharacterState::WINNING,
                weiqingfightingPos + glm::vec2(this->width * 0.017, 0)}};
  sizes.clear();
  sizes = {{GameCharacterState::FIGHTING,
            glm::vec2(this->height / 1.222 / 4, this->height / 4)},
           {GameCharacterState::SAD,
            glm::vec2(this->height / 1.6902 / 4, this->height / 4)},
           {GameCharacterState::HAPPY,
            glm::vec2(this->height / 1.757 / 4, this->height / 4)},
           {GameCharacterState::WINNING,
            glm::vec2(this->height / 1.4359 / 4, this->height / 4)}};
  textures.clear();
  textures = {
      {GameCharacterState::FIGHTING,
       resourceManager.GetTexture("weiqingfight")},
      {GameCharacterState::SAD, resourceManager.GetTexture("weiqingsad")},
      {GameCharacterState::HAPPY, resourceManager.GetTexture("weiqinghappy")},
      {GameCharacterState::WINNING, resourceManager.GetTexture("weiqingwin")}};
  gameCharacters["weiqing"] = std::make_shared<GameCharacter>(
      "weiqing", GameCharacterState::HAPPY, positions, sizes, 0.0f,
      glm::vec2(0.5f, 0.5f), textures);

  // Set health for characters weiqing and guojie
  totalHealthBarPos =
      glm::vec2(positions[GameCharacterState::FIGHTING].x + kBaseUnit * 1.0f,
                positions[GameCharacterState::FIGHTING].y - kBaseUnit * 0.9f);
  const float totalHealthOfWeiqing = 3;
  gameCharacters["weiqing"]->GetHealth().SetTotalHealth(totalHealthOfWeiqing);
  gameCharacters["weiqing"]->GetHealth().SetTotalHealthBar(
      totalHealthBarPos, totalHealthBarSize, glm::vec4(1.f, 1.f, 1.f, 1.f));
  gameCharacters["weiqing"]->GetHealth().SetCurrentHealth(totalHealthOfWeiqing);
  gameCharacters["weiqing"]->SetHealthBarRelativeCenterRatios();

  // Set the rotation pivot for the characters
  gameCharacters["weiqing"]->SetRotationPivot(glm::vec2(0.5f, 0.9f));

  // Create shooter
  shooter = std::make_unique<Shooter>(
      glm::vec2(
          this->width / 2 - gameBoard->GetSize().x * 14.f / 135,
          gameBoard->GetPosition().y + gameBoard->GetSize().y * 0.800117578f),
      glm::vec2(gameBoard->GetSize().x * 7.f / 135,
                gameBoard->GetSize().y * 300.f / 1701),
      glm::vec2(0.5f, 0.53f), glm::vec2(0.5f, 0.8f),
      resourceManager.GetTexture("shooter1"));
  shooter->GenerateBubble();
  shooter->SetPosition(glm::vec2(
      this->width / 2 - shooter->GetCarriedBubble().GetRadius(),
      gameBoard->GetPosition().y + gameBoard->GetSize().y * 0.800117578f));

  // Initialize particles
  shadowTrailSystem = std::make_unique<ShadowTrailSystem>(
      resourceManager.GetShader("particle"),
      resourceManager.GetTexture("particle"), 6000);
  explosionSystem = std::make_unique<ExplosionSystem>(
      resourceManager.GetShader("particle"),
      resourceManager.GetTexture("particle1"), 3000);
  // Initialize text renderer and text box
  this->LoadTextRenderer();
  this->LoadCommonCharacters();
  this->LoadTexts();
  this->LoadButtons();

  // Create page "story" / "Main Menu"
  auto textRenderer = textRenderers.at(language);
  auto textUnit =
      std::make_shared<TextUnit>("storytextunit", texts["story"], textRenderer);
  auto startButtonUnit = std::make_shared<ButtonUnit>(
      "startbuttonunit", buttons["start"], textRenderer, colorRenderer);
  auto controlButtonUnit = std::make_shared<ButtonUnit>(
      "controlbuttonunit", buttons["controls"], textRenderer, colorRenderer);
  auto difficultyButtonUnit = std::make_shared<ButtonUnit>(
      "difficultybuttonunit", buttons["difficulty"], textRenderer,
      colorRenderer);
  auto displaySettingsButtonUnit = std::make_shared<ButtonUnit>(
      "displaysettingsbuttonunit", buttons["displaysettings"], textRenderer,
      colorRenderer);
  auto languagePreferenceButtonUnit = std::make_shared<ButtonUnit>(
      "languagepreferencebuttonunit", buttons["languagepreference"],
      textRenderer, colorRenderer);
  auto exitButtonUnit = std::make_shared<ButtonUnit>(
      "exitbuttonunit", buttons["exit"], textRenderer, colorRenderer);

  auto liucheIconUnit = std::make_shared<ImageUnit>(
      "liucheiconunit",
      /*pos=*/glm::vec2(0.f), /*size=*/
      gameCharacters.at("liuche")->GetGeneralSize(GameCharacterState::HAPPY) *
          0.75f,
      /*texture=*/
      gameCharacters.at("liuche")->GetGeneralTexture(GameCharacterState::HAPPY),
      spriteRenderer);
  auto liucheTextUnit = std::make_shared<TextUnit>(
      "liuchetextunit", texts.at("liucheintro"), textRenderer);
  auto liucheIconTextUnit = std::make_shared<OptionUnit>(
      "liucheicontextunit", liucheIconUnit, liucheTextUnit);
  // Get the icon size of liucheiconunit
  glm::vec2 liucheIconSize = liucheIconUnit->GetSize();

  auto weizifuIconUnit = std::make_shared<ImageUnit>(
      "weizifuiconunit",
      /*pos=*/glm::vec2(0.f), /*size=*/
      gameCharacters.at("weizifu")->GetGeneralSize(GameCharacterState::HAPPY) *
          0.75f,
      /*texture=*/
      gameCharacters.at("weizifu")->GetGeneralTexture(
          GameCharacterState::HAPPY),
      spriteRenderer);
  auto weizifuTextUnit = std::make_shared<TextUnit>(
      "weizifutextunit", texts["weizifuintro"], textRenderer);
  auto weizifuIconTextUnit = std::make_shared<OptionUnit>(
      "weizifuicontextunit", weizifuIconUnit, weizifuTextUnit);
  // Set the horizontal spacing between the icon and text in weizifuicontextunit
  float horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() +
                            liucheIconSize.x - weizifuIconUnit->GetSize().x;
  weizifuIconTextUnit->SetHorizontalSpacing(horizontalSpacing);
  const float weiqingIconOffset = -0.75f * kBaseUnit;
  auto weiqingIconUnit = std::make_shared<ImageUnit>(
      "weiqingiconunit",
      /*pos=*/glm::vec2(0.f), /*size=*/
      gameCharacters.at("weiqing")->GetGeneralSize(GameCharacterState::HAPPY) *
          0.75f,
      /*texture=*/
      gameCharacters.at("weiqing")->GetGeneralTexture(
          GameCharacterState::HAPPY),
      spriteRenderer);
  weiqingIconUnit->SetTextureRenderingMode(
      TextureRenderingMode::FlipHorizontally);
  auto weiqingTextUnit = std::make_shared<TextUnit>(
      "weiqingtextunit", texts["weiqingintro"], textRenderer);
  auto weiqingIconTextUnit = std::make_shared<OptionUnit>(
      "weiqingicontextunit", weiqingIconUnit, weiqingTextUnit);
  // Set the horizontal spacing between the icon and text in weiqingicontextunit
  horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() +
                      liucheIconSize.x - weiqingIconUnit->GetSize().x -
                      weiqingIconOffset;
  weiqingIconTextUnit->SetHorizontalSpacing(horizontalSpacing);
  const float guojieIconOffset = -0.3f * kBaseUnit;
  auto guojieIconUnit = std::make_shared<ImageUnit>(
      "guojieiconunit",
      /*pos=*/glm::vec2(0.f), /*size=*/
      gameCharacters.at("guojie")->GetGeneralSize(
          GameCharacterState::FIGHTING) *
          0.75f,
      /*texture=*/
      gameCharacters.at("guojie")->GetGeneralTexture(
          GameCharacterState::FIGHTING),
      spriteRenderer);
  auto guojieTextUnit = std::make_shared<TextUnit>(
      "guojiestextunit", texts["guojieintro"], textRenderer);
  auto guojieIconTextUnit = std::make_shared<OptionUnit>(
      "guojieicontextunit", guojieIconUnit, guojieTextUnit);
  // Set the horizontal spacing between the icon and text in guojieicontextunit
  horizontalSpacing = liucheIconTextUnit->GetHorizontalSpacing() +
                      liucheIconSize.x - guojieIconUnit->GetSize().x -
                      guojieIconOffset;
  guojieIconTextUnit->SetHorizontalSpacing(horizontalSpacing);

  const std::string textSectionBaseName = "textsection";
  const std::string buttonSectionBaseName = "buttonsection";
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
  buttonSection->AddContent(difficultyButtonUnit);
  buttonSection->AddContent(displaySettingsButtonUnit);
  buttonSection->AddContent(languagePreferenceButtonUnit);
  buttonSection->AddContent(exitButtonUnit);
  buttonSection->SetInterUnitSpacing("startbuttonunit", "controlbuttonunit",
                                     0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing("controlbuttonunit",
                                     "difficultybuttonunit", 0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing(
      "difficultybuttonunit", "displaysettingsbuttonunit", 0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing("displaysettingsbuttonunit",
                                     "languagepreferencebuttonunit",
                                     0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing("languagepreferencebuttonunit",
                                     "exitbuttonunit", 0.1f * kBaseUnit);
  pages["story"] = std::make_unique<Page>("story");
  pages["story"]->AddSection(textSection);
  pages["story"]->AddSection(buttonSection);
  // Set the top, bottom and left spacing of the page "story".
  pages["story"]->SetTopSpacing(0.5f * kBaseUnit);
  pages["story"]->SetBottomSpacing(0.5f * kBaseUnit);
  pages["story"]->SetLeftSpacing(0.5f * kBaseUnit);
  // Set the inter spacing between the sections of the page "story".
  const float kCommonInterSectionSpacing = 1.f * kBaseUnit;
  pages["story"]->SetInterSectionSpacing(
      "storytextsection", "storybuttonsection", kCommonInterSectionSpacing);
  glm::vec2 commonButtionPosition = glm::vec2(
      pages["story"]->GetPosition().x + pages["story"]->GetLeftSpacing(),
      this->height * 0.84f);
  glm::vec2 commonButtonSize = glm::vec2(
      gameBoard->GetSize().x -
          2 * (commonButtionPosition.x - pages["story"]->GetPosition().x),
      kBaseUnit * 2.0f);
  for (const auto& buttonName : buttonSection->GetOrder()) {
    std::shared_ptr<ButtonUnit> buttonUnit =
        std::dynamic_pointer_cast<ButtonUnit>(
            buttonSection->GetContent(buttonName));
    buttonUnit->SetPosition(commonButtionPosition);
    buttonUnit->SetSize(commonButtonSize);
    auto button = buttonUnit->GetButton();
    button->SetTextOnCenter(true);
  }
  float interspacingBetweenTextAndButton =
      pages["story"]->GetInterSectionSpacing("storytextsection",
                                             "storybuttonsection");
  float maxHeightForTextSection =
      gameBoard->GetSize().y - buttonSection->GetHeight() -
      pages["story"]->GetBottomSpacing() - pages["story"]->GetTopSpacing() -
      interspacingBetweenTextAndButton;
  textSection->SetInterUnitSpacing("storytextunit", "liucheicontextunit",
                                   3.5f * kBaseUnit);
  textSection->SetInterUnitSpacing("liucheicontextunit", "weizifuicontextunit",
                                   2.f * kBaseUnit);
  textSection->SetInterUnitSpacing("weizifuicontextunit", "weiqingicontextunit",
                                   2.f * kBaseUnit);
  textSection->SetInterUnitSpacing("weiqingicontextunit", "guojieicontextunit",
                                   2.f * kBaseUnit);
  textSection->SetUnitHorizontalOffset("weiqingicontextunit",
                                       weiqingIconOffset);
  textSection->SetUnitHorizontalOffset("guojieicontextunit", guojieIconOffset);
  textSection->SetMaxHeight(maxHeightForTextSection);
  textSection->SetMaxWidth(gameBoard->GetSize().x -
                           pages["story"]->GetLeftSpacing() -
                           0.5 * kBubbleRadius);
  pages["story"]->SetPosition(glm::vec2(
      this->gameBoard->GetPosition().x,
      std::max(
          this->gameBoard->GetCenter().y - pages["story"]->GetHeight() * 0.5f,
          this->gameBoard->GetPosition().y)));
  if (textSection->NeedScrollIcon()) {
    // Adjust the line width of text content
    textSection->SetMaxWidth(textSection->GetMaxWidth() -
                             PageSection::GetScrollIconWidth());
    // Create scroll icon
    textSection->InitScrollIcon(colorRenderer, circleRenderer, lineRenderer,
                                this->gameBoard->GetPosition().x +
                                    this->gameBoard->GetSize().x -
                                    Scroll::GetSilkEdgeWidth() -
                                    0.5f * PageSection::GetScrollIconWidth());
  }

  pages.at("story")->UpdatePosition();
  // Create page "controls"
  textUnit = std::make_shared<TextUnit>("controltextunit", texts["controls"],
                                        textRenderer);
  /*auto startButtonUnit = std::make_shared<ButtonUnit>("startbuttonunit",
   * buttons["controlstart"], textRenderer, colorRenderer);*/
  auto backButtonUnit = std::make_shared<ButtonUnit>(
      "backbuttonunit", buttons["back"], textRenderer, colorRenderer);
  auto resumeButtonUnit = std::make_shared<ButtonUnit>(
      "resumebuttonunit", buttons["resume"], textRenderer, colorRenderer);
  auto restartButtonUnit = std::make_shared<ButtonUnit>(
      "restartbuttonunit", buttons["restart"], textRenderer, colorRenderer);
  auto stopButtonUnit = std::make_shared<ButtonUnit>(
      "stopbuttonunit", buttons["stop"], textRenderer, colorRenderer);
  /*auto exitButtonUnit = std::make_shared<ButtonUnit>("exitbuttonunit",
   * buttons["controlexit"], textRenderer, colorRenderer);*/
  textSection = std::make_shared<PageSection>("controltextsection");
  textSection->AddContent(textUnit);

  buttonSection = std::make_shared<PageSection>("controlbuttonsection");
  // Content to be added to the button section when the game is not started
  buttonSection->AddContent(backButtonUnit);
  // Content to be added to the button section when the game is started
  buttonSection->AddContent(resumeButtonUnit);
  buttonSection->AddContent(restartButtonUnit);
  buttonSection->AddContent(stopButtonUnit);
  buttonSection->SetInterUnitSpacing("resumebuttonunit", "restartbuttonunit",
                                     0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing("restartbuttonunit", "stopbuttonunit",
                                     0.1f * kBaseUnit);
  buttonSection->SetInterUnitSpacing("stopbuttonunit", "backbuttonunit",
                                     0.1f * kBaseUnit);
  buttonSection->SetOrder({"resumebuttonunit", "restartbuttonunit",
                           "stopbuttonunit", "backbuttonunit"});
  pages["controls"] = std::make_unique<Page>("controls");
  pages["controls"]->AddSection(textSection);
  pages["controls"]->AddSection(buttonSection);
  // Set the top, bottom and left spacing of the page "controls".
  pages["controls"]->SetTopSpacing(0.5f * kBaseUnit);
  pages["controls"]->SetBottomSpacing(0.5f * kBaseUnit);
  pages["controls"]->SetLeftSpacing(0.5f * kBaseUnit);
  // Set the inter spacing between the sections of the page "controls".
  pages["controls"]->SetInterSectionSpacing(
      "controltextsection", "controlbuttonsection", kCommonInterSectionSpacing);

  commonButtionPosition = glm::vec2(
      pages["controls"]->GetPosition().x + pages["controls"]->GetLeftSpacing(),
      this->height * 0.84f);
  commonButtonSize = glm::vec2(
      gameBoard->GetSize().x -
          2 * (commonButtionPosition.x - pages["controls"]->GetPosition().x),
      kBaseUnit * 2.0f);
  for (const auto& buttonName : buttonSection->GetOrder()) {
    std::shared_ptr<ButtonUnit> buttonUnit =
        std::dynamic_pointer_cast<ButtonUnit>(
            buttonSection->GetContent(buttonName));
    buttonUnit->SetPosition(commonButtionPosition);
    buttonUnit->SetSize(commonButtonSize);
    auto button = buttonUnit->GetButton();
    button->SetTextOnCenter(true);
  }
  // Contorls page only has "back" button when the game is not started
  buttonSection->SetOrder({"backbuttonunit"});

  interspacingBetweenTextAndButton = pages["controls"]->GetInterSectionSpacing(
      "controltextsection", "controlbuttonsection");
  maxHeightForTextSection =
      gameBoard->GetSize().y - buttonSection->GetHeight() -
      pages["controls"]->GetBottomSpacing() -
      pages["controls"]->GetTopSpacing() - interspacingBetweenTextAndButton;
  textSection->SetMaxHeight(maxHeightForTextSection);
  textSection->SetMaxWidth(gameBoard->GetSize().x -
                           pages["controls"]->GetLeftSpacing() -
                           0.5 * kBaseUnit);
  pages["controls"]->SetPosition(
      glm::vec2(this->gameBoard->GetPosition().x,
                std::max(this->gameBoard->GetCenter().y -
                             pages["controls"]->GetHeight() * 0.5f,
                         this->gameBoard->GetPosition().y)));
  if (textSection->NeedScrollIcon()) {
    // Adjust the line width of text content
    textSection->SetMaxWidth(textSection->GetMaxWidth() -
                             PageSection::GetScrollIconWidth());
    // Create scroll icon
    textSection->InitScrollIcon(colorRenderer, circleRenderer, lineRenderer,
                                this->gameBoard->GetPosition().x +
                                    this->gameBoard->GetSize().x -
                                    Scroll::GetSilkEdgeWidth() -
                                    0.5f * PageSection::GetScrollIconWidth());
  }

  // Create page "difficulty"
  auto easyMode = CreateClickableOptionUnit(
      "easy", resourceManager.GetText("difficulty", "easy"));
  auto mediumMode = CreateClickableOptionUnit(
      "medium", resourceManager.GetText("difficulty", "medium"));
  auto hardMode = CreateClickableOptionUnit(
      "hard", resourceManager.GetText("difficulty", "hard"));
  auto expertMode = CreateClickableOptionUnit(
      "expert", resourceManager.GetText("difficulty", "expert"));
  switch (this->GetDifficulty()) {
    case Difficulty::EASY:
      easyMode->SetState(OptionState::kClicked);
      break;
    case Difficulty::MEDIUM:
      mediumMode->SetState(OptionState::kClicked);
      break;
    case Difficulty::HARD:
      hardMode->SetState(OptionState::kClicked);
      break;
    case Difficulty::EXPERT:
      expertMode->SetState(OptionState::kClicked);
      break;
    default:
      break;
  }
  textSection = std::make_shared<PageSection>("difficultytextsection");
  buttonSection = std::make_shared<PageSection>("difficultybuttonsection");
  textSection->AddContent(easyMode);
  textSection->AddContent(mediumMode);
  textSection->AddContent(hardMode);
  textSection->AddContent(expertMode);
  textSection->SetInterUnitSpacing("easy", "medium", 0.5f * kBaseUnit);
  textSection->SetInterUnitSpacing("medium", "hard", 0.5f * kBaseUnit);
  textSection->SetInterUnitSpacing("hard", "expert", 0.5f * kBaseUnit);
  buttonSection->AddContent(backButtonUnit);
  pages["difficulty"] = std::make_unique<Page>("difficulty");
  pages["difficulty"]->AddSection(textSection);
  pages["difficulty"]->AddSection(buttonSection);
  // Set the top, bottom and left spacing of the page "difficulty".
  pages["difficulty"]->SetTopSpacing(0.5f * kBaseUnit);
  pages["difficulty"]->SetBottomSpacing(0.5f * kBaseUnit);
  pages["difficulty"]->SetLeftSpacing(0.5f * kBaseUnit);
  // Set the inter spacing between the sections of the page "difficulty".
  pages["difficulty"]->SetInterSectionSpacing("difficultytextsection",
                                              "difficultybuttonsection",
                                              kCommonInterSectionSpacing);
  interspacingBetweenTextAndButton =
      pages["difficulty"]->GetInterSectionSpacing("difficultytextsection",
                                                  "difficultybuttonsection");
  maxHeightForTextSection =
      gameBoard->GetSize().y - buttonSection->GetHeight() -
      pages["difficulty"]->GetBottomSpacing() -
      pages["difficulty"]->GetTopSpacing() - interspacingBetweenTextAndButton;
  textSection->SetMaxHeight(maxHeightForTextSection);
  textSection->SetMaxWidth(gameBoard->GetSize().x -
                           pages["difficulty"]->GetLeftSpacing() -
                           0.5 * kBaseUnit);
  pages["difficulty"]->SetPosition(
      glm::vec2(this->gameBoard->GetPosition().x,
                std::max(this->gameBoard->GetCenter().y -
                             pages["difficulty"]->GetHeight() * 0.5f,
                         this->gameBoard->GetPosition().y)));

  // Create page "Display Settings"
  auto displaysetting1 = CreateClickableOptionUnit(
      "fullscreen", resourceManager.GetText("screenmode", "fullscreen"));
  auto displaysetting2 = CreateClickableOptionUnit(
      "windowedborderless",
      resourceManager.GetText("screenmode", "windowedborderless"));
  auto displaysetting3 = CreateClickableOptionUnit(
      "windowed", resourceManager.GetText("screenmode", "windowed"));
  switch (this->GetScreenMode()) {
    case ScreenMode::FULLSCREEN:
      displaysetting1->SetState(OptionState::kClicked);
      break;
    case ScreenMode::WINDOWED_BORDERLESS:
      displaysetting2->SetState(OptionState::kClicked);
      break;
    case ScreenMode::WINDOWED:
      displaysetting3->SetState(OptionState::kClicked);
      break;
    default:
      break;
  }
  textSection = std::make_shared<PageSection>("displaysettingtextsection");
  buttonSection = std::make_shared<PageSection>("displaysettingbuttonsection");
  textSection->AddContent(displaysetting1);
  textSection->AddContent(displaysetting2);
  textSection->AddContent(displaysetting3);
  textSection->SetInterUnitSpacing("fullscreen", "windowedborderless",
                                   0.5f * kBaseUnit);
  textSection->SetInterUnitSpacing("windowedborderless", "windowed",
                                   0.5f * kBaseUnit);
  buttonSection->AddContent(backButtonUnit);
  pages["displaysettings"] = std::make_unique<Page>("displaysettings");
  pages["displaysettings"]->AddSection(textSection);
  pages["displaysettings"]->AddSection(buttonSection);
  // Set the top, bottom and left spacing of the page "Display Settings".
  pages["displaysettings"]->SetTopSpacing(0.5f * kBaseUnit);
  pages["displaysettings"]->SetBottomSpacing(0.5f * kBaseUnit);
  pages["displaysettings"]->SetLeftSpacing(0.5f * kBaseUnit);
  // Set the inter spacing between the sections of the page "Display Settings".
  pages["displaysettings"]->SetInterSectionSpacing(
      "displaysettingtextsection", "displaysettingbuttonsection",
      kCommonInterSectionSpacing);
  interspacingBetweenTextAndButton =
      pages["displaysettings"]->GetInterSectionSpacing(
          "displaysettingtextsection", "displaysettingbuttonsection");
  maxHeightForTextSection = gameBoard->GetSize().y -
                            buttonSection->GetHeight() -
                            pages["displaysettings"]->GetBottomSpacing() -
                            pages["displaysettings"]->GetTopSpacing() -
                            interspacingBetweenTextAndButton;
  textSection->SetMaxHeight(maxHeightForTextSection);
  textSection->SetMaxWidth(gameBoard->GetSize().x -
                           pages["displaysettings"]->GetLeftSpacing() -
                           0.5 * kBaseUnit);
  pages["displaysettings"]->SetPosition(
      glm::vec2(this->gameBoard->GetPosition().x,
                std::max(this->gameBoard->GetCenter().y -
                             pages["displaysettings"]->GetHeight() * 0.5f,
                         this->gameBoard->GetPosition().y)));

  // Create page "Language Preference"
  std::unordered_map<Language, std::shared_ptr<OptionUnit>> languagePreferences;
  languagePreferences[Language::GERMAN] =
      CreateClickableOptionUnit(language_map[Language::GERMAN], U"Deutsch",
                                textRenderers.at(Language::GERMAN));
  languagePreferences[Language::ENGLISH] =
      CreateClickableOptionUnit(language_map[Language::ENGLISH], U"English",
                                textRenderers.at(Language::ENGLISH));
  languagePreferences[Language::SPANISH] =
      CreateClickableOptionUnit(language_map[Language::SPANISH], U"Español",
                                textRenderers.at(Language::SPANISH));
  languagePreferences[Language::ITALIAN] =
      CreateClickableOptionUnit(language_map[Language::ITALIAN], U"Italiano",
                                textRenderers.at(Language::ITALIAN));
  languagePreferences[Language::FRENCH] =
      CreateClickableOptionUnit(language_map[Language::FRENCH], U"Français",
                                textRenderers.at(Language::FRENCH));
  languagePreferences[Language::JAPANESE] =
      CreateClickableOptionUnit(language_map[Language::JAPANESE], U"日本語",
                                textRenderers.at(Language::JAPANESE));
  languagePreferences[Language::KOREAN] =
      CreateClickableOptionUnit(language_map[Language::KOREAN], U"한국어",
                                textRenderers.at(Language::KOREAN));
  languagePreferences[Language::RUSSIAN] =
      CreateClickableOptionUnit(language_map[Language::RUSSIAN], U"Русский",
                                textRenderers.at(Language::RUSSIAN));
  languagePreferences[Language::PORTUGUESE_BR] = CreateClickableOptionUnit(
      language_map[Language::PORTUGUESE_BR], U"Português (Brasil)",
      textRenderers.at(Language::PORTUGUESE_BR));
  languagePreferences[Language::PORTUGUESE_PT] = CreateClickableOptionUnit(
      language_map[Language::PORTUGUESE_PT], U"Português (Portugal)",
      textRenderers.at(Language::PORTUGUESE_PT));
  languagePreferences[Language::CHINESE_SIMPLIFIED] = CreateClickableOptionUnit(
      language_map[Language::CHINESE_SIMPLIFIED], U"中文（简体）",
      textRenderers.at(Language::CHINESE_SIMPLIFIED));
  languagePreferences[Language::CHINESE_TRADITIONAL] =
      CreateClickableOptionUnit(
          language_map[Language::CHINESE_TRADITIONAL], U"中文（繁體）",
          textRenderers.at(Language::CHINESE_TRADITIONAL));
  languagePreferences.at(ConfigManager::GetInstance().GetLanguage())
      ->SetState(OptionState::kClicked);

  const std::string languagePreferencePageName = "languagepreference";
  textSection = std::make_shared<PageSection>(languagePreferencePageName +
                                              textSectionBaseName);
  buttonSection = std::make_shared<PageSection>(languagePreferencePageName +
                                                buttonSectionBaseName);
  std::vector<Language> languagePreferenceOrder = {
      Language::GERMAN,
      Language::ENGLISH,
      Language::SPANISH,
      Language::ITALIAN,
      Language::FRENCH,
      Language::JAPANESE,
      Language::KOREAN,
      Language::RUSSIAN,
      Language::PORTUGUESE_BR,
      Language::PORTUGUESE_PT,
      Language::CHINESE_SIMPLIFIED,
      Language::CHINESE_TRADITIONAL};
  for (size_t i = 0; i < languagePreferenceOrder.size(); ++i) {
    textSection->AddContent(languagePreferences.at(languagePreferenceOrder[i]));
    if (i > 0) {
      textSection->SetInterUnitSpacing(
          language_map[languagePreferenceOrder[i - 1]],
          language_map[languagePreferenceOrder[i]], 0.5f * kBaseUnit);
    }
  }

  buttonSection->AddContent(backButtonUnit);
  pages[languagePreferencePageName] =
      std::make_unique<Page>(languagePreferencePageName);
  pages[languagePreferencePageName]->AddSection(textSection);
  pages[languagePreferencePageName]->AddSection(buttonSection);
  // Set the top, bottom and left spacing of the page "Display Settings".
  pages[languagePreferencePageName]->SetTopSpacing(0.5f * kBaseUnit);
  pages[languagePreferencePageName]->SetBottomSpacing(0.5f * kBaseUnit);
  pages[languagePreferencePageName]->SetLeftSpacing(0.5f * kBaseUnit);
  // Set the inter spacing between the sections of the page "Display Settings".
  pages[languagePreferencePageName]->SetInterSectionSpacing(
      languagePreferencePageName + textSectionBaseName,
      languagePreferencePageName + buttonSectionBaseName,
      kCommonInterSectionSpacing);
  interspacingBetweenTextAndButton =
      pages[languagePreferencePageName]->GetInterSectionSpacing(
          languagePreferencePageName + textSectionBaseName,
          languagePreferencePageName + buttonSectionBaseName);
  maxHeightForTextSection =
      gameBoard->GetSize().y - buttonSection->GetHeight() -
      pages[languagePreferencePageName]->GetBottomSpacing() -
      pages[languagePreferencePageName]->GetTopSpacing() -
      interspacingBetweenTextAndButton;
  textSection->SetMaxHeight(maxHeightForTextSection);
  textSection->SetMaxWidth(gameBoard->GetSize().x -
                           pages[languagePreferencePageName]->GetLeftSpacing() -
                           0.5 * kBaseUnit);
  pages[languagePreferencePageName]->SetPosition(glm::vec2(
      this->gameBoard->GetPosition().x,
      std::max(this->gameBoard->GetCenter().y -
                   pages[languagePreferencePageName]->GetHeight() * 0.5f,
               this->gameBoard->GetPosition().y)));

  // Initialize the timer
  timer = std::make_shared<Timer>();

  // Set the target silk length for closing and opening the scroll
  scroll->SetTargetSilkLenForClosing(0.f);
  scroll->SetTargetSilkLenForOpening(gameBoard->GetSize().y);
  scroll->SetTargetSilkLenForNarrowing(scroll->GetTargetSilkLenForOpening());
  scroll->SetTargetSilkLenForNarrowing(scroll->GetTargetSilkLenForNarrowing());
  // Set the narrowing, closing, and opening velocity of the scroll
  scroll->SetVelocityForNarrowing(kBaseUnit);
  scroll->SetVelocityForClosing(40 * kBaseUnit);
  scroll->SetVelocityForOpening(54 * kBaseUnit);
  // Set the target position for scroll retracting, deploying, and attacking
  scroll->SetTargetPositionForRetracting(
      glm::vec2(this->width * 0.16f, this->height * 0.83f));
  scroll->SetTargetPositionForDeploying(
      glm::vec2(this->width / 2.0f, this->height / 2.0f));
  glm::vec2 targetPositionForAttacking =
      weiqingfightingPos + glm::vec2(this->width * 0.045, this->height * 0.065);
  scroll->SetTargetPositionForAttacking(targetPositionForAttacking);

  // Go to the splash screen state
  this->GoToState(GameState::SPLASH_SCREEN);
}

void GameManager::LoadSounds() {
  // Add sound resources
  SoundEngine& soundEngine = SoundEngine::GetInstance();
  // Load splash screen sound
  soundEngine.LoadSound("white_noise", "audio/splash/white_noise2.wav", 0.7f);
  soundEngine.LoadSound("shock_wave", "audio/splash/shock_wave1.wav");
  soundEngine.LoadSound("splash_end", "audio/splash/splash_end.wav", 0.35f);
  // Loas key typing sound
  soundEngine.LoadSound("key_s", "audio/keypressed/key_s.wav");
  soundEngine.LoadSound("keys_s_j", "audio/keypressed/keys_s_j.wav", 0.5f);
  soundEngine.LoadSound("key_enter", "audio/keypressed/key_enter.wav");
  soundEngine.LoadSound("key_space", "audio/keypressed/key_space.wav", 0.15f);

  // Game Play sound
  soundEngine.LoadSound("wood_collide", "audio/gameplay/wood_collide.wav",
                        0.8f);
  soundEngine.LoadSound("earthquake", "audio/gameplay/earthquake.wav");
  soundEngine.LoadSound("bubble_pop", "audio/gameplay/bubble_pop.wav", 0.4f);
  soundEngine.LoadSound("bubble_explode", "audio/gameplay/bubble_explode5.wav",
                        0.8f);
  soundEngine.LoadSound("arrow_shoot", "audio/gameplay/bubble_shoot.wav");
  soundEngine.LoadSound("arrow_hit", "audio/gameplay/arrow_hit1.wav");
  soundEngine.LoadSound("scroll_open", "audio/gameplay/scroll_open3.wav");
  soundEngine.LoadSound("scroll_close", "audio/gameplay/scroll_close3.wav");
  soundEngine.LoadSound("scroll_closed", "audio/gameplay/scroll_closed1.wav",
                        0.7f);
  soundEngine.LoadSound("scroll_hit", "audio/gameplay/scroll_hit2.wav", 0.8f);
  soundEngine.LoadSound("scroll_vibrate", "audio/gameplay/scroll_vibrate5.wav");
  soundEngine.LoadSound("scroll_out_sleeve", "audio/gameplay/wood_swing.wav",
                        0.7f);
  soundEngine.LoadSound("flip_paper", "audio/gameplay/flip_paper2.wav");
  soundEngine.LoadSound("drop", "audio/gameplay/drop5.wav");

  // Victory sound
  soundEngine.LoadSound("victory", "audio/background/victory2.wav", 0.7f);
  // Defeated sound
  soundEngine.LoadSound("defeated", "audio/background/defeated.wav", 1.f);

  // Interaction
  soundEngine.LoadSound("button_click", "audio/interaction/button_click1.wav");
}

void GameManager::LoadStreams() {
  auto& soundEngine = SoundEngine::GetInstance();
  // background music
  soundEngine.LoadStream("background_relax0",
                         "audio/background/background_music_relaxing.wav",
                         0.3f);
  soundEngine.LoadStream("background_relax1",
                         "audio/background/background_music_relaxing1.wav",
                         0.3f);
  soundEngine.SetBackgroundMusicNames(
      {"background_relax0", "background_relax1"},
      /*isFighting=*/false);
  soundEngine.LoadStream("background_fight0",
                         "audio/background/background_music_fighting0.wav",
                         0.2f);
  soundEngine.LoadStream("background_fight1",
                         "audio/background/background_music_fighting1.wav",
                         0.2f);
  soundEngine.LoadStream("background_fight2",
                         "audio/background/background_music_fighting2.wav",
                         0.2f);
  soundEngine.SetBackgroundMusicNames(
      {"background_fight0", "background_fight1", "background_fight2"},
      /*isFighting=*/true);
}

void GameManager::Reload() {
  // Memorize the current state
  glm::vec2 scrollCenter = this->scroll->GetCenter();
  auto gameBoardState = this->gameBoard->GetState();
  auto screenMode = this->GetScreenMode();
  bool isScollIconAllowed =
      pages.at("story")->GetSection("storytextsection")->IsScrollIconAllowed();
  float storyPageTextOffset =
      pages.at("story")->GetSection("storytextsection")->GetOffset();
  // First clear all resources
  this->ClearResources();
  // Re-load all resources
  this->Init();
  // Set gameobjects to the correct state
  scroll->SetCenter(scrollCenter);
  gameBoard->SetState(gameBoardState);
  this->SetScreenMode(screenMode);
  pages.at("story")
      ->GetSection("storytextsection")
      ->SetOffset(storyPageTextOffset);
  pages.at("story")
      ->GetSection("storytextsection")
      ->SetScrollIconAllowed(isScollIconAllowed);
  pages.at("story")->GetSection("storytextsection")->ResetSrcollIconPosition();

  // Set to display mode setting page.
  this->SetState(GameState::DISPLAY_SETTINGS);
  this->lastState = GameState::STORY;
  this->targetState = GameState::UNDEFINED;
  this->scroll->SetSilkLen(std::min(pages.at(this->activePage)->GetHeight(),
                                    gameBoard->GetSize().y));
  this->scroll->SetState(ScrollState::OPENED);
}

void GameManager::ProcessInput(float dt) {
  // If 'W' is pressed, then we toggle the windowed mode.
  if (this->keys[GLFW_KEY_W] && this->keysLocked[GLFW_KEY_W] == false) {
    this->keysLocked[GLFW_KEY_W] = true;
    if (this->GetScreenMode() != ScreenMode::WINDOWED) {
      this->GoToScreenMode(ScreenMode::WINDOWED);
    }
  }

  if (this->state == GameState::ACTIVE) {
    // move shooter when not failed
    if (this->keys[GLFW_KEY_LEFT]) {
      // if the key 'ctrl' is hold, then we rotate the shooter in a smaller
      // angle.
      if (this->keys[GLFW_KEY_LEFT_CONTROL]) {
        shooter->SetRoll(shooter->GetRoll() - 0.001f);
      } else {
        shooter->SetRoll(shooter->GetRoll() - 0.008f);
      }
      shooter->GetRay().UpdatePath(this->gameBoard->GetValidBoundaries(),
                                   this->statics);
    } else if (this->keys[GLFW_KEY_RIGHT]) {
      // if the key 'ctrl' is hold, then we rotate the shooter in a smaller
      // angle.
      if (this->keys[GLFW_KEY_LEFT_CONTROL]) {
        shooter->SetRoll(shooter->GetRoll() + 0.001f);
      } else {
        shooter->SetRoll(shooter->GetRoll() + 0.008f);
      }
      shooter->GetRay().UpdatePath(this->gameBoard->GetValidBoundaries(),
                                   this->statics);
    } else if (this->keys[GLFW_KEY_Q] &&
               this->keysLocked[GLFW_KEY_Q] == false) {
      this->keysLocked[GLFW_KEY_Q] = true;
      if (this->targetState == GameState::UNDEFINED) {
        // if Q is pressed, then we go to the state 'Control' of the game.
        this->GoToState(GameState::CONTROL);
        // Get the buttion section of the page "controls"
        auto buttonSection =
            pages.at("controls")->GetSection("controlbuttonsection");
        // Set the order of the buttons in the button section of the page
        auto oldButtonSectionHeight = buttonSection->GetHeight();
        buttonSection->SetOrder(
            {"resumebuttonunit", "restartbuttonunit", "stopbuttonunit"});
        auto newButtonSectionHeight = buttonSection->GetHeight();
        auto textSection =
            pages.at("controls")->GetSection("controltextsection");
        textSection->SetMaxHeight(textSection->GetMaxHeight() +
                                  oldButtonSectionHeight -
                                  newButtonSectionHeight);

        pages.at("controls")
            ->SetPosition(
                glm::vec2(this->gameBoard->GetPosition().x,
                          std::max(this->gameBoard->GetCenter().y -
                                       pages.at("controls")->GetHeight() * 0.5f,
                                   this->gameBoard->GetPosition().y)));
        if (textSection->NeedScrollIcon()) {
          // Adjust the line width of text content
          textSection->SetMaxWidth(textSection->GetMaxWidth() -
                                   PageSection::GetScrollIconWidth());
          // Create scroll icon
          textSection->InitScrollIcon(
              colorRenderer, circleRenderer, lineRenderer,
              this->gameBoard->GetPosition().x + this->gameBoard->GetSize().x -
                  Scroll::GetSilkEdgeWidth() -
                  0.5f * PageSection::GetScrollIconWidth());
        }
        // Set scroll state to be CLOSING
        this->scroll->SetState(ScrollState::CLOSING);
        // pause timer of related events if it is not paused yet.
        if (!this->timer->IsPaused("beforenarrowing")) {
          this->timer->PauseEventTimer("beforenarrowing");
        }
        // pause counting play time
        this->timer->PauseEventTimer("playtime");
      }
    }

    if (this->keys[GLFW_KEY_SPACE] &&
        this->keysLocked[GLFW_KEY_SPACE] == false) {
      this->keysLocked[GLFW_KEY_SPACE] = true;
      if (this->targetState == GameState::UNDEFINED) {
        std::unique_ptr<Bubble> bubble =
            shooter->ShootBubble(GetNextBubbleColor());
        moves.emplace(bubble->GetID(), std::move(bubble));
      }
      //// update the gameboard color based on the color of the ray.
      // gameBoard->UpdateColor(shooter->GetRay().GetColorWithoutAlpha());
    }
  } else if (this->state == GameState::STORY) {
  } else if (this->state == GameState::CONTROL) {
  } else if (this->state == GameState::WIN || this->state == GameState::LOSE) {
    if (this->keys[GLFW_KEY_ENTER] &&
        this->keysLocked[GLFW_KEY_ENTER] == false) {
      this->keysLocked[GLFW_KEY_ENTER] = true;
      // postProcessor->SetBlur(false);
      postProcessor->SetGrayscale(false);
      // gradually fading out the blur effect
      timer->SetEventTimer("deblurring", 2.f);
      // reset the state of each game character
      this->ResetGameCharacters();

      // Delete all the arrows
      arrows.clear();

      // Delete all the static bubbles.
      statics.clear();

      //// Reset score
      // this->ResetScore(ConfigManager::GetInstance().GetScore());

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

  if (!activePage.empty()) {
    // Move the text content of the page "controls" based on the scroll offset.
    if (this->scrollYOffset != 0.f) {
      // Iterate over the sections of the page "controls"
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
        // assume that only one section is scrolled at a time, so we break the
        // loop.
        break;
      }
      // reset the scroll offset
      this->scrollYOffset = 0.f;
    } else if (this->leftMousePressed) {
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
        bool mouseIsCustomBox = mousePosition.x > sectionboundingBox.x &&
                                mousePosition.x < sectionboundingBox.z &&
                                mousePosition.y > sectionboundingBox.y &&
                                mousePosition.y < sectionboundingBox.w;
        if (mouseIsCustomBox && (icon.IsPositionInside(mousePosition) ||
                                 isReadyToDrag && isDragging)) {
          if (isReadyToDrag) {
            isDragging = true;
            // move the story text scroll based on the mouse cursor postion
            // offset.
            float mouseYOffset = this->mouseY - this->mouseLastY;
            section->MoveScrollIcon(mouseYOffset);
            this->mouseLastX = this->mouseX;
            this->mouseLastY = this->mouseY;
          }
        } else {
          isReadyToDrag = false;
          isDragging = false;
          // If the mouse cursor is not in the scroll, we reset the mouse last
          // position to the current position.
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
              // Play the sound of clicking the button
              SoundEngine::GetInstance().PlaySound("button_click");
              if (content == "controls") {
                this->GoToState(GameState::CONTROL);
                // Reset the layout of the control page if needed.
                auto buttonSection =
                    pages.at("controls")->GetSection("controlbuttonsection");
                auto oldButtonSectionHeight = buttonSection->GetHeight();
                buttonSection->SetOrder({"backbuttonunit"});
                auto newButtonSectionHeight = buttonSection->GetHeight();
                auto textSection =
                    pages.at("controls")->GetSection("controltextsection");
                textSection->SetMaxHeight(textSection->GetMaxHeight() +
                                          oldButtonSectionHeight -
                                          newButtonSectionHeight);

                // Update the page "controls" position.
                pages.at("controls")
                    ->SetPosition(glm::vec2(
                        this->gameBoard->GetPosition().x,
                        std::max(this->gameBoard->GetCenter().y -
                                     pages.at("controls")->GetHeight() * 0.5f,
                                 this->gameBoard->GetPosition().y)));

                if (textSection->NeedScrollIcon()) {
                  // Adjust the line width of text content
                  textSection->SetMaxWidth(textSection->GetMaxWidth() -
                                           PageSection::GetScrollIconWidth());
                  // Create scroll icon
                  textSection->InitScrollIcon(
                      colorRenderer, circleRenderer, lineRenderer,
                      this->gameBoard->GetPosition().x +
                          this->gameBoard->GetSize().x -
                          Scroll::GetSilkEdgeWidth() -
                          0.5f * PageSection::GetScrollIconWidth());
                } else {
                  // Remove the scroll icon if exists
                  textSection->DeleteScrollIcon();
                }
              } else if (content == "difficulty") {
                this->GoToState(GameState::DIFFICULTY_SETTINGS);
              } else if (content == "displaysettings") {
                this->GoToState(GameState::DISPLAY_SETTINGS);
              } else if (content == "languagepreference") {
                this->GoToState(GameState::LANGUAGE_PREFERENCE);
              } else if (content == "start") {
                this->GoToState(GameState::PREPARING);
                if (gameCharacters["guojie"]->GetState() !=
                    GameCharacterState::FIGHTING) {
                  gameCharacters["guojie"]->SetState(
                      GameCharacterState::FIGHTING);
                }
                // Set the position of guojie to be out of the screen.
                gameCharacters["guojie"]->SetPosition(
                    gameCharacters["guojie"]->GetTargetPosition("outofscreen"));
                // Set the target position of guojie to be landing
                gameCharacters["guojie"]->SetAndMoveToTargetPosition(
                    gameCharacters["guojie"]->GetTargetPosition("landing"));
                // Set the initial velocity and acceleration of guojie.
                gameCharacters["guojie"]->SetVelocity(
                    glm::vec2(0.0f, 10.0f * kBaseUnit));
                gameCharacters["guojie"]->SetAcceleration(
                    glm::vec2(0.0f, 10.0f * 9.8f * kBaseUnit));
                // Reset the score to 0.
                this->ResetScore(0);

              } else if (content == "exit") {
                this->SetState(GameState::EXIT);
              } else if (content == "back") {
                // Go to the last state
                this->GoToState(this->lastState);
              } else if (content == "resume") {
                // Go to the last state
                this->GoToState(this->lastState);
              } else if (content == "restart") {
                this->GoToState(GameState::PREPARING);
                // Reset the health of characters
                int totalHealth =
                    gameCharacters["guojie"]->GetHealth().GetTotalHealth();
                gameCharacters["guojie"]->GetHealth().SetCurrentHealth(
                    totalHealth);
                totalHealth =
                    gameCharacters["weiqing"]->GetHealth().GetTotalHealth();
                gameCharacters["weiqing"]->GetHealth().SetCurrentHealth(
                    totalHealth);
                // Delete the objects carried by the characters
                gameCharacters["guojie"]->ClearCarriedObjects();

                // Delete all the arrows
                arrows.clear();

                // Delete all the static bubbles.
                statics.clear();

                // Delete all the moving bubbles.
                moves.clear();

                // Reset the game level to 1.
                this->level = 1;

                // Reset the score to 0.
                this->ResetScore(0);

              } else if (content == "stop") {
                // Go to the state "Story"
                this->GoToState(GameState::STORY);
                // reset the state of each game character
                this->ResetGameCharacters();

                // Delete all the arrows
                arrows.clear();

                // Delete all the static bubbles.
                statics.clear();

                // Delete all the moving bubbles.
                moves.clear();

                // Reset score
                this->ResetScore(ConfigManager::GetInstance().GetScore());

                // Reset the game level to 1.
                this->level = 1;
              }

              // Set scroll state to be CLOSING
              this->scroll->SetState(ScrollState::CLOSING);

              // assume that only one button is pressed at a time, so we break
              // the loop.
              break;
            }
          } else {
            button->SetState(ButtonState::kHovered);
          }
        } else {
          button->SetState(ButtonState::kNormal);
        }
      }

      // Iterate options in the current active page
      if (!activePage.empty()) {
        for (size_t sectionIdx = 0;
             sectionIdx < pages.at(activePage)->GetOrder().size();
             ++sectionIdx) {
          auto sectionName = pages.at(activePage)->GetOrder()[sectionIdx];
          auto section = pages.at(activePage)->GetSection(sectionName);
          std::string optionToBeClicked = "";
          std::string optionAlreadyClicked = "";
          for (size_t unitIdx = 0; unitIdx < section->GetOrder().size();
               ++unitIdx) {
            auto unitName = section->GetOrder()[unitIdx];
            auto contentUnit = section->GetContent(unitName);
            auto option = std::dynamic_pointer_cast<OptionUnit>(contentUnit);
            if (option == nullptr) {
              continue;
            }

            assert(option->GetType() == ContentType::kOption &&
                   "The content type should be an option.");
            if (option->GetState() == OptionState::kUnclickable) {
              continue;
            }

            if (option->IsPositionInsideIcon(mousePosition)) {
              if (this->leftMousePressed) {
                if (option->GetState() == OptionState::kHovered) {
                  optionToBeClicked = option->GetName();
                  option->SetState(OptionState::kClicked);
                  if (activePage == "difficulty") {
                    optionAlreadyClicked =
                        difficulty_map.at(this->GetDifficulty());
                    if (optionToBeClicked == "easy") {
                      this->SetDifficulty(Difficulty::EASY);
                    } else if (optionToBeClicked == "medium") {
                      this->SetDifficulty(Difficulty::MEDIUM);
                    } else if (optionToBeClicked == "hard") {
                      this->SetDifficulty(Difficulty::HARD);
                    } else if (optionToBeClicked == "expert") {
                      this->SetDifficulty(Difficulty::EXPERT);
                    }
                  } else if (activePage == "displaysettings") {
                    switch (this->screenMode) {
                      case ScreenMode::FULLSCREEN:
                        optionAlreadyClicked = "fullscreen";
                        break;
                      case ScreenMode::WINDOWED_BORDERLESS:
                        optionAlreadyClicked = "windowedborderless";
                        break;
                      default:
                        optionAlreadyClicked = "windowed";
                        break;
                    }
                    if (optionToBeClicked == "fullscreen") {
                      this->GoToScreenMode(ScreenMode::FULLSCREEN);
                    } else if (optionToBeClicked == "windowedborderless") {
                      this->GoToScreenMode(ScreenMode::WINDOWED_BORDERLESS);
                    } else if (optionToBeClicked == "windowed") {
                      this->GoToScreenMode(ScreenMode::WINDOWED);
                    }
                  } else if (activePage == "languagepreference") {
                    optionAlreadyClicked = language_map.at(this->GetLanguage());
                    this->SetLanguage(language_enum_map.at(optionToBeClicked));
                  }
                  // Play the sound of clicking the option, similar to the sound
                  // of bubble popping.
                  SoundEngine::GetInstance().PlaySound("bubble_pop");
                  break;
                }
              } else {
                if (option->GetState() != OptionState::kClicked) {
                  option->SetState(OptionState::kHovered);
                }
              }
            } else {
              if (option->GetState() != OptionState::kClicked) {
                option->SetState(OptionState::kNormal);
              }
            }
          }
          // Set the state of the unclicked options to be kNormal if there's any
          // option been clicked.
          if (!optionToBeClicked.empty()) {
            auto option = std::dynamic_pointer_cast<OptionUnit>(
                section->GetContent(optionAlreadyClicked));
            assert(option != nullptr && "The option should not be nullptr.");
            assert(option->GetType() == ContentType::kOption &&
                   "The content type should be an option.");
            assert(option->GetState() != OptionState::kUnclickable &&
                   "The option should be clickable.");
            if (option->GetState() == OptionState::kClicked) {
              option->SetState(OptionState::kNormal);
            }
          }
        }
      }
    }
  }
}

void GameManager::Update(float dt) {
  auto& soundEngine = SoundEngine::GetInstance();
  // Update sound sources' volume.
  soundEngine.UpdateSourcesVolume(dt);
  // Update stream sources' volume.
  soundEngine.UpdateStreamsVolume(dt);
  // Clean up the sound sources that are not playing.
  soundEngine.CleanUpSources();
  // Refresh the background music if needed.
  soundEngine.RefreshBackgroundMusic(dt);
  if (this->state == GameState::PRELOAD) {
    if (this->targetState == GameState::SPLASH_SCREEN) {
      postProcessor->SetChaos(true);
      postProcessor->SetSampleOffsets(1.f / 20000.f);
      this->SetToTargetState();
    }
    return;
  } else if (this->state == GameState::SPLASH_SCREEN) {
    SoundEngine& soundEngine = SoundEngine::GetInstance();
    if (soundEngine.IsPlaying("white_noise") &&
        soundEngine.GetPlaybackPosition("white_noise") > 0.45f) {
      float curVolume = soundEngine.GetVolume("white_noise");
      float newVolume = curVolume - 0.35f * dt;
      if (newVolume < 0.05f) {
        newVolume = 0.05f;
        soundEngine.SetVolume("white_noise", newVolume);
      } else {
        soundEngine.SetVolume("white_noise", newVolume);
      }
    }
    if (this->targetState == GameState::UNDEFINED) {
      // Make the whole screen become clear from the chaos effect gradually.
      float originalIntensity = 1.f;
      float originalSampleOffsets = 1.f / 30000.f;
      float targetIntensity = 0.6f;
      float targetSampleOffsets = 1.f / 500.f;
      if (postProcessor->GetIntensity() > targetIntensity &&
          !timer->HasEvent("splash")) {
        float newIntensity = postProcessor->GetIntensity() - 0.08f * dt;
        float intensifyDiffProportion = (originalIntensity - newIntensity) /
                                        (originalIntensity - targetIntensity);
        float newSampleOffsets =
            originalSampleOffsets -
            intensifyDiffProportion *
                (originalSampleOffsets - targetSampleOffsets);
        SoundEngine& soundEngine = SoundEngine::GetInstance();
        constexpr float thresholdSampleOffsets = 1.f / 1050.f;
        if (newSampleOffsets >= thresholdSampleOffsets &&
            soundEngine.GetPlayCount("shock_wave") == 0) {
          soundEngine.PlaySound("shock_wave");
          soundEngine.PlaySound("splash_end", false);
        }
        if (soundEngine.IsPlaying("splash_end")) {
          float sampleOffsetsDiffProportion =
              (thresholdSampleOffsets - newSampleOffsets) /
              (thresholdSampleOffsets - targetSampleOffsets);
          float splashSoundVolume =
              glm::mix(0.02f, 0.35f, sampleOffsetsDiffProportion);
          soundEngine.SetVolume("splash_end", splashSoundVolume);
        }
        if (newIntensity < targetIntensity) {
          newIntensity = targetIntensity;
          newSampleOffsets = targetSampleOffsets;
          // Stay at the target state for a while.
          timer->SetEventTimer("splash", 3.f);
          timer->StartEventTimer("splash");
        }
        postProcessor->SetIntensity(newIntensity);
        postProcessor->SetSampleOffsets(newSampleOffsets);
      } else if (timer->HasEvent("splash") &&
                 (timer->IsEventTimerExpired("splash") ||
                  postProcessor->GetIntensity() == 1.f)) {
        postProcessor->SetChaos(false);
        postProcessor->SetSampleOffsets(0.f);
        postProcessor->SetIntensity(1.f);
        this->SetState(GameState::INTRO);
        SoundEngine& soundEngine = SoundEngine::GetInstance();
        // Stop splash screen sound.
        if (soundEngine.IsPlaying("splash_end")) {
          soundEngine.StopSound("splash_end");
        }
        if (soundEngine.IsPlaying("shock_wave")) {
          soundEngine.StopSound("shock_wave");
        }
        if (soundEngine.IsPlaying("white_noise")) {
          soundEngine.StopSound("white_noise");
        }
        // Remove the splash screen sound
        soundEngine.UnloadSound("splash_end");
        soundEngine.UnloadSound("shock_wave");
        soundEngine.UnloadSound("white_noise");
      } else if (timer->HasEvent("splash") &&
                 !timer->IsEventTimerExpired("splash")) {
        originalSampleOffsets = targetSampleOffsets;
        targetSampleOffsets = 1.f / 10000.f;
        originalIntensity = targetIntensity;
        targetIntensity = 1.f;

        if (postProcessor->GetSampleOffsets() > targetSampleOffsets) {
          float newSampleOffsets =
              postProcessor->GetSampleOffsets() - 0.002 * dt;
          float sampleOffsetsDiffProportion =
              (originalSampleOffsets - newSampleOffsets) /
              (originalSampleOffsets - targetSampleOffsets);
          float splashSoundVolume =
              glm::mix(0.35f, 0.02f, sampleOffsetsDiffProportion);
          soundEngine.SetVolume("splash_end", splashSoundVolume);
          float newIntensity =
              originalIntensity -
              (originalSampleOffsets - newSampleOffsets) /
                  (originalSampleOffsets - targetSampleOffsets) *
                  (originalIntensity - targetIntensity);
          if (newSampleOffsets < targetSampleOffsets) {
            newSampleOffsets = targetSampleOffsets;
            newIntensity = targetIntensity;
          }
          postProcessor->SetSampleOffsets(newSampleOffsets);
          postProcessor->SetIntensity(newIntensity);
        }
      }
    }
    return;
  } else if (this->state == GameState::INTRO) {
    // typing the introduction text
    if (!timer->HasEvent("intro") && !timer->HasEvent("introFadeOut") &&
        !texts.at("introduction")->UpdateTypingEffect(dt)) {
      // stay for 1 second
      timer->SetEventTimer("intro", 4.f);
      timer->StartEventTimer("intro");
    } else if (timer->HasEvent("intro")) {
      assert(!texts.at("introduction")->UpdateTypingEffect(dt) &&
             "The introduction text should be fully typed.");
      // jump to the next state when the timer is expired
      if (timer->IsEventTimerExpired("intro")) {
        // Play sound of pressing key 'Enter'
        SoundEngine& soundEngine = SoundEngine::GetInstance();
        soundEngine.PlaySound("key_enter");
        // Disable the typing effect of the introduction text
        texts.at("introduction")->DisableTypingEffect();
        // Set up timer for the introduction text to fade out
        timer->SetEventTimer("introFadeOut", 1.5f);
        timer->StartEventTimer("introFadeOut");
        timer->CleanEvent("intro");
      }
    } else if (timer->HasEvent("introFadeOut") &&
               timer->IsEventTimerExpired("introFadeOut")) {
      // Unload the key typing sound
      SoundEngine& soundEngine = SoundEngine::GetInstance();
      this->SetState(GameState::INITIAL);
      this->GoToState(GameState::STORY);
      soundEngine.UnloadSound("key_s");
      soundEngine.UnloadSound("keys_s_j");
      soundEngine.UnloadSound("key_enter");
      soundEngine.UnloadSound("key_space");
    }
    return;
  }
  // silk boundary before scrolling
  glm::vec4 silkBoundsBefore = this->scroll->GetSilkBounds();

  if (this->scroll->GetState() == ScrollState::CLOSING) {
    this->scroll->Close(dt, this->scroll->GetTargetSilkLenForClosing());
    if (this->scroll->GetState() == ScrollState::CLOSED) {
      this->timer->SetEventTimer("scrollReadyToOpen", 0.15f);
      this->timer->StartEventTimer("scrollReadyToOpen");
    }
  } else if (this->scroll->GetState() == ScrollState::OPENING &&
             (!this->timer->HasEvent("scrollReadyToOpen") ||
              this->timer->IsEventTimerExpired("scrollReadyToOpen"))) {
    if (this->lastState != GameState::PREPARING &&
        this->state == GameState::ACTIVE) {
      this->scroll->Open(dt, this->scroll->GetCurrentSilkLenForNarrowing());
      if (this->scroll->GetState() == ScrollState::OPENED) {
        if (this->scroll->GetCurrentSilkLenForNarrowing() >
            this->scroll->GetTargetSilkLenForNarrowing()) {
          this->scroll->SetState(ScrollState::NARROWING);
          silkBoundsBefore = this->scroll->GetSilkBounds();
        }
      }
    } else {
      this->scroll->Open(dt, this->scroll->GetTargetSilkLenForOpening());
    }
  } else if (this->scroll->GetState() == ScrollState::NARROWING) {
    this->scroll->Narrow(dt, this->scroll->GetTargetSilkLenForNarrowing());
  } else if (this->scroll->GetState() == ScrollState::RETRACTING) {
    this->scroll->Retract(dt);
    if (this->scroll->GetState() == ScrollState::RETRACTED) {
      if (this->level <= this->GetNumGameLevels() &&
          gameCharacters["weiqing"]->GetHealth().GetCurrentHealth() > 0) {
        this->timer->SetEventTimer("scrollInSleeve", 0.8f);
        this->timer->StartEventTimer("scrollInSleeve");
        this->scroll->SetState(ScrollState::DEPLOYING);
      } else {
        assert(this->targetState == GameState::LOSE &&
               "The target state should be LOSE.");
        this->SetToTargetState();
        // Disable the scroll
        this->scroll->SetState(ScrollState::DISABLED);
        // Play defeated sound
        SoundEngine::GetInstance().PlaySound("defeated");
      }
    }
  } else if (this->scroll->GetState() == ScrollState::DEPLOYING) {
    if (this->timer->HasEvent("scrollInSleeve")) {
      if (this->timer->IsEventTimerExpired("scrollInSleeve")) {
        this->scroll->SetAlpha(1.f);
        this->timer->CleanEvent("scrollInSleeve");
        SoundEngine::GetInstance().PlaySound("scroll_out_sleeve");
      }
    } else {
      this->scroll->Deploy(dt);
    }
  } else if (this->scroll->GetState() == ScrollState::ATTACKING) {
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
  } else if (this->scroll->GetState() == ScrollState::RETURNING) {
    if (this->timer->IsEventTimerExpired("scrollHitting")) {
      this->scroll->Return(dt);
    }
    if (gameCharacters["weiqing"]->IsStunned()) {
      gameCharacters["weiqing"]->RotateTo(
          gameCharacters["weiqing"]->GetTargetRoll(), 1.9f * glm::pi<float>(),
          dt);
      if (!gameCharacters["weiqing"]->IsRotating()) {
        gameCharacters["weiqing"]->SetTargetRoll(0.f);
        gameCharacters["weiqing"]->DeactivateStun();
      }
    } else if (gameCharacters["weiqing"]->IsRotating()) {
      gameCharacters["weiqing"]->RotateTo(
          gameCharacters["weiqing"]->GetTargetRoll(), -2.4f * glm::pi<float>(),
          dt);
    } else if (gameCharacters["weiqing"]->GetHealth().GetCurrentHealth() == 0 &&
               gameCharacters["weiqing"]->GetState() !=
                   GameCharacterState::SAD) {
      gameCharacters["weiqing"]->SetState(GameCharacterState::SAD);
      // Go to the state of LOSE
      this->GoToState(GameState::LOSE);
      // Gradually lower the volume of the background music
      auto& soundEngine = SoundEngine::GetInstance();
      std::string currentBackgroundMusic =
          soundEngine.GetPlayingBackgroundMusic();
      if (soundEngine.IsStreamPlaying(currentBackgroundMusic)) {
        soundEngine.GraduallyChangeStreamVolume(currentBackgroundMusic, 0.f,
                                                3.f);
      }
      soundEngine.DoNotPlayNextBackgroundMusic();
    }
  }

  // Scroll should only shake when the scroll is OPENED.
  this->gameArenaShaking &= this->scroll->GetState() == ScrollState::OPENED;
  if (!this->gameArenaShaking &&
      SoundEngine::GetInstance().IsPlaying("scroll_vibrate")) {
    float volume = SoundEngine::GetInstance().GetVolume("scroll_vibrate");
    if (volume == 1.f) {
      SoundEngine::GetInstance().GraduallyChangeVolume("scroll_vibrate", 0.f,
                                                       0.5f);
    }
  }

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
        if (gameCharacters["guojie"]->GetCarriedObjects().count(
                arrow->GetID()) == 0) {
          gameCharacters["guojie"]->AddCarriedObject(arrow);
          // gameCharacters["guojie"]->GetCarriedObjects()[arrow->GetID()] =
          // arrow;
          hasNewArrowHit = true;
          // sort the arrows based on the x and y penetration position of the
          // arrow. If the x position is the same, then we sort based on the y
          // position.
          std::sort(arrows.begin(), arrows.end(),
                    [](const std::shared_ptr<Arrow>& a,
                       const std::shared_ptr<Arrow>& b) {
                      if (a->GetPenetrationPosition().x ==
                          b->GetPenetrationPosition().x) {
                        return a->GetPenetrationPosition().y >
                               b->GetPenetrationPosition().y;
                      }
                      return a->GetPenetrationPosition().x <
                             b->GetPenetrationPosition().x;
                    });
        }
        // if (arrow->IsStopped() && this->level > this->GetNumGameLevels()) {
        //     gameCharacters["guojie"]->SetState(GameCharacterState::SAD);
        // }
      }
    }
  }
  if (!arrows.empty() && !hasNewArrowHit) {
    if (gameCharacters["guojie"]->IsStunned()) {
      gameCharacters["guojie"]->RotateTo(
          gameCharacters["guojie"]->GetTargetRoll(), -1.9f * glm::pi<float>(),
          dt);
      if (!gameCharacters["guojie"]->IsRotating()) {
        gameCharacters["guojie"]->SetTargetRoll(0.f);
        gameCharacters["guojie"]->DeactivateStun();
      }
    } else if (gameCharacters["guojie"]->IsRotating()) {
      gameCharacters["guojie"]->RotateTo(
          gameCharacters["guojie"]->GetTargetRoll(), 2.4f * glm::pi<float>(),
          dt);
    } else if (gameCharacters["guojie"]->GetHealth().GetCurrentHealth() == 0 &&
               gameCharacters["guojie"]->GetState() !=
                   GameCharacterState::SAD &&
               (this->state == GameState::ACTIVE ||
                this->state == GameState::PREPARING)) {
      gameCharacters["guojie"]->SetState(GameCharacterState::SAD);
      gameCharacters["weiqing"]->SetState(GameCharacterState::WINNING);
      gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);
      gameCharacters["liuche"]->SetState(GameCharacterState::HAPPY);

      // Get the current center of the scroll
      glm::vec2 scrollCenter = this->scroll->GetCenter();
      // Exploding the scroll as guojie is defeated.
      std::vector<ExplosionInfo> explosionInfo;
      // 0.388235f, 0.174671f, 0.16863f, 1.0f
      explosionInfo.emplace_back(
          this->scroll->GetCenter(),
          glm::vec4(0.388235f, 0.174671f, 0.16863f, 1.0f), false, 2000,
          this->scroll->GetSilkWidth() * 0.5f,
          this->scroll->GetSilkLen() * 0.5f,
          glm::vec2(2 * kBaseUnit / 15.f, 4 * kBaseUnit / 15.f));
      explosionSystem->CreateExplosions(explosionInfo);

      /*SoundEngine::GetInstance().PlaySound("scroll_explode");*/

      // Set the scroll to DISABLED state.
      this->scroll->SetState(ScrollState::DISABLED);

      // Set the state to be WINNING
      this->SetState(GameState::WIN);

      // Play victory music
      SoundEngine::GetInstance().PlaySound("victory");
    }
  }

  // silk boundary after scrolling
  glm::vec4 silkBoundsAfter = this->scroll->GetSilkBounds();

  // deterimine whether to hide the default mouse cursor based on the silk
  // boundaries.
  this->hideDefaultMouseCursor =
      this->scroll->GetSilkLen() > 0 && this->mouseX >= silkBoundsAfter.x &&
      this->mouseX <= silkBoundsAfter.z && this->mouseY >= silkBoundsAfter.y &&
      this->mouseY <= silkBoundsAfter.w;

  // Update health damage texts.
  gameCharacters["guojie"]->GetHealth().UpdateDamageTexts(dt);
  gameCharacters["weiqing"]->GetHealth().UpdateDamageTexts(dt);

  // Update the transparency of the score text.
  if (this->state != GameState::ACTIVE && this->state != GameState::PREPARING &&
      this->state != GameState::WIN && this->state != GameState::LOSE &&
      this->lastState != GameState::ACTIVE) {
    float scoreAlpha = texts.at("score")->GetAlpha();
    if (scoreAlpha > kScoreAlpha) {
      scoreAlpha -= 0.8f * dt;
      if (scoreAlpha < kScoreAlpha) {
        scoreAlpha = kScoreAlpha;
      }
    } else if (scoreAlpha < kScoreAlpha) {
      scoreAlpha += 0.8f * dt;
      if (scoreAlpha > kScoreAlpha) {
        scoreAlpha = kScoreAlpha;
      }
    }
    texts.at("score")->SetAlpha(scoreAlpha);
  }

  if (this->state == GameState::ACTIVE) {
    // Get the game board position and size
    glm::vec2 gameBoardPostition = gameBoard->GetValidPosition();
    glm::vec2 gameBoardSize = gameBoard->GetValidSize();
    glm::vec4 gameBoardBoundaries = gameBoard->GetValidBoundaries();

    // If the target state is UNDEFINED and the scroll is narrowing or narrowed,
    // then we get real boundaries by mixing the game board boundaries and the
    // scroll boundaries.
    if (this->targetState == GameState::UNDEFINED &&
        (this->scroll->GetState() == ScrollState::NARROWING ||
         this->scroll->GetState() == ScrollState::NARROWED)) {
      float offsetY = silkBoundsAfter.y - silkBoundsBefore.y;
      // Reset the valid boundaries of the game board.
      gameBoard->SetValidPosition(
          glm::vec2(silkBoundsAfter.x, silkBoundsAfter.y));
      gameBoard->SetValidSize(glm::vec2(silkBoundsAfter.z - silkBoundsAfter.x,
                                        silkBoundsAfter.w - silkBoundsAfter.y));
      gameBoardSize = gameBoard->GetValidSize();
      gameBoardPostition = gameBoard->GetValidPosition();
      gameBoardBoundaries = gameBoard->GetValidBoundaries();

      // move all the static bubbles downwards by the offset.
      for (auto& [id, bubble] : statics) {
        bubble->SetPosition(bubble->GetPosition() + glm::vec2(0.f, offsetY));
      }

      // Check and adjust the postisions of the moving bubbles if they are
      // hitting with the top wall or with the bottom wall.
      for (auto& [id, bubble] : moves) {
        if (bubble->GetPosition().y < gameBoardBoundaries.y) {
          bubble->SetPosition(
              glm::vec2(bubble->GetPosition().x, gameBoardBoundaries.y));
        } else if (bubble->GetPosition().y + bubble->GetSize().y >
                   gameBoardBoundaries.w) {
          bubble->SetPosition(
              glm::vec2(bubble->GetPosition().x,
                        gameBoardBoundaries.w - bubble->GetSize().y));
        }
      }

      // Move the shooter upwards by the offset.
      shooter->SetPosition(shooter->GetPosition() - glm::vec2(0.f, offsetY));
      shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);

      // Move the timer's text downwards by the offset.
      // texts["time"]->SetPosition(texts["time"]->GetPosition() +
      //                           glm::vec2(0.f, offsetY));
      texts["time"]->SetCenter(texts["time"]->GetCenter() +
                               glm::vec2(0.f, offsetY));

      if (this->scroll->GetState() == ScrollState::NARROWED) {
        // Reset the scroll state to OPENED.
        this->scroll->SetState(ScrollState::OPENED);
        // Resume the timer.
        this->timer->ResumeEventTimer("beforenarrowing");
        // Set the time to be displayed on the screen.
        texts["time"]->SetParagraph(
            0, intToU32String(static_cast<int64_t>(
                   std::ceil(this->timer->GetEventTimer("beforenarrowing")))));
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

      // We would like to adjust the postion of the bubble to make it have at
      // least two static neighbors if possible.
      std::vector<int> candidateIds;
      std::vector<int> neighborIds = GetNeighborIds(bubble);

      bool funeTuneDone = false;

      // If the new bubble only has 1 neighbor, then we try to adjust the
      // position of the bubble to make it have more neighbors.
      if (neighborIds.size() == 1) {
        // Fine tuning the position of the collidef moving bubbles to the best
        // free slot.
        int staticBubbleId = neighborIds[0];
        funeTuneDone = FineTuneToNeighbor(id, staticBubbleId);
        if (!funeTuneDone) {
          funeTuneDone = FineTuneToClose(id, staticBubbleId);
        }
      } else if (neighborIds.size() == 0) {
        // If the bubble has no neighbor, it must be at the top of the game
        // board.
        assert(IsAtUpperBoundary(bubble->GetPosition()) &&
               "The bubble should be at the top of the game board.");
        // Fine tuning the position of the collidef moving bubbles to the best
        // free slot at the upper boundary of the game board.
        funeTuneDone = FineTuneToCloseUpper(id);
      }

      if (!funeTuneDone) {
        FineTuneToCorrectPosition(id);
      }

      // Add the bubble to the static bubbles
      statics[id] = std::move(bubble);
      ++colorCount[statics[id]->GetColorEnum()];
      // Remove the bubble from the moving bubbles
      moves[id] = nullptr;
      moves.erase(id);
      shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
      // Check if the bubble connect to a group of bubbles of the same color.
      // And if they together form a group of more than 2 bubbles, then we
      // remove them.
      std::vector<int> connectedBubbleIds = FindConnectedBubblesOfSameColor(id);
      if (connectedBubbleIds.size() > 2) {
        // Remove the connected bubbles from the static bubbles and push them
        // into explodings.
        for (int connectedBubbleId : connectedBubbleIds) {
          --colorCount[statics[connectedBubbleId]->GetColorEnum()];
          explodings[connectedBubbleId] = std::move(statics[connectedBubbleId]);
          statics[connectedBubbleId] = nullptr;
          statics.erase(connectedBubbleId);
          shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
        }

        // Find all the bubbles that are falling after the explosion. A bubble
        // is falling if it is not connected to the top wall or the static
        // bubbles.
        std::vector<int> fallingIds = FindAllFallingBubbles();

        // Remove the falling bubbles from the static bubbles and push them into
        // falling.
        for (int fallingId : fallingIds) {
          // the falling bubble should be in the statics.
          assert(statics.count(fallingId) > 0 &&
                 "Failed to find the bubble by ID.");
          --colorCount[statics[fallingId]->GetColorEnum()];
          fallings.emplace(fallingId, std::move(statics[fallingId]));
          // Remove the falling bubble from the static bubbles.
          statics[fallingId] = nullptr;
          statics.erase(fallingId);
          shooter->GetRay().UpdatePath(gameBoardBoundaries, this->statics);
        }

        // Calculate the score based on the number of bubbles exploded or
        // falling
        if (!explodings.empty()) {
          this->CalculateScore(explodings.size(),
                               explodings.begin()->second->GetRadius(),
                               BubbleState::Exploding,
                               this->timer->GetEventUsedTime("playtime"));
        }
        if (!fallings.empty()) {
          this->CalculateScore(
              fallings.size(), fallings.begin()->second->GetRadius(),
              BubbleState::Falling, this->timer->GetEventUsedTime("playtime"));
        }

        // clear all the exploding bubbles.
        std::vector<ExplosionInfo> explosionInfo;
        for (auto& [id, bubble] : explodings) {
          explosionInfo.emplace_back(bubble->GetCenter(), bubble->GetColor(),
                                     isDeepColor(bubble->GetColor()), 150,
                                     bubble->GetRadius() * 0.6f);
        }
        explosionSystem->CreateExplosions(explosionInfo);
        SoundEngine::GetInstance().PlaySound("bubble_explode", false);
        explodings.clear();
      }

      // If all statuc bubbles are removed, we set the GameState to PREPAREING
      // and switch to the next level.
      if (statics.empty()) {
        // Clear all the moving bubbles.
        moves.clear();

        // Initialize an arrow firing by Weiqing towards Guojie.
        // Get target position on the charactor guojie.
        glm::vec2 targetPostion =
            glm::vec2(gameCharacters["guojie"]->GetPosition().x +
                          gameCharacters["guojie"]->GetSize().x / 2.0f,
                      gameCharacters["guojie"]->GetPosition().y +
                          gameCharacters["guojie"]->GetSize().y * 0.53f);
        // Randomly add offset to the target position within a circle of
        // kBaseUnit.
        float angle = static_cast<float>(rand()) /
                      static_cast<float>(RAND_MAX) * 2 * glm::pi<float>();
        float radius = static_cast<float>(rand()) /
                       static_cast<float>(RAND_MAX) * kBaseUnit * 0.65f;
        glm::vec2 offset =
            glm::vec2(radius * std::cos(angle), radius * std::sin(angle));
        targetPostion += offset;

        arrows.emplace_back(std::make_shared<Arrow>(
            glm::vec2(
                gameCharacters["weiqing"]->GetPosition().x - 2 * kBaseUnit,
                gameCharacters["weiqing"]->GetPosition().y + 5.5 * kBaseUnit),
            glm::vec2(this->width / 13.6708861f, this->height / 70.f),
            ResourceManager::GetInstance().GetTexture("arrow3")));
        /*arrows.emplace_back(glm::vec2(gameCharacters["guojie"]->GetPosition().x
         * + 3 * kBubbleRadius, gameCharacters["guojie"]->GetPosition().y + 5.5
         * * kBubbleRadius), glm::vec2(this->width / 13.6708861f, this->height
         * / 70.f), ResourceManager::GetInstance().GetTexture("arrow3"));*/
        arrows.back()->Fire(targetPostion, 65.0f * kBaseUnit);

        this->GoToState(GameState::PREPARING);
        numOfScoreIncrementsReady += scoreIncrements.size();
        this->timer->SetEventTimer("refreshscore", 0.05f);
        this->timer->StartEventTimer("refreshscore");
        this->scroll->SetState(ScrollState::CLOSING);
        ++(this->level);
        if (this->level > this->GetNumGameLevels()) {
          // Gradually lower the fighting music volume as we are ready to win
          auto& soundEngine = SoundEngine::GetInstance();
          std::string currentBackgroundMusic =
              soundEngine.GetPlayingBackgroundMusic();
          if (soundEngine.IsStreamPlaying(currentBackgroundMusic)) {
            soundEngine.GraduallyChangeStreamVolume(currentBackgroundMusic, 0.f,
                                                    3.f);
          }
          soundEngine.DoNotPlayNextBackgroundMusic();
        }
        break;
      }
    }

    // Check if the current level is failed.
    if (this->IsLevelFailed()) {
      // If the current level is failed, then we restart the current level.
      this->GoToState(GameState::PREPARING);
      this->scroll->SetState(ScrollState::CLOSING);
      if (!scoreIncrements.empty()) {
        numOfScoreIncrementsReady += scoreIncrements.size();
        this->timer->SetEventTimer("refreshscore", 0.05f);
        this->timer->StartEventTimer("refreshscore");
      }
    }

    if (this->scroll->GetState() == ScrollState::OPENED) {
      if (this->timer->IsPaused("playtime")) {
        this->timer->ResumeEventTimer("playtime");
      }
      // check if the event timer for narrowing the scroll is triggered.
      if (this->timer->IsPaused("beforenarrowing")) {
        this->timer->ResumeEventTimer("beforenarrowing");
      }
      if (this->timer->IsEventTimerExpired("beforenarrowing")) {
        // Stop the shake effect
        this->gameArenaShaking = false;
        SoundEngine::GetInstance().GraduallyChangeVolume("scroll_vibrate", 0.f,
                                                         0.5f);
        // Narrow the scroll
        this->scroll->SetState(ScrollState::NARROWING);
        this->scroll->SetTargetSilkLenForNarrowing(gameBoardSize.y -
                                                   2 * kBaseUnit);
        // Restart the event timer for narrowing the scroll.
        this->timer->StartEventTimer("beforenarrowing");
        // Pause the event timer for narrowing the scroll.
        this->timer->PauseEventTimer("beforenarrowing");
      } else {
        // Update the time to be displayed on the screen.
        texts["time"]->SetParagraph(
            0, intToU32String(static_cast<int64_t>(std::ceil(
                   this->timer->GetEventRemainingTime("beforenarrowing")))));
        // Shaking the whole scroll if the time is less than 1.5s.
        if (!this->gameArenaShaking &&
            this->timer->GetEventRemainingTime("beforenarrowing") < 1.5f) {
          this->gameArenaShaking = true;
          SoundEngine::GetInstance().PlaySound("scroll_vibrate");
        }
      }
    }

    if (this->targetState != GameState::UNDEFINED &&
        this->scroll->GetState() == ScrollState::CLOSED) {
      this->SetToTargetState();
      this->SetTransitionState(TransitionState::TRANSITION);
      // Set the state of the gameboard to be ACTIVE if the new state is not
      // PREPARING.
      if (this->state != GameState::PREPARING) {
        gameBoard->SetState(GameBoardState::ACTIVE);
        SetScrollState(ScrollState::OPENING);
      }
    }
  } else if (this->state == GameState::PREPARING) {
    this->scroll->SetTargetSilkLenForNarrowing(
        this->scroll->GetTargetSilkLenForOpening());
    this->scroll->SetCurrentSilkLenForNarrowing(
        this->scroll->GetTargetSilkLenForOpening());
    if (this->scroll->GetState() == ScrollState::CLOSED) {
      moves.clear();
      // If the static bubbles are empty, then retract the scroll, else the
      // scroll would be attacking the player.
      if (statics.empty()) {
        this->scroll->SetState(ScrollState::RETRACTING);
      } else {
        this->scroll->SetState(ScrollState::ATTACKING);
      }
      statics.clear();
      colorCount.clear();
    } else if (this->scroll->GetState() == ScrollState::DEPLOYED ||
               this->scroll->GetState() == ScrollState::RETURNED) {
      if (this->targetState != GameState::LOSE) {
        SetScrollState(ScrollState::OPENING);
      } else {
        this->scroll->SetState(ScrollState::RETRACTING);
      }
    }
    if (statics.empty()) {
      // Generate random static bubbles
      GenerateRandomStaticBubbles();
      // refresh the color of the carried bubble and the next bubble based the
      // existing colors of all static bubbles if it is the first level.
      if (this->level == 1) {
        shooter->RefreshCarriedBubbleColor(GetNextBubbleColor());
        shooter->RefreshNextBubbleColor(GetNextBubbleColor());
        //// update the gameboard color based on the color of the ray.
        // gameBoard->UpdateColor(shooter->GetRay().GetColorWithoutAlpha());
      }
      // Get half total offset of the scroll narrowing
      glm::vec2 halfOffset =
          gameBoard->GetValidPosition() - gameBoard->GetPosition();

      // Set the valid boundaries of the game board.
      gameBoard->SetValidPosition(gameBoard->GetPosition());
      gameBoard->SetValidSize(gameBoard->GetSize());
      // reset the time text position
      // texts["time"]->SetPosition(texts["time"]->GetPosition() - halfOffset);
      texts["time"]->SetCenter(texts["time"]->GetCenter() - halfOffset);
      // reset the position of the shooter
      shooter->SetPosition(shooter->GetPosition() + halfOffset);
      shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(),
                                   this->statics);
    } else {
      if (scroll->GetState() == ScrollState::OPENED) {
        this->SetState(GameState::ACTIVE);
        this->targetState = GameState::UNDEFINED;
        float duration = GetNarrowingTimeInterval();
        this->timer->SetEventTimer("beforenarrowing", duration);
        this->timer->StartEventTimer("beforenarrowing");
        // start counting the time
        this->timer->SetEventTimer("playtime", 1000.f);
        this->timer->StartEventTimer("playtime");
      }
    }
  } else if (this->state == GameState::WIN || this->state == GameState::LOSE) {
    // if (this->scroll->GetState() == ScrollState::CLOSED) {
    //     this->scroll->SetState(ScrollState::RETRACTING);
    // }
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

        SoundEngine::GetInstance().PlaySound("drop");
      }
      texts[topic]->SetScale(newScale);
    } else {
      // If the game is lost, then grayscale the screen.
      if (this->state == GameState::LOSE) {
        postProcessor->SetGrayscale(true);
      }
      if (!this->timer->HasEvent("prompttomainmenu")) {
        // Make the whole screen blur immediately.
        this->timer->SetEventTimer("prompttomainmenu", 1.5f);
        this->timer->SetEventTimer("hideprompttomainmenu", 0.5f);
        this->timer->SetEventTimer("beforegraduallyclear", 3.5f);
        this->timer->StartEventTimer("prompttomainmenu");
        this->timer->StartEventTimer("hideprompttomainmenu");
        this->timer->PauseEventTimer("hideprompttomainmenu");
        this->timer->StartEventTimer("beforegraduallyclear");
        postProcessor->SetSampleOffsets(1.f / 240);
      } else if (this->timer->HasEvent("beforegraduallyclear") &&
                 this->timer->IsEventTimerExpired("beforegraduallyclear")) {
        // Make the whole screen clear gradually.
        float newSampleOffsets =
            postProcessor->GetSampleOffsets() - 0.00085f * dt;
        if (newSampleOffsets <= 0.f) {
          newSampleOffsets = 0.f;
          this->timer->CleanEvent("beforegraduallyclear");
        }
        postProcessor->SetSampleOffsets(newSampleOffsets);
      }
      if (this->timer->HasEvent("prompttomainmenu") &&
          !this->timer->IsPaused("prompttomainmenu")) {
        if (this->timer->IsEventTimerExpired("prompttomainmenu")) {
          this->timer->StartEventTimer("prompttomainmenu");
          this->timer->PauseEventTimer("prompttomainmenu");
          this->timer->StartEventTimer("hideprompttomainmenu");
        }
      } else if (this->timer->HasEvent("hideprompttomainmenu") &&
                 !this->timer->IsPaused("hideprompttomainmenu")) {
        if (this->timer->IsEventTimerExpired("hideprompttomainmenu")) {
          this->timer->StartEventTimer("hideprompttomainmenu");
          this->timer->PauseEventTimer("hideprompttomainmenu");
          this->timer->StartEventTimer("prompttomainmenu");
        }
      }
      // Increase the opacity of the score gradually.
      if (texts.at("score")->GetAlpha() < 1.f) {
        float newAlpha = texts.at("score")->GetAlpha() + 0.8 * dt;
        if (newAlpha > 1.f) {
          newAlpha = 1.f;
        }
        texts.at("score")->SetAlpha(newAlpha);
      }
    }
  } else if (this->state == GameState::INITIAL) {
    if (this->transitionState == TransitionState::START) {
      float scrollCenterX = scroll->GetCenter().x;
      float scrollCenterY = scroll->GetCenter().y;
      // If the scroll is out of the top of the screen and its velocity is 0.f,
      // then we give it a velocity and a acceleration to make it gradually move
      // down.
      if (scrollCenterY < 0.f && scroll->GetVelocity() == glm::vec2(0.f, 0.f)) {
        glm::vec2 scrollCenter = scroll->GetCenter();
        scroll->SetVelocity(glm::vec2(0.0f, 26 * kBaseUnit));
        float stopPoint = this->height * 0.545f;
        float timeToStop =
            2.f * (stopPoint - scrollCenter.y) / scroll->GetVelocity().y;
        glm::vec2 acceleration =
            glm::vec2(0, -scroll->GetVelocity().y / timeToStop);
        scroll->SetAcceleration(acceleration);
      } else {
        // Move the scroll with a gradually decreasing speed.
        scroll->Move(dt);

        if (scroll->GetVelocity().y <= 0.f &&
            areFloatsEqual(scrollCenterY, this->height * 0.5f,
                           kBaseUnit * 0.04f)) {
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
          SetScrollState(ScrollState::OPENING);
          if (lastLastState == GameState::INTRO) {
            // Set the offset of the text "Story".
            glm::vec2 buttonSectionPos = this->pages.at("story")
                                             ->GetSection("storybuttonsection")
                                             ->GetPosition();
            glm::vec2 textSectionPos = this->pages.at("story")
                                           ->GetSection("storytextsection")
                                           ->GetPosition();
            float initialOffset = buttonSectionPos.y - textSectionPos.y;
            pages.at("story")
                ->GetSection("storytextsection")
                ->SetOffset(initialOffset);
          }
        }
      }
    } else if (this->transitionState == TransitionState::TRANSITION) {
    }
  } else if (this->state == GameState::STORY) {
    auto textSection = this->pages.at("story")->GetSection("storytextsection");
    float targetOffset =
        0.95f * textSection->GetMaxHeight() - textSection->GetHeight();
    if ((this->transitionState == TransitionState::TRANSITION &&
             this->targetState == GameState::UNDEFINED ||
         this->targetState != GameState::UNDEFINED &&
             this->scroll->GetState() == ScrollState::CLOSING) &&
        !textSection->IsScrollIconAllowed()) {
      // If the offset of the text section is greater than 0, then we decrease
      // the offset to move the text section upwards.
      float offset = textSection->GetOffset();
      targetOffset = std::min(0.f, targetOffset);
      if (offset > targetOffset) {
        offset = std::max(offset - 3.f * kBaseUnit * dt, targetOffset);
        this->pages.at("story")
            ->GetSection("storytextsection")
            ->SetOffset(offset);
      } else {
        // Finish the transition to GameState::STORY
        this->SetTransitionState(TransitionState::END);
        // Enable the scroll icon
        textSection->SetScrollIconAllowed(true);
        // Set the position of the scroll icon
        textSection->ResetSrcollIconPosition();
      }
    }
    if (this->targetState != GameState::UNDEFINED &&
        this->scroll->GetState() == ScrollState::CLOSED) {
      this->SetToTargetState();
      this->SetTransitionState(TransitionState::TRANSITION);
      if (this->state != GameState::PREPARING) {
        SetScrollState(ScrollState::OPENING);
      }
      // If the new state is Preparing, then we set the gameboard state to be
      // INGAME.
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
      if (this->scroll->GetState() == ScrollState::OPENED &&
          (buttons["back"]->GetState() != ButtonState::kInactive ||
           buttons["restart"]->GetState() != ButtonState::kInactive)) {
        this->SetTransitionState(TransitionState::END);
      }
    }
    if (this->targetState != GameState::UNDEFINED &&
        this->scroll->GetState() == ScrollState::CLOSED) {
      this->SetToTargetState();
      this->SetTransitionState(TransitionState::TRANSITION);
      if (this->state != GameState::PREPARING) {
        SetScrollState(ScrollState::OPENING);
      }

      // If the new state is PREPARING or ACTIVE, then we set the gameboard
      // state to be INGAME.
      if (this->state == GameState::PREPARING ||
          this->state == GameState::ACTIVE) {
        gameBoard->SetState(GameBoardState::INGAME);
      }
    }
  } else if (this->state == GameState::DIFFICULTY_SETTINGS ||
             this->state == GameState::DISPLAY_SETTINGS ||
             this->state == GameState::LANGUAGE_PREFERENCE) {
    if (this->targetState != GameState::UNDEFINED &&
        this->scroll->GetState() == ScrollState::CLOSED) {
      this->SetToTargetState();
      this->SetTransitionState(TransitionState::TRANSITION);
      SetScrollState(ScrollState::OPENING);
    }
  }
  // Check if the characters are moving to the target position.
  for (auto& [name, character] : gameCharacters) {
    if (character->IsMoving()) {
      float characterVelocity = glm::length(
          character->GetVelocity() + character->GetAcceleration() * dt * 0.5f);
      glm::vec2 characterDirection =
          character->GetCurrentTargetPosition() - character->GetPosition();
      glm::vec2 characterVelocityVec =
          glm::normalize(characterDirection) * characterVelocity;

      // Create shadow as the character moves.
      /*      std::cout << "frame count while doing respawn: " <<
       * this->frameCount << std::endl;*/
      shadowTrailSystem->respawnParticles(
          *character, 40, characterVelocityVec * 0.1f,
          glm::vec2(kBaseUnit * 0.2f, kBaseUnit * 0.4f),
          glm::vec2(character->GetSize().x * 0.46f,
                    character->GetSize().y * 0.48f));

      /*           if (name == "guojie") {
                                 character->SetAlpha(0.2f);
                         }*/
      ;
      /*        glm::vec2 avgVelocity = character->GetVelocity() +
       * character->GetAcceleration() * dt * 0.5f;*/
      character->MoveTo(character->GetCurrentTargetPosition(),
                        characterVelocity, dt);
      character->SetVelocity(character->GetVelocity() +
                             character->GetAcceleration() * dt);
      if (!character->IsMoving()) {
        character->StopMoving();
      }
      if (name == "guojie" && !character->IsMoving()) {
        // Shake the screen when guojie lands on the target position.
        postProcessor->SetShake(true, /*strength=*/0.0081f,
                                /*timeMultiplierForX=*/30.f,
                                /*timeMultiplierForY=*/80.f);
        postProcessor->SetBlur(true);
        postProcessor->SetSampleOffsets(0.0005f);
        // Set time for shaking the screen.
        this->timer->SetEventTimer("guojieshaking", 0.95f);
        this->timer->StartEventTimer("guojieshaking");
        // Set the time for cracks on the ground.
        this->timer->SetEventTimer("cracks", 1.5f);
        this->timer->StartEventTimer("cracks");
        // Play the sound of guojie landing on the ground.
        SoundEngine::GetInstance().PlaySound("wood_collide", false);
        SoundEngine::GetInstance().PlaySound("earthquake", false);
      }
    }
  }

  // Set the state of the game characters after guojie arrives at the target
  // position.
  if (this->timer->HasEvent("guojieshaking") &&
      this->timer->GetEventUsedTime("guojieshaking") > 0.085f &&
      gameCharacters["weiqing"]->GetState() != GameCharacterState::FIGHTING) {
    gameCharacters["liuche"]->SetState(GameCharacterState::SAD);
    gameCharacters["weizifu"]->SetState(GameCharacterState::SAD);
    gameCharacters["weiqing"]->SetState(GameCharacterState::FIGHTING);
    // if (SoundEngine::GetInstance().IsPlaying("background_relax")) {
    //   SoundEngine::GetInstance().StopSound("background_relax");
    // }
    // SoundEngine::GetInstance().PlaySound("background_fight0", false);
    SoundEngine::GetInstance().StartBackgroundMusic(/*isFighting=*/true);
  }

  // Check if the event timer for shaking the screen has expired.
  if (this->timer->HasEvent("guojieshaking") &&
      this->timer->IsEventTimerExpired("guojieshaking")) {
    postProcessor->SetShake(false);
    postProcessor->SetBlur(false);
    SoundEngine& soundEngine = SoundEngine::GetInstance();
    if (soundEngine.GetVolume("earthquake") == 1.f) {
      /*this->timer->CleanEvent("guojieshaking");*/
      SoundEngine& soundEngine = SoundEngine::GetInstance();
      if (soundEngine.IsPlaying("wood_collide")) {
        soundEngine.StopSound("wood_collide");
      }
      if (soundEngine.IsPlaying("earthquake")) {
        soundEngine.GraduallyChangeVolume("earthquake", 0.f, 0.8f);
      }
    }
    this->timer->CleanEvent("guojieshaking");
  }

  // Update all the particles
  shadowTrailSystem->Update(dt);
  explosionSystem->Update(dt);

  // Update all the falling bubbles
  std::vector<int> toRemove;
  for (auto& [id, bubble] : fallings) {
    // update the position of the falling bubble
    bubble->Move(dt);
    // Apply gravity to the falling bubble
    bubble->ApplyGravity(dt);
    // If the falling bubble pass the bottom of the game board, then we remove
    // it from the falling bubbles.
    if (bubble->GetPosition().y >
        gameBoard->GetPosition().y + gameBoard->GetSize().y) {
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
      } else {
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
      } else {
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

  // Refreshing the score
  if (this->timer->HasEvent("refreshscore") &&
      this->timer->IsEventTimerExpired("refreshscore")) {
    assert(!this->scoreIncrements.empty() && numOfScoreIncrementsReady > 0 &&
           "The score increments should not be empty.");
    int increment = this->scoreIncrements.front();
    this->scoreIncrements.pop();
    --numOfScoreIncrementsReady;
    this->IncreaseScore(increment);
    if (numOfScoreIncrementsReady > 0) {
      this->timer->SetEventTimer("refreshscore", 0.05f);
      this->timer->StartEventTimer("refreshscore");
      // Increase the opacity of the score text.
      float alpha = this->texts.at("score")->GetAlpha();
      if (alpha < 1.f) {
        alpha = std::min(1.f, alpha + 0.05f);
      }
      this->texts.at("score")->SetAlpha(alpha);
      // Change color to pink gradually.
      glm::vec3 color = this->texts.at("score")->GetColor();
      if (color != kScoreColorPink) {
        if (color.r < kScoreColorPink.r) {
          color.r = std::min(kScoreColorPink.r, color.r + 0.1f);
        } else {
          color.r = std::max(kScoreColorPink.r, color.r - 0.1f);
        }
        if (color.g < kScoreColorPink.g) {
          color.g = std::min(kScoreColorPink.g, color.g + 0.1f);
        } else {
          color.g = std::max(kScoreColorPink.g, color.g - 0.1f);
        }
        if (color.b < kScoreColorPink.b) {
          color.b = std::min(kScoreColorPink.b, color.b + 0.1f);
        } else {
          color.b = std::max(kScoreColorPink.b, color.b - 0.1f);
        }
        this->texts.at("score")->SetColor(color);
      } else {
        if (!this->timer->HasEvent("reduceflipvolume")) {
          this->timer->SetEventTimer("reduceflipvolume",
                                     1.5f + 0.01f * numOfScoreIncrementsReady);
          this->timer->StartEventTimer("reduceflipvolume");
        }
      }
      float volume = 1.f;
      if (this->timer->HasEvent("reduceflipvolume")) {
        if (!this->timer->IsEventTimerExpired("reduceflipvolume")) {
          volume =
              glm::mix(0.f, 1.f,
                       this->timer->GetEventRemainingTime("reduceflipvolume") /
                           this->timer->GetEventTimer("reduceflipvolume"));
        } else {
          volume = 0.f;
        }
      }
      if (volume > 0.f) {
        SoundEngine::GetInstance().PlaySound("flip_paper", false, volume);
      }
    } else {
      this->timer->CleanEvent("refreshscore");
      this->timer->CleanEvent("reduceflipvolume");
      this->timer->SetEventTimer("displayscore", 1.5f);
      this->timer->StartEventTimer("displayscore");
    }
  } else if (this->timer->HasEvent("displayscore") &&
             this->timer->IsEventTimerExpired("displayscore")) {
    // Decrease the opacity of the score text.
    float alpha = this->texts.at("score")->GetAlpha();
    if (alpha > kScoreAlpha) {
      alpha -= 0.8f * dt;
    }
    if (alpha < kScoreAlpha) {
      alpha = kScoreAlpha;
    }
    if (alpha == kScoreAlpha) {
      this->timer->CleanEvent("displayscore");
    }
    // Change color to orange gradually.
    glm::vec3 color = this->texts.at("score")->GetColor();
    if (color != kScoreColorOrange) {
      if (color.r < kScoreColorOrange.r) {
        color.r = std::min(kScoreColorOrange.r, color.r + 1.2f * dt);
      } else {
        color.r = std::max(kScoreColorOrange.r, color.r - 1.2f * dt);
      }
      if (color.g < kScoreColorOrange.g) {
        color.g = std::min(kScoreColorOrange.g, color.g + 1.2f * dt);
      } else {
        color.g = std::max(kScoreColorOrange.g, color.g - 1.2f * dt);
      }
      if (color.b < kScoreColorOrange.b) {
        color.b = std::min(kScoreColorOrange.b, color.b + 1.2f * dt);
      } else {
        color.b = std::max(kScoreColorOrange.b, color.b - 1.2f * dt);
      }
      this->texts.at("score")->SetColor(color);
    }
    this->texts.at("score")->SetAlpha(alpha);
  }
}

void GameManager::Render() {
  if (this->state == GameState::EXIT) {
    return;
  } else if (this->state == GameState::PRELOAD &&
             this->targetState == GameState::UNDEFINED) {
    ResourceManager& resourceManager = ResourceManager::GetInstance();

    spriteRenderer->DrawSprite(resourceManager.GetTexture("logo"),
                               glm::vec2(0, 0),
                               glm::vec2(this->width, this->height), 0.0f,
                               glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    return;
  } else if ((this->state == GameState::SPLASH_SCREEN &&
              this->targetState == GameState::UNDEFINED) ||
             this->targetState == GameState::SPLASH_SCREEN) {
    if (!this->postProcessor->IsChaos()) {
      return;
    }
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    postProcessor->BeginRender();
    /*   float originalSampleOffsets = 1.f / 30000.f;*/
    /*   float targetIntensity = 0.6f;*/
    float targetSampleOffsets = 1.f / 1000.f;
    float redChannelMin = 0.f, greenChannelMin = 0.f, blueChannelMin = 0.f;
    if (!timer->HasEvent("splash") &&
        this->postProcessor->GetSampleOffsets() <= targetSampleOffsets) {
      const float interpolationFactor =
          this->postProcessor->GetSampleOffsets() / targetSampleOffsets;
      redChannelMin = glm::mix(1.f, 0.f, interpolationFactor);
      greenChannelMin = glm::mix(1.f, 0.f, interpolationFactor);
      blueChannelMin = glm::mix(1.f, 0.f, interpolationFactor);
    }
    glm::vec2 redChannelRange = glm::vec2(redChannelMin, 1.f);
    glm::vec2 greenChannelRange = glm::vec2(greenChannelMin, 1.f);
    glm::vec2 blueChannelRange = glm::vec2(blueChannelMin, 1.f);
    glm::vec2 alphaChannelRange = glm::vec2(0.f, 1.f);
    partialTextureRenderer->DrawPartialTexture(
        resourceManager.GetTexture("splash"),
        /*position=*/glm::vec2(0, 0),
        /*size=*/glm::vec2(this->width, this->height),
        /*redChannelRange=*/redChannelRange,
        /*greenChannelRange=*/greenChannelRange,
        /*blueChannelRange=*/blueChannelRange,
        /*alphaChannelRange=*/alphaChannelRange);
    postProcessor->EndRender();
    postProcessor->Render(glfwGetTime());
    return;
  } else if (this->state == GameState::INTRO) {
    // type the introduction text
    texts.at("introduction")
        ->Draw(textRenderers.at(language), /*centered=*/true);
    // Draw a Black Overlay used for text fading
    if (timer->HasEvent("introFadeOut")) {
      float totalTime = timer->GetEventTimer("introFadeOut");
      float remainingTime = timer->GetEventRemainingTime("introFadeOut");
      float newAlpha = 1.0f - remainingTime / totalTime;
      colorRenderer->DrawColor(
          glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f,
          glm::vec2(0.5f, 0.5f), glm::vec4(0.0f, 0.0f, 0.0f, newAlpha));
    }
    return;
  }

  // Background
  ResourceManager& resourceManager = ResourceManager::GetInstance();

  postProcessor->BeginRender();

  spriteRenderer->DrawSprite(resourceManager.GetTexture("background"),
                             glm::vec2(0, 0),
                             glm::vec2(this->width, this->height), 0.0f,
                             glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

  auto textRenderer = textRenderers.at(language);

  // Draw shadow particles
  shadowTrailSystem->Draw(/*isDarkBackGround=*/true);

  if (this->state == GameState::ACTIVE || this->state == GameState::PREPARING ||
      this->state == GameState::WIN || this->state == GameState::LOSE) {
    // Create cracks on the ground when guojie lands on the target position.
    if (this->timer->HasEvent("cracks") ||
        this->timer->HasEvent("cracksFading")) {
      glm::vec2 cracksPosition =
          gameCharacters["guojie"]->GetPosition() +
          glm::vec2(gameCharacters["guojie"]->GetSize().x * 0.0f,
                    gameCharacters["guojie"]->GetSize().y * 0.84f);
      float crackswidth = gameCharacters["guojie"]->GetSize().x * 1.5f;
      float cracksheight = crackswidth * 0.56479f;
      /*spriteRenderer->DrawSprite(resourceManager.GetTexture("cracks"),
       * cracksPosition, glm::vec2(crackswidth, cracksheight));*/
      // Set the alpha channel range based on the remaining time of the event
      // timer.
      glm::vec2 alphaChannelRange = glm::vec2(0.0f, 1.0f);
      if (this->timer->HasEvent("cracksFading")) {
        alphaChannelRange =
            glm::vec2(0.0f, this->timer->GetEventRemainingTime("cracksFading") /
                                this->timer->GetEventTimer("cracksFading"));
      }
      partialTextureRenderer->DrawPartialTexture(
          resourceManager.GetTexture("cracks"),
          /*position=*/cracksPosition,
          /*size=*/glm::vec2(crackswidth, cracksheight),
          /*redChannelRange=*/glm::vec2(0.f, 0.15686f),
          /*greenChannelRange=*/glm::vec2(0.f, 0.07843f),
          /*blueChannelRange=*/glm::vec2(0.f, 0.03922f),
          /*alphaChannelRange=*/alphaChannelRange);
      if (this->timer->HasEvent("cracks") &&
          this->timer->IsEventTimerExpired("cracks")) {
        this->timer->CleanEvent("cracks");
        this->timer->SetEventTimer("cracksFading", 2.f);
        this->timer->StartEventTimer("cracksFading");
      } else if (this->timer->HasEvent("cracksFading") &&
                 this->timer->IsEventTimerExpired("cracksFading")) {
        this->timer->CleanEvent("cracksFading");
      }
    }

    // Draw all sad and happy state of wei zi fu
    /*  gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);*/
    gameCharacters["weizifu"]->Draw(spriteRenderer);

    gameCharacters["liuche"]->Draw(spriteRenderer);

    if (gameCharacters["guojie"]->IsMoving()) {
      // Do not draw health bar when guojie is moving.
      gameCharacters["guojie"]->Draw(spriteRenderer);
      gameCharacters["weiqing"]->Draw(spriteRenderer);
    } else {
      gameCharacters["guojie"]->DrawGameCharacter(spriteRenderer, colorRenderer,
                                                  circleRenderer, textRenderer);
      gameCharacters["weiqing"]->DrawGameCharacter(
          spriteRenderer, colorRenderer, circleRenderer, textRenderer);
    }

    for (auto& arrow : arrows) {
      if (arrow->IsPenetrating() || arrow->IsStopped()) {
        arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
      } else {
        arrow->Draw(spriteRenderer);
      }
    }

    if (this->state == GameState::ACTIVE ||
        this->state == GameState::PREPARING) {
      if (scroll->GetState() != ScrollState::DISABLED) {
        if (this->gameArenaShaking) {
          // Memorize the current positions for each shaking object.
          originalPositionsForShaking.clear();
          originalPositionsForShaking["scroll"] = this->scroll->GetCenter();
          originalPositionsForShaking["gameboard"] = gameBoard->GetPosition();
          originalPositionsForShaking["shooter"] = shooter->GetPosition();
          for (auto& [id, bubble] : moves) {
            originalPositionsForShaking[std::to_string(id)] =
                bubble->GetPosition();
          }
          for (auto& [id, bubble] : statics) {
            originalPositionsForShaking[std::to_string(id)] =
                bubble->GetPosition();
          }
          originalPositionsForShaking["time"] = texts["time"]->GetCenter();

          // start shake effect
          float shakingStrengthForX =
              kBaseUnit * 0.6048f;  // Shaking intensity for the X axis
          float shakingStrengthForY =
              kBaseUnit * 0.3402f;  // Shaking intensity for the Y axis
          float timeMultiplierForX =
              60.0f;  // Controls the frequency of shaking along the X axis
          float timeMultiplierForY =
              160.0f;  // Controls the frequency of shaking along the Y axis
          float currentTime = glfwGetTime();
          // Calculate the shake offset based on the current time
          float shakeOffsetX =
              cos(currentTime * timeMultiplierForX) * shakingStrengthForX;
          float shakeOffsetY =
              cos(currentTime * timeMultiplierForY) * shakingStrengthForY;
          // shake the scroll
          scroll->SetCenter(scroll->GetCenter() +
                            glm::vec2(shakeOffsetX, shakeOffsetY));
          // shake the game board
          gameBoard->SetPosition(gameBoard->GetPosition() +
                                 glm::vec2(shakeOffsetX, shakeOffsetY));
          // shake the shooter
          shooter->SetPosition(shooter->GetPosition() +
                               glm::vec2(shakeOffsetX, shakeOffsetY));
          // Update the path of the ray
          shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(),
                                       this->statics);
          // shake the moving bubbles
          for (auto& [id, bubble] : moves) {
            bubble->SetPosition(bubble->GetPosition() +
                                glm::vec2(shakeOffsetX, shakeOffsetY));
          }
          // shake the static bubbles
          for (auto& [id, bubble] : statics) {
            bubble->SetPosition(bubble->GetPosition() +
                                glm::vec2(shakeOffsetX, shakeOffsetY));
          }
          // shake the time text
          // texts["time"]->SetPosition(texts["time"]->GetPosition() +
          //                           glm::vec2(shakeOffsetX, shakeOffsetY));
          texts["time"]->SetCenter(texts["time"]->GetCenter() +
                                   glm::vec2(shakeOffsetX, shakeOffsetY));
        }

        scroll->Draw(spriteRenderer);

        // Enable scissor test
        ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
        handler.EnableScissorTest();

        // Set the scissor box based on the upper and lower scroll
        glm::vec4 silkBounds = scroll->GetSilkBounds();
        handler.SetScissorBox(silkBounds[0], this->height - silkBounds[3],
                              scroll->GetSilkWidth(), scroll->GetSilkLen());
        /* glScissor(silkBounds[0], this->height - silkBounds[3],
         * scroll->GetSilkWidth(), scroll->GetSilkLen());*/

        // GameBoard
        gameBoard->Draw(spriteRenderer);

        // Shooter
        shooter->Draw(spriteRenderer);

        //// Ray
        /*  shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(),
         * this->statics);*/
        shooter->GetRay().Draw(rayRenderer);

        // Particles
        explosionSystem->Draw(/*isDarkBackground=*/false);

        // Draw all moving bubbles
        for (auto& bubble : moves) {
          bubble.second->Draw(spriteRenderer);
        }

        // Draw all static bubbles
        for (auto& bubble : statics) {
          bubble.second->Draw(spriteRenderer);
        }

        //// Draw all the free slots
        // for (auto slot : this->freeSlots) {
        //     spriteRenderer->DrawSprite(resourceManager.GetTexture("bubble"),
        //     glm::vec2(slot.x-kBubbleRadius, slot.y-kBubbleRadius),
        //     glm::vec2(2*kBubbleRadius, 2 * kBubbleRadius), 0.f,
        //     glm::vec2(0.5,0.5), glm::vec4(0.5,0.5,0.5,0.7));
        // }

        // Draw all falling bubbles
        for (auto& bubble : fallings) {
          bubble.second->Draw(spriteRenderer);
        }

        // Disable scissor test
        /*glDisable(GL_SCISSOR_TEST);*/
        handler.DisableScissorTest();

        // Draw the time when the scroll is opened
        if (this->scroll->GetState() == ScrollState::OPENED) {
          texts["time"]->Draw(textRenderer, true);
        }

        // Restore the original positions for each shaking object
        if (this->gameArenaShaking) {
          this->scroll->SetCenter(originalPositionsForShaking["scroll"]);
          gameBoard->SetPosition(originalPositionsForShaking["gameboard"]);
          shooter->SetPosition(originalPositionsForShaking["shooter"]);
          shooter->GetRay().UpdatePath(this->gameBoard->GetBoundaries(),
                                       this->statics);
          for (auto& [id, bubble] : moves) {
            bubble->SetPosition(
                originalPositionsForShaking[std::to_string(id)]);
          }
          for (auto& [id, bubble] : statics) {
            bubble->SetPosition(
                originalPositionsForShaking[std::to_string(id)]);
          }
          // texts["time"]->SetPosition(originalPositionsForShaking["time"]);
          texts["time"]->SetCenter(originalPositionsForShaking["time"]);
        }
      }
    } else {
      if (scroll->GetState() != ScrollState::DISABLED) {
        scroll->Draw(spriteRenderer);
      }
      // Particles
      explosionSystem->Draw();
    }
  } else if (this->state == GameState::INITIAL ||
             this->state == GameState::STORY ||
             this->state == GameState::CONTROL ||
             this->state == GameState::DIFFICULTY_SETTINGS ||
             this->state == GameState::DISPLAY_SETTINGS ||
             this->state == GameState::LANGUAGE_PREFERENCE) {
    gameCharacters["weizifu"]->Draw(spriteRenderer);
    gameCharacters["liuche"]->Draw(spriteRenderer);
    bool isPausedDuringGame = this->state == GameState::CONTROL &&
                              this->lastState == GameState::ACTIVE;
    if (isPausedDuringGame) {
      gameCharacters["guojie"]->DrawGameCharacter(spriteRenderer, colorRenderer,
                                                  circleRenderer, textRenderer);
      gameCharacters["weiqing"]->DrawGameCharacter(
          spriteRenderer, colorRenderer, circleRenderer, textRenderer);
    } else {
      gameCharacters["weiqing"]->Draw(spriteRenderer);
      if (this->targetState == GameState::PREPARING) {
        gameCharacters["guojie"]->Draw(spriteRenderer);
      }
    }

    // Draw arrows when the game is paused but not stopped.
    if (this->state == GameState::CONTROL &&
        this->lastState == GameState::ACTIVE) {
      for (auto& arrow : arrows) {
        if (arrow->IsPenetrating() || arrow->IsStopped()) {
          arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
        } else {
          arrow->Draw(spriteRenderer);
        }
      }
    }

    // Draw scroll
    scroll->Draw(spriteRenderer);
    // Enable scissor test
    /* glEnable(GL_SCISSOR_TEST);*/
    ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
    handler.EnableScissorTest();

    // Set the scissor box based on the upper and lower scroll
    glm::vec4 silkBounds = scroll->GetSilkBounds();
    /*glScissor(silkBounds[0], this->height - silkBounds[3],
     * scroll->GetSilkWidth(), scroll->GetSilkLen());*/
    handler.SetScissorBox(silkBounds[0], this->height - silkBounds[3],
                          scroll->GetSilkWidth(), scroll->GetSilkLen());
    // Draw the game board if it is ACTIVE
    if (gameBoard->GetState() == GameBoardState::ACTIVE) {
      gameBoard->Draw(spriteRenderer);
    }

    if (!activePage.empty()) {
      pages.at(activePage)->Draw();
    }

    handler.DisableScissorTest();
  }

  if (!this->graduallyTransparentObjects.empty()) {
    for (auto& item : graduallyTransparentObjects) {
      item.second.first->Draw(spriteRenderer);
      // if it's a game character, then draw the carried objects.
      if (std::shared_ptr<GameCharacter> character =
              std::dynamic_pointer_cast<GameCharacter>(item.second.first)) {
        auto carriedObjects = character->GetCarriedObjects();
        for (auto& [id, carriedObject] : carriedObjects) {
          if (std::shared_ptr<Arrow> arrow =
                  std::dynamic_pointer_cast<Arrow>(carriedObject)) {
            arrow->Draw(spriteDynamicRenderer, arrow->GetTextureCoords());
          } else {
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
      texts["victory"]->Draw(textRenderer, true);
    } else {
      texts["defeated"]->Draw(textRenderer, true);
    }
    if (this->timer->HasEvent("prompttomainmenu") &&
        !this->timer->IsPaused("prompttomainmenu")) {
      texts["prompttomainmenu"]->Draw(textRenderer, true);
    }
  }

  // Draw the score on the top right corner of the screen.
  // if (this->state == GameState::PREPARING || this->state == GameState::ACTIVE
  // || this->state == GameState::WIN || this->state == GameState::LOSE ||
  //    (this->state == GameState::CONTROL &&
  //     this->lastState == GameState::ACTIVE)) {
  //
  //  texts.at("score")->Draw(textRenderer, /*centerAligned=*/false,
  //                          /*rightAligned=*/true);
  //}
  texts.at("score")->Draw(textRenderer, /*centerAligned=*/false,
                          /*rightAligned=*/true);

  // Render the mouse cursor on the silk area.
  if (hideDefaultMouseCursor) {
    spriteRenderer->DrawSprite(
        resourceManager.GetTexture("mouse"),
        glm::vec2(this->mouseX, this->mouseY),
        glm::vec2(
            kMouseCursorWidth *
                (this->width / Renderer::GetActualWindowSizePadding().width),
            kMouseCursorHeight *
                (this->height / Renderer::GetActualWindowSizePadding().height)),
        0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  }
}

int GameManager::GetNumGameLevels() {
  switch (difficulty) {
    case Difficulty::EASY:
      return 10;
    case Difficulty::MEDIUM:
      return 20;
    case Difficulty::HARD:
      return 30;
    case Difficulty::EXPERT:
      return 40;
    default:
      return 20;
  }
}

std::string GameManager::GetPageName(GameState gameState) {
  switch (gameState) {
    case GameState::STORY:
      return "story";
    case GameState::CONTROL:
      return "controls";
    case GameState::DIFFICULTY_SETTINGS:
      return "difficulty";
    case GameState::DISPLAY_SETTINGS:
      return "displaysettings";
    case GameState::LANGUAGE_PREFERENCE:
      return "languagepreference";
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
    for (const auto& sectionName : pages.at(lastPage)->GetOrder()) {
      auto section = pages.at(lastPage)->GetSection(sectionName);
      for (const auto& contentName : section->GetOrder()) {
        if (auto buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(
                section->GetContent(contentName))) {
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
        if (auto buttonUnit = std::dynamic_pointer_cast<ButtonUnit>(
                section->GetContent(contentName))) {
          buttonUnit->GetButton()->SetState(ButtonState::kNormal);
        }
      }
    }
    // if (this->state == GameState::CONTROL && this->lastState ==
    // GameState::ACTIVE) {
    //   // Consider buttons "Resume", "Restart", and "Stop" size.
    //   auto buttonSection =
    //   pages.at(activePage)->GetSection("controlbuttonsection");
    //   buttonSection->SetOrder({"resume", "restart", "stop"});
    // }
    //  Refresh the position of each units in the active page.
    pages.at(activePage)->UpdatePosition();
  }
  if (this->state == GameState::WIN || this->state == GameState::LOSE) {
    // Store the final score to the global config.
    ConfigManager& configManager = ConfigManager::GetInstance();

    // As there might be some score increments that are not added to the total
    // score yet, we need to create a temporary variable to store the total
    // score
    int tempScore = this->score;
    std::queue<int> tempScoreIncrements = this->scoreIncrements;
    while (!tempScoreIncrements.empty()) {
      tempScore += tempScoreIncrements.front();
      tempScoreIncrements.pop();
    }
    configManager.SetScore(tempScore);
  } else if (this->state == GameState::INITIAL ||
             this->state == GameState::STORY) {
    // Reset the background music to the main theme.
    SoundEngine& soundEngine = SoundEngine::GetInstance();
    /*std::string currentMusic = soundEngine.GetPlayingBackgroundMusic();
    if (!currentMusic.empty() && soundEngine.IsFightingMusic(currentMusic)) {
      soundEngine.StopSound(currentMusic);
    }
    if (!soundEngine.IsPlaying("background_relax")) {
      soundEngine.PlaySound("background_relax", true);
    }*/
    soundEngine.StartBackgroundMusic(/*isFighting=*/false);
  } else if (this->state == GameState::SPLASH_SCREEN) {
    // Play the white noise sound effect.
    SoundEngine& soundEngine = SoundEngine::GetInstance();
    if (!soundEngine.IsPlaying("white_noise")) {
      soundEngine.PlaySound("white_noise", false);
    }
  }
}

void GameManager::SetToTargetState() {
  SetState(this->targetState);
  this->targetState = GameState::UNDEFINED;
}

GameState GameManager::GetState() { return this->state; }

void GameManager::GoToState(GameState newState) {
  this->targetState = newState;
  this->transitionState = TransitionState::START;
}

void GameManager::GoToScreenMode(ScreenMode newScreenMode) {
  this->targetScreenMode = newScreenMode;
}

void GameManager::SetScreenMode(ScreenMode newScreenMode) {
  assert(newScreenMode != ScreenMode::UNDEFINED &&
         "The screen mode is not defined.");
  this->screenMode = newScreenMode;
  // Store the screen mode to the global config.
  ConfigManager& configManager = ConfigManager::GetInstance();
  configManager.SetScreenMode(this->screenMode);
}

ScreenMode GameManager::GetScreenMode() const { return this->screenMode; }

void GameManager::SetToTargetScreenMode() {
  SetScreenMode(this->targetScreenMode);
  this->targetScreenMode = ScreenMode::UNDEFINED;
}

void GameManager::SetLanguage(Language newLanguage) {
  assert(newLanguage != Language::UNDEFINED && "The language is not defined.");
  this->language = newLanguage;
  // Store the language to the global config.
  ConfigManager& configManager = ConfigManager::GetInstance();
  configManager.SetLanguage(this->language);
  LoadTexts();
  LoadButtons();
  // Update page components' position based on the new language.
  for (const auto& [pageName, page] : pages) {
    // No need to update text renderers for the language preference page. As
    // they are preloaded and fixed.
    if (pageName == "languagepreference") {
      continue;
    }
    page->SetCompenentsTextRenderer(textRenderers.at(language));
    page->UpdateComponentsHeight();
    page->SetPosition(glm::vec2(
        this->gameBoard->GetPosition().x,
        std::max(this->gameBoard->GetCenter().y - page->GetHeight() * 0.5f,
                 this->gameBoard->GetPosition().y)));

    // Reinit the scroll icon of each section
    for (const auto& sectionName : page->GetOrder()) {
      auto section = page->GetSection(sectionName);
      section->UpdateScrollIconAndSectionOffset();
    }
  }
  // Update the position of all components in the active page.
  if (!activePage.empty()) {
    pages.at(activePage)->UpdatePosition();
  }
}

void GameManager::SetDifficulty(Difficulty newDifficulty) {
  assert(newDifficulty != Difficulty::UNDEFINED &&
         "The difficulty is not defined.");
  this->difficulty = newDifficulty;
  // Set the total health based on the difficulty.
  this->gameCharacters.at("guojie")->GetHealth().SetTotalHealth(
      this->GetNumGameLevels());
  this->gameCharacters.at("guojie")->GetHealth().SetCurrentHealth(
      this->GetNumGameLevels());
  // Store the difficulty to the global config.
  ConfigManager& configManager = ConfigManager::GetInstance();
  configManager.SetDifficulty(this->difficulty);
}

Difficulty GameManager::GetDifficulty() const { return this->difficulty; }

void GameManager::LoadTextRenderer() {
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  // Load renderers for all languages
  for (const auto& [language, benchmark] : benchmark_char_map) {
    if (textRenderers.find(language) == textRenderers.end()) {
      switch (language) {
        case Language::GERMAN:
        case Language::ENGLISH:
        case Language::SPANISH:
        case Language::ITALIAN:
        case Language::FRENCH:
        case Language::KOREAN:
        case Language::PORTUGUESE_BR:
        case Language::PORTUGUESE_PT:
        case Language::RUSSIAN:

          textRenderers[language] = std::make_shared<WesternTextRenderer>(
              resourceManager.GetShader("text"), this->width, this->height,
              benchmark);
          break;
        default:
          textRenderers[language] = std::make_shared<CJTextRenderer>(
              resourceManager.GetShader("text"), this->width, this->height,
              benchmark);
      }
    }
  }
}

void GameManager::ReloadTextRenderer() {
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  // Load renderers for all languages
  for (const auto& [language, benchmark] : benchmark_char_map) {
    switch (language) {
      case Language::GERMAN:
      case Language::ENGLISH:
      case Language::SPANISH:
      case Language::ITALIAN:
      case Language::FRENCH:
      case Language::KOREAN:
      case Language::PORTUGUESE_BR:
      case Language::PORTUGUESE_PT:
      case Language::RUSSIAN:

        textRenderers[language] = std::make_shared<WesternTextRenderer>(
            resourceManager.GetShader("text"), this->width, this->height,
            benchmark);
        break;
      default:
        textRenderers[language] = std::make_shared<CJTextRenderer>(
            resourceManager.GetShader("text"), this->width, this->height,
            benchmark);
    }
  }
}

void GameManager::LoadCommonCharacters() {
  std::unordered_set<char32_t> benchmarkChars;
  for (const auto& [language, benchmark] : benchmark_char_map) {
    benchmarkChars.insert(benchmark);
  }
  std::u32string controlCharacters = U"1234567890|gjpqyN ";
  for (const auto& benchmark : benchmarkChars) {
    controlCharacters.push_back(benchmark);
  }
  TextRenderer::Load(controlCharacters);
}

void GameManager::LoadTexts() {
  static int count = 0;
  ++count;
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  bool hashex5A = false;
  // Load the texts for the game
  resourceManager.LoadText(
      ConfigManager::GetInstance().GetTextFilePath().c_str());
  // Initialize text box
  std::vector<std::u32string> textsToLoad;
  textsToLoad.push_back(resourceManager.GetText("story", "1"));
  textsToLoad.push_back(resourceManager.GetText("story", "2"));
  textsToLoad.push_back(resourceManager.GetText("story", "3"));
  if (texts.find("story") == texts.end()) {
    texts["story"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(gameBoard->GetPosition().x + kBaseUnit / 2.0f,
                          gameBoard->GetPosition().y + kBaseUnit / 2.0f),
        /*lineWidth=*/gameBoard->GetSize().x - 2 * kBaseUnit);
    for (const auto& textToLoad : textsToLoad) {
      texts["story"]->AddParagraph(textToLoad);
    }
    texts["story"]->SetScale(0.026f / kFontScale);
  } else {
    for (size_t i = 0; i < textsToLoad.size(); ++i) {
      texts["story"]->SetParagraph(i, textsToLoad[i]);
    }
  }

  textsToLoad.clear();
  textsToLoad.push_back(resourceManager.GetText("controls", "1"));
  textsToLoad.push_back(resourceManager.GetText("controls", "2"));
  textsToLoad.push_back(resourceManager.GetText("controls", "3"));
  textsToLoad.push_back(resourceManager.GetText("controls", "4"));
  if (texts.find("controls") == texts.end()) {
    texts["controls"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(gameBoard->GetPosition().x + kBaseUnit / 2.0f,
                          gameBoard->GetPosition().y + kBaseUnit / 2.0f),
        /*lineWidth=*/gameBoard->GetSize().x - kBaseUnit);
    for (const auto& textToLoad : textsToLoad) {
      texts["controls"]->AddParagraph(textToLoad);
    }
    texts["controls"]->SetScale(0.0216f / kFontScale);
  } else {
    for (size_t i = 0; i < textsToLoad.size(); ++i) {
      texts["controls"]->SetParagraph(i, textsToLoad[i]);
    }
  }

  textsToLoad.clear();

  if (texts.find("victory") == texts.end()) {
    texts["victory"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(0, 0),
        /*lineWidth=*/std::numeric_limits<float>::max(), /*boxBounds=*/
        glm::vec4(0.f, 0.f, std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max()));
    texts["victory"]->AddParagraph(resourceManager.GetText("victory"));
    texts["victory"]->SetColor(glm::vec3(1.f, 0.f, 0.f));
    texts["victory"]->SetTargetScale("max", 2.f / kFontScale);
    texts["victory"]->SetScale(texts["victory"]->GetTargetScale("max"));
    texts["victory"]->SetCenter(
        glm::vec2(this->width / 2.0f, this->height / 2.0f));
  } else {
    texts["victory"]->SetParagraph(0, resourceManager.GetText("victory"));
  }

  if (texts.find("defeated") == texts.end()) {
    texts["defeated"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(0, 0),
        /*lineWidth=*/std::numeric_limits<float>::max(), /*boxBounds=*/
        glm::vec4(0.f, 0.f, std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max()));
    texts["defeated"]->AddParagraph(resourceManager.GetText("defeated"));
    texts["defeated"]->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
    texts["defeated"]->SetTargetScale("max", 2.f / kFontScale);
    texts["defeated"]->SetScale(texts["defeated"]->GetTargetScale("max"));
    texts["defeated"]->SetCenter(
        glm::vec2(this->width / 2.0f, this->height / 2.0f));
  } else {
    texts["defeated"]->SetParagraph(0, resourceManager.GetText("defeated"));
  }

  // score.
  if (texts.find("score") == texts.end()) {
    texts["score"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(this->width * 0.98f, this->height * 0.04f),
        /*lineWidth=*/std::numeric_limits<float>::max());
    texts["score"]->AddParagraph(resourceManager.GetText("score"));
    texts["score"]->AddParagraph(U"{" + intToU32String(this->score) + U"}");
    // Set the color of the score to purple.
    /*texts["score"]->SetColor(glm::vec3(1.0f, 0.0f, 0.56471f));*/
    texts["score"]->SetColor(kScoreColorOrange);
    texts["score"]->SetAlpha(kScoreAlpha);
    texts["score"]->SetScale(0.05f / kFontScale);
    texts["score"]->SetLineSpacingFactor(0.5f);
    texts["score"]->SetAdditionalPadding(0.f);
  } else {
    texts["score"]->SetParagraph(0, resourceManager.GetText("score"));
  }

  if (texts.find("prompttomainmenu") == texts.end()) {
    texts["prompttomainmenu"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(this->width * 0.4f, this->height * 0.65f),
        /*lineWidth=*/std::numeric_limits<float>::max());
    texts["prompttomainmenu"]->AddParagraph(
        resourceManager.GetText("prompttomainmenu"));
    texts["prompttomainmenu"]->SetColor(glm::vec3(1.f, 1.f, 1.f));
    texts["prompttomainmenu"]->SetScale(0.025f / kFontScale);
    texts["prompttomainmenu"]->SetCenter(
        glm::vec2(this->width / 2.0f, this->height * 0.65f));
  } else {
    texts["prompttomainmenu"]->SetParagraph(
        0, resourceManager.GetText("prompttomainmenu"));
  }

  if (texts.find("introduction") == texts.end()) {
    texts["introduction"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(this->width * 0.4f, this->height * 0.65f),
        /*lineWidth=*/this->width * 0.5f);
    texts["introduction"]->AddParagraph(
        resourceManager.GetText("introduction"));
    texts["introduction"]->SetColor(glm::vec3(1.f, 1.f, 1.f));
    texts["introduction"]->SetScale(0.03f / kFontScale);
    texts["introduction"]->SetCenter(
        glm::vec2(this->width / 2.f, this->height / 2.f));
    texts["introduction"]->EnableTypingEffect(typingSpeeds.at(this->language));
  }

  if (texts.find("time") == texts.end()) {
    texts["time"] = std::make_shared<Text>(
        /*pos=*/glm::vec2(
            gameBoard->GetPosition().x + gameBoard->GetSize().x - 2 * kBaseUnit,
            gameBoard->GetPosition().y - kBaseUnit),
        /*lineWidth=*/gameBoard->GetSize().x);
    texts["time"]->AddParagraph(U"30");
    texts["time"]->SetScale(0.025f / kFontScale);
    // Get the center of the text "time"
    glm::vec2 centerTime;
    centerTime.x =
        gameBoard->GetPosition().x + gameBoard->GetSize().x - kBaseUnit;
    centerTime.y = gameBoard->GetPosition().y -
                   this->scroll->GetTopRoller()->GetSize().y * 0.37f;
    texts["time"]->SetCenter(centerTime);
  }

  std::vector<std::string> clickableOptionNames = {
      "fullscreen", "windowedborderless", "windowed"};
  for (const auto& clickableOptionName : clickableOptionNames) {
    if (texts.find(clickableOptionName) != texts.end()) {
      if (clickableOptionName == "fullscreen" ||
          clickableOptionName == "windowedborderless" ||
          clickableOptionName == "windowed") {
        texts[clickableOptionName]->SetParagraph(
            0, resourceManager.GetText("screenmode", clickableOptionName));
      }
    }
  }

  clickableOptionNames = {"easy", "medium", "hard", "expert"};
  for (const auto& clickableOptionName : clickableOptionNames) {
    if (texts.find(clickableOptionName) != texts.end()) {
      texts[clickableOptionName]->SetParagraph(
          0, resourceManager.GetText("difficulty", clickableOptionName));
    }
  }

  // Create character introduction units
  std::vector<std::string> gameCharacterNames = {"liuche", "weizifu", "weiqing",
                                                 "guojie"};
  for (const auto& gameCharacterName : gameCharacterNames) {
    if (texts.find(gameCharacterName + "intro") == texts.end()) {
      texts[gameCharacterName + "intro"] = std::make_shared<Text>(
          /*pos=*/glm::vec2(0.f),
          /*lineWidth=*/gameBoard->GetSize().x - kBaseUnit);
      texts[gameCharacterName + "intro"]->AddParagraph(
          resourceManager.GetText("characterintro", gameCharacterName));
      texts[gameCharacterName + "intro"]->SetScale(0.0216f / kFontScale);
    } else {
      texts[gameCharacterName + "intro"]->SetParagraph(
          0, resourceManager.GetText("characterintro", gameCharacterName));
    }
  }
}

void GameManager::ReloadTexts() {
  ResourceManager& resourceManager = ResourceManager::GetInstance();

  // Initialize text box
  std::vector<std::u32string> textsToLoad;
  textsToLoad.push_back(resourceManager.GetText("story", "1"));
  textsToLoad.push_back(resourceManager.GetText("story", "2"));
  texts["story"]->RemoveAllParagraphs(false);
  for (size_t i = 0; i < textsToLoad.size(); ++i) {
    texts["story"]->AddParagraph(textsToLoad[i]);
  }

  textsToLoad.clear();
  textsToLoad.push_back(resourceManager.GetText("controls", "1"));
  textsToLoad.push_back(resourceManager.GetText("controls", "2"));
  textsToLoad.push_back(resourceManager.GetText("controls", "3"));
  textsToLoad.push_back(resourceManager.GetText("controls", "4"));
  texts["controls"]->RemoveAllParagraphs(false);
  for (size_t i = 0; i < textsToLoad.size(); ++i) {
    texts["controls"]->AddParagraph(textsToLoad[i]);
  }

  textsToLoad.clear();

  texts["victory"]->RemoveAllParagraphs(false);
  texts["victory"]->AddParagraph(resourceManager.GetText("victory"));

  texts["defeated"]->RemoveAllParagraphs(false);
  texts["defeated"]->AddParagraph(resourceManager.GetText("defeated"));

  // score.
  texts["score"]->RemoveAllParagraphs(false);
  texts["score"]->AddParagraph(resourceManager.GetText("score"));
  texts["score"]->AddParagraph(U"{" + intToU32String(this->score) + U"}");

  texts["prompttomainmenu"]->RemoveAllParagraphs(false);
  texts["prompttomainmenu"]->AddParagraph(
      resourceManager.GetText("prompttomainmenu"));

  texts["time"]->RemoveAllParagraphs(false);
  texts["time"]->AddParagraph(U"30");

  std::vector<std::string> clickableOptionNames = {
      "fullscreen", "windowedborderless", "windowed"};
  for (const auto& clickableOptionName : clickableOptionNames) {
    if (texts.find(clickableOptionName) != texts.end()) {
      if (clickableOptionName == "fullscreen" ||
          clickableOptionName == "windowedborderless" ||
          clickableOptionName == "windowed") {
        texts[clickableOptionName]->RemoveAllParagraphs(false);
        texts[clickableOptionName]->AddParagraph(
            resourceManager.GetText("screenmode", clickableOptionName));
      }
    }
  }

  clickableOptionNames = {"easy", "medium", "hard", "expert"};
  for (const auto& clickableOptionName : clickableOptionNames) {
    if (texts.find(clickableOptionName) != texts.end()) {
      texts[clickableOptionName]->RemoveAllParagraphs(false);
      texts[clickableOptionName]->AddParagraph(
          resourceManager.GetText("difficulty", clickableOptionName));
    }
  }

  for (const auto& [languageEnum, clickableOptionName] : language_map) {
    texts[clickableOptionName]->RemoveAllParagraphs(false);
    std::u32string languageName;
    switch (languageEnum) {
      case Language::GERMAN:
        languageName = U"Deutsch";
        break;
      case Language::ENGLISH:
        languageName = U"English";
        break;
      case Language::SPANISH:
        languageName = U"Español";
        break;
      case Language::ITALIAN:
        languageName = U"Italiano";
        break;
      case Language::FRENCH:
        languageName = U"Français";
        break;
      case Language::JAPANESE:
        languageName = U"日本語";
        break;
      case Language::KOREAN:
        languageName = U"한국어";
        break;
      case Language::RUSSIAN:
        languageName = U"Русский";
        break;
      case Language::PORTUGUESE_BR:
        languageName = U"Português (Brasil)";
        break;
      case Language::PORTUGUESE_PT:
        languageName = U"Português (Portugal)";
        break;
      case Language::CHINESE_SIMPLIFIED:
        languageName = U"中文（简体）";
        break;
      case Language::CHINESE_TRADITIONAL:
        languageName = U"中文（繁體）";
        break;
    }
    texts[clickableOptionName]->AddParagraph(languageName);
  }

  // Create character introduction units
  std::vector<std::string> gameCharacterNames = {"liuche", "weizifu", "weiqing",
                                                 "guojie"};
  for (const auto& gameCharacterName : gameCharacterNames) {
    texts[gameCharacterName + "intro"]->RemoveAllParagraphs(false);
    texts[gameCharacterName + "intro"]->AddParagraph(
        resourceManager.GetText("characterintro", gameCharacterName));
  }
}

void GameManager::LoadButtons() {
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  //  Create buttons
  std::vector<std::string> buttonNames = {"back",       "controls",
                                          "start",      "exit",
                                          "restart",    "resume",
                                          "stop",       "displaysettings",
                                          "difficulty", "languagepreference"};
  for (const auto& buttonName : buttonNames) {
    if (buttons.find(buttonName) == buttons.end()) {
      buttons[buttonName] = std::make_shared<Button>(
          glm::vec2(this->width / 2.0f - kBaseUnit * 4.5f,
                    this->height * 0.84f),
          glm::vec2(kBaseUnit * 6.5f, kBaseUnit * 3.0f),
          resourceManager.GetText("button", buttonName));
    } else {
      buttons[buttonName]->GetText().SetParagraph(
          0, resourceManager.GetText("button", buttonName));
    }
  }
}

void GameManager::ReloadButtons() {
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  //  Create buttons
  std::vector<std::string> buttonNames = {"back",       "controls",
                                          "start",      "exit",
                                          "restart",    "resume",
                                          "stop",       "displaysettings",
                                          "difficulty", "languagepreference"};
  for (const auto& buttonName : buttonNames) {
    buttons[buttonName]->GetText().RemoveAllParagraphs();
    buttons[buttonName]->GetText().AddParagraph(
        resourceManager.GetText("button", buttonName));
  }
}

std::shared_ptr<TextRenderer> GameManager::GetTextRenderer() {
  return textRenderers.at(language);
}

void GameManager::SetTransitionState(TransitionState newTransitionState) {
  this->transitionState = newTransitionState;
}

TransitionState GameManager::GetTransitionState() {
  return this->transitionState;
}

void GameManager::SetScrollState(ScrollState newScrollState) {
  this->scroll->SetState(newScrollState);
  if (newScrollState == ScrollState::OPENING) {
    // Adjust the length of scroll based on the length of the current page or
    // the current game board.
    if (!this->activePage.empty() &&
        this->scroll->GetState() == ScrollState::OPENING) {
      this->scroll->SetTargetSilkLenForOpening(std::min(
          pages.at(this->activePage)->GetHeight(), gameBoard->GetSize().y));
    } else {
      this->scroll->SetTargetSilkLenForOpening(gameBoard->GetSize().y);
    }
  }
}

ScrollState GameManager::GetScrollState() { return this->scroll->GetState(); }

std::vector<int> GameManager::GetNeighborIds(std::unique_ptr<Bubble>& bubble,
                                             float absError) {
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

std::vector<int> GameManager::GetNeighborIds(
    std::unordered_map<int, std::unique_ptr<Bubble>>& bubbles) {
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
  return areFloatsEqual(pos.y, gameBoard->GetValidPosition().y, 1.f);
}

bool GameManager::IsConnectedToTopHelper(std::unique_ptr<Bubble>& bubble,
                                         bool* visited, bool* isConnectToTop) {
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

std::vector<int> GameManager::FindConnectedBubbles(
    std::vector<int>& bubbleIds) {
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
  // Traverse all the unvisited bubbles and find the bubbles that are connected
  // to the given bubble.
  size_t i = 0;
  while (i < unvisited.size()) {
    int unvisitedId = unvisited[i++];
    for (size_t j = 0; j < connectedBubbles.size(); ++j) {
      if (IsNeighbor(statics[unvisitedId]->GetCenter(),
                     statics[connectedBubbles[j]]->GetCenter())) {
        connectedBubbles.emplace_back(unvisitedId);
        std::swap(unvisited[i - 1], unvisited.back());
        unvisited.pop_back();
        if (unvisited.empty()) {
          return connectedBubbles;
        } else {
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

  // Find all the statics bubbles that are not connected to the top of the game
  // board and add them to the falling bubbles.
  for (auto& [id, bubble] : statics) {
    if (std::find(connectedIds.begin(), connectedIds.end(), id) ==
        connectedIds.end()) {
      fallingIds.emplace_back(id);
    }
  }

  return fallingIds;
}

std::vector<int> GameManager::FindCloseUpperBubbles(
    std::unique_ptr<Bubble>& bubble) {
  std::vector<int> closeUpperIds;
  for (auto& [id, staticBubble] : statics) {
    if (IsAtUpperBoundary(staticBubble->GetPosition()) &&
        glm::distance(staticBubble->GetCenter(), bubble->GetCenter()) <
            (1.5f * bubble->GetRadius() + staticBubble->GetRadius())) {
      closeUpperIds.emplace_back(id);
    }
  }
  return closeUpperIds;
}

int GameManager::FindLeftBubble(std::unique_ptr<Bubble>& bubble) {
  int leftId = -1;
  float minDistance = std::numeric_limits<float>::max();
  for (auto& [id, staticBubble] : statics) {
    // the center y should be the same
    if (areFloatsEqual(staticBubble->GetCenter().y, bubble->GetCenter().y,
                       1e-1)) {
      float distance = bubble->GetCenter().x - staticBubble->GetCenter().x;
      if (distance > 0 && distance < minDistance) {
        minDistance = distance;
        leftId = id;
      }
    }
  }
  return leftId;
}

int GameManager::FindRightBubble(std::unique_ptr<Bubble>& bubble) {
  int rightId = -1;
  float minDistance = std::numeric_limits<float>::max();
  for (auto& [id, staticBubble] : statics) {
    // the center y should be the same
    if (areFloatsEqual(staticBubble->GetCenter().y, bubble->GetCenter().y,
                       1e-1)) {
      float distance = staticBubble->GetCenter().x - bubble->GetCenter().x;
      if (distance > 0 && distance < minDistance) {
        minDistance = distance;
        rightId = id;
      }
    }
  }
  return rightId;
}

bool GameManager::FineTuneToCloseUpper(int bubbleId) {
  // Get all the static bubbles that are connected to the top of the game board
  // and close to the current bubble.
  auto& bubble = moves[bubbleId];
  std::vector<int> closeUpperIds = FindCloseUpperBubbles(bubble);
  if (closeUpperIds.empty()) {
    return false;
  }
  // Iterate through all the close upper bubbles and find the best free slot
  // center.
  float highestWeight = 0.0f;
  glm::vec2 bestFreeSlotCenter;
  for (const auto& id : closeUpperIds) {
    // Check if the bubble is to the left or to the right of the static bubble.
    float deltaX = statics[id]->GetCenter().x - bubble->GetCenter().x;
    bool isLeft = deltaX < 0.f;
    // If the static bubble is to the left of the moving bubble,then the free
    // slot center should be to the right of the static bubble.
    glm::vec2 freeSlotCenter;
    if (isLeft) {
      freeSlotCenter =
          statics[id]->GetCenter() +
          glm::vec2(statics[id]->GetRadius() + bubble->GetRadius(), 0.0f);
    } else {
      freeSlotCenter =
          statics[id]->GetCenter() -
          glm::vec2(statics[id]->GetRadius() + bubble->GetRadius(), 0.0f);
    }
    // Get the weight of the free slot
    float weight =
        GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

    // If the weight is greater than the highest weight, then we update the
    // highest weight and the free slot center.
    if (weight > highestWeight) {
      highestWeight = weight;
      bestFreeSlotCenter = freeSlotCenter;
    }
  }
  // If the highest weight is greater than 0, then we move the bubble to the
  // best free slot center.
  if (highestWeight > 0.0f) {
    bubble->SetPosition(bestFreeSlotCenter -
                        glm::vec2(kBubbleRadius, kBubbleRadius));
    return true;
  }
  // No fine tuning happened.
  return false;
}

bool GameManager::FineTuneToCorrectPosition(int bubbleId) {
  assert(moves.count(bubbleId) > 0 && "Failed to find the bubble by ID.");
  auto& bubble = moves[bubbleId];
  std::vector<int> neighborIds = GetNeighborIds(bubble);
  glm::vec4 boundaries = gameBoard->GetValidBoundaries();
  glm::vec2 newPosition = bubble->GetPosition();
  if (neighborIds.empty()) {
    assert(IsAtUpperBoundary(bubble->GetPosition()) &&
           "The bubble is not at the upper boundary.");
    int leftId = FindLeftBubble(bubble);
    int rightId = FindRightBubble(bubble);
    assert((leftId != -1 || rightId != -1) &&
           "At least one static bubble can be found at the upper boundary.");
    float leftDistance = bubble->GetPosition().x - boundaries.x;
    float rightDistance =
        boundaries.z - 2 * kBubbleRadius - bubble->GetPosition().x;
    float offSetToLeft = -1.f;
    float offSetToRight = -1.f;
    if (leftId != -1) {
      // Get the distance between the left static bubble and the current bubble.
      float leftDistance1 =
          bubble->GetCenter().x - statics.at(leftId)->GetCenter().x;
      // assert(leftDistance1 > 0 &&
      //        !areFloatsGreater(leftDistance1, leftDistance) &&
      //        "The distance calculation bettween bubbles is wrong");
      leftDistance = leftDistance1;
      while (leftDistance >= 4 * kBubbleRadius) {
        leftDistance -= 4 * kBubbleRadius;
        if (leftDistance < 0) {
          leftDistance = 0;
        }
      }
      if (leftDistance < 2 * std::sqrt(2) * kBubbleRadius) {
        offSetToLeft = leftDistance;
        offSetToRight = 2 * std::sqrt(2) * kBubbleRadius - leftDistance;
      } else if (leftDistance < 2 * std::sqrt(3) * kBubbleRadius) {
        offSetToLeft =
            2 * std::sqrt(3) * kBubbleRadius - 2 * std::sqrt(2) * kBubbleRadius;
        offSetToRight = 2 * std::sqrt(3) * kBubbleRadius - leftDistance;
      } else if (leftDistance < 4 * kBubbleRadius) {
        offSetToLeft = 4 * kBubbleRadius - 2 * std::sqrt(3) * kBubbleRadius;
        offSetToRight = 4 * kBubbleRadius - leftDistance;
      }
    } else if (rightId != -1) {
      // Get the distance between the right static bubble and the current
      // bubble.
      float rightDistance1 =
          statics.at(rightId)->GetCenter().x - bubble->GetCenter().x;
      // assert(rightDistance1 > 0 &&
      //        !areFloatsGreater(rightDistance1, rightDistance) &&
      //        "The distance calculation bettween bubbles is wrong");
      rightDistance = rightDistance1;
      while (rightDistance >= 4 * kBubbleRadius) {
        rightDistance -= 4 * kBubbleRadius;
        if (rightDistance < 0) {
          rightDistance = 0;
        }
      }
      if (rightDistance < 2 * std::sqrt(2) * kBubbleRadius) {
        offSetToRight = rightDistance;
        offSetToLeft = 2 * std::sqrt(2) * kBubbleRadius - rightDistance;
      } else if (rightDistance < 2 * std::sqrt(3) * kBubbleRadius) {
        offSetToRight =
            2 * std::sqrt(3) * kBubbleRadius - 2 * std::sqrt(2) * kBubbleRadius;
        offSetToLeft = 2 * std::sqrt(3) * kBubbleRadius - rightDistance;
      } else if (rightDistance < 4 * kBubbleRadius) {
        offSetToRight = 4 * kBubbleRadius - 2 * std::sqrt(3) * kBubbleRadius;
        offSetToLeft = 4 * kBubbleRadius - rightDistance;
      }
    }
    assert(offSetToLeft >= 0.f && offSetToRight >= 0.f &&
           "The offset to the left and the right should be greater than or "
           "equal to 0.");
    if (offSetToLeft < offSetToRight) {
      newPosition = bubble->GetPosition() - glm::vec2(offSetToLeft, 0.f);
      /*bubble->SetPosition(bubble->GetPosition() - glm::vec2(offSetToLeft,
       * 0.f));*/
    } else {
      newPosition = bubble->GetPosition() + glm::vec2(offSetToRight, 0.f);
      // bubble->SetPosition(bubble->GetPosition() +
      //                     glm::vec2(offSetToRight, 0.f));
    }
    // float weight =
    //     GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), newPosition);
    // if (weight > 0.f) {
    // bubble->SetPosition(newPosition);
    // return true;
    // }
    // else {
    //   return false;
    // }
    /*  return std::min(offSetToLeft, offSetToRight) == 0.f;*/
  } else if (neighborIds.size() == 1) {
    // Fine-tune the bubble's position to ensure its angle with a single
    // neighbor is a multiple of 30 degrees, measured clockwise from the right
    // side.
    int neighborId = neighborIds[0];
    auto& neighborBubble = statics[neighborId];
    glm::vec2 curDirection =
        glm::normalize(bubble->GetCenter() - neighborBubble->GetCenter());
    glm::vec2 baseDirection = glm::vec2(1.f, 0.f);
    // calculate the cosine of the angle between the two vectors
    float cosAngle = glm::dot(curDirection, baseDirection);
    float curAngle = glm::acos(cosAngle);
    if (curDirection.y < 0.f) {
      curAngle = 2 * glm::pi<float>() - curAngle;
    }
    float targetAngle =
        std::floor(curAngle / (glm::pi<float>() / 6)) * glm::pi<float>() / 6;
    if (targetAngle <= curAngle) {
      float diff1 = curAngle - targetAngle;
      float diff2 = targetAngle + glm::pi<float>() / 6 - curAngle;
      if (diff1 > diff2) {
        targetAngle += glm::pi<float>() / 6;
      }
    }
    targetAngle = std::round(targetAngle / (glm::pi<float>() / 6)) *
                  (glm::pi<float>() / 6);
    glm::vec2 targetDirection = rotateVector(baseDirection, targetAngle);
    glm::vec2 targetCenter =
        neighborBubble->GetCenter() + 2 * kBubbleRadius * targetDirection;
    newPosition = targetCenter - glm::vec2(kBubbleRadius, kBubbleRadius);
    // bubble->SetPosition(targetCenter - glm::vec2(kBubbleRadius,
    // kBubbleRadius)); return true;
  }
  float weight = GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), newPosition);
  if (weight > 0.f) {
    bubble->SetPosition(newPosition);
    return true;
  } else {
    return false;
  }
  /* return false;*/
}

bool GameManager::FineTuneToNeighbor(int bubbleId, int staticBubbleId) {
  assert(moves.count(bubbleId) > 0 && statics.count(staticBubbleId) > 0 &&
         "Failed to find the bubble by ID.");
  // bubbles.
  auto& bubble = moves[bubbleId];
  auto& staticBubble = statics[staticBubbleId];
  float highestWeight = 0.0f;
  glm::vec2 bestFreeSlotCenter;
  // Get the normalized vector from the static bubble to the bubble
  glm::vec2 bubbleDirection =
      glm::normalize(bubble->GetCenter() - staticBubble->GetCenter());
  for (auto& [id, neighborBubble] : statics) {
    if (id == staticBubbleId) {
      continue;
    }
    if (IsNeighbor(neighborBubble->GetCenter(), staticBubble->GetCenter())) {
      // Get the normalized vector from the static bubble to the neighbor
      glm::vec2 neighborDirection = glm::normalize(neighborBubble->GetCenter() -
                                                   staticBubble->GetCenter());

      // Get the cosine of the angle between the two vectors
      float cosAngle = glm::dot(neighborDirection, bubbleDirection);
      // If the angle is less than 60 degrees or greater than 100 degrees, then
      // we won't consider it.
      if (cosAngle < 0.0f || cosAngle > 0.5f) {
        continue;
      }
      // Get the free slot center. It should form a 60 degree angle with the
      // static bubble and the neighbor bubble.
      glm::vec2 freeSlotCenter;
      int a = getSignOfCrossProduct(neighborDirection, bubbleDirection);
      // if a is 0, then the two vectors are parallel. We don't consider this
      // case.
      if (a == 0) {
        continue;
      } else if (a > 0) {
        freeSlotCenter = rotateVector(
            staticBubble->GetCenter() + 2 * kBubbleRadius * neighborDirection,
            glm::pi<float>() / 3, staticBubble->GetCenter());
      } else {
        freeSlotCenter = rotateVector(
            staticBubble->GetCenter() + 2 * kBubbleRadius * neighborDirection,
            -glm::pi<float>() / 3, staticBubble->GetCenter());
      }

      // Get the weight of the free slot
      float weight =
          GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

      // If the weight is greater than the highest weight, then we update the
      // highest weight and the free slot center.
      if (weight > highestWeight) {
        highestWeight = weight;
        bestFreeSlotCenter = freeSlotCenter;
      }
    }
  }
  // If the highest weight is greater than 0, then we move the bubble to the
  // best free slot center.
  if (highestWeight > 0.0f) {
    bubble->SetPosition(bestFreeSlotCenter -
                        glm::vec2(kBubbleRadius, kBubbleRadius));
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
  glm::vec2 bubbleDirection =
      glm::normalize(bubble->GetCenter() - staticBubble->GetCenter());
  // If the highest weight is 0, we get the closest static bubbles that are not
  // neighbors of the static bubble, but its distance to the static bubble is
  // less than or equal to 4*kBubbleRadius.
  for (auto& [id, closeBubble] : statics) {
    if (id == staticBubbleId) {
      continue;
    }
    // If it is a neighbor of the static bubble, then we don't consider it.
    if (IsNeighbor(closeBubble->GetCenter(), staticBubble->GetCenter())) {
      continue;
    }
    // If the distance between the two bubbles is greater than 4*kBubbleRadius,
    // then we don't consider it.
    float distance =
        glm::distance(closeBubble->GetCenter(), staticBubble->GetCenter());
    if (distance > 4 * kBubbleRadius) {
      continue;
    }
    // Get the normalized vector from the static bubble to the close bubble
    glm::vec2 closeBubbleDirection =
        glm::normalize(closeBubble->GetCenter() - staticBubble->GetCenter());
    // Get the cosine of the angle between the two vectors
    float cosAngle = glm::dot(closeBubbleDirection, bubbleDirection);
    // If the angle is less than 60 degrees or greater than 90 degrees, then we
    // won't consider it.
    if (cosAngle < 0.0f || cosAngle > 0.866f) {
      continue;
    }
    // calculate the angle from the cosAngle using glm::acos
    float rotationAngle =
        lawOfCosinesAngle(distance, 2 * kBubbleRadius, 2 * kBubbleRadius);
    // Get the free slot center. This free slot should be a circle neighboring
    // both the static bubble and the close bubble.
    glm::vec2 freeSlotCenter;
    int a = getSignOfCrossProduct(closeBubbleDirection, bubbleDirection);
    // if a is 0, then the two vectors are parallel. We don't consider this
    // case.
    if (a == 0) {
      continue;
    } else if (a > 0) {
      freeSlotCenter = rotateVector(
          staticBubble->GetCenter() + 2 * kBubbleRadius * closeBubbleDirection,
          rotationAngle, staticBubble->GetCenter());
    } else {
      freeSlotCenter = rotateVector(
          staticBubble->GetCenter() + 2 * kBubbleRadius * closeBubbleDirection,
          -rotationAngle, staticBubble->GetCenter());
    }
    // Get the weight of the free slot
    float weight =
        GetFreeSlotWeight(bubble->GetColorWithoutAlpha(), freeSlotCenter);

    // If the weight is greater than the highest weight, then we update the
    // highest weight and the free slot center.
    if (weight > highestWeight) {
      highestWeight = weight;
      bestFreeSlotCenter = freeSlotCenter;
    }
  }

  // If the highest weight is greater than 0, then we move the bubble to the
  // best free slot center.
  if (highestWeight > 0.0f) {
    bubble->SetPosition(bestFreeSlotCenter -
                        glm::vec2(kBubbleRadius, kBubbleRadius));
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
    // If the current id already exists in the connected bubble ids, then we
    // skip it.
    if (std::find(connectedBubbleIds.begin(), connectedBubbleIds.end(),
                  currentId) != connectedBubbleIds.end()) {
      continue;
    }
    connectedBubbleIds.push_back(currentId);
    // Get the neighbors of the current bubble
    std::vector<int> neighborIds = GetNeighborIds(statics[currentId]);
    for (auto& neighborId : neighborIds) {
      // If the neighbor has the same color as the bubble and it is not in the
      // connected bubble ids, then we add it to the queue.
      if (isSameColor(color, statics[neighborId]->GetColorWithoutAlpha())) {
        q.emplace(neighborId);
      }
    }
  }
  return connectedBubbleIds;
}

std::vector<int> GameManager::IsCollidingWithStaticBubbles(
    std::unique_ptr<Bubble>& bubble) {
  std::vector<int> ids;
  for (auto& other : statics) {
    if (areFloatsEqual(
            glm::distance(bubble->GetCenter(), other.second->GetCenter()),
            2 * kBubbleRadius)) {
      ids.push_back(other.first);
    }
  }
  return ids;
}

bool GameManager::IsNeighbor(glm::vec2 bubbleCenter, glm::vec2 slotCenter,
                             float absError) {
  return areFloatsEqual(glm::distance(bubbleCenter, slotCenter),
                        2 * kBubbleRadius, absError);
}

std::vector<glm::vec2> GameManager::GetCommonFreeSlots(
    std::vector<std::unique_ptr<Bubble>>& bubbles,
    std::vector<glm::vec2> candidateFreeSlots) {
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

std::vector<glm::vec2> GameManager::GetCommonFreeSlots(
    std::vector<int>& bubbleIds, std::vector<glm::vec2> candidateFreeSlots) {
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
  if (areFloatsLess(slotCenter.x - kBubbleRadius, boundaries[0]) ||
      areFloatsGreater(slotCenter.x + kBubbleRadius, boundaries[2]) ||
      areFloatsLess(slotCenter.y - kBubbleRadius, boundaries[1]) ||
      areFloatsGreater(slotCenter.y + kBubbleRadius, boundaries[3])) {
    return -1.0f;
  }

  // First check if the slot is already occupied by a static bubble. That case,
  // the distance between the slot center and the center of the static bubble is
  // less than 2*kBubbleRadius.
  for (auto& other : statics) {
    if (glm::distance(slotCenter, other.second->GetCenter()) <
        2 * kBubbleRadius - 1.0f) {
      // return -1.0f to indicate that the slot is not free.
      return -1.0f;
    }
  }

  float weight = 0.0f;
  for (auto& other : statics) {
    if (IsNeighbor(other.second->GetCenter(), slotCenter)) {
      if (isSameColor(other.second->GetColorWithoutAlpha(), color)) {
        weight += 1.0f;
      } else {
        weight += 0.1f;
      }
    }
  }

  // incrase weight if it's at the upper boundary
  if (IsAtUpperBoundary(slotCenter - glm::vec2(kBubbleRadius, kBubbleRadius))) {
    weight += 1.0f;
  }

  return weight;
}

std::vector<glm::vec2> GameManager::GetPotentialNeighborFreeSlots(
    glm::vec2 bubbleCenter) {
  std::vector<glm::vec2> neighborCenters;
  // The angle between two neighbors is mutiple of 30 degrees.
  constexpr float angle = glm::radians(30.0f);
  float totalNeighborNum = std::round(2 * glm::pi<float>() / angle);
  // The first neighbor is the right neighbor.
  glm::vec2 rightNeighbor = bubbleCenter + glm::vec2(2 * kBubbleRadius, 0.0f);
  neighborCenters.emplace_back(rightNeighbor);

  // Get the other 5 neighbors
  for (size_t neighborNum = 1; neighborNum < totalNeighborNum; ++neighborNum) {
    glm::vec2 neighborCenter =
        rotateVector(rightNeighbor, angle * neighborNum, bubbleCenter);
    neighborCenters.emplace_back(neighborCenter);
  }

  // Remove the neighbors that are outside the boundaries
  auto boundaries = gameBoard->GetBoundaries();
  for (auto it = neighborCenters.begin(); it != neighborCenters.end();) {
    bool lessThenXBegin = areFloatsLess(it->x - kBubbleRadius, boundaries[0]);
    bool greaterThenXEnd =
        areFloatsGreater(it->x + kBubbleRadius, boundaries[2]);
    bool lessThenYBegin = areFloatsLess(it->y - kBubbleRadius, boundaries[1]);
    bool greaterThenYEnd =
        areFloatsGreater(it->y + kBubbleRadius, boundaries[3]);
    if (lessThenXBegin || greaterThenXEnd || lessThenYBegin ||
        greaterThenYEnd) {
      it = neighborCenters.erase(it);
    } else {
      ++it;
    }
  }

  // Remove the neighbors that are overlapping with the static bubbles
  for (auto it = neighborCenters.begin(); it != neighborCenters.end();) {
    bool overlap = false;
    for (auto& [id, bubble] : statics) {
      if (areFloatsLess(glm::distance(*it, bubble->GetCenter()),
                        2 * kBubbleRadius)) {
        overlap = true;
        break;
      }
    }
    if (overlap) {
      it = neighborCenters.erase(it);
    } else {
      ++it;
    }
  }

  return neighborCenters;
}

void GameManager::UpdateFreeSlots(std::unique_ptr<Bubble>& bubble,
                                  float minDistanceToBottom,
                                  float minHorizontalDistanceToShooter,
                                  float minVerticalDistanceToShooter) {
  // Get the center of the bubble
  glm::vec2 bubbleCenter = bubble->GetCenter();
  // Remove the free slots whose distance to the bubble center is less than
  // 2*kBubbleRadius.
  freeSlots.erase(std::remove_if(freeSlots.begin(), freeSlots.end(),
                                 [&](glm::vec2 slotCenter) {
                                   return glm::distance(slotCenter,
                                                        bubbleCenter) <
                                          2 * kBubbleRadius;
                                 }),
                  freeSlots.end());

  // Get potential free slots
  std::vector<glm::vec2> potentialFreeSlots =
      GetPotentialNeighborFreeSlots(bubble->GetCenter());

  // Remove the potential free slots whose distance to the shooter is less than
  // minDistanceToShooter and whose distance to the bottom is less than
  // minDistanceToBottom.
  glm::vec2 shooterCenter = this->shooter->GetCarriedBubble().GetCenter();
  float gameBoardBottomBound = this->gameBoard->GetBoundaries()[3];
  Ellipse ellipse(shooterCenter, minHorizontalDistanceToShooter,
                  minVerticalDistanceToShooter);
  potentialFreeSlots.erase(
      std::remove_if(potentialFreeSlots.begin(), potentialFreeSlots.end(),
                     [&](glm::vec2 slotCenter) {
                       return slotCenter.y >
                                  gameBoardBottomBound - minDistanceToBottom ||
                              ellipse.isWithin(slotCenter);
                     }),
      potentialFreeSlots.end());

  // Add the potential free slots to the free slots if they are not in free
  // slots.
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
  for (auto& [colorEnum, colorVal] : colorMap) {
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

  //// Record the number of bubbles of each color. The key is the index of the
  /// color in the color pool and the value is the number of bubbles of that
  /// color.
  // std::unordered_map<int, int> colorCount;

  // Generate bubble one by one
  for (int i = 0; i < gameLevel.numInitialBubbles; ++i) {
    bool hasExistingBubble = !statics.empty();
    bool toBeNeighborOfBubble =
        randomBool(gameLevel.probabilityNewBubbleIsNeighborOfBubble) &&
        hasExistingBubble;
    bool toBeNeighborOfLastAdded =
        randomBool(gameLevel.probabilityNewBubbleIsNeighborOfLastAdded) &&
        toBeNeighborOfBubble;
    bool toBeNeighborOfBubbleOfSameColor =
        randomBool(
            gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor) &&
        toBeNeighborOfBubble;
    // toBeNeighborOfBubble = false;
    // toBeNeighborOfLastAdded = false;
    // toBeNeighborOfBubbleOfSameColor = false;
    if (toBeNeighborOfLastAdded) {
      assert(lastAddedBubbleID != -1 && "Have not added any bubble yet.");
      // Get potential neighbors of the last added bubble
      std::vector<glm::vec2> lastAddedFreeSlots;
      lastAddedFreeSlots = GetPotentialNeighborFreeSlots(
          statics[lastAddedBubbleID]->GetCenter());
      while (lastAddedFreeSlots.empty()) {
        addedBubbleIds.pop_back();
        assert(!addedBubbleIds.empty() &&
               "There should always be a static bubble that has available "
               "neighbor free slots.");
        lastAddedBubbleID = addedBubbleIds.back();
        lastAddedFreeSlots = GetPotentialNeighborFreeSlots(
            statics[lastAddedBubbleID]->GetCenter());
      }
      // Get the common free slots of the last added free slots and the
      // available free slots.
      std::vector<glm::vec2> commonFreeSlots =
          GetCommonFreeSlots(lastAddedFreeSlots, freeSlots);
      //  Randomly select a free slot from the common free slots.
      int randomIndex = rand() % lastAddedFreeSlots.size();
      centerForNewBubble = lastAddedFreeSlots[randomIndex];
    } else if (toBeNeighborOfBubble) {
      // Randomly select a free slot that is a neighbor of an existing static
      // bubble.
      do {
        int randomIndex = rand() % freeSlots.size();
        centerForNewBubble = freeSlots[randomIndex];
      } while (!IsNeighborOfStaticBubbles(centerForNewBubble));
    } else {
      // Randomly select a free slot from the provided free slots.
      int randomIndex = rand() % freeSlots.size();
      centerForNewBubble = freeSlots[randomIndex];
    }

    // Create a new bubble
    std::unique_ptr<Bubble> newBubble = std::make_unique<Bubble>(
        centerForNewBubble - glm::vec2(kBubbleRadius, kBubbleRadius),
        kBubbleRadius, glm::vec2(0.0f, 0.0f),
        glm::vec4(colorPool[rand() % colorPool.size()], 0.8),
        ResourceManager::GetInstance().GetTexture("bubble"));

    if (toBeNeighborOfBubbleOfSameColor) {
      // Get the ids of the bubbles that are neighbors of the new bubble.
      std::vector<int> neighborIds = GetNeighborIds(newBubble);
      assert(!neighborIds.empty() && "Failed to get neighbor ids");
      // Randomly select a neighbor and set the color of the new bubble to the
      // color of the selected neighbor.
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
    UpdateFreeSlots(statics[lastAddedBubbleID], gameLevel.minDistanceToBottom,
                    gameLevel.minHorizontalDistanceToShooter,
                    gameLevel.minVerticalDistanceToShooter);

    // Memorize the id of the new bubble
    addedBubbleIds.emplace_back(lastAddedBubbleID);

    // Update the color count
    ++colorCount[statics[lastAddedBubbleID]->GetColorEnum()];
  }
}

void GameManager::GenerateRandomStaticBubbles() {
  // The radius of the bubble decreases as per 10 levels.
  kBubbleRadius = kBaseUnit * std::pow(0.9f, (level - 1) / 10);

  // Update the game level
  gameLevel.numColors =
      level < colorMap.size() ? std::sqrt(level * 4.f - 3.f) : colorMap.size();
  gameLevel.numInitialBubbles = 10 + level * 5;
  gameLevel.minDistanceToBottom = 4 * kBaseUnit + kBubbleRadius;
  gameLevel.minHorizontalDistanceToShooter = 3 * kBaseUnit + kBubbleRadius;
  gameLevel.minVerticalDistanceToShooter = 6 * kBaseUnit;
  float difficultyScalingFactor = 0.8f;
  switch (this->difficulty) {
    case Difficulty::MEDIUM:
      difficultyScalingFactor += 0.02f;
      break;
    case Difficulty::HARD:
      difficultyScalingFactor += 0.04f;
      break;
    case Difficulty::EXPERT:
      difficultyScalingFactor += 0.06f;
      break;
    default:
      break;
  }
  gameLevel.probabilityNewBubbleIsNeighborOfLastAdded =
      1.f - difficultyScalingFactor / GetNumGameLevels() * level;
  gameLevel.probabilityNewBubbleIsNeighborOfBubble =
      1.f - difficultyScalingFactor / GetNumGameLevels() * level;
  gameLevel.probabilityNewBubbleIsNeighborOfBubbleOfSameColor =
      1.f - difficultyScalingFactor / GetNumGameLevels() * level;

  float baseTime = 9.f, initialTime = 18.f;
  switch (this->difficulty) {
    case Difficulty::MEDIUM:
      baseTime -= 1.f;
      initialTime -= 1.f;
      break;
    case Difficulty::HARD:
      baseTime -= 2.f;
      initialTime -= 2.f;
      break;
    case Difficulty::EXPERT:
      baseTime -= 3.f;
      initialTime -= 3.f;
      break;
  }
  gameLevel.narrowingTimeInterval =
      glm::mix(initialTime, baseTime, level * 1.f / GetNumGameLevels());

  // Get the boundaries of the game board (left, upper, right, lower)
  glm::vec4 boundaries =
      glm::vec4(gameBoard->GetPosition().x, gameBoard->GetPosition().y,
                gameBoard->GetPosition().x + gameBoard->GetSize().x,
                gameBoard->GetPosition().y + gameBoard->GetSize().y);

  // generate random static bubble either on the top or as the neighbor of an
  // existing static bubble.
  freeSlots.clear();

  // First get all the free slots on the top of the game board.
  for (float x = boundaries.x + kBubbleRadius;
       !areFloatsGreater(x, boundaries.z - kBubbleRadius); x += kBubbleRadius) {
    freeSlots.emplace_back(x, boundaries.y + kBubbleRadius);
  }

  // Use a random number engine with a seed based on the current time
  auto rng = std::default_random_engine(
      std::chrono::system_clock::now().time_since_epoch().count());

  // Shuffle the free slots
  std::shuffle(freeSlots.begin(), freeSlots.end(), rng);

  this->shooter->UpdateCarriedBubbleRadius(kBubbleRadius);
  GenerateRandomStaticBubblesHelper(gameLevel);
}

float GameManager::GetNarrowingTimeInterval() {
  // float baseTime = 6.f;

  // float decreasingFactor = 0.9f;

  // float time = baseTime * pow(decreasingFactor, level - 1);  // level >= 1

  //// minimum time interval is 6 seconds
  // if (time < 6.f) {
  //   time = 6.f;
  // }
  // return time;

  return this->gameLevel.narrowingTimeInterval;
}

std::vector<glm::vec2> GameManager::GetCommonFreeSlots(
    std::vector<glm::vec2> freeSlots1, std::vector<glm::vec2> freeSlots2) {
  std::vector<glm::vec2> commonFreeSlots;
  for (auto& slot1 : freeSlots1) {
    for (auto& slot2 : freeSlots2) {
      if (areFloatsEqual(slot1.x, slot2.x) &&
          areFloatsEqual(slot1.y, slot2.y)) {
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
    static std::random_device rd;   // Initialize a random device
    static std::mt19937 gen(rd());  // Seed the generator
    static std::uniform_int_distribution<> dis(
        0,
        static_cast<int>(colorMap.size()) - 1);  // Distribution for enum values

    Color randomColor =
        static_cast<Color>(dis(gen));  // Get a random color enum value
    // Set the color
    return glm::vec4(colorMap[randomColor], 0.8);
  }
  // If statics is not empty, then we determine the new color based on the count
  // of bubbles of each color.
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
    // Determine the new color based on the counts of bubbles of each color
    // using random weighted selection.
    int randomIndex = randomWeightedSelect(weights);
    return glm::vec4(colorMap[colors[randomIndex]], 0.8);
  }
}

void GameManager::AdjustButtonsHorizontalPosition(
    const std::vector<std::string>& buttonName, float interval) {
  if (buttonName.empty()) {
    return;
  }
  assert(buttons.count(buttonName[0]) == 1 && "Button name does not exist.");
  glm::vec2 pos = buttons[buttonName[0]]->GetPosition();
  glm::vec2 size = buttons[buttonName[0]]->GetSize();
  glm::vec2 intervalVec = glm::vec2(interval + size.x, 0);
  glm::vec2 nextPos = pos + intervalVec;
  // Iterate through the rest of buttons and adjust their positions.
  for (size_t i = 1; i < buttonName.size(); ++i) {
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
  if (this->state != GameState::ACTIVE ||
      this->targetState != GameState::UNDEFINED) {
    return false;
  }
  // Get the shooter's carried bubble
  const Bubble& carriedBubble = shooter->GetCarriedBubble();
  // Get the radius and center of the carried bubble
  float radius = carriedBubble.GetRadius();
  glm::vec2 center = carriedBubble.GetCenter();
  // If part of the carried bubble is outside the actual upper boundary of the
  // game board, then the current level is failed.
  if (areFloatsLess(center.y - radius, gameBoard->GetValidPosition().y)) {
    return true;
  }
  // If the carried bubble is overlapping with the static bubbles, then the
  // current level is failed.
  for (auto& [id, bubble] : statics) {
    if (glm::distance(center, bubble->GetCenter()) <
        radius + bubble->GetRadius()) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<OptionUnit> GameManager::CreateClickableOptionUnit(
    const std::string& name, const std::u32string& text,
    std::shared_ptr<TextRenderer> textRenderer) {
  //  Create the image unit
  std::shared_ptr<ImageUnit> imageUnit = std::make_shared<ImageUnit>(
      "clickableiconunit",
      /*pos=*/glm::vec2(0.f), /*size=*/kBubbleSize,
      /*texture=*/ResourceManager::GetInstance().GetTexture("bubble"),
      this->spriteRenderer);
  // Create the text unit
  texts[name] = std::make_shared<Text>(
      /*pos=*/glm::vec2(0.f),
      /*lineWidth=*/gameBoard->GetSize().x - kBaseUnit);
  texts[name]->AddParagraph(text);
  texts[name]->SetScale(0.0018f * imageUnit->GetHeight());
  if (textRenderer == nullptr) {
    textRenderer = this->GetTextRenderer();
  }
  std::shared_ptr<TextUnit> textUnit =
      std::make_shared<TextUnit>(name, texts.at(name), textRenderer);
  // Create the option unit
  std::shared_ptr<OptionUnit> optionUnit = std::make_shared<OptionUnit>(
      name, imageUnit, textUnit, OptionState::kNormal);
  optionUnit->SetTextOnCenter(true);
  optionUnit->SetPosition(optionUnit->GetPosition());

  return optionUnit;
}

void GameManager::ResetGameCharacters() {
  // Set game character to be transparent gradually in its old state and be
  // opaque in its new state.
  const float transparency = 0.4f, opaque = 0.3f;
  auto guojieCopied =
      std::make_shared<GameCharacter>(*gameCharacters["guojie"]);
  graduallyTransparentObjects["guojie"] =
      std::make_pair(guojieCopied, transparency);
  // Clear all carried objects of guojie
  gameCharacters["guojie"]->ClearCarriedObjects();
  if (gameCharacters["weiqing"]->GetState() != GameCharacterState::HAPPY) {
    auto weiqingCopied =
        std::make_shared<GameCharacter>(*gameCharacters["weiqing"]);
    graduallyTransparentObjects["weiqing"] =
        std::make_pair(weiqingCopied, transparency);
    gameCharacters["weiqing"]->SetState(GameCharacterState::HAPPY);
    gameCharacters["weiqing"]->SetAlpha(0.f);
    graduallyOpaqueObjects["weiqing"] =
        std::make_pair(gameCharacters["weiqing"], opaque);
  }
  if (gameCharacters["weizifu"]->GetState() != GameCharacterState::HAPPY) {
    auto weizifuCopied =
        std::make_shared<GameCharacter>(*gameCharacters["weizifu"]);
    // Add 0 before "Weizifu" and 1 before "Liuche" to make sure that
    // Weizifu is drawn before Liuche.
    graduallyTransparentObjects["0weizifu"] =
        std::make_pair(weizifuCopied, transparency);
    gameCharacters["weizifu"]->SetState(GameCharacterState::HAPPY);
    gameCharacters["weizifu"]->SetAlpha(0.f);
    graduallyOpaqueObjects["0weizifu"] =
        std::make_pair(gameCharacters["weizifu"], opaque);
  }
  if (gameCharacters["liuche"]->GetState() != GameCharacterState::HAPPY) {
    auto liucheCopied =
        std::make_shared<GameCharacter>(*gameCharacters["liuche"]);
    graduallyTransparentObjects["1liuche"] =
        std::make_pair(liucheCopied, transparency);
    gameCharacters["liuche"]->SetState(GameCharacterState::HAPPY);
    gameCharacters["liuche"]->SetAlpha(0.f);
    graduallyOpaqueObjects["1liuche"] =
        std::make_pair(gameCharacters["liuche"], opaque);
  }

  gameCharacters["guojie"]->SetState(GameCharacterState::FIGHTING);
  // Reset the health of characters
  int totalHealth = gameCharacters["guojie"]->GetHealth().GetTotalHealth();
  gameCharacters["guojie"]->GetHealth().SetCurrentHealth(totalHealth);
  totalHealth = gameCharacters["weiqing"]->GetHealth().GetTotalHealth();
  gameCharacters["weiqing"]->GetHealth().SetCurrentHealth(totalHealth);

  gameCharacters["guojie"]->SetPosition(
      gameCharacters["guojie"]->GetTargetPosition("outofscreen"));
}

int GameManager::CalculateScore(int numBubbles, float bubbleRadius,
                                BubbleState bubbleType, float timeUsed) {
  if (numBubbles < 3) {
    return 0;
  }
  float score = 40.f - 0.5f * bubbleRadius +
                0.5f * this->level * std::sqrt(this->level) -
                timeUsed / std::sqrt(this->gameLevel.numInitialBubbles);
  if (score < 0.f) {
    // Map (-inf, 0) to (0, 1).
    score = std::exp(score);
  } else {
    // Map [0, inf) to [1, inf).
    ++score;
  }
  if (bubbleType == BubbleState::Falling) {
    score *= 1.5f;
  }
  int totalIncrement = 0;
  for (int i = 2; i < numBubbles; ++i) {
    if (i > 2) {
      score *= 1.2f;
    }
    scoreIncrements.push(static_cast<int>(std::ceil(score)));
    totalIncrement += scoreIncrements.back();
  }
  // score *= numBubbles + 0.1f * (numBubbles - 2) * (numBubbles - 3);
  // scoreIncrements.push_back(static_cast<int>(std::ceil(score)));
  return totalIncrement;
}

void GameManager::IncreaseScore(int64_t score) {
  this->score += score;
  // Set the score text
  texts.at("score")->SetParagraph(1, U"{" + intToU32String(this->score) + U"}");
}

void GameManager::DecreaseScore(int64_t score) { this->IncreaseScore(-score); }

void GameManager::ResetScore(int64_t value) {
  this->score = value;
  // Set the score text
  texts.at("score")->SetParagraph(1, U"{" + intToU32String(this->score) + U"}");

  // Empty the queue of score increments
  scoreIncrements = std::queue<int>();
  numOfScoreIncrementsReady = 0;

  // Clean the score increment event
  if (this->timer->HasEvent("refreshscore")) {
    this->timer->CleanEvent("refreshscore");
  }
}

void GameManager::ClearResources() {
  // Clear text characters textures
  for (auto textureID : TextRenderer::characterMap) {
    glDeleteTextures(1, &textureID.second[CharStyle::REGULAR].TextureID);
    glDeleteTextures(1, &textureID.second[CharStyle::BOLD].TextureID);
    glDeleteTextures(1, &textureID.second[CharStyle::ITALIC].TextureID);
    glDeleteTextures(1, &textureID.second[CharStyle::BOLD_ITALIC].TextureID);
  }
  TextRenderer::characterMap.clear();
  TextRenderer::characterCount.clear();
  // Clear texts
  texts.clear();
  // Clear other resources
  ResourceManager::GetInstance().Clear();
  // Detach all shared pointers
  this->spriteRenderer = nullptr;
  this->spriteDynamicRenderer = nullptr;
  this->partialTextureRenderer = nullptr;
  this->colorRenderer = nullptr;
  this->circleRenderer = nullptr;
  this->rayRenderer = nullptr;
  this->lineRenderer = nullptr;
  textRenderers.clear();
  buttons.clear();
  this->postProcessor = nullptr;
  this->timer = nullptr;
  gameCharacters.clear();

  // Detach all unique pointers
  this->gameBoard = nullptr;
  this->scroll = nullptr;
  this->shooter = nullptr;
  this->shadowTrailSystem = nullptr;
  this->explosionSystem = nullptr;
  pages.clear();

  //// Reset scissors parameters
  // ScissorBoxHandler::GetInstance().Reset();
}
