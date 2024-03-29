#include "Starter.hpp"

// num rows and cols labyrinth
#define NUMROW 45
#define NUMCOL 45

//max number of lights
#define MAX_ON_LIGHTS 150

//default random seed for the generation of the labyrinth
#define DEFAULT_RANDOM_SEED 1

//obtain automatically all the keys (debug)
#define OBTAIN_AUTOMATICALLY_ALL_THE_KEYS false//shoud be false

//you can fly (debug)
#define YOU_CAN_FLY false//shoud be false

//there is the ciel (debug)
#define THERE_IS_THE_CIEL true//shoud be true

//if you lose you can't move (debug)
#define IF_YOU_LOSE_YOU_CANT_MOVE true//shoud be true

//if there are off lights (debug)
#define THERE_ARE_OFF_LIGHTS true//shoud be true

// The uniform buffer object
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
    alignas(4) float roughness;
    alignas(4) float ao;
    alignas(4) float metallic;
};

// The global uniform buffer object
struct GlobalUniformBufferObject {
	alignas(16) glm::vec4 lightPos[MAX_ON_LIGHTS];
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
    alignas(4) int numberOfLights;
};

// The global uniform buffer object related to the UI
struct UniformBufferObjectUI {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};

//enum for the written to see during the boss fight
enum writtenDuringBossFight { none, fired, safeArea, notWatchingTheBoss, tooDistantFromTheBoss, bossHurtsYou, bossHasSlammedYouAgainstToTheWall };
writtenDuringBossFight writtenDuringBossFightToVisualize = writtenDuringBossFight::none;

class LabyrinthSurvival;

// MAIN !
class LabyrinthSurvival : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSL1{};//main descriptor set layout
    DescriptorSetLayout DSLUI{};//descriptor set for UI elements

	// Pipelines [Shader couples]
	Pipeline P1{};//main pipeline
    Pipeline PUI{};//pipeline for the UI

	// Models, textures and Descriptors (values assigned to the uniforms)
    
    // Descriptor sets for the UI
    DescriptorSet DSStartBarYourHP;//DescriptorSet DSStartBarYourHP for the bar of your HP (start)
    DescriptorSet DSEndBarYourHP;//DescriptorSet DSEndBarYourHP for the bar of your HP (end)
    DescriptorSet DSMidBarYourHP;//DescriptorSet DSMidBarYourHP for the bar of your HP (mid)
    DescriptorSet DSStartBarBossHP;//DescriptorSet DSStartBarYourHP for the bar of boss HP (start)
    DescriptorSet DSEndBarBossHP;//DescriptorSet DSEndBarYourHP for the bar of boss HP (end)
    DescriptorSet DSMidBarBossHP;//DescriptorSet DSMidBarBossHP for the bar of boss HP (mid)
    DescriptorSet DSyourHPwritten;//DescriptorSet DSyourHPwritten for the written about your HP
    DescriptorSet DSbossHPwritten;//DescriptorSet DSbossHPwritten for the written about boss HP
    DescriptorSet DSfindAllTheKeys;//DescriptorSet DSfindAllTheKeys for the written that you have to find all the keys
    DescriptorSet DSnowGoToTheBoss;//DescriptorSet DSnowGoToTheBoss for the written that you have to go to the boss
    std::vector<DescriptorSet> DSKeyToTake;//DescriptorSet DSKeyToTake for the keys you have to take icon
    std::vector<DescriptorSet> DSKeyTook;//DescriptorSet DSKeyTook for the keys you have took icon
    DescriptorSet DSWfired;//DescriptorSet DSWfired for the written that you have fired (during boss fight)
    DescriptorSet DSWsafeArea;//DescriptorSet DSWsafeArea for the written that you are in the safe are and the fire is not valid (during boss fight)
    DescriptorSet DSWnotWatchingTheBoss;//DescriptorSet DSWnotWatchingTheBoss for the written that you are not watching the boss (during boss fight)
    DescriptorSet DSWtooDistantFromTheBoss;//DescriptorSet DSWtooDistantFromTheBoss for the written that you are too distant from the boss (during boss fight)
    DescriptorSet DSWbossHurtsYou;//DescriptorSet DSWtooDistantFromTheBoss for the written that the boss hurts you (during boss fight)
    DescriptorSet DSWbossHasSlammedYouAgainstToTheWall;//DescriptorSet DSWbossHasSlammedYouAgainstToTheWall for the written that the boss has slummed you againist the wall (during boss fight)
    DescriptorSet DSFireIcon;//DescriptorSet DSFireIcon for the icon of a fire that appears when you hit the boss (during boss fight)
    DescriptorSet DSYouWinIcon;//DescriptorSet DSYouWinIcon for the icon that you win
    DescriptorSet DSYouLoseIcon;//DescriptorSet DSYouLoseIcon for the icon that you lose

    // Descriptor sets game
    std::vector<DescriptorSet> DSW;//DescriptorSet for the walls
    std::vector<DescriptorSet> DSLights;//DescriptorSet for the lights
    std::vector<DescriptorSet> DSK;//DescriptorSet DSK for the keys
    std::vector<DescriptorSet> DSF;//DescriptorSet DSF for the food
    std::vector<DescriptorSet> DSG;//DescriptorSet for the ground
    DescriptorSet DSD;//DescriptorSet for the door
    DescriptorSet DSB;//DescriptorSet for the boss
    DescriptorSet DSC;//DescriptorSet DSC for the ciel of the labyrinth

    // Models ui
    Model MStartBarYourHP{};//Model MStartBarYourHP for the bar of your HP (start)
    Model MEndBarYourHP{};//Model MEndBarYourHP for the bar of your HP (end)
    Model MMidBarYourHP{};//Model MMidBarYourHP for the bar of your HP (mid)
    Model MyourHPwritten{};//Model MyourHPwritten for the written about your HP
    Model MStartBarBossHP{};//Model MStartBarBossHP for the bar of boss HP (start)
    Model MEndBarBossHP{};//Model MEndBarBossHP for the bar of boss HP (end)
    Model MMidBarBossHP{};//Model MMidBarBossHP for the bar of boss HP (mid)
    Model MbossHPwritten{};//Model MbossHPwritten for the written about boss HP
    Model MfindAllTheKeys{};//Model MfindAllTheKeys for the written that you have to find all the keys
    Model MnowGoToTheBoss{};//Model MnowGoToTheBoss for the written that you have to go to the boss
    std::vector<Model> MKeyToTake;//model MKeyToTake for the keys you have to take icon
    std::vector<Model> MKeyTook;//model MKeyTook for the keys you have took icon
    Model MWfired{};//Model MWfired for the written that you have fired (during boss fight)
    Model MWsafeArea{};//Model MWsafeArea for the written that you are in the safe are and the fire is not valid (during boss fight)
    Model MWnotWatchingTheBoss{};//Model MWnotWatchingTheBoss for the written that you are not watching the boss (during boss fight)
    Model MWtooDistantFromTheBoss{};//Model MWtooDistantFromTheBoss for the written that you are too distant from the boss (during boss fight)
    Model MWbossHurtsYou{};//Model MWbossHurtsYou for the written that the boss hurts you (during boss fight)
    Model MWbossHasSlammedYouAgainstToTheWall{};//Model MWbossHasSlammedYouAgainstToTheWall for the written that the boss has slummed you againist the wall (during boss fight)
    Model MFireIcon;//Model MFireIcon for the icon of a fire that appears when you hit the boss (during boss fight)
    Model MYouWinIcon;//Model MYouWinIcon for the icon that you win
    Model MYouLoseIcon;//Model MYouLoseIcon for the icon that you lose

    // Models game
    Model MW;//model MW for the walls
    Model MLight;//model MLight for the lights
    Model MK;//model MK for the keys
    Model MF;//model MF for the food
    Model MG1, MG2;//model MW for the ground
    Model MD;//model MD for the door
    Model MB;//model MB for the boss
    Model MC{};//Model MC for the ciel of the labyrinth

    // Textures
    Texture TC, TLab, TD, TB, TK, TF;//Texture for the ciel of the labyrinth, for the labyrinth (walls and ground), for the door, for the boss, for the keys, for the food, for your HP bar
    //Texture for your HP bar (start, end and mid), for boss HP bar (start, end and mid), for the written about your HP, for the written about boss HP, for the written that you have to find all the keys, for the keys you have to take icon, for the keys you have took icon, for the written that you have to go to the boss, for the icon of a fire that appears when you hit the boss (during boss fight), for the icon that you win, for the icon that you lose
    Texture TStartBarYourHP, TEndBarYourHP, TMidBarYourHP, TStartBarBossHP, TEndBarBossHP, TMidBarBossHP, TyourHPwritten, TbossHPwritten, TfindAllTheKeys, TKeyToTake, TKeyTook, TnowGoToTheBoss, TFireIcon, TYouWinIcon, TYouLoseIcon;
    Texture Tfired, TsafeArea, TnotWatchingTheBoss, TtooDistantFromTheBoss, TbossHurtsYou, TbossHasSlammedYouAgainstToTheWall;//Texture for the writtens during the boss fight

    // Variables concerning the generated labyrinth
    bool labyrinthShape[NUMROW][NUMCOL];//to keep where there is a wall in the labyrinth
    bool reducedLabyrinthShape[NUMROW][NUMCOL];//to keep where there is a wall or the door in the labyrinth
    bool labyrinthShapeInitialized = false;//if the labyrinth is initialized
    int effectiveNumberOfKeys = 0;//this var will be set with the number of keys in the labyrinth (after labyrinth generation)
    int effectiveNumberOfWalls = 0;//this var will be set with the number of walls in the labyrinth (after labyrinth generation)
    int effectiveNumberOfFood = 0;//this var will be set with the number of food in the labyrinth (after labyrinth generation)
    int effectiveNumberOfGround = 0;//this var will be set with the number of ground in the labyrinth (after labyrinth generation)
    int effectiveNumberOfOnLights = 2;//number of effective numbers of on lights (this value will be passed to the shader to turn on these lights) (the first two on lights are you and the boss)
    int effectiveNumberOfLights = 0;//number of effective numbers of on lights (on or off lights)
    
    std::vector<int> xKeyPos;//x position of the keys in the labyrinth
    std::vector<int> yKeyPos;//y position of the keys in the labyrinth
    std::vector<int> xFoodPos;//x position of the food in the labyrinth
    std::vector<int> yFoodPos;//y position of the food in the labyrinth

    // Variables useful when playing
    std::vector<bool> tookKey;//if you took each of the keys
    std::vector<bool> tookFood;//if you took each of the keys
    bool bossFightStarted = false;//when you enter in the boss room the boss appear and the boss fight start
    bool bossFightStartedAnimationFinished = false;//when the boss fight start there is an animation that the boss appear
    bool youNeedToWalkAwayFromTheBoss = false;//in case the boss hurts you and you have to walk away your movement is temporanely locked
    float fireCharging = 0.0f;//remaining time to charge the fire
    float tiltTimeCounter = 0.0f;//in case, during the boss fight, you are slammed against the wall, you are tilted and you can't move: this var is to keep the time you are tilt
    float lastFirePow = 0.0f;//to remember how power was the last time you hit the boss
    
    // Others variables useful when playing
    float yourHP = 40.0f;//at the beginning you have 40 HP
    float bossHP = 0.0f;//at the beginning there isn't the boss (0 HP) because he has to appire when you arrive to the boss room
    bool youLose = false;//if you lose all HP and you haven't won you lose
    bool youWin = false;//if you are in the boss fight and the boss lose all HP and you haven't lose, you win
    
    // parameters to place elements in the UI...
    //locations and dimensions of some UI elements
    const float startHPBarX = -0.8f;
    const float startHPBarY = -1.00f;
    const float dimHPBar = 0.025f;
    const float startyourHPwrittenY = -1.0125f;
    const float HPwrittenOffsetFromBar = 0.15f;
    const float dimHPwrittenX = 0.14f;
    const float dimHPwrittenY = 0.05f;
    const float startFindAllTheKeysWrittenY = -0.95f;
    const float dimFindAllTheKeysWrittenX = 0.35f;
    const float dimFindAllTheKeysWrittenY = 0.05f;
    const float dimIconKeyX = 0.10f;
    const float dimIconKeyY = 0.05f;
    const float spaceXBetweenIconsKey = 0.05f;
    const float dimNowGoToTheBossWrittenX = 0.40f;
    const float dimNowGoToTheBossWrittenY = 0.05f;
    //locations and dimensions of writtens witch appears during the boss fight
    const float writtensDuringBossFightX = -0.8f;
    const float writtensDuringBossFightY = -0.9f;
    const float dimWrittensDuringBossFightY = 0.05f;
    const float dimWrittenFireX = 0.20f;
    const float dimWrittenSafeAreaX = 0.90f;
    const float dimWrittenNotWatchingTheBossX = 0.60f;
    const float dimWrittenTooDistantFromTheBossX = 0.70f;
    const float dimWrittenBossHurtsYouX = 0.40f;
    const float dimWrittenBossHasSlammedYouAgainstToTheWallYouX = 0.80f;
    //location and dimension of the fire icon that appears when you hit the boss (during boss fight)
    const float fireIconX = -0.55f;
    const float fireIconY = -0.9f;
    const float dimBaseFireIconX = 0.05f;
    const float dimBaseFireIconY = 0.05f;
    //location and dimension for the icons that you win or you lose
    const float youWinLoseX = -0.95f;
    const float youWinLoseY = -0.55f;
    const float dimYouWinLoseY = 0.05f;
    const float dimyouWinX = 0.23f;
    const float dimyouLoseX = 0.85f;

    //variables for the boss pattern (the boss has a pattern that sometimes approaches you and sometimes goes away from you)
    const float timeBossApproachYou = 4.0f;//how much time the boss approaches you
    const float timeBossGoesAwayFromYou = 0.475f;//how much time the boss goes away from you
    const float PenalityMultiplierBossGoesAwayFromYou = 5.0f;//when the boss goes away from you how faster the boss to go away respect to when the boss approaches you
    float timeBossPatternCounter = 0.0f;//time counter for the parttner of the boss
    float bossPatternMultiplier = 1.0f;//if positive the boss approaches you; if negative the boss goes away from you

	// Other application parameters
	glm::vec3 CamPos = glm::vec3(0.5, 0.5, 10.0);//camera position
    glm::vec3 CamPosOriginal = glm::vec3(0.5, 0.5, 10.0);//useful to reset the labirinth
    glm::vec3 CamPosPrec = glm::vec3(0.5, 0.5, 10.0);//remember prew position to check if you are not overriding a wall
    float CamAlpha = glm::radians(0.0f);
    float CamAlphaOriginal = glm::radians(0.0f);//useful to reset the labirinth
    float CamBeta = glm::radians(0.0f);
	float Ar;//aspect ratio
    glm::vec3 notVisiblePosition = glm::vec3(-5, -5, -5);//a position to place an object witch disappears

    const float notVisibleUIXPosition = -100.0f;//a position X to place an object of the UI witch disappears
    const float notVisibleUIYPosition = -100.0f;//a position Y to place an object of the UI witch disappears

    //parameters for the key pos, rot and scale
    std::vector<glm::vec3> keyPos;
    glm::quat KeyRot = glm::quat(glm::vec3(0, glm::radians(45.0f), 0)) *
                       glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                       glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
    glm::vec3 KeyScale = glm::vec3(0.20f);

    //parameters for the food pos, rot and scale
    std::vector<glm::vec3> foodPos;
    glm::quat FoodRot = glm::quat(glm::vec3(0, glm::radians(45.0f), 0)) *
                       glm::quat(glm::vec3(glm::radians(-90.0f), 0, 0)) *
                       glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
    glm::vec3 FoodScale = glm::vec3(0.018f);

    //parameters for the wall pos, rot and scale
    std::vector<glm::vec3> wallPos;
    std::vector<glm::quat> wallRots;
    glm::vec3 wallScale = glm::vec3(0.25f);

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightFirePositions;
    std::vector<glm::quat> lightRots;
    glm::vec3 lightScale = glm::vec3(0.25f);

    //parameters for the door pos, rot and scale
    glm::vec3 doorPos;
    glm::quat doorRot;
    glm::vec3 doorScale = glm::vec3(0.7f);
    glm::vec3 originalDoorPos;//the door will reappear also during the boss fight because you won't be able to exit from the boss room during the boss fight

    //parameters for the boss pos, rot and scale
    glm::vec3 bossPos;
    glm::vec3 bossPosOriginal;//useful to reset the labirinth
    float bossRot;
    float bossScale = 0.0f;
    const float maxBossScale = 2.25f;//when the boss fight starts there is an animation that the boss appear form 0 to max scale

    //limit the area where the boss can stay
    float xStartLimitBossPos;
    float xEndLimitBossPos;
    float zStartLimitBossPos;
    float zEndLimitBossPos;

    //parameters for the ground pos, rot and scale
    std::vector<bool> groundType;
    std::vector<glm::vec3> groundPos;
    glm::quat groundRot = glm::quat(glm::vec3(0, glm::radians(180.0f), 0)) *
                       glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                       glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
    glm::vec3 groundScale = glm::vec3(0.25f);

    //vPos and vIdx for the model for the ciel of the labyrinth
	std::vector<float> vPos;//position of the vertices
    std::vector<float> vNorms;//for each vertex the 3 coordinates of the norm
	std::vector<int> vIdx;//vertices for triangles

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1000;
		windowHeight = 750;
		windowTitle = "LabyrinthSurvival";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.6f, 0.8f, 1.0f};

        // Descriptor pool sizes
        uniformBlocksInPool = 60000;
        texturesInPool = 60000;
        setsInPool = 60000;

		Ar = 4.0f / 3.0f;
	}

	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
    
    //set the model for a squared UI element locating the vertices
    void addUiElement(Model * model, float xStart, float yStart, float xLenght, float yLenght){
        const float uiDist = 0.5f;
        const float uiDim = 0.25f;
        const float uiOffset = 0.2f;
        Vertex vertex1{};
        vertex1.pos = {(xStart+uiOffset)*uiDim, uiDist, (-yStart-yLenght-uiOffset)*uiDim};
        vertex1.texCoord = {0, 1};
        model->vertices.push_back(vertex1);
        Vertex vertex2{};
        vertex2.pos = {(xStart+uiOffset)*uiDim, uiDist, (-yStart-uiOffset)*uiDim};
        vertex2.texCoord = {0, 0};
        model->vertices.push_back(vertex2);
        Vertex vertex3{};
        vertex3.pos = {(xStart+uiOffset+xLenght)*uiDim, uiDist, (-yStart-yLenght-uiOffset)*uiDim};
        vertex3.texCoord = {1, 1};
        model->vertices.push_back(vertex3);
        Vertex vertex4{};
        vertex4.pos = {(xStart+uiOffset+xLenght)*uiDim, uiDist, (-yStart-uiOffset)*uiDim};
        vertex4.texCoord = {1, 0};
        model->vertices.push_back(vertex4);
        model->indices.push_back(0);
        model->indices.push_back(1);
        model->indices.push_back(2);
        model->indices.push_back(1);
        model->indices.push_back(2);
        model->indices.push_back(3);
    }

	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSL1.init(this, {
            // this array contains the binding:
            // first  element : the binding number
            // second element : the type of element (buffer or texture)
            // third  element : the pipeline stage where it will be used
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
            {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
          });
        
        DSLUI.init(this, {
            // this array contains the binding:
            // first  element : the binding number
            // second element : the type of element (buffer or texture)
            // third  element : the pipeline stage where it will be used
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS}
          });
        
		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, "shaders/GGXShaderVert.spv", "shaders/GGXShaderFrag.spv", {&DSL1});
        P1.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
                                     VK_CULL_MODE_NONE, false);
        
        PUI.init(this, "shaders/uiVert.spv", "shaders/uiFrag.spv", {&DSLUI});
        PUI.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
                                     VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

        int r = NUMROW;
        int c = NUMCOL;
        char **maze = genMaze(r, c);

        MW.init(this, "models/HighWall.obj");//model of a wall
        MLight.init(this, "models/Light.obj");//model of a light
        MD.init(this, "models/Door_Iron.obj");//model of a door
        MB.init(this, "models/Character.obj");//model of the boss
        //models of the ground
        MG1.init(this, "models/Ground.obj");
        MG2.init(this, "models/Ground_2pr.obj");
        MK.init(this, "models/Key.obj");//model of a key
        MF.init(this, "models/12190_Heart_v1_L3.obj");//model of a food

		for(int i=0; i < r; i++) {//print the generated maze
			std::cout << maze[i] << "\n";
		}
		std::cout << "\n";
        
        //Add the ciel
        const float upWallPoint = 2.0f;
        int counterCreatedVertex = 0;
        counterCreatedVertex += 4;
        int col = c;
        int row = r;
        if(!THERE_IS_THE_CIEL){//in case you you don't want to add the ciel
            col = 0;
            row = 0;
        }
        vPos.push_back(0.0f); vPos.push_back(upWallPoint); vPos.push_back(0.0f);
        vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
        vPos.push_back(col); vPos.push_back(upWallPoint); vPos.push_back(0.0f);
        vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
        vPos.push_back(0.0f); vPos.push_back(upWallPoint); vPos.push_back(row);
        vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
        vPos.push_back(col); vPos.push_back(upWallPoint); vPos.push_back(row);
        vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
        vIdx.push_back(counterCreatedVertex-4); vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2);
        vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2); vIdx.push_back(counterCreatedVertex-1);
        
        std::cout << "Mesh size (for the ciel of the labyrinth): V=" << vPos.size() << ", I=" << vIdx.size() << "\n";
        
        //Model of the ciel of the labyrinth
        MC.BP = this;
        for(int i = 0; i < vPos.size(); i+=3) {
            Vertex vertex{};
            vertex.pos = {vPos[i], vPos[i+1], vPos[i+2]};
            vertex.norm = {vNorms[i], vNorms[i+1], vNorms[i+2]};
            //texCoord for the texture of the labyrinth
            if(i % 4 == 0){
                vertex.texCoord = {1, 1};
            }
            if(i % 4 == 1){
                vertex.texCoord = {0, 0};
            }
            if(i % 4 == 2){
                vertex.texCoord = {0, 1};
            }
            if(i % 4 == 3){
                vertex.texCoord = {1, 0};
            }
            //
            MC.vertices.push_back(vertex);
        }
        for(int i = 0; i < vIdx.size(); i++) {
            if((vIdx[i] < 0) || (vIdx[i] >= MC.vertices.size())) {
                std::cout << "Error! Index: " << i << " is outside range (" << vIdx[i] << ")\n";
                MC.indices.push_back(0);
            } else {
                MC.indices.push_back(vIdx[i]);
            }
        }
        
        //Models of the UI...
        
        //Model of your HP bar
        MStartBarYourHP.BP = this;
        addUiElement(&MStartBarYourHP, startHPBarX, startHPBarY, dimHPBar, dimHPBar);
        MEndBarYourHP.BP = this;
        addUiElement(&MEndBarYourHP, startHPBarX, startHPBarY, dimHPBar, dimHPBar);
        MMidBarYourHP.BP = this;
        addUiElement(&MMidBarYourHP, startHPBarX, startHPBarY, dimHPBar, dimHPBar);
        MyourHPwritten.BP = this;
        addUiElement(&MyourHPwritten, startHPBarX-HPwrittenOffsetFromBar, startyourHPwrittenY, dimHPwrittenX, dimHPwrittenY);
        
        MStartBarBossHP.BP = this;
        addUiElement(&MStartBarBossHP, startHPBarX, startFindAllTheKeysWrittenY, dimHPBar, dimHPBar);
        MEndBarBossHP.BP = this;
        addUiElement(&MEndBarBossHP, startHPBarX, startFindAllTheKeysWrittenY, dimHPBar, dimHPBar);
        MMidBarBossHP.BP = this;
        addUiElement(&MMidBarBossHP, startHPBarX, startFindAllTheKeysWrittenY, dimHPBar, dimHPBar);
        MbossHPwritten.BP = this;
        addUiElement(&MbossHPwritten, startHPBarX-HPwrittenOffsetFromBar, startFindAllTheKeysWrittenY, dimHPwrittenX, dimHPwrittenY);
        
        //Model for the written that you have to take all the keys
        MfindAllTheKeys.BP = this;
        addUiElement(&MfindAllTheKeys, startHPBarX-HPwrittenOffsetFromBar, startFindAllTheKeysWrittenY, dimFindAllTheKeysWrittenX, dimFindAllTheKeysWrittenY);
        
        //Models for the keys icon
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            float offsetXThisIcon = i * (dimIconKeyX + spaceXBetweenIconsKey);
            Model keyToTakeIcon;
            keyToTakeIcon.BP = this;
            addUiElement(&keyToTakeIcon, startHPBarX-HPwrittenOffsetFromBar+offsetXThisIcon, startFindAllTheKeysWrittenY, dimIconKeyX, dimIconKeyY);//model key to take icon
            MKeyToTake.push_back(keyToTakeIcon);
            Model keyTookIcon;
            keyTookIcon.BP = this;
            addUiElement(&keyTookIcon, startHPBarX-HPwrittenOffsetFromBar+offsetXThisIcon, startFindAllTheKeysWrittenY, dimIconKeyX, dimIconKeyY);//model key took icon
            MKeyTook.push_back(keyTookIcon);
        }
        
        //Model for the written that now you have to go to the boss
        MnowGoToTheBoss.BP = this;
        addUiElement(&MnowGoToTheBoss, startHPBarX-HPwrittenOffsetFromBar, startFindAllTheKeysWrittenY, dimNowGoToTheBossWrittenX, dimNowGoToTheBossWrittenY);
        
        //Models for the writtens during the boss fight
        MWfired.BP = this;
        addUiElement(&MWfired, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenFireX, dimWrittensDuringBossFightY);
        MWsafeArea.BP = this;
        addUiElement(&MWsafeArea, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenSafeAreaX, dimWrittensDuringBossFightY);
        MWnotWatchingTheBoss.BP = this;
        addUiElement(&MWnotWatchingTheBoss, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenNotWatchingTheBossX, dimWrittensDuringBossFightY);
        MWtooDistantFromTheBoss.BP = this;
        addUiElement(&MWtooDistantFromTheBoss, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenTooDistantFromTheBossX, dimWrittensDuringBossFightY);
        MWbossHurtsYou.BP = this;
        addUiElement(&MWbossHurtsYou, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenBossHurtsYouX, dimWrittensDuringBossFightY);
        MWbossHasSlammedYouAgainstToTheWall.BP = this;
        addUiElement(&MWbossHasSlammedYouAgainstToTheWall, writtensDuringBossFightX, writtensDuringBossFightY, dimWrittenBossHasSlammedYouAgainstToTheWallYouX, dimWrittensDuringBossFightY);
        
        //Model for the icon of a fire that appears when you hit the boss (during boss fight)
        MFireIcon.BP = this;
        addUiElement(&MFireIcon, fireIconX, fireIconY, dimBaseFireIconX, dimBaseFireIconY);
        
        //Models for the icons that you win or lose
        MYouWinIcon.BP = this;
        addUiElement(&MYouWinIcon, youWinLoseX, youWinLoseY, dimyouWinX, dimYouWinLoseY);
        MYouLoseIcon.BP = this;
        addUiElement(&MYouLoseIcon, youWinLoseX, youWinLoseY, dimyouLoseX, dimYouWinLoseY);
        
        //initialize textures...
        
        //initialize textures for the models of the objects in the labyrinth
        TC.init(this, "textures/wall.png");
        TLab.init(this, "textures/ModelWallTexture.png");
        TD.init(this, "textures/door.png");
        TB.init(this, "textures/bossTry2.png");
        TK.init(this, "textures/key.png");
        TF.init(this, "textures/food.png");
        
        //initialize textures for the UI
        TStartBarYourHP.init(this, "textures/StartBarYourHP.png");
        TEndBarYourHP.init(this, "textures/EndBarYourHP.png");
        TMidBarYourHP.init(this, "textures/MidBarYourHP.png");
        TyourHPwritten.init(this, "textures/yourHPwritten.png");
        TStartBarBossHP.init(this, "textures/StartBarBossHP.png");
        TEndBarBossHP.init(this, "textures/EndBarBossHP.png");
        TMidBarBossHP.init(this, "textures/MidBarBossHP.png");
        TbossHPwritten.init(this, "textures/bossHPwritten.png");
        TfindAllTheKeys.init(this, "textures/findAllTheKeys.png");
        TKeyToTake.init(this, "textures/keyToTake.png");
        TKeyTook.init(this, "textures/keyTook.png");
        TnowGoToTheBoss.init(this, "textures/nowGoToTheBoss.png");
        Tfired.init(this, "textures/fired.png");
        TsafeArea.init(this, "textures/safeArea.png");
        TnotWatchingTheBoss.init(this, "textures/notWatchingTheBoss.png");
        TtooDistantFromTheBoss.init(this, "textures/tooDistantFromTheBoss.png");
        TbossHurtsYou.init(this, "textures/bossHurtsYou.png");
        TbossHasSlammedYouAgainstToTheWall.init(this, "textures/bossHasSlammedYouAgainstToTheWall.png");
        TFireIcon.init(this, "textures/fire.png");
        TYouWinIcon.init(this, "textures/youWin.png");
        TYouLoseIcon.init(this, "textures/youLose.png");
        
        //createVertexBuffer and createIndexBuffer for the model of the ciel of the labyrinth
        MC.createVertexBuffer();
        MC.createIndexBuffer();
        std::cout << "Created model (for the ciel of the labyrinth): V=" << MC.vertices.size() << ", I=" << MC.indices.size() << "\n";

        //createVertexBuffer and createIndexBuffer for the models of the UI
        
        MStartBarYourHP.createVertexBuffer();
        MStartBarYourHP.createIndexBuffer();
        
        MEndBarYourHP.createVertexBuffer();
        MEndBarYourHP.createIndexBuffer();
        
        MMidBarYourHP.createVertexBuffer();
        MMidBarYourHP.createIndexBuffer();
        
        MyourHPwritten.createVertexBuffer();
        MyourHPwritten.createIndexBuffer();
        
        MStartBarBossHP.createVertexBuffer();
        MStartBarBossHP.createIndexBuffer();
        
        MEndBarBossHP.createVertexBuffer();
        MEndBarBossHP.createIndexBuffer();
        
        MMidBarBossHP.createVertexBuffer();
        MMidBarBossHP.createIndexBuffer();
        
        MbossHPwritten.createVertexBuffer();
        MbossHPwritten.createIndexBuffer();
        
        MfindAllTheKeys.createVertexBuffer();
        MfindAllTheKeys.createIndexBuffer();
        
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            MKeyToTake[i].createVertexBuffer();
            MKeyToTake[i].createIndexBuffer();
            MKeyTook[i].createVertexBuffer();
            MKeyTook[i].createIndexBuffer();
        }
        
        MnowGoToTheBoss.createVertexBuffer();
        MnowGoToTheBoss.createIndexBuffer();
        
        MWfired.createVertexBuffer();
        MWfired.createIndexBuffer();
        
        MWsafeArea.createVertexBuffer();
        MWsafeArea.createIndexBuffer();
        
        MWnotWatchingTheBoss.createVertexBuffer();
        MWnotWatchingTheBoss.createIndexBuffer();
        
        MWtooDistantFromTheBoss.createVertexBuffer();
        MWtooDistantFromTheBoss.createIndexBuffer();
        
        MWbossHurtsYou.createVertexBuffer();
        MWbossHurtsYou.createIndexBuffer();
        
        MWbossHasSlammedYouAgainstToTheWall.createVertexBuffer();
        MWbossHasSlammedYouAgainstToTheWall.createIndexBuffer();
        
        MFireIcon.createVertexBuffer();
        MFireIcon.createIndexBuffer();
        
        MYouWinIcon.createVertexBuffer();
        MYouWinIcon.createIndexBuffer();
        
        MYouLoseIcon.createVertexBuffer();
        MYouLoseIcon.createIndexBuffer();

		destroyMaze(r, c, maze);

	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		//create the pipelines
		P1.create();
        PUI.create();
        
        //initialize Descriptor Sets related to the ciel of the labyrinth
        DSC.init(this, &DSL1, {
                            {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                            {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                            {2, TEXTURE, 0, &TC}
                        });
        
        //initialize Descriptor Sets related to the UI

        DSStartBarYourHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TStartBarYourHP}
                });
        
        DSEndBarYourHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TEndBarYourHP}
                });
        
        DSMidBarYourHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TMidBarYourHP}
                });
        
        DSyourHPwritten.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TyourHPwritten}
                });
        
        DSStartBarBossHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TStartBarBossHP}
                });
        
        DSEndBarBossHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TEndBarBossHP}
                });
        
        DSMidBarBossHP.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TMidBarBossHP}
                });
        
        DSbossHPwritten.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TbossHPwritten}
                });
        
        DSfindAllTheKeys.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TfindAllTheKeys}
                });
        
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            DescriptorSet additiveDS;
            additiveDS.init(this, &DSLUI, {
                        {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                        {1, TEXTURE, 0, &TKeyToTake}
                    });
            DSKeyToTake.push_back(additiveDS);
        }
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            DescriptorSet additiveDS;
            additiveDS.init(this, &DSLUI, {
                        {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                        {1, TEXTURE, 0, &TKeyTook}
                    });
            DSKeyTook.push_back(additiveDS);
        }
        
        DSnowGoToTheBoss.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TnowGoToTheBoss}
                });
        
        DSWfired.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &Tfired}
                });
        
        DSWsafeArea.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TsafeArea}
                });
        
        DSWnotWatchingTheBoss.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TnotWatchingTheBoss}
                });
        
        DSWtooDistantFromTheBoss.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TtooDistantFromTheBoss}
                });
        
        DSWbossHurtsYou.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TbossHurtsYou}
                });
        
        DSWbossHasSlammedYouAgainstToTheWall.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TbossHasSlammedYouAgainstToTheWall}
                });
        
        DSFireIcon.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TFireIcon}
                });
        
        DSYouWinIcon.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TYouWinIcon}
                });
        
        DSYouLoseIcon.init(this, &DSLUI, {
                    {0, UNIFORM, sizeof(UniformBufferObjectUI), nullptr},
                    {1, TEXTURE, 0, &TYouLoseIcon}
                });
        
        //initialize Descriptor Sets related to the objects in the labyrinth
        
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            DescriptorSet additiveDS;
            additiveDS.init(this, &DSL1, {
                        {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                        {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                        {2, TEXTURE, 0, &TK}
                    });
            DSK.push_back(additiveDS);
        }

        for(int i = 0; i < effectiveNumberOfFood; i++){
            DescriptorSet additiveDS;
            additiveDS.init(this, &DSL1, {
                        {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                        {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                        {2, TEXTURE, 0, &TF}
                    });
            DSF.push_back(additiveDS);
        }

        for (int i = 0; i < effectiveNumberOfWalls; i++) {
            DescriptorSet DSWall;
            DSWall.init(this, &DSL1, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TLab}
            });
            DSW.push_back(DSWall);
        }

        for (int i = 0; i < effectiveNumberOfLights; i++) {
            DescriptorSet DSLight;
            DSLight.init(this, &DSL1, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TLab}
            });
            DSLights.push_back(DSLight);
        }

        DSD.init(this, &DSL1, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                {2, TEXTURE, 0, &TD}
        });

        DSB.init(this, &DSL1, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                {2, TEXTURE, 0, &TB}
        });

        for (int i = 0; i < effectiveNumberOfGround; i++) {
            DescriptorSet DSGround;
            DSGround.init(this, &DSL1, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TLab}
            });
            DSG.push_back(DSGround);
        }

	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {
        //clean up the pipelines
		P1.cleanup();
        PUI.cleanup();
        
        //clean up Descriptor Sets related to the ciel
        DSC.cleanup();
		
        //clean up Descriptor Sets related to the UI
        DSStartBarYourHP.cleanup();
        DSEndBarYourHP.cleanup();
        DSMidBarYourHP.cleanup();
        DSyourHPwritten.cleanup();
        DSStartBarBossHP.cleanup();
        DSEndBarBossHP.cleanup();
        DSMidBarBossHP.cleanup();
        DSbossHPwritten.cleanup();
        DSfindAllTheKeys.cleanup();
        for(DescriptorSet ds : DSKeyToTake){
            ds.cleanup();
        }
        for(DescriptorSet ds : DSKeyTook){
            ds.cleanup();
        }
        DSnowGoToTheBoss.cleanup();
        DSWfired.cleanup();
        DSWsafeArea.cleanup();
        DSWnotWatchingTheBoss.cleanup();
        DSWtooDistantFromTheBoss.cleanup();
        DSWbossHurtsYou.cleanup();
        DSWbossHasSlammedYouAgainstToTheWall.cleanup();
        DSFireIcon.cleanup();
        DSYouWinIcon.cleanup();
        DSYouLoseIcon.cleanup();
        
        //clean up Descriptor Sets related to the objects in the labyrinth
        for(DescriptorSet ds : DSK){
            ds.cleanup();
        }
        for(DescriptorSet ds : DSF){
            ds.cleanup();
        }
        for (DescriptorSet ds : DSW) {
            ds.cleanup();
        }
        for (DescriptorSet ds : DSLights) {
            ds.cleanup();
        }
        DSD.cleanup();
        DSB.cleanup();
        for (DescriptorSet ds : DSG) {
            ds.cleanup();
        }

	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {
        //clean up textures related to the objects in the labyrinth
        TC.cleanup();
        TLab.cleanup();
        TD.cleanup();
        TB.cleanup();
        TK.cleanup();
        TF.cleanup();
        
        //clean up textures related to the UI
        TStartBarYourHP.cleanup();
        TEndBarYourHP.cleanup();
        TMidBarYourHP.cleanup();
        TyourHPwritten.cleanup();
        TStartBarBossHP.cleanup();
        TEndBarBossHP.cleanup();
        TMidBarBossHP.cleanup();
        TbossHPwritten.cleanup();
        TfindAllTheKeys.cleanup();
        TKeyToTake.cleanup();
        TKeyTook.cleanup();
        TnowGoToTheBoss.cleanup();
        Tfired.cleanup();
        TsafeArea.cleanup();
        TnotWatchingTheBoss.cleanup();
        TtooDistantFromTheBoss.cleanup();
        TbossHurtsYou.cleanup();
        TbossHasSlammedYouAgainstToTheWall.cleanup();
        TFireIcon.cleanup();
        TYouWinIcon.cleanup();
        TYouLoseIcon.cleanup();
        
        //clean up models related to the UI
        MStartBarYourHP.cleanup();
        MEndBarYourHP.cleanup();
        MMidBarYourHP.cleanup();
        MyourHPwritten.cleanup();
        MStartBarBossHP.cleanup();
        MEndBarBossHP.cleanup();
        MMidBarBossHP.cleanup();
        MbossHPwritten.cleanup();
        MfindAllTheKeys.cleanup();
        for (Model keyToTake : MKeyToTake){
            keyToTake.cleanup();
        }
        for (Model keyTook : MKeyTook){
            keyTook.cleanup();
        }
        MnowGoToTheBoss.cleanup();
        MWfired.cleanup();
        MWsafeArea.cleanup();
        MWnotWatchingTheBoss.cleanup();
        MWtooDistantFromTheBoss.cleanup();
        MWbossHurtsYou.cleanup();
        MWbossHasSlammedYouAgainstToTheWall.cleanup();
        MFireIcon.cleanup();
        MYouWinIcon.cleanup();
        MYouLoseIcon.cleanup();
        
        //clean up models related to the objects in the labyrinth
        MC.cleanup();
        MK.cleanup();
        MW.cleanup();
        MLight.cleanup();
        MD.cleanup();
        MB.cleanup();
        MG1.cleanup();
        MG2.cleanup();
        MF.cleanup();
        
        //clean up descriptor set layout
		DSL1.cleanup();
        DSLUI.cleanup();
        
        //destroy pipelines
		P1.destroy();
        PUI.destroy();
        
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		P1.bind(commandBuffer);//bind main pipeline
        
        MC.bind(commandBuffer);
                DSC.bind(commandBuffer, P1, currentImage);
                vkCmdDrawIndexed(commandBuffer,
                        static_cast<uint32_t>(MC.indices.size()), 1, 0, 0, 0);

        MK.bind(commandBuffer);
        for(int i = 0; i < DSK.size(); i++){
            DSK[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MK.indices.size()), 1, 0, 0, 0);
        }

        MF.bind(commandBuffer);
        for(int i = 0; i < DSF.size(); i++){
            DSF[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MF.indices.size()), 1, 0, 0, 0);
        }

        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MStartBarYourHP.indices.size()), 1, 0, 0, 0);//draw the models based on their indexes
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MEndBarYourHP.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MMidBarYourHP.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MyourHPwritten.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MStartBarBossHP.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MEndBarBossHP.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MMidBarBossHP.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MbossHPwritten.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MfindAllTheKeys.indices.size()), 1, 0, 0, 0);
        
        for (int i = 0; i < MKeyToTake.size(); i++) {
            vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MKeyToTake[i].indices.size()), 1, 0, 0, 0);
        }
        for (int i = 0; i < MKeyTook.size(); i++) {
            vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MKeyTook[i].indices.size()), 1, 0, 0, 0);
        }
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MnowGoToTheBoss.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWfired.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWsafeArea.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWnotWatchingTheBoss.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWtooDistantFromTheBoss.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWbossHurtsYou.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MWbossHasSlammedYouAgainstToTheWall.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MFireIcon.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MYouWinIcon.indices.size()), 1, 0, 0, 0);
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MYouLoseIcon.indices.size()), 1, 0, 0, 0);

        MW.bind(commandBuffer);
        for (int i = 0; i < DSW.size(); i++) {
            DSW[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MW.indices.size()), 1, 0, 0, 0);//for each descriptor set draw the model
        }

        MLight.bind(commandBuffer);
        for(int i = 0; i < DSLights.size(); i++){
            DSLights[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(MLight.indices.size()), 1, 0, 0, 0);
        }

        MD.bind(commandBuffer);
        DSD.bind(commandBuffer, P1, currentImage);
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MD.indices.size()), 1, 0, 0, 0);

        MB.bind(commandBuffer);
        DSB.bind(commandBuffer, P1, currentImage);
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MB.indices.size()), 1, 0, 0, 0);

        for (int i = 0; i < DSG.size(); i++) {
            DSG[i].bind(commandBuffer, P1, currentImage);
            if(groundType[i]){//draw a different ground type based on groundType vector
                MG2.bind(commandBuffer);
                vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MG2.indices.size()), 1, 0, 0, 0);
            } else {
                MG1.bind(commandBuffer);
                vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MG1.indices.size()), 1, 0, 0, 0);
            }
        }
        
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MC.indices.size()), 1, 0, 0, 0);
        
        PUI.bind(commandBuffer);//bind pipeline for the UI
        
        MStartBarYourHP.bind(commandBuffer);
        DSStartBarYourHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MStartBarYourHP.indices.size()), 1, 0, 0, 0);
        
        MEndBarYourHP.bind(commandBuffer);
        DSEndBarYourHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MEndBarYourHP.indices.size()), 1, 0, 0, 0);
        
        MMidBarYourHP.bind(commandBuffer);
        DSMidBarYourHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MMidBarYourHP.indices.size()), 1, 0, 0, 0);
        
        MyourHPwritten.bind(commandBuffer);
        DSyourHPwritten.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MyourHPwritten.indices.size()), 1, 0, 0, 0);
        
        MStartBarBossHP.bind(commandBuffer);
        DSStartBarBossHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MStartBarBossHP.indices.size()), 1, 0, 0, 0);
        
        MEndBarBossHP.bind(commandBuffer);
        DSEndBarBossHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MEndBarBossHP.indices.size()), 1, 0, 0, 0);
        
        MMidBarBossHP.bind(commandBuffer);
        DSMidBarBossHP.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MMidBarBossHP.indices.size()), 1, 0, 0, 0);
        
        MbossHPwritten.bind(commandBuffer);
        DSbossHPwritten.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MbossHPwritten.indices.size()), 1, 0, 0, 0);
        
        MfindAllTheKeys.bind(commandBuffer);
        DSfindAllTheKeys.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MfindAllTheKeys.indices.size()), 1, 0, 0, 0);
        
        for (int i = 0; i < MKeyToTake.size(); i++) {
            MKeyToTake[i].bind(commandBuffer);
            DSKeyToTake[i].bind(commandBuffer, PUI, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MKeyToTake[i].indices.size()), 1, 0, 0, 0);
        }
        for (int i = 0; i < MKeyTook.size(); i++) {
            MKeyTook[i].bind(commandBuffer);
            DSKeyTook[i].bind(commandBuffer, PUI, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MKeyTook[i].indices.size()), 1, 0, 0, 0);
        }
        
        MnowGoToTheBoss.bind(commandBuffer);
        DSnowGoToTheBoss.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MnowGoToTheBoss.indices.size()), 1, 0, 0, 0);
        
        MWfired.bind(commandBuffer);
        DSWfired.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWfired.indices.size()), 1, 0, 0, 0);
        
        MWsafeArea.bind(commandBuffer);
        DSWsafeArea.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWsafeArea.indices.size()), 1, 0, 0, 0);
        
        MWnotWatchingTheBoss.bind(commandBuffer);
        DSWnotWatchingTheBoss.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWnotWatchingTheBoss.indices.size()), 1, 0, 0, 0);
        
        MWtooDistantFromTheBoss.bind(commandBuffer);
        DSWtooDistantFromTheBoss.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWtooDistantFromTheBoss.indices.size()), 1, 0, 0, 0);
        
        MWbossHurtsYou.bind(commandBuffer);
        DSWbossHurtsYou.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWbossHurtsYou.indices.size()), 1, 0, 0, 0);
        
        MWbossHasSlammedYouAgainstToTheWall.bind(commandBuffer);
        DSWbossHasSlammedYouAgainstToTheWall.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MWbossHasSlammedYouAgainstToTheWall.indices.size()), 1, 0, 0, 0);
        
        MFireIcon.bind(commandBuffer);
        DSFireIcon.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MFireIcon.indices.size()), 1, 0, 0, 0);
        
        MYouWinIcon.bind(commandBuffer);
        DSYouWinIcon.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MYouWinIcon.indices.size()), 1, 0, 0, 0);
        
        MYouLoseIcon.bind(commandBuffer);
        DSYouLoseIcon.bind(commandBuffer, PUI, currentImage);
        vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(MYouLoseIcon.indices.size()), 1, 0, 0, 0);

	}

    glm::mat4 MakeWorldMatrix(glm::vec3 pos, glm::quat rQ, glm::vec3 size) {
        // creates and returns a World Matrix that positions the object at <pos>,
        // orients it according to <rQ>, and scales it according to the sizes
        // given in vector <size>
        glm::mat4 Translation = glm::translate(glm::mat4(1), pos);
        glm::mat4 Rotation = glm::mat4(rQ);
        glm::mat4 Scale = glm::scale(glm::mat4(1), size);
        glm::mat4 M = Translation * Rotation * Scale;
        return M;
    }
    
    glm::mat4 MakeWorldMatrixForUI(glm::vec3 UiPos, glm::vec3 UiPosOffset, glm::quat rVertical, glm::quat rHorizhontal, glm::vec3 size, float scaleMoveX, float scaleMoveY) {
        //similar to MakeWorldMatrix but for the UI where the UiPosOffset translation is done then to move the UI according to this
        //offset (dued to window dim and Aspect Ratio) after all rotations; moreover there is a scale to scale an UI element horizzontaly
        //or vertically respect to the point 0,0 of the UI
        const float uiMov = 0.05f;
        const float uiOffset = 0.2f;
        glm::mat4 ScaleMove = glm::mat4(scaleMoveX,0,0,0,0,1,0,0,0,0,scaleMoveY,0,0,0,0,1);
        glm::mat4 backTr = glm::translate(glm::mat4(1), glm::vec3(-uiOffset*uiMov,0.0f,uiOffset*uiMov));
        glm::mat4 backTrR = glm::translate(glm::mat4(1), glm::vec3(uiOffset*uiMov,0.0f,-uiOffset*uiMov));
        glm::mat4 TranslationPos = glm::translate(glm::mat4(1), UiPos);
        glm::mat4 TranslationPosOffset = glm::translate(glm::mat4(1), UiPosOffset);
        glm::mat4 RotationVertical = glm::mat4(rVertical);
        glm::mat4 RotationHorizhontal = glm::mat4(rHorizhontal);
        glm::mat4 Scale = glm::scale(glm::mat4(1), size);
        glm::mat4 M = TranslationPos * RotationHorizhontal * RotationVertical * TranslationPosOffset * backTrR * ScaleMove * backTr * Scale;
        return M;
    }
    
    glm::mat4 ObtainWorldMatrixForUI(float xMov, float yMov, float scaleMoveX, float scaleMoveY){
        //objects for the UI are rotated by 90 degree vertically; then rotated basing on the camera rot, then translated basing on the
        //position of the camera and finally adjusted in the window with some offset also depending by the Aspect Ratio of the window
        //and the movement in case the element of the UI can move
        const float uiMov = 0.05f;
        glm::mat4 baseTr = glm::mat4(1.0f);
        glm::quat UiRotVertical = glm::quat(glm::vec3((CamBeta + glm::radians(-90.0f)), 0, 0));
        glm::quat UiRotHorizhontal = glm::quat(glm::vec3(0, CamAlpha, 0));
        glm::vec3 UiPos = CamPos;
        const float screenPosOffset = -0.013f;
        const float startingAr = 4.0f/3.0f;
        const float arAdjustmentDivider = 25.0f;
        float arAdjustment = startingAr/arAdjustmentDivider - Ar/arAdjustmentDivider;
        glm::vec3 UiPosOffset = glm::vec3(screenPosOffset + arAdjustment, 0.0f, 0.0f) + glm::vec3(xMov*uiMov, 0.0f, -yMov*uiMov);
        const float sizeUi = 0.20f;
        return MakeWorldMatrixForUI(UiPos, UiPosOffset, UiRotVertical, UiRotHorizhontal, glm::vec3(sizeUi), scaleMoveX, scaleMoveY) * baseTr;
    }

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {

		const float ROT_SPEED = glm::radians(150.0f);
        const float FAST_ROT_SPEED = glm::radians(300.0f);
		const float MOVE_SPEED = 6.0f;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;

        static bool goingInstantLeft = false;//you are continuing to go to watch to the nearest 90 degree angle on your left because you have decided about this before
        static bool goingInstantRight = false;//you are continuing to go to watch to the nearest 90 degree angle on your right because you have decided about this before
        bool goInstantLeft = false;//you decide to watch to the nearest 90 degree angle on your left
        bool goInstantRight = false;//you decide to watch to the nearest 90 degree angle on your right
        bool youWantToRestartThisLabyrinth = false;//you decide to restart with this labyrinth

		getSixAxis(deltaT, m, r, fire, goInstantLeft, goInstantRight, youWantToRestartThisLabyrinth);

        //if you lose, you can't move
        if(IF_YOU_LOSE_YOU_CANT_MOVE && youLose){
            m.x = 0;
            m.y = 0;
            m.z = 0;
            r.x = 0;
            r.y = 0;
            fire = false;
            goInstantLeft = false;
            goInstantRight = false;
        }

        //check if you have to (continue to) go to watch a nearest 90 degree angle
        if(goInstantLeft){
            goingInstantLeft = true;
        }
        if(goInstantRight){
            goingInstantRight = true;
        }
        if(goingInstantLeft && goingInstantRight){
            goingInstantLeft = false;
            goingInstantRight = false;
        }
        
        //move your camera (CamAlpha)
        if(!goingInstantLeft && !goingInstantRight){//if you are not using fast rotation to the nearest 90 degree angle
            CamAlpha = CamAlpha - ROT_SPEED * deltaT * r.y;//manage rotation normally (normal rotation of CamAlpha)
        } else {//manage fast rotation to the nearest 90 degree angle
            const float pih = 3.14159265358979f / 2.0f;//there are 4 possible direction 90 degree directions
            float resDiv = CamAlpha / pih;
            int resDivInt = resDiv;
            float anglePrev = pih * resDivInt;//previous 90 degree angle
            float angleNext = anglePrev + pih;//next 90 degree angle
            if(resDiv < 0){
                angleNext = pih * resDivInt;
                anglePrev = anglePrev - pih;
            }
            if(CamAlpha == anglePrev){//manage if you start by a 90 degree position and you want to go to another 90 degree position on left
                anglePrev = anglePrev - 1;
            }
            if(CamAlpha == angleNext){//manage if you start by a 90 degree position and you want to go to another 90 degree position on right
                angleNext = angleNext + 1;
            }
            float requiredMovement = 0.0f;
            if(goingInstantLeft){
                requiredMovement = -1.0f;
            }
            if(goingInstantRight){
                requiredMovement = 1.0f;
            }
            float nextCamAlpha = CamAlpha - FAST_ROT_SPEED * deltaT * requiredMovement;//where you should see after this rotation
            if(nextCamAlpha > anglePrev && nextCamAlpha < angleNext){//rotation is ok: you can rotate normally
                CamAlpha = nextCamAlpha;
            } else {//you collapse to the nearest 90 degree angle and you complete the rotation
                if(nextCamAlpha <= anglePrev){
                    CamAlpha = anglePrev;
                } else if(nextCamAlpha >= angleNext){
                    CamAlpha = angleNext;
                }
                goingInstantLeft = false;
                goingInstantRight = false;
            }
        }

        //Animation boss appear when the boss fight is started
        const float BOSS_SCALE_SPEED = 3.5f;
        const float BOSS_START_HP_SPEED = 70.0f;
        if(bossFightStarted){
            bossScale += BOSS_SCALE_SPEED * deltaT;
            if(!bossFightStartedAnimationFinished){
                bossHP += BOSS_START_HP_SPEED * deltaT;//during the animation the boss bar HP fill and the boss obtain it's HP
                //at the end of the animation the boss will have about 55 HP
            }
        }
        if(bossScale > maxBossScale){
            bossScale = maxBossScale;
            bossFightStartedAnimationFinished = true;//the animation that the boss appears is finished
        }
        
        //move your camera (CamBeta)
        float CamBetaPrec = CamBeta;
        CamBeta  = CamBeta - ROT_SPEED * deltaT * r.x;
		CamBeta  =  CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
				   (CamBeta > glm::radians( 90.0f) ? glm::radians( 90.0f) : CamBeta);
        
        //there is a limitation of camera movement (CamBeta)
        if(!YOU_CAN_FLY){
            const float limitationCamBeta = glm::radians(45.0f);
            if(abs(CamBeta) > limitationCamBeta){
                CamBeta = CamBetaPrec;
            }
        }
        
        //your movements (CamAlpha and CamPos)
		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
        CamPosPrec = CamPos;
        if(!youNeedToWalkAwayFromTheBoss){//normally you move using W A S D keys; but, in case the boss hurts you and you have to walk away from him, your movements are temporanely locked
            if(tiltTimeCounter <= 0.0f){//moreover, if you are tilt (because during the boss fight you are slammed against the wall) you can't move for a while
                CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
                if(YOU_CAN_FLY){
                    CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
                }
                CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
            }
        }

        const float minDistToWalls = 0.15f;//min distance to walls you have to mantain when moving in the labyrinth (otherwise you see through the walls)

        //You can't go over a wall or pass through the door if there are other keys to take in the labyrinth
        if(labyrinthShapeInitialized){
            //you can't pass through a wall
            if(labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x-minDistToWalls)] == true){
                CamPos = CamPosPrec;//return to the previous position
                //go on even if you are lightly passing through walls
                const float pih = 3.14159265358979f / 2.0f;//there are 4 possible direction the walls are oriented
                //find the residue value to the nearest direction of the 4 possible you are going
                int resultDiv = CamAlpha / pih;
                float resDivVal = resultDiv * ((float)pih);
                float residue = CamAlpha - resDivVal;
                if(residue < 0){
                    residue = residue + pih;
                    resDivVal = resDivVal - pih;
                }
                const float tolleranceDivider = 3.0f;
                float CamAlphaPrec = CamAlpha;
                bool adjustLightlyPassingThroughWalls = false;
                //if you are lightly passing through a wall, the correct direction is temporanely recovered to let you go on
                if(residue < pih / tolleranceDivider){
                    CamAlpha = resDivVal;
                    adjustLightlyPassingThroughWalls = true;
                } else if(residue > pih - (pih / tolleranceDivider)){
                    CamAlpha = resDivVal + pih;
                    adjustLightlyPassingThroughWalls = true;
                }
                if(adjustLightlyPassingThroughWalls == true){
                    //go on
                    ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
                    uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
                    CamPosPrec = CamPos;
                    CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
                    CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
                    if(labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x-minDistToWalls)] == true){
                        CamPos = CamPosPrec;//the new position need to be safe (not inside a wall)
                    }
                    CamAlpha = CamAlphaPrec;//recover the true direction you was going
                }
            }
            //you can't pass through the door if there are other keys to take in the labyrinth
            if(reducedLabyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || reducedLabyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || reducedLabyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || reducedLabyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x-minDistToWalls)] == true){
                if(CamPos.x <= originalDoorPos.x + minDistToWalls){//if you are also trying to pass through the door going left (into the room)
                    bool youHaveTakenAllTheKeys = true;//check if you have taken all the keys
                    for(int i = 0; i < effectiveNumberOfKeys; i++){
                        if(!tookKey[i]){
                            youHaveTakenAllTheKeys = false;
                        }
                    }
                    if(!youHaveTakenAllTheKeys){//if you have not taken all the keys
                        CamPos = CamPosPrec;//return to the previous position
                    } else {//you have passed normally having all the keys; so you are in and the boss fight start
                        bossFightStarted = true;
                        doorPos = originalDoorPos + glm::vec3(minDistToWalls, 0.0f, 0.0f);
                        doorRot = glm::quat(glm::vec3(0, glm::radians(90.0f), 0)) *
                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));//now you are locked into the boss fight room
                    }
                }
            }
            //you must stay in the boss room if you are in and the boss fight is started
            if(bossFightStarted){
                if(CamPos.x > originalDoorPos.x && CamPos.x > CamPosPrec.x){//you can't exit from the boss room when you are in and the boss fight is started
                    CamPos.x = CamPosPrec.x;
                }
            }
        }
        
        //periodically you lose some HP
        if(!youWin){
            yourHP -= deltaT * 0.5f;
            if(yourHP < 0.0f){
                yourHP = 0.0f;
                if(!youLose && !youWin){
                    youLose = true;
                    std::cout << "you lose!\n";
                }
            }
        }
        
        //your position in the labyrinth
        int x = CamPos.x;
        int y = CamPos.z;

        // In case you take a key
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(x == xKeyPos[i] && y == yKeyPos[i] && tookKey[i] == false){
                int howManyRemainingKeys = 0;
                for(int j = 0; j < effectiveNumberOfKeys; j++){
                    if(tookKey[j] == false){
                        howManyRemainingKeys++;
                    }
                }
                std::cout << "You took a key (remaining keys: ";
                std::cout << howManyRemainingKeys;
                std::cout << ")\n";
                tookKey[i] = true;
            }
        }

        // In case you have taken all the keys and the boss fight isn't started, open the door
        bool youHaveTakenAllTheKeys = true;//check if you have taken all the keys
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(!tookKey[i]){
                youHaveTakenAllTheKeys = false;
            }
        }
        if(youHaveTakenAllTheKeys && !bossFightStarted){
            doorPos = notVisiblePosition;
        }

        // In case you take a food
        for(int i = 0; i < effectiveNumberOfFood; i++){
            if(x == xFoodPos[i] && y == yFoodPos[i] && tookFood[i] == false){
                if(tookFood[i] == false){
                    std::cout << "You took a food\n";
                    yourHP += 6.8f;//you recover 6.8f HP when you take a food
                }
                tookFood[i] = true;
            }
        }

        const float BOSS_ROT_SPEED = glm::radians(110.0f);
        const float BOSS_MOVE_SPEED = 0.01f;
        const float WALK_AWAY_SPEED = 0.04f;//speed to walk away from the boss in case the boss hurts you

        float distBossYou = sqrt((pow((CamPos.x - bossPos.x),2)) + (pow((CamPos.z - bossPos.z),2)));//distance between the boss and you
        if(CamPos.x - bossPos.x == 0.0f){//to avoid division by 0
            const float nearToZero = 0.00000000001;
            bossPos.x = nearToZero;
        }
        float dirBossYou = atan((CamPos.z - bossPos.z) / (CamPos.x - bossPos.x));//direction of you from the boss
        //adjust dirBossYou val
        const float pih = (3.14159265358979f / 2.0f);
        if((CamPos.x - bossPos.x) >= 0.0f && (CamPos.z - bossPos.z) >= 0.0f){
            dirBossYou += 0.0f * pih;//I clock face
        }
        if((CamPos.x - bossPos.x) < 0.0f && (CamPos.z - bossPos.z) >= 0.0f){
            dirBossYou += 2.0f * pih;//II clock face
        }
        if((CamPos.x - bossPos.x) < 0.0f && (CamPos.z - bossPos.z) < 0.0f){
            dirBossYou += 2.0f * pih;//III clock face
        }
        if((CamPos.x - bossPos.x) >= 0.0f && (CamPos.z - bossPos.z) < 0.0f){
            dirBossYou += 4.0 * pih;//IV clock face
        }

        // boss rotation
        float incrDir = 1.0f;
        if(abs(dirBossYou - bossRot) <= glm::radians(180.0f) && bossRot > dirBossYou ||
           abs(dirBossYou - bossRot) > glm::radians(180.0f) && bossRot < dirBossYou){//the boss watchs you
            incrDir = -1.0f;
        }
        const float minIncrRotation = glm::radians(1.0f);//to avoid lag in boss rotation
        if(abs(dirBossYou - bossRot) <= glm::radians(180.0f) && (abs(bossRot - dirBossYou)) < minIncrRotation ||
           abs(dirBossYou - bossRot) > glm::radians(180.0f) && (abs(bossRot - dirBossYou)) > glm::radians(360.0f) - minIncrRotation){
            incrDir = 0.0f;
        }
        bossRot = bossRot + BOSS_ROT_SPEED * deltaT * incrDir;//the boss rotate
        if(bossRot < glm::radians(0.0f)){// bossRot between 0 and 360 degree
            bossRot += glm::radians(360.0f);
        } else if(bossRot >= glm::radians(360.0f)){
            bossRot -= glm::radians(360.0f);
        }

        // in case you are too near to the boss the boss the boss deals damage to you and you walk away morover you lose some life
        const float minDistFromTheBoss = 1.0f;
        const float maxDistFromTheBoss = 2.0f;//distance to walk away from the boss
        const float damageHP = 3.65f;
        if(distBossYou < minDistFromTheBoss || youNeedToWalkAwayFromTheBoss){
            if(youNeedToWalkAwayFromTheBoss == false){//in this instant the boss hurts you
                std::cout << "the boss hurts you\n";
                writtenDuringBossFightToVisualize = writtenDuringBossFight::bossHurtsYou;
                const float fireChargingPenality = 0.75f;//a penality to wait some time to fire if the boss hurts you
                fireCharging = fireChargingPenality;
                //you take damage and you lose some HPs
                if(!youWin){
                    yourHP -= damageHP;
                }
            }
            youNeedToWalkAwayFromTheBoss = true;
            float walkAwayXIncr = WALK_AWAY_SPEED * cos(dirBossYou);
            float walkAwayZIncr = WALK_AWAY_SPEED * sin(dirBossYou);
            CamPos.x = CamPos.x + walkAwayXIncr;
            CamPos.z = CamPos.z + walkAwayZIncr;
            //in this case that you are walking away from the boss, we have to check that you are not going over a wall or you are
            //exiting from the boss fight area; else it menas that you are slammed against the wall and you also take a lot of extra
            //damages and there are some other penalities
            if(youNeedToWalkAwayFromTheBoss){
                if(labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || CamPos.x > originalDoorPos.x){
                    youNeedToWalkAwayFromTheBoss = false;//you are slammed against the wall and you stop to walk away from the boss
                    CamPos = CamPosPrec;//remain to that position (you can't go over the wall)
                    timeBossPatternCounter = timeBossApproachYou;//the pattern of the boss changes to go away from you; in this way you have the possibility to return to fight the boss
                    const float fireChargingPenalitySlammedAgainistWall = 1.5f;//in case you take damage being slammed against the wall, the penality to wait some time to fire is different
                    fireCharging = fireChargingPenalitySlammedAgainistWall;
                    const float tiltTimePenality = 0.25f;//morover another penality is that you are tilt and you can't move for a while
                    tiltTimeCounter = tiltTimePenality;
                    std::cout << "you are slammed against the wall and you take extra damages\n";
                    writtenDuringBossFightToVisualize = writtenDuringBossFight::bossHasSlammedYouAgainstToTheWall;
                    //you take extra damages and you lose other HPs
                    if(!youWin){
                        yourHP -= damageHP;
                    }
                }
            }
            if(distBossYou >= maxDistFromTheBoss){
                youNeedToWalkAwayFromTheBoss = false;//you are enough distant from the boss and so you stop to go away from him
            }
        }

        // in case, during the boss fight, you are slammed against the wall, you are tilted and you can't move for a while: here this time counter is managed
        tiltTimeCounter -= deltaT;
        if(tiltTimeCounter < 0.0f){
            tiltTimeCounter = 0.0f;
        }

        // the boss has a pattern that sometimes approaches you and sometimes goes away from you
        //here there is managed the time (when the boss approaches you and when the boss goes away from you) and the multiplier bossPatternMultiplier for the movement of the boss is setted
        timeBossPatternCounter += deltaT;
        if(timeBossPatternCounter < timeBossApproachYou){
            bossPatternMultiplier = 1.0f;
        } else if(timeBossPatternCounter < timeBossApproachYou + timeBossGoesAwayFromYou){
            bossPatternMultiplier = -PenalityMultiplierBossGoesAwayFromYou;
        } else {
            timeBossPatternCounter = 0.0f;
        }
        //here, in case the animation that the boss fight is started is finished, the movement of the boss is managed
        if(bossFightStartedAnimationFinished){
            glm::vec3 bossPosPrec = bossPos;
            float bossPosXIncrFact = cos(dirBossYou);
            float bossPosZIncrFact = sin(dirBossYou);
            const float minIncrToApproach = 0.05f;//to avoid lag
            if(abs(bossPosXIncrFact) < minIncrToApproach){
                bossPosXIncrFact = 0.0f;
            }
            if(abs(bossPosZIncrFact) < minIncrToApproach){
                bossPosZIncrFact = 0.0f;
            }
            float bossPosXIncr = BOSS_MOVE_SPEED * bossPosXIncrFact * bossPatternMultiplier;
            float bossPosZIncr = BOSS_MOVE_SPEED * bossPosZIncrFact * bossPatternMultiplier;
            bossPos.x = bossPos.x + bossPosXIncr;
            bossPos.z = bossPos.z + bossPosZIncr;
            if(bossPos.x < xStartLimitBossPos || bossPos.x > xEndLimitBossPos){// the boss can't exit from its area
                bossPos.x = bossPosPrec.x;
            }
            if(bossPos.z < zStartLimitBossPos || bossPos.z > zEndLimitBossPos){
                bossPos.z = bossPosPrec.z;
            }
        }

        // in case you fire to the boss
        const float timeToChargeTheFire = 0.5f;//needed time to fire again
        fireCharging -= deltaT;
        if(fireCharging < 0.0f){//you can fire
            fireCharging = 0.0f;
            writtenDuringBossFightToVisualize = writtenDuringBossFight::none;
        }
        // in case you use the fire command and the boss fight is started
        if(fire){
            if(bossFightStarted){
                if(fireCharging <= 0){//the fire need to be charged
                    fireCharging = timeToChargeTheFire;
                    const float minDistFromTheBossToFire = 3.0f;
                    const float minDistAngleToFireTheBoss = glm::radians(30.0f);
                    if(distBossYou <= minDistFromTheBossToFire){//to fire the boss you need to be near to it
                        if(CamPos.x > originalDoorPos.x - 0.5f){//in case you fire from the safe area, the hit is not valid
                            std::cout << "You have fired but you are in the safe area and so the hit is not valid\n";
                            writtenDuringBossFightToVisualize = writtenDuringBossFight::safeArea;
                        } else {
                            float dirYouBoss = glm::radians(90.0f) - CamAlpha;//the direction you are watching has this relation from CamAlpha; this value (after a moudle 360 degre adjustment) can be compared with dirBossYou to check if you are looking to the boss direction
                            while(dirYouBoss <= glm::radians(0.0f)){//TODO questo while si può spostare
                                dirYouBoss += glm::radians(360.0f);
                            }
                            while(dirYouBoss > glm::radians(360.0f)){
                                dirYouBoss -= glm::radians(360.0f);
                            }
                            if(abs(dirBossYou - dirYouBoss) < minDistAngleToFireTheBoss || abs(dirBossYou - dirYouBoss) > glm::radians(360.0f) - minDistAngleToFireTheBoss){//to fire the boss you need also to watch it with your camera
                                float firePow = (pow((minDistFromTheBossToFire - distBossYou),3)*0.57f)+0.13f;//the power of your fire depends by the distance you manage to fire the boss! If you are nearer to the boss you inflict more damages but you are more in danger in the boss hurts you
                                std::cout << "You have fired!!! (pow: ";
                                std::cout << firePow;
                                std::cout << ") (you are near to the boss and you are watching the boss with your camera)\n";
                                writtenDuringBossFightToVisualize = writtenDuringBossFight::fired;
                                //the boss takes damages
                                bossHP -= firePow;
                                lastFirePow = firePow;//show the fire icon based on how muche power was your fire
                            } else {
                                std::cout << "You have fired but you are not watching the boss with your camera; you are near to the boss\n";
                                writtenDuringBossFightToVisualize = writtenDuringBossFight::notWatchingTheBoss;
                            }
                        }
                    } else {
                        std::cout << "You have fired but you are too distant from the boss\n";
                        writtenDuringBossFightToVisualize = writtenDuringBossFight::tooDistantFromTheBoss;
                    }
                }
            } else {
                std::cout << "You can't fire if the boss fight isn't started\n";
            }
        }
        
        //check if maybe you have won
        if(bossHP <= 0.0f && bossFightStartedAnimationFinished && !youLose){
            youWin = true;
            //in this case there is also an animation that the boss disappears
            const float bossDisappearsSpeed = 0.5f;
            bossPos += glm::vec3(0.0f, -bossDisappearsSpeed * deltaT, 0.0f);
            const float posBossToDisapperaDefinitely = -1.5f;
            if(bossPos.y < posBossToDisapperaDefinitely){
                bossPos = notVisiblePosition;
            }
        }
        
        //in case you won (or lose) there won't be any written about the boss fight
        if(youWin || youLose){
            writtenDuringBossFightToVisualize = writtenDuringBossFight::none;
        }
        
        //in case you want to restart with this labyrint all vars are resetted
        if(youWantToRestartThisLabyrinth){
            std::cout << "Restart with this labyrint!\n";
            //reset all vars
            for(int i = 0; i < effectiveNumberOfKeys; i++){
                tookKey[i] = OBTAIN_AUTOMATICALLY_ALL_THE_KEYS;
            }
            for(int i = 0; i < effectiveNumberOfFood; i++){
                tookFood[i] = false;
            }
            bossFightStarted = false;
            bossFightStartedAnimationFinished = false;
            youNeedToWalkAwayFromTheBoss = false;
            fireCharging = 0.0f;
            tiltTimeCounter = 0.0f;
            lastFirePow = 0.0f;
            yourHP = 40.0f;
            bossHP = 0.0f;
            youLose = false;
            youWin = false;
            timeBossPatternCounter = 0.0f;
            bossPatternMultiplier = 1.0f;
            CamBeta = glm::radians(0.0f);
            CamAlpha = CamAlphaOriginal;
            CamPos = CamPosOriginal;
            writtenDuringBossFightToVisualize = writtenDuringBossFight::none;
            bossPos = bossPosOriginal;
            bossRot = 0.0f;
            bossScale = 0.0f;
        }

        // close the application
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        
        // perspective
        // this is the perspective matrix:
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.01f, 75.0f);//the vertical field of view, the aspect ratio, the near and the far plane distances.
		M[1][1] *= -1;
        
        //view transform
        // this is the view transform matrix (a matrix witch represents the position and the rotation of the camera):
		glm::mat4 Mv =  glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1,0,0)) *
						glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0,1,0)) *
						glm::translate(glm::mat4(1.0), -CamPos);

		glm::mat4 ViewPrj =  M * Mv;//View projection matrix
		UniformBufferObject ubo{};
        UniformBufferObjectUI uboUI{};
		glm::mat4 baseTr = glm::mat4(1.0f);
        
        //add a light in your position (first light)
        GlobalUniformBufferObject gubo{};
        gubo.lightPos[0] = glm::vec4(CamPos, 1.0f);
        //add a light in boss position (second light)
        gubo.lightPos[1] = glm::vec4(bossPos, 1.0f);
        
		// Here is where you actually update your uniforms

		// updates global uniforms
        for (int i = 2; i < MAX_ON_LIGHTS; i++) {
            gubo.lightPos[i] = glm::vec4(lightFirePositions[i-1], 1.0f);
        }
		gubo.lightColor = glm::vec4(1.0f, 0.27f, 0.0f, 1.0f);
		gubo.eyePos = CamPos;
        gubo.numberOfLights = effectiveNumberOfOnLights;

        //locate the UI in the scene...
        
        //start your HP bar
        if(yourHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIXPosition, 1.0f, 1.0f);//in case you don't have any HP, the bar is not shown
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//normal position for start HP bar
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSStartBarYourHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //end your HP bar
        if(yourHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case you don't have any HP, the bar is not shown
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(dimHPBar * yourHP, 0.0f, 1.0f, 1.0f);//normal position for end HP bar
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSEndBarYourHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //mid your HP bar
        if(yourHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case you don't have any HP, the bar is not shown
        } else {
            //normal position for mid HP bar
            float dimMidBarYourHPMultiplier = yourHP - 1.0f;//enlarge the mid bar based on your HP according to this factor
            if(dimMidBarYourHPMultiplier < 0.0f){
                dimMidBarYourHPMultiplier = 0.0f;
            }
            uboUI.mMat = ObtainWorldMatrixForUI(-startHPBarX * (dimMidBarYourHPMultiplier - 1.0f) + dimHPBar, 0.0f, dimMidBarYourHPMultiplier, 1.0f);//enlarge the mid bar based on your HP and adjust it's location
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSMidBarYourHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //your HP written
        if(yourHP > 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//show normally your HP bar written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case you don't have any HP, also this written is not shown
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSyourHPwritten.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //start boss HP bar
        if(bossHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIXPosition, 1.0f, 1.0f);//in case the boss doesn't have any HP, the bar is not shown
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//normal position for boss start HP bar
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSStartBarBossHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //end boss HP bar
        if(bossHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case the boss doesn't have any HP, the bar is not shown
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(dimHPBar * bossHP, 0.0f, 1.0f, 1.0f);//normal position for boss end HP bar
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSEndBarBossHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //mid boss HP bar
        if(bossHP <= 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case the boss doesn't have any HP, the bar is not shown
        } else {
            //normal position for boss mid HP bar
            float dimMidBarBossHPMultiplier = bossHP - 1.0f;//enlarge the mid bar based on boss HP according to this factor
            if(dimMidBarBossHPMultiplier < 0.0f){
                dimMidBarBossHPMultiplier = 0.0f;
            }
            uboUI.mMat = ObtainWorldMatrixForUI(-startHPBarX * (dimMidBarBossHPMultiplier - 1.0f) + dimHPBar, 0.0f, dimMidBarBossHPMultiplier, 1.0f);//enlarge the mid bar based on boss HP and adjust it's location
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSMidBarBossHP.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //boss HP written
        if(bossHP > 0.0f){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//show normally boss HP bar written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//in case the boss doesn't have any HP, also this written is not shown
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSbossHPwritten.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //check here if you have found at least a key
        bool youHaveFoundAtLeastAKey = false;
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(tookKey[i] == true){
                youHaveFoundAtLeastAKey = true;
            }
        }
        
        //check here if you have found all the keys
        bool youHaveFoundAllTheKeys = true;
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(tookKey[i] == false){
                youHaveFoundAllTheKeys = false;
            }
        }
        
        //find all the keys written
        if(youHaveFoundAtLeastAKey == false){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//if you have to find all the keys
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//if you have found at least a key this written is not shown
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSfindAllTheKeys.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //keys icon (keys you have to take)
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(tookKey[i] == false && youHaveFoundAtLeastAKey && !youHaveFoundAllTheKeys){//in case you have to take this key and you have found at least a key and you have not found all the keys
                uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//show you have to take this key
            } else {
                uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show you have to take this key
            }
            uboUI.mvpMat = ViewPrj * uboUI.mMat;
            uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
            DSKeyToTake[i].map(currentImage, &uboUI, sizeof(uboUI), 0);
        }
        
        //keys icon (keys you have took)
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(tookKey[i] == true && youHaveFoundAtLeastAKey && !youHaveFoundAllTheKeys){//in case you have took this key and you have found at least a key and you have not found all the keys
                uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//show you have took this key
            } else {
                uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show you have took this key
            }
            uboUI.mvpMat = ViewPrj * uboUI.mMat;
            uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
            DSKeyTook[i].map(currentImage, &uboUI, sizeof(uboUI), 0);
        }
        
        //now go to the boss written
        if(youHaveFoundAllTheKeys == true && !bossFightStarted){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//if you have found all the keys and you aren't to the boss fight
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//else this written is not shown
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSnowGoToTheBoss.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that you have fired (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::fired){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWfired.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that you are in the safe are and the fire is not valid (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::safeArea){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWsafeArea.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that you are not watching the boss (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::notWatchingTheBoss){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWnotWatchingTheBoss.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that you are too distant from the boss (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::tooDistantFromTheBoss){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWtooDistantFromTheBoss.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that the boss hurts you (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::bossHurtsYou){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWbossHurtsYou.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //written that the boss has slummed you againist the wall (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::bossHasSlammedYouAgainstToTheWall){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize this written
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this written
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSWbossHasSlammedYouAgainstToTheWall.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //icon of a fire that appears when you hit the boss (during boss fight)
        if(writtenDuringBossFightToVisualize == writtenDuringBossFight::fired){
            //visualize this icon
            const float enlargementFactor = 1.0f;
            float dimFireIconMultiplier = lastFirePow * enlargementFactor + 1.0f;//enlarge the fire icon based on how power was your last fire to this factor
            uboUI.mMat = ObtainWorldMatrixForUI(-fireIconX * (dimFireIconMultiplier - 1.0f) + dimBaseFireIconX, -fireIconY * (dimFireIconMultiplier - 1.0f) + dimBaseFireIconY, dimFireIconMultiplier, dimFireIconMultiplier);//enlarge the mid bar the fire icon based on how power was your last fire and adjust it's location
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't show this icon
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSFireIcon.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //icon that you win
        if(youWin){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize that you win
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't visualize that you win
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSYouWinIcon.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //icon that you lose
        if(youLose){
            uboUI.mMat = ObtainWorldMatrixForUI(0.0f, 0.0f, 1.0f, 1.0f);//visualize that you lose
        } else {
            uboUI.mMat = ObtainWorldMatrixForUI(notVisibleUIXPosition, notVisibleUIYPosition, 1.0f, 1.0f);//don't visualize that you lose
        }
        uboUI.mvpMat = ViewPrj * uboUI.mMat;
        uboUI.nMat = glm::inverse(glm::transpose(uboUI.mMat));
        DSYouLoseIcon.map(currentImage, &uboUI, sizeof(uboUI), 0);
        
        //locate models in the scene...
        
        //ciel of the labyrinth
        ubo.mMat = baseTr;
                ubo.mvpMat = ViewPrj * ubo.mMat;
                ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
                ubo.roughness = 1.0;
                ubo.ao = 0.5;
                ubo.metallic = 0.0;
                DSC.map(currentImage, &ubo, sizeof(ubo), 0);
                DSC.map(currentImage, &gubo, sizeof(gubo), 1);
        
        //keys
        for(int i = 0; i < DSK.size(); i++){
            if(tookKey[i]){
                ubo.mMat = MakeWorldMatrix(notVisiblePosition, KeyRot, KeyScale) * baseTr;//do not show a key you have already took
            } else {
                ubo.mMat = MakeWorldMatrix(keyPos[i], KeyRot, KeyScale) * baseTr;//translate the key to locate
            }
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            ubo.roughness = 0.4;
            ubo.ao = 0.5;
            ubo.metallic = 0.9;
            DSK[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSK[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }
        
        //food
        for(int i = 0; i < DSF.size(); i++){
            if(tookFood[i]){
                ubo.mMat = MakeWorldMatrix(notVisiblePosition, FoodRot, FoodScale) * baseTr;//do not show a pice of food you have already took
            } else {
                ubo.mMat = MakeWorldMatrix(foodPos[i], FoodRot, FoodScale) * baseTr;//translate the food to locate
            }
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            ubo.roughness = 0.1;
            ubo.ao = 0.5;
            ubo.metallic = 0.8;
            DSF[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSF[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }
        
        //Walls
        for (int i = 0; i < DSW.size(); i++) {
            ubo.mMat = MakeWorldMatrix(wallPos[i], wallRots[i], wallScale) * baseTr;//translate and rotate the walls to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            ubo.roughness = 0.8;
            ubo.ao = 0.5;
            ubo.metallic = 0.2;
            DSW[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSW[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }

        //Lights
        for (int i = 0; i < DSLights.size(); i++) {
            ubo.mMat = MakeWorldMatrix(lightPositions[i], lightRots[i], lightScale);//translate and rotate the lights to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            ubo.roughness = 0.1;
            ubo.ao = 0.5;
            ubo.metallic = 1.0;
            DSLights[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSLights[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }

        //Door
        ubo.mMat = MakeWorldMatrix(doorPos, doorRot, doorScale) * baseTr;//translate and rotate the door to locate
        ubo.mvpMat = ViewPrj * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
        ubo.roughness = 0.2;
        ubo.ao = 0.5;
        ubo.metallic = 0.9;
        DSD.map(currentImage, &ubo, sizeof(ubo), 0);
        DSD.map(currentImage, &gubo, sizeof(gubo), 1);

        //Boss
        glm::quat bossRotQuat = glm::quat(glm::vec3(0, -bossRot, 0)) * //equivalent quat of bossRot in radians anticlockwise
                         glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                         glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
        ubo.mMat = MakeWorldMatrix(bossPos, bossRotQuat, glm::vec3(bossScale)) * baseTr;//translate and rotate the boss to locate
        ubo.mvpMat = ViewPrj * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
        ubo.roughness = 1.0;
        ubo.ao = 0.5;
        ubo.metallic = 0.0;
        DSB.map(currentImage, &ubo, sizeof(ubo), 0);
        DSB.map(currentImage, &gubo, sizeof(gubo), 1);
        
        //ground
        for (int i = 0; i < DSG.size(); i++) {
            if (groundType[i]) {
                ubo.mMat = MakeWorldMatrix(groundPos[i], groundRot, groundScale) * baseTr;//translate the ground to locate
            } else {
                ubo.mMat = MakeWorldMatrix(groundPos[i], groundRot, groundScale) * baseTr;//translate the ground to locate
            }
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            ubo.roughness = 0.2;
            ubo.ao = 0.5;
            ubo.metallic = 0.1;
            DSG[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSG[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }
        
    }

	char **genMaze(int nr, int nc) {
        // Here the labyrinth is randomly generated
        // # Wall
        // P Player
        // K Key
        // D Door
        // B Boss fight
        // F Food
        static int numberOfTentatives = 0;
        numberOfTentatives++;
		char **out = (char **)calloc(nr, sizeof(char *));
		for(int i = 0; i < nr; i++) {
			out[i] = (char *)malloc(nc+1);
			for(int j = 0; j < nc; j++) {
				out[i][j] = '#';
			}
			out[i][nc] = 0;
		}
		srand (time(NULL));
        const int maxNumberOfTentatives = 7;//max number of tentatives to try to generate the labyrinth
        if(numberOfTentatives > maxNumberOfTentatives){
            srand(DEFAULT_RANDOM_SEED);//this is a default labyrinth in case the generation of the labyrinth take many tentatives
        }
        // Select randomly where to locate the boss fight
        const int xLenghtBossFight = 9;//need to be an odd number
        const int yLenghtBossFight = 9;
        int startXBossFight = rand() % (nr - xLenghtBossFight);
        int endXBossFight = startXBossFight + xLenghtBossFight;
        int startYBossFight = rand() % (nc - yLenghtBossFight);
        int endYBossFight = startYBossFight + yLenghtBossFight;
        for(int i = startXBossFight; i < endXBossFight; i++){//place boss fight
            for(int j = startYBossFight; j < endYBossFight; j++){
                out[i][j] = 'B';
            }
        }
        for(int i = startXBossFight; i < endXBossFight; i++){//place walls around boss fight
            out[i][startYBossFight] = 'W';
            out[i][endYBossFight-1] = 'W';
        }
        for(int j = startYBossFight; j < endYBossFight; j++){
            out[startXBossFight][j] = 'W';
            out[endXBossFight-1][j] = 'W';
        }
        out[startXBossFight+(xLenghtBossFight/2)][endYBossFight-1] = 'D';//place the door
        doorPos = glm::vec3(endYBossFight-1+0.5, 0.0, startXBossFight+(xLenghtBossFight/2)+1.0);//locate the door (position)
        originalDoorPos = doorPos;//this is this original door pos (the door will reappear also during the boss fight because you won't be able to exit from the boss room during the boss fight)
        doorRot = glm::quat(glm::vec3(0, glm::radians(90.0f), 0)) *
                            glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                            glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));//locate the door (rotation)
        bossPos = glm::vec3(startYBossFight+(yLenghtBossFight/2), 0.0, startXBossFight+(xLenghtBossFight/2)+0.5);//locate the boss (position)
        bossPosOriginal = bossPos;
        bossRot = 0.0f;//locate the boss (rotation)
        // Limit the movements of the boss
        const float minDistFromWallBoss = 0.75f;
        xStartLimitBossPos = startYBossFight + (minDistFromWallBoss + 1.0f);
        xEndLimitBossPos = endYBossFight - (minDistFromWallBoss + 1.0f);
        zStartLimitBossPos = startXBossFight + (minDistFromWallBoss + 1.0f);
        zEndLimitBossPos = endXBossFight - (minDistFromWallBoss + 1.0f);
		// Select where to dig randomly the labyrinth
        int nEstr = (nr * nc) / 7;
        for(int count = 0; count < nEstr; count++){
            int randX = rand() % nr;
            int randY = rand() % nc;
            if(out[randX][randY] == '#'){
                int elem = rand() % 3;//elem 0 = road limit ; elem 1 = turn ; elem 2 = cross
                char elemChar = 'L';//road limit
                if(elem == 1){
                    elemChar = 'T';//turn
                }
                if(elem == 2){
                    elemChar = 'C';//cross
                }
                out[randX][randY] = elemChar;
            }
        }
        // Add the border wall
        for(int i = 0; i < nr; i++){
            out[i][0] = 'W';
            out[i][nc-1] = 'W';
        }
        for(int j = 0; j < nc; j++){
            out[0][j] = 'W';
            out[nr-1][j] = 'W';
        }
        // Dig the labyrinth
        const int maxIteration = 3;
        bool firstIteration = true;
        for(int count = 0; count < maxIteration; count++){//dig roads for some iterations
            for(int i = 0; i < nr; i++){//iterations on the row roads
                for(int j = 0; j < nc; j++){
                    if(out[i][j] == 'D' && firstIteration == true){//first dig starting from the door
                        int adv = j + 1;
                        bool flag = true;
                        bool firstTimeFoundLimit = true;
                        while(flag == true){
                            if(out[i][adv] == 'L'){
                                if(firstTimeFoundLimit == true){
                                    firstTimeFoundLimit = false;
                                    out[i][adv] = 'c';
                                } else {
                                    out[i][adv] = ' ';
                                    flag = false;
                                }
                            } else {
                                if(out[i][adv] == 'W' || out[i][adv] == 'D' || out[i][adv] == ' ' || out[i][adv] == 't' || out[i][adv] == 'c'){
                                    flag = false;
                                } else if(out[i][adv] == 'T'){
                                    out[i][adv] = 't';
                                } else if(out[i][adv] == 'C'){
                                    out[i][adv] = 'c';
                                } else {
                                    out[i][adv] = ' ';
                                }
                            }
                            adv++;
                        }
                        j = adv;
                    } else if(out[i][j] == 't' || out[i][j] == 'c'){//turn move or cross move
                        bool itsACross = false;
                        if(out[i][j] == 'c'){
                            itsACross = true;
                        }
                        out[i][j] = ' ';
                        int turnLeftRight = rand() % 2;//elem 0 = turn left ; elem 1 = turn right
                        int incr = 1;
                        if(turnLeftRight == 1 && itsACross == false){
                            incr = -1;
                        }
                        int initial = j;
                        int adv = initial + incr;
                        bool flag = true;
                        while(flag == true){
                            if(out[i][adv] == 'L'){
                                out[i][adv] = ' ';
                                flag = false;
                            } else {
                                if(out[i][adv] == 'W' || out[i][adv] == 'D' || out[i][adv] == ' ' || out[i][adv] == 't' || out[i][adv] == 'c'){
                                    flag = false;
                                } else if(out[i][adv] == 'T'){
                                    out[i][adv] = 't';
                                } else if(out[i][adv] == 'C'){
                                    out[i][adv] = 'c';
                                } else {
                                    out[i][adv] = ' ';
                                }
                            }
                            adv += incr;
                        }
                        if(adv > j){
                            j = adv;
                        }
                        if(itsACross == true){
                            adv = initial - 1;
                            flag = true;
                            while(flag == true){
                                if(out[i][adv] == 'L'){
                                    out[i][adv] = ' ';
                                    flag = false;
                                } else {
                                    if(out[i][adv] == 'W' || out[i][adv] == 'D' || out[i][adv] == ' ' || out[i][adv] == 't' || out[i][adv] == 'c'){
                                        flag = false;
                                    } else if(out[i][adv] == 'T'){
                                        out[i][adv] = 't';
                                    } else if(out[i][adv] == 'C'){
                                        out[i][adv] = 'c';
                                    } else {
                                        out[i][adv] = ' ';
                                    }
                                }
                                adv -= 1;
                            }
                        }
                    }
                }

            }
            for(int j = 0; j < nc; j++){//iterations on the col roads
                for(int i = 0; i < nr; i++){
                    if(out[i][j] == 't' || out[i][j] == 'c'){//turn move or cross move
                        bool itsACross = false;
                        if(out[i][j] == 'c'){
                            itsACross = true;
                        }
                        out[i][j] = ' ';
                        int turnUpDown = rand() % 2;//elem 0 = turn up ; elem 1 = turn down
                        int incr = 1;
                        if(turnUpDown == 1 && itsACross == false){
                            incr = -1;
                        }
                        int initial = i;
                        int adv = initial + incr;
                        bool flag = true;
                        while(flag == true){
                            if(out[adv][j] == 'L'){
                                out[adv][j] = ' ';
                                flag = false;
                            } else {
                                if(out[adv][j] == 'W' || out[adv][j] == 'D' || out[adv][j] == ' ' || out[adv][j] == 't' || out[adv][j] == 'c'){
                                    flag = false;
                                } else if(out[adv][j] == 'T'){
                                    out[adv][j] = 't';
                                } else if(out[adv][j] == 'C'){
                                    out[adv][j] = 'c';
                                } else {
                                    out[adv][j] = ' ';
                                }
                            }
                            adv += incr;
                        }
                        if(adv > i){
                            i = adv;
                        }
                        if(itsACross == true){
                            adv = initial - 1;
                            flag = true;
                            while(flag == true){
                                if(out[adv][j] == 'L'){
                                    out[adv][j] = ' ';
                                    flag = false;
                                } else {
                                    if(out[adv][j] == 'W' || out[adv][j] == 'D' || out[adv][j] == ' ' || out[adv][j] == 't' || out[adv][j] == 'c'){
                                        flag = false;
                                    } else if(out[adv][j] == 'T'){
                                        out[adv][j] = 't';
                                    } else if(out[adv][j] == 'C'){
                                        out[adv][j] = 'c';
                                    } else {
                                        out[adv][j] = ' ';
                                    }
                                }
                                adv -= 1;
                            }
                        }
                    }
                }
            }
            firstIteration = false;
        }
        // Adjust labyrinth
        for(int i = 0; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i][j] == 't' || out[i][j] == 'c'){
                    out[i][j] = ' ';
                }
                if(out[i][j] == 'W' || out[i][j] == 't' || out[i][j] == 'c' || out[i][j] == 'T' || out[i][j] == 'C' || out[i][j] == 'L'){
                    out[i][j] = '#';
                }
            }
        }
        // Count cells
        int cellNum = 0;
        for(int i = 0; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i][j] == ' '){
                    cellNum++;
                }
            }
        }
        const int minCellNum = 25;
        if(cellNum < minCellNum){
            free(out);
            return genMaze(nr, nc);
        }
        // Locate the player, the keys and the food
        //first decide the number of keys
        const int minNumOfKeys = 3;
        const int maxNumOfKeys = 7;
        int varNumOfKeys = maxNumOfKeys - minNumOfKeys;
        int numOfKeys = rand() % varNumOfKeys;
        numOfKeys += minNumOfKeys;
        effectiveNumberOfKeys = numOfKeys;
        //second decide the number of food
        const int minNumOfFood = 3;
        const int maxNumOfFood = 7;
        int varNumOfFood = maxNumOfFood - minNumOfFood;
        int numOfFood = rand() % varNumOfFood;
        numOfFood += minNumOfFood;
        effectiveNumberOfFood = numOfFood;
        //a var to represent what you are locating:
        int locatingWhat = 0;// 0 = player ; 1 = keys ; 2 = food
        int cellNumForLocation = cellNum;//cell number available for location
        //locate these objects
        int counterForCurrentObject = 0;
        while(locatingWhat < 3){
            int location = rand() % cellNumForLocation;
            for(int i = 0; i < nr; i++){
                for(int j = 0; j < nc; j++){
                    if(out[i][j] == ' '){
                        location -= 1;
                        if(location == 0 && out[i][j] == ' '){
                            //here locate the object
                            if(locatingWhat == 0){//player
                                out[i][j] = 'P';
                                CamPos = glm::vec3(j+0.5, 0.5, i+0.5);//set starting position
                                //in case you are watching to the wall, CamAlpha is rotated so that you start without having a wall in front of you
                                if(out[i-1][j] == '#'){
                                    CamAlpha = glm::radians(90.0f);
                                    if(out[i][j-1] == '#'){
                                        CamAlpha = glm::radians(180.0f);
                                        if(out[i+1][j] == '#'){
                                            CamAlpha = glm::radians(270.0f);
                                        }
                                    }
                                }
                                CamAlphaOriginal = CamAlpha;
                                CamPosOriginal = CamPos;
                                locatingWhat++;
                            } else if(locatingWhat == 1){//keys
                                out[i][j] = 'K';
                                keyPos.push_back(glm::vec3(j+0.5, 0.5, i+0.5));
                                xKeyPos.push_back(j);
                                yKeyPos.push_back(i);
                                tookKey.push_back(OBTAIN_AUTOMATICALLY_ALL_THE_KEYS);
                                counterForCurrentObject++;
                                if(counterForCurrentObject >= numOfKeys){
                                    counterForCurrentObject = 0;
                                    locatingWhat++;
                                }
                            } else if(locatingWhat == 2){//food
                                out[i][j] = 'F';
                                foodPos.push_back(glm::vec3(j+0.5, 0.35, i+0.5));
                                xFoodPos.push_back(j);
                                yFoodPos.push_back(i);
                                tookFood.push_back(false);
                                counterForCurrentObject++;
                                if(counterForCurrentObject >= numOfFood){
                                    counterForCurrentObject = 0;
                                    locatingWhat++;
                                }
                            }
                            cellNumForLocation -= 1;
                        }
                    }
                }
            }
        }
        // update labyrinthShape with walls positions
        for(int i = 0; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i][j] == '#'){
                    labyrinthShape[i][j] = true;
                } else {
                    labyrinthShape[i][j] = false;
                    // decide which type of ground place
                    groundPos.push_back(glm::vec3(j+0.5, 0.0, i+0.5));//also locate the ground where you can go
                    int kindOfGround = rand() % 5;
                    if(kindOfGround == 0){
                        groundType.push_back(true);
                    } else {
                        groundType.push_back(false);
                    }
                    effectiveNumberOfGround++;
                }
            }
        }
        // update also reducedLabyrinthShape with walls and door positions
        for(int i = 0; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i][j] == '#' || out[i][j] == 'D'){
                    reducedLabyrinthShape[i][j] = true;
                } else {
                    reducedLabyrinthShape[i][j] = false;
                }
            }
        }
        labyrinthShapeInitialized = true;
        // Add the models of the walls
        //moreover add models on lights on each wall and turn on some of these lights
        //in the boss area there could be more on lights
        //there is a max number of on lights
        const int lightFrequence = 5;//fequence to place a light
        const int lightFrequenceBoss = 5;//fequence to place a light in the room of the boss
        int checkInsertLight1 = 0;
        int checkInsertLight2 = 0;
        //| walls
        for(int j = 1; j < nc; j++){
            for(int i = 0; i < nr; i++){
                if(out[i][j-1] == '#' && out[i][j] != '#'){
                    effectiveNumberOfWalls++;
                    checkInsertLight1++;
                    glm::vec3 pos = glm::vec3(j, 0, i+0.5);
                    wallPos.push_back(pos);
                    glm::quat rot = glm::quat(glm::vec3(0, glm::radians(90.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(rot);
                    bool conditionToAddAnOnLight = (checkInsertLight1 % lightFrequence == 0 || out[i][j] == 'B' && checkInsertLight1 % lightFrequenceBoss == 0) && effectiveNumberOfOnLights < MAX_ON_LIGHTS;
                    if(THERE_ARE_OFF_LIGHTS || conditionToAddAnOnLight){
                        effectiveNumberOfLights++;//add on every wall an off light
                        glm::vec3 lightSinglePos = glm::vec3(j+0.025, 0.5, i+0.5);
                        lightPositions.push_back(lightSinglePos);
                        lightRots.push_back(rot);
                    }
                    if (conditionToAddAnOnLight) {
                        effectiveNumberOfOnLights++;//some of these light will be also turned on
                        glm::vec3 lightFirePos = glm::vec3(j+0.125, 0.6, i+0.5);
                        lightFirePositions.push_back(lightFirePos);
                    }
                }
                if(out[i][j-1] != '#' && out[i][j] == '#'){
                    effectiveNumberOfWalls++;
                    checkInsertLight2++;
                    glm::vec3 pos = glm::vec3(j, 0, i+0.5);
                    wallPos.push_back(pos);
                    glm::quat rot = glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(rot);
                    bool conditionToAddAnOnLight = (checkInsertLight2 % lightFrequence == 0 || out[i][j-1] == 'B' && checkInsertLight2 % lightFrequenceBoss == 0) && effectiveNumberOfOnLights < MAX_ON_LIGHTS;
                    if(THERE_ARE_OFF_LIGHTS || conditionToAddAnOnLight){
                        effectiveNumberOfLights++;//add on every wall an off light
                        glm::vec3 lightSinglePos = glm::vec3(j-0.025, 0.5, i+0.5);
                        lightPositions.push_back(lightSinglePos);
                        lightRots.push_back(rot);
                    }
                    if (conditionToAddAnOnLight) {
                        effectiveNumberOfOnLights++;//some of these light will be also turned on
                        glm::vec3 lightFirePos = glm::vec3(j-0.125, 0.6, i+0.5);
                        lightFirePositions.push_back(lightFirePos);
                    }
                }
            }
        }
        //- walls
        for(int i = 1; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i-1][j] == '#' && out[i][j] != '#'){
                    effectiveNumberOfWalls++;
                    checkInsertLight1++;
                    glm::vec3 pos = glm::vec3(j+0.5, 0, i);
                    wallPos.push_back(pos);
                    glm::quat rot = glm::quat(glm::vec3(0, glm::radians(0.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(rot);
                    bool conditionToAddAnOnLight = (checkInsertLight1 % lightFrequence == 0 || out[i][j] == 'B' && checkInsertLight1 % lightFrequenceBoss == 0) && effectiveNumberOfOnLights < MAX_ON_LIGHTS;
                    if(out[i][j] != 'D'){
                        if(THERE_ARE_OFF_LIGHTS || conditionToAddAnOnLight){
                            effectiveNumberOfLights++;//add on every wall an off light
                            glm::vec3 lightSinglePos = glm::vec3(j+0.5, 0.5, i+0.025);
                            lightPositions.push_back(lightSinglePos);
                            lightRots.push_back(rot);
                        }
                        if (conditionToAddAnOnLight) {
                            effectiveNumberOfOnLights++;//some of these light will be also turned on
                            glm::vec3 lightFirePos = glm::vec3(j+0.5, 0.6, i+0.125);
                            lightFirePositions.push_back(lightFirePos);
                        }
                    }
                }
                if(out[i-1][j] != '#' && out[i][j] == '#'){
                    effectiveNumberOfWalls++;
                    checkInsertLight2++;
                    glm::vec3 pos = glm::vec3(j+0.5, 0, i);
                    wallPos.push_back(pos);
                    glm::quat rot = glm::quat(glm::vec3(0, glm::radians(-180.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(rot);
                    bool conditionToAddAnOnLight = (checkInsertLight2 % lightFrequence == 0 || out[i-1][j] == 'B' && checkInsertLight2 % lightFrequenceBoss == 0) && effectiveNumberOfOnLights < MAX_ON_LIGHTS;
                    if(out[i-1][j] != 'D'){
                        if(THERE_ARE_OFF_LIGHTS || conditionToAddAnOnLight){
                            effectiveNumberOfLights++;//add on every wall an off light
                            glm::vec3 lightSinglePos = glm::vec3(j+0.5, 0.5, i-0.025);
                            lightPositions.push_back(lightSinglePos);
                            lightRots.push_back(rot);
                        }
                        if (conditionToAddAnOnLight) {
                            effectiveNumberOfOnLights++;//some of these light will be also turned on
                            glm::vec3 lightFirePos = glm::vec3(j+0.5, 0.6, i-0.125);
                            lightFirePositions.push_back(lightFirePos);
                        }
                    }
                }
            }
        }
        std::cout << "number of on lights: " << effectiveNumberOfOnLights << std::endl;
        // Return
		return out;
	}

	void destroyMaze(int row, int col, char **maze) {
		for(int i = 0; i < row; i++) {
			free(maze[i]);
		}
		free(maze);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    LabyrinthSurvival app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
