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
    {1, {"Now you can open the gate", "", "", ""}, 0, 0},//7
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
	alignas(16) glm::vec3 lightDir;
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
    Texture TL, TW, TG, TK, TF;//Texture for the labyrinth, for the wall, the ground, for the keys and for the food
    std::vector<Model> MK;//model MK for the keys
    std::vector<Model> MF;//model MF for the food
    std::vector<DescriptorSet> DSK;//DescriptorSet DSK for the keys
    std::vector<DescriptorSet> DSF;//DescriptorSet DSF for the food
    std::vector<Model> MW;//model MW for the walls
    std::vector<Model> MG;//model MW for the ground
    std::vector<DescriptorSet> DSW;//DescriptorSet for the walls
    std::vector<DescriptorSet> DSG;//DescriptorSet for the ground
    
    TextMaker txt;//To insert a text in the UI of this application
    
    // Variables concerning the generated labyrinth
    bool labyrinthShape[NUMROW][NUMCOL];//to keep where there is a wall in the labyrinth
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
    
	// Other application parameters
	glm::vec3 CamPos = glm::vec3(0.5, 0.5, 10.0);//camera position
    glm::vec3 CamPosPrec = glm::vec3(0.5, 0.5, 10.0);//remember prew position to check if you are not overriding a wall
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
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
    
    //parameters for the ground pos, rot and scale
    std::vector<glm::vec3> groundPos;
    glm::quat groundRot = glm::quat(glm::vec3(0, glm::radians(180.0f), 0)) *
                       glm::quat(glm::vec3(glm::radians(0.0f), 0, 0)) *
                       glm::quat(glm::vec3(0, 0, glm::radians(0.0f)));
    glm::vec3 groundScale = glm::vec3(0.25f);
    
    //vPos and vIdx for the model for the labyrinth
	std::vector<float> vPos;
	std::vector<int> vIdx;
    
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
				vertex.norm = {0, 1, 0};//TODO Fix the norm
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
            float pih = 3.14159265358979f / 2.0f;//there are 4 possible direction 90 degree directions
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

        CamBeta  = CamBeta - ROT_SPEED * deltaT * r.x;
		CamBeta  =  CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
				   (CamBeta > glm::radians( 90.0f) ? glm::radians( 90.0f) : CamBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
        CamPosPrec = CamPos;
		CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
        
        //You can't go over a wall
        if(labyrinthShapeInitialized){
            const float minDistToWalls = 0.15f;
            //you can't pass through a wall
            if(labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z+minDistToWalls)][int(CamPos.x-minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x+minDistToWalls)] == true || labyrinthShape[int(CamPos.z-minDistToWalls)][int(CamPos.x-minDistToWalls)] == true){
                CamPos = CamPosPrec;//return to the previous position
                //go on even if you are lightly passing through walls
                float pih = 3.14159265358979f / 2.0f;//there are 4 possible direction the walls are oriented
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
        }
        
        //your position in the labyrinth
        int x = CamPos.x;
        int y = CamPos.z;
        
        // In case you take a key
        for(int i = 0; i < effectiveNumberOfKeys; i++){
            if(x == xKeyPos[i] && y == yKeyPos[i] && tookKey[i] == false){
                std::cout << "You took a key\n";
                tookKey[i] = true;
                keyPos[i] = notVisiblePosition;
                //implement here that the text that changes when you take a key
            }
        }
        
        // In case you take a food
        for(int i = 0; i < effectiveNumberOfFood; i++){
            if(x == xFoodPos[i] && y == yFoodPos[i] && tookFood[i] == false){
                std::cout << "You took a food\n";
                tookFood[i] = true;
                foodPos[i] = notVisiblePosition;
                //implement here the text that you recover some life when you take a food
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
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f))*cos(glm::radians(30.0f)), sin(glm::radians(135.0f)), sin(glm::radians(30.0f)));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = glm::vec3(100.0, 100.0, 100.0);
        
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
        const int xLenghtBossFight = 7;
        const int yLenghtBossFight = 10;
        int startXBossFight = rand() % (nr - xLenghtBossFight);
        int endXBossFight = startXBossFight + xLenghtBossFight;
        int startYBossFight = rand() % (nc - yLenghtBossFight);
        int endYBossFight = startYBossFight + yLenghtBossFight;
        for(int i = startXBossFight; i < endXBossFight; i++){//place boss fight
            for(int j = startYBossFight; j < endYBossFight; j++){
                out[i][j] = 'B';
            }
        }
        for(int i = startXBossFight; i < endXBossFight; i++){//place waals around boss fight
            out[i][startYBossFight] = 'W';
            out[i][endYBossFight-1] = 'W';
        }
        for(int j = startYBossFight; j < endYBossFight; j++){
            out[startXBossFight][j] = 'W';
            out[endXBossFight-1][j] = 'W';
        }
        out[startXBossFight+(xLenghtBossFight/2)][endYBossFight-1] = 'D';//place the door
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
                                locatingWhat++;
                            } else if(locatingWhat == 1){//keys
                                out[i][j] = 'K';
                                keyPos.push_back(glm::vec3(j+0.5, 0.5, i+0.5));
                                xKeyPos.push_back(j);
                                yKeyPos.push_back(i);
                                tookKey.push_back(false);
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
                    groundPos.push_back(glm::vec3(j+0.5, 0.0, i+0.5));
                    effectiveNumberOfGround++;
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
