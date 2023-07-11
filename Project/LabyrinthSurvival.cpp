#include "Starter.hpp"
#include "TextMaker.hpp"

std::vector<SingleText> textToVisualize = {
    {1, {"Welcome in LabyrinthSurvival", "", "", ""}, 0, 0},//0
    {1, {"There are different keys to take", "", "", ""}, 0, 0},//1
    {1, {"There are other 5 keys to take", "", "", ""}, 0, 0},//2
    {1, {"There are other 4 keys to take", "", "", ""}, 0, 0},//3
    {1, {"There are other 3 keys to take", "", "", ""}, 0, 0},//4
    {1, {"There are other 2 keys to take", "", "", ""}, 0, 0},//5
    {1, {"There is another 1 key to take", "", "", ""}, 0, 0},//6
    {1, {"Now you can open the door", "", "", ""}, 0, 0},//7
    {1, {"Labyrinth completed", "", "", ""}, 0, 0},//8
    {1, {"Labyrinth failed", "", "", ""}, 0, 0},//9
};

// num rows and cols labyrinth
#define NUMROW 45
#define NUMCOL 45

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightPos[2];
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

class LabyrinthSurvival;

// MAIN ! 
class LabyrinthSurvival : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSL1{};

	// Pipelines [Shader couples]
	Pipeline P1{};

	// Models, textures and Descriptors (values assigned to the uniforms)

    Model M{};//Model M for the labyrinth
    DescriptorSet DS;//DescriptorSet DS for the labyrinth
    Texture TL, TW, TD, TB, TG, TK, TF;//Texture for the labyrinth, for the wall, the door, the boss, the ground, for the keys and for the food
    std::vector<Model> MK;//model MK for the keys
    std::vector<Model> MF;//model MF for the food
    std::vector<DescriptorSet> DSK;//DescriptorSet DSK for the keys
    std::vector<DescriptorSet> DSF;//DescriptorSet DSF for the food
    std::vector<Model> MW;//model MW for the walls
    Model MD;//model MD for the door
    Model MB;//model MB for the boss
    std::vector<Model> MG;//model MW for the ground
    std::vector<DescriptorSet> DSW;//DescriptorSet for the walls
    DescriptorSet DSD;//DescriptorSet for the door
    DescriptorSet DSB;//DescriptorSet for the boss
    std::vector<DescriptorSet> DSG;//DescriptorSet for the ground
    
    TextMaker txt;//To insert a text in the UI of this application
    
    // Variables concerning the generated labyrinth
    bool labyrinthShape[NUMROW][NUMCOL];//to keep where there is a wall in the labyrinth
    bool reducedLabyrinthShape[NUMROW][NUMCOL];//to keep where there is a wall or the door in the labyrinth
    bool labyrinthShapeInitialized = false;//if the labyrinth is initialized
    int effectiveNumberOfKeys = 0;//this var will be set with the number of keys in the labyrinth (after labyrinth generation)
    int effectiveNumberOfWalls = 0;//this var will be set with the number of walls in the labyrinth (after labyrinth generation)
    int effectiveNumberOfFood = 0;//this var will be set with the number of food in the labyrinth (after labyrinth generation)
    int effectiveNumberOfGround = 0;//this var will be set with the number of ground in the labyrinth (after labyrinth generation)
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
    
    //variables for the boss pattern (the boss has a pattern that sometimes approaches you and sometimes goes away from you)
    const float timeBossApproachYou = 4.5f;//how much time the boss approaches you
    const float timeBossGoesAwayFromYou = 0.75f;//how much time the boss goes away from you
    const float PenalityMultiplierBossGoesAwayFromYou = 4.0f;//when the boss goes away from you how faster the boss to go away respect to when the boss approaches you
    float timeBossPatternCounter = 0.0f;//time counter for the parttner of the boss
    float bossPatternMultiplier = 1.0f;//if positive the boss approaches you; if negative the boss goes away from you
    
	// Other application parameters
	glm::vec3 CamPos = glm::vec3(0.5, 0.5, 10.0);//camera position
    glm::vec3 CamPosPrec = glm::vec3(0.5, 0.5, 10.0);//remember prew position to check if you are not overriding a wall
    float CamAlpha = glm::radians(0.0f);
    float CamBeta = glm::radians(0.0f);
	float Ar;//aspect ratio
    glm::vec3 notVisiblePosition = glm::vec3(-1, -1, -1);//a position to place an object witch disappear
    
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
    
    //parameters for the door pos, rot and scale
    glm::vec3 doorPos;
    glm::quat doorRot;
    glm::vec3 doorScale = glm::vec3(0.25f);
    glm::vec3 originalDoorPos;//the door will reappear also during the boss fight because you won't be able to exit from the boss room during the boss fight
    
    //parameters for the boss pos, rot and scale
    glm::vec3 bossPos;
    float bossRot;
    float bossScale = 0.0f;
    float maxBossScale = 2.25f;//when the boss fight starts there is an animation that the boss appear form 0 to max scale
    
    //limit the area where the boss can stay
    float xStartLimitBossPos;
    float xEndLimitBossPos;
    float zStartLimitBossPos;
    float zEndLimitBossPos;
    
    //parameters for the ground pos, rot and scale
    std::vector<glm::vec3> groundPos;
    glm::quat groundRot = glm::quat(glm::vec3(0, glm::radians(180.0f), 0)) *
                       glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                       glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
    glm::vec3 groundScale = glm::vec3(0.25f);
    
    //vPos and vIdx for the model for the labyrinth
	std::vector<float> vPos;//position of the vertices
    std::vector<float> vNorms;//for each vertex the 3 coordinates of the norm
	std::vector<int> vIdx;//vertices for triangles
    
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
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
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
        //std::cout << "DSL\n";
		DSL1.init(this, {
					// this array contains the binding:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					// third  element : the pipeline stage where it will be used
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
					{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                    {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				  });

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
        std::cout << "P1\n";
		P1.init(this, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", {&DSL1});
		P1.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)
        
        int r = NUMROW;
        int c = NUMCOL;
        char **maze = genMaze(r, c);
        
        for (int i = 0; i < effectiveNumberOfWalls; i++) {
            Model wall;
            wall.init(this, "models/Wall.obj");//model of a wall
            MW.push_back(wall);
        }
        
        MD.init(this, "models/Wall.obj");//model of a door
        
        MB.init(this, "models/Character.obj");//model of the boss
        
        for (int i = 0; i < effectiveNumberOfGround; i++) {
            Model ground;
            //model of a ground: for each pice of ground, initializate randomly one of the two model of a ground
            int kindOfGround = rand() % 2;
            if(kindOfGround == 0){
                ground.init(this, "models/Groundpr.obj");
            } else {
                ground.init(this, "models/Ground_2pr.obj");
            }
            MG.push_back(ground);
        }
        
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            Model key;
            key.init(this, "models/Key.obj");//model of a key
            MK.push_back(key);
        }
        
        for(int i = 0; i < effectiveNumberOfFood; i++){
            Model food;
            food.init(this, "models/12190_Heart_v1_L3.obj");//model of a food
            MF.push_back(food);
        }
		
        //std::cout << "Maze Show\n";
		for(int i=0; i < r; i++) {
			std::cout << maze[i] << "\n";
		}
		std::cout << "\n";
		
        //std::cout << "create Mesh\n";
		createMazeMesh(r, c, maze);
		std::cout << "Mesh size: V=" << vPos.size() << ", I=" << vIdx.size() << "\n";

        M.BP = this;
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
            M.vertices.push_back(vertex);
		}
		for(int i = 0; i < vIdx.size(); i++) {
			if((vIdx[i] < 0) || (vIdx[i] >= M.vertices.size())) {
				std::cout << "Error! Index: " << i << " is outside range (" << vIdx[i] << ")\n";
				M.indices.push_back(0);
			} else {
				M.indices.push_back(vIdx[i]);
			}
		}
        
        TL.init(this, "textures/IMG_9647.png");
        TW.init(this, "textures/LowPolyDungeonsLite_Texture_01.png");
        TD.init(this, "textures/door.png");
        TB.init(this, "textures/boss.png");
        TG.init(this, "textures/ground.png");
        TK.init(this, "textures/key.png");
        TF.init(this, "textures/food.png");

		M.createVertexBuffer();
		M.createIndexBuffer();
		std::cout << "Created model: V=" << M.vertices.size() << ", I=" << M.indices.size() << "\n";

		destroyMaze(r, c, maze);
        
        txt.init(this, &textToVisualize);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P1.create();

		DS.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TL}
				});
        
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

        for (Model wall : MW) {
            DescriptorSet DSWall;
            DSWall.init(this, &DSL1, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TW}
            });
            DSW.push_back(DSWall);
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
        
        for (Model ground : MG) {
            DescriptorSet DSGround;
            DSGround.init(this, &DSL1, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr},
                    {2, TEXTURE, 0, &TG}
            });
            DSG.push_back(DSGround);
        }
        
        txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {
		P1.cleanup();
		
		DS.cleanup();
        for(DescriptorSet ds : DSK){
            ds.cleanup();
        }
        for(DescriptorSet ds : DSF){
            ds.cleanup();
        }
        for (DescriptorSet ds : DSW) {
            ds.cleanup();
        }
        DSD.cleanup();
        DSB.cleanup();
        for (DescriptorSet ds : DSG) {
            ds.cleanup();
        }

        txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {
        TL.cleanup();
        TW.cleanup();
        TD.cleanup();
        TB.cleanup();
        TG.cleanup();
        TK.cleanup();
        TF.cleanup();
        
		M.cleanup();
        
        for (Model key : MK){
            key.cleanup();
        }
        for (Model wall : MW) {
            wall.cleanup();
        }
        MD.cleanup();
        MB.cleanup();
        for (Model ground : MG) {
            ground.cleanup();
        }
        for (Model food : MF){
            food.cleanup();
        }

		DSL1.cleanup();
		
		P1.destroy();
        
        txt.localCleanup();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		P1.bind(commandBuffer);

		M.bind(commandBuffer);
		DS.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M.indices.size()), 1, 0, 0, 0);
        
        for(int i = 0; i < MK.size(); i++){
            MK[i].bind(commandBuffer);
            DSK[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MK[i].indices.size()), 1, 0, 0, 0);
        }
        
        for(int i = 0; i < MF.size(); i++){
            MF[i].bind(commandBuffer);
            DSF[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(MF[i].indices.size()), 1, 0, 0, 0);
        }
        
		vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(M.indices.size()), 1, 0, 0, 0);
        
        for (int i = 0; i < MW.size(); i++) {
            MW[i].bind(commandBuffer);
            DSW[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MW[i].indices.size()), 1, 0, 0, 0);
        }
        
        MD.bind(commandBuffer);
        DSD.bind(commandBuffer, P1, currentImage);
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MD.indices.size()), 1, 0, 0, 0);
        
        MB.bind(commandBuffer);
        DSB.bind(commandBuffer, P1, currentImage);
        vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MB.indices.size()), 1, 0, 0, 0);
        
        for (int i = 0; i < MG.size(); i++) {
            MG[i].bind(commandBuffer);
            DSG[i].bind(commandBuffer, P1, currentImage);
            vkCmdDrawIndexed(commandBuffer,static_cast<uint32_t>(MG[i].indices.size()), 1, 0, 0, 0);
        }

        txt.populateCommandBuffer(commandBuffer, currentImage, 0);
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

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		const float ROT_SPEED = glm::radians(150.0f);
        const float FAST_ROT_SPEED = glm::radians(300.0f);
		const float MOVE_SPEED = 6.0f;

		static float debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
        
        static bool goingInstantLeft = false;//you are continuing to go to watch to the nearest 90 degree angle on your left because you have decided about this before
        static bool goingInstantRight = false;//you are continuing to go to watch to the nearest 90 degree angle on your right because you have decided about this before
        bool goInstantLeft = false;//you decide to watch to the nearest 90 degree angle on your left
        bool goInstantRight = false;//you decide to watch to the nearest 90 degree angle on your right
        
		getSixAxis(deltaT, m, r, fire, goInstantLeft, goInstantRight);
        
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
        
        if(!goingInstantLeft && !goingInstantRight){//if you are not using fast rotation to the nearest 90 degree angle
            CamAlpha = CamAlpha - ROT_SPEED * deltaT * r.y;//manage rotation normally
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
        if(bossFightStarted){
            bossScale += BOSS_SCALE_SPEED * deltaT;
        }
        if(bossScale > maxBossScale){
            bossScale = maxBossScale;
            bossFightStartedAnimationFinished = true;//the animation that the boss appears is finished
        }
        
        CamBeta  = CamBeta - ROT_SPEED * deltaT * r.x;
		CamBeta  =  CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
				   (CamBeta > glm::radians( 90.0f) ? glm::radians( 90.0f) : CamBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
        CamPosPrec = CamPos;
        if(!youNeedToWalkAwayFromTheBoss){//normally you move using W A S D keys; but, in case the boss hurts you and you have to walk away from him, your movements are temporanely locked
            if(tiltTimeCounter <= 0.0f){//moreover, if you are tilt (because during the boss fight you are slammed against the wall) you can't move for a while
                CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
                CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
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
                        doorRot = glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)) *
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
        
        //your position in the labyrinth
        int x = CamPos.x;
        int y = CamPos.z;
        
        // In case you take a key
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(x == xKeyPos[i] && y == yKeyPos[i] && tookKey[i] == false){
                tookKey[i] = true;
                keyPos[i] = notVisiblePosition;
                int howManyRemainingKeys = 0;
                for(int j = 0; j < effectiveNumberOfKeys; j++){
                    if(tookKey[j] == false){
                        howManyRemainingKeys++;
                    }
                }
                std::cout << "You took a key (remaining keys: ";
                std::cout << howManyRemainingKeys;
                std::cout << ")\n";
                //TODO implement here that the text changes when you take a key
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
                std::cout << "You took a food\n";
                tookFood[i] = true;
                foodPos[i] = notVisiblePosition;
                //TODO implement here the text that you recover some life when you take a food
            }
        }
        
        const float BOSS_ROT_SPEED = glm::radians(110.0f);
        const float BOSS_MOVE_SPEED = 0.005f;
        const float WALK_AWAY_SPEED = 0.04f;//speed to alk away from the boss in case the boss hurts you
        
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
        if(distBossYou < minDistFromTheBoss || youNeedToWalkAwayFromTheBoss){
            if(youNeedToWalkAwayFromTheBoss == false){//in this instant the boss hurts you
                std::cout << "the boss hurts you\n";
                const float fireChargingPenality = 0.75f;//a penality to wait some time to fire if the boss hurts you
                fireCharging = fireChargingPenality;
                //TODO implement here that you take damage and you lose some of you life
            }
            youNeedToWalkAwayFromTheBoss = true;
            float walkAwayXIncr = WALK_AWAY_SPEED * cos(dirBossYou);
            float walkAwayZIncr = WALK_AWAY_SPEED * sin(dirBossYou);
            CamPos.x = CamPos.x + walkAwayXIncr;
            CamPos.z = CamPos.z + walkAwayZIncr;
            //in this case that you are walking away from the boss, we have to check that you are not going over a wall or you are exiting from the boss fight area; else it menas that you are slammed against the wall and you also take a lot of extra damages and there are some other penalities
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
                    //TODO implement here you take extra damages
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
        if(fireCharging < 0.0f){
            fireCharging = 0.0f;
        }
        // in case you use the fire command and the boss fight is started
        if(fire){
            if(bossFightStarted){
                if(fireCharging <= 0){//the fire need to be charged
                    fireCharging = timeToChargeTheFire;
                    const float minDistFromTheBossToFire = 3.0f;
                    const float minDistAngleToFireTheBoss = glm::radians(30.0f);
                    if(distBossYou <= minDistFromTheBossToFire){//to fire the boss you need to be near to it
                        float dirYouBoss = glm::radians(90.0f) - CamAlpha;//the direction you are watching has this relation from CamAlpha; this value (after a moudle 360 degre adjustment) can be compared with dirBossYou to check if you are looking to the boss direction
                        while(dirYouBoss <= glm::radians(0.0f)){//TODO questo while si può spostare
                            dirYouBoss += glm::radians(360.0f);
                        }
                        while(dirYouBoss > glm::radians(360.0f)){
                            dirYouBoss -= glm::radians(360.0f);
                        }
                        if(abs(dirBossYou - dirYouBoss) < minDistAngleToFireTheBoss || abs(dirBossYou - dirYouBoss) > glm::radians(360.0f) - minDistAngleToFireTheBoss){//to fire the boss you need also to watch it with your camera
                            float firePowFloat = (pow((minDistFromTheBossToFire - distBossYou),2)*50.0f)+2.0f;//the power of your fire depends by the distance you manage to fire the boss! If you are nearer to the boss you inflict more damages but you are more in danger in the boss hurts you
                            int firePow = firePowFloat;//num of damages you inflict to the boss
                            std::cout << "You have fired!!! (pow: ";
                            std::cout << firePow;
                            std::cout << ") (you are near to the boss and you are watching the boss with your camera)\n";
                            //TODO implement here that the boss takes damages
                        } else {
                            std::cout << "You have fired but you are not watching the boss with your camera; you are near to the boss\n";
                        }
                    } else {
                        std::cout << "You have fired but you are too distant from the boss\n";
                    }
                }
            } else {
                std::cout << "You can't fire if the boss fight isn't started\n";
            }
        }
		
        // close the application
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 50.0f);
		M[1][1] *= -1;

		glm::mat4 Mv =  glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1,0,0)) *
						glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0,1,0)) *
						glm::translate(glm::mat4(1.0), -CamPos);

		glm::mat4 ViewPrj =  M * Mv;
		UniformBufferObject ubo{};
		glm::mat4 baseTr = glm::mat4(1.0f);
								
		// Here is where you actually update your uniforms

		// updates global uniforms
		GlobalUniformBufferObject gubo{};
        gubo.lightPos[0] = CamPos + glm::vec3(0.0f, 1.0f, 0.0f);
        gubo.lightPos[1] = CamPos + glm::vec3(0.0f, 5.0f, 0.0f);
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = CamPos;
        
        ubo.mMat = baseTr;
        ubo.mvpMat = ViewPrj * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
        DS.map(currentImage, &ubo, sizeof(ubo), 0);
        DS.map(currentImage, &gubo, sizeof(gubo), 1);
        
        for(int i = 0; i < MK.size(); i++){
            ubo.mMat = MakeWorldMatrix(keyPos[i], KeyRot, KeyScale) * baseTr;//translate the key to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            DSK[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSK[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }
        
        for(int i = 0; i < MF.size(); i++){
            ubo.mMat = MakeWorldMatrix(foodPos[i], FoodRot, FoodScale) * baseTr;//translate the key to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            DSF[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSF[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }

        for (int i = 0; i < MW.size(); i++) {
            ubo.mMat = MakeWorldMatrix(wallPos[i], wallRots[i], wallScale) * baseTr;//translate and rotate the walls to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
            DSW[i].map(currentImage, &ubo, sizeof(ubo), 0);
            DSW[i].map(currentImage, &gubo, sizeof(gubo), 1);
        }
        
        ubo.mMat = MakeWorldMatrix(doorPos, doorRot, doorScale) * baseTr;//translate and rotate the door to locate
        ubo.mvpMat = ViewPrj * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
        DSD.map(currentImage, &ubo, sizeof(ubo), 0);
        DSD.map(currentImage, &gubo, sizeof(gubo), 1);
        
        glm::quat bossRotQuat = glm::quat(glm::vec3(0, -bossRot, 0)) *//equivalent quat of bossRot in radians anticlockwise
                         glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                         glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
        ubo.mMat = MakeWorldMatrix(bossPos, bossRotQuat, glm::vec3(bossScale)) * baseTr;//translate and rotate the boss to locate
        ubo.mvpMat = ViewPrj * ubo.mMat;
        ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
        DSB.map(currentImage, &ubo, sizeof(ubo), 0);
        DSB.map(currentImage, &gubo, sizeof(gubo), 1);
        
        for (int i = 0; i < MG.size(); i++) {
            ubo.mMat = MakeWorldMatrix(groundPos[i], groundRot, groundScale) * baseTr;//translate the ground to locate
            ubo.mvpMat = ViewPrj * ubo.mMat;
            ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
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
            srand(1);//this is a default labyrinth in case the generation of the labyrinth take many tentatives
        }
        // Select randomly where to locate the boss fight
        const int xLenghtBossFight = 11;//need to be an odd number
        const int yLenghtBossFight = 11;
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
        doorPos = glm::vec3(endYBossFight-1+0.5, 0.0, startXBossFight+(xLenghtBossFight/2)+0.5);//locate the door (position)
        originalDoorPos = doorPos;//this is this original door pos (the door will reappear also during the boss fight because you won't be able to exit from the boss room during the boss fight)
        doorRot = glm::quat(glm::vec3(0, glm::radians(90.0f), 0)) *
                            glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                            glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));//locate the door (rotation)
        bossPos = glm::vec3(startYBossFight+(yLenghtBossFight/2), 0.0, startXBossFight+(xLenghtBossFight/2)+0.5);//locate the boss (position)
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
        const int maxIteration = 4;
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
                                locatingWhat++;
                            } else if(locatingWhat == 1){//keys
                                out[i][j] = 'K';
                                keyPos.push_back(glm::vec3(j+0.5, 0.5, i+0.5));
                                xKeyPos.push_back(j);
                                yKeyPos.push_back(i);
                                tookKey.push_back(false);//false
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
                    groundPos.push_back(glm::vec3(j+0.5, 0.0, i+0.5));//also locate the ground where you can go
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
        //| walls
        for(int i = 0; i < nr; i++){
            for(int j = 1; j < nc; j++){
                if(out[i][j-1] == '#' && out[i][j] != '#'){
                    effectiveNumberOfWalls++;
                    glm::vec3 pos = glm::vec3(j, 0, i+0.5);
                    wallPos.push_back(pos);
                    glm::quat wallRot = glm::quat(glm::vec3(0, glm::radians(90.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(wallRot);
                }
                if(out[i][j-1] != '#' && out[i][j] == '#'){
                    effectiveNumberOfWalls++;
                    glm::vec3 pos = glm::vec3(j, 0, i+0.5);
                    wallPos.push_back(pos);
                    glm::quat wallRot = glm::quat(glm::vec3(0, glm::radians(-90.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(wallRot);
                }
            }
        }
        //- walls
        for(int i = 1; i < nr; i++){
            for(int j = 0; j < nc; j++){
                if(out[i-1][j] == '#' && out[i][j] != '#'){
                    effectiveNumberOfWalls++;
                    glm::vec3 pos = glm::vec3(j+0.5, 0, i);
                    wallPos.push_back(pos);
                    glm::quat wallRot = glm::quat(glm::vec3(0, glm::radians(0.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(wallRot);
                }
                if(out[i-1][j] != '#' && out[i][j] == '#'){
                    effectiveNumberOfWalls++;
                    glm::vec3 pos = glm::vec3(j+0.5, 0, i);
                    wallPos.push_back(pos);
                    glm::quat wallRot = glm::quat(glm::vec3(0, glm::radians(180.0f), 0)) *
                                        glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                                        glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
                    wallRots.push_back(wallRot);
                }
            }
        }
        // Return
		return out;
	}
	
	void destroyMaze(int row, int col, char **maze) {
		for(int i = 0; i < row; i++) {
			free(maze[i]);
		}
		free(maze);
	}
	
	void createMazeMesh(int row, int col, char **maze);
};

#include "mazeGen.hpp"

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
