#define BIGNUM 99

//Positions hight and low floor and ciel
float downPoint = 0.0f;
float upWallPoint = 2.0f;
float upPoint = 4.0f;

//Counter creted vertex
int counterCreatedVertex = 0;

//Vars used when draving the labyrinth; | or - shaped walls to be drawn
//walls to draw
bool rowsToDraw[BIGNUM][BIGNUM];
bool colsToDraw[BIGNUM][BIGNUM];
//direction of the walls (left or right for | walls and up or down for - walls
float rowsWallsDir[BIGNUM][BIGNUM];
float colsWallsDir[BIGNUM][BIGNUM];

void colsSequence(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<float> &vNorms, std::vector<int> &vIdx, bool keepVertex, bool proposeGrowingDirection);

//Draw the wall following the perimeter; starting with a | wall
void rowsSequence(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<float> &vNorms, std::vector<int> &vIdx, bool keepVertex, bool proposeGrowingDirection){
    //Find the place to start drawing the wall
    int colNumber = i;
    int starting = j;
    bool growingDirection = true;//Find also in witch direction to draw the wall
    if(starting > 0){
        if(rowsToDraw[colNumber][starting-1] == true){
            growingDirection = false;
        }
    }
    int ending = starting;
    bool iMStillLookingForTheFinalPosition = true;
    while(iMStillLookingForTheFinalPosition == true){//Find the final position to complete drawing the wall
        int tryNewFinal = ending;
        if(growingDirection == true){
            tryNewFinal += 1;
        } else {
            tryNewFinal -= 1;
        }
        if(tryNewFinal < 0 || tryNewFinal > row){
            iMStillLookingForTheFinalPosition = false;
        } else {
            if(rowsToDraw[colNumber][tryNewFinal] == false){
                iMStillLookingForTheFinalPosition = false;
            }
        }
        if(iMStillLookingForTheFinalPosition == true){
            ending = tryNewFinal;
        }
    }
    if(starting == ending){
        growingDirection = proposeGrowingDirection;
    }
    //You are gonig to draw the wall, so here we take note we won't have to draw it again
    if(ending >= starting){
        for(int k = starting; k <= ending; k++){
            rowsToDraw[colNumber][k] = false;
        }
    } else {
        for(int k = starting; k >= ending; k--){
            rowsToDraw[colNumber][k] = false;
        }
    }
    //Adjust initial and final points
    int startingPoint = starting;
    int finalPoint = ending + 1;
    if(growingDirection == false){
        startingPoint = starting + 1;
        finalPoint = ending;
    }
    //Draw really the wall
    counterCreatedVertex += 4;
    keepVertex = false;//new change in this algorithm: the vertex now won't be keeped in every case because we have to create the norm
    if(keepVertex == true){
        counterCreatedVertex -= 2;
    }
    if(keepVertex == false){
        vPos.push_back(colNumber); vPos.push_back(downPoint); vPos.push_back(startingPoint);
        vNorms.push_back(rowsWallsDir[i][j]); vNorms.push_back(0); vNorms.push_back(0);
        vPos.push_back(colNumber); vPos.push_back(upPoint); vPos.push_back(startingPoint);
        vNorms.push_back(rowsWallsDir[i][j]); vNorms.push_back(0); vNorms.push_back(0);
    }
    vPos.push_back(colNumber); vPos.push_back(downPoint); vPos.push_back(finalPoint);
    vNorms.push_back(rowsWallsDir[i][j]); vNorms.push_back(0); vNorms.push_back(0);
    vPos.push_back(colNumber); vPos.push_back(upPoint); vPos.push_back(finalPoint);
    vNorms.push_back(rowsWallsDir[i][j]); vNorms.push_back(0); vNorms.push_back(0);
    vIdx.push_back(counterCreatedVertex-4); vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2);
    vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2); vIdx.push_back(counterCreatedVertex-1);
    //Find a next consecutive wall -
    int guardorowNumber = ending + 1;
    if(growingDirection == false){
        guardorowNumber = ending;
    }
    if(colNumber > 0 && colNumber <= col){
        if(colsToDraw[guardorowNumber][colNumber-1] == true && colsToDraw[guardorowNumber][colNumber] == false){
            colsSequence(guardorowNumber,colNumber-1,row,col,vPos,vNorms,vIdx, true, false);
            return;
        } else if(colsToDraw[guardorowNumber][colNumber-1] == false && colsToDraw[guardorowNumber][colNumber] == true){
            colsSequence(guardorowNumber,colNumber,row,col,vPos,vNorms,vIdx, true, true);
            return;
        }
    }
    if(colNumber == 0){
        if(colsToDraw[guardorowNumber][colNumber] == true){
            colsSequence(guardorowNumber,colNumber,row,col,vPos,vNorms,vIdx, true, true);
            return;
        }
    }
    if(colNumber == col+1){
        if(colsToDraw[guardorowNumber][colNumber-1] == true){
            colsSequence(guardorowNumber,colNumber-1,row,col,vPos,vNorms,vIdx, true, false);
            return;
        }
    }
    return;
}

//Draw the wall following the perimeter; starting with a - wall
void colsSequence(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<float> &vNorms, std::vector<int> &vIdx, bool keepVertex, bool proposeGrowingDirection){
    //Find the place to start drawing the wall
    int rowNumber = i;
    int starting = j;
    bool growingDirection = true;//Find also in witch direction to draw the wall
    if(starting > 0){
        if(colsToDraw[rowNumber][starting-1] == true){
            growingDirection = false;
        }
    }
    int ending = starting;
    bool iMStillLookingForTheFinalPosition = true;
    while(iMStillLookingForTheFinalPosition == true){//Find the final position to complete drawing the wall
        int tryNewFinal = ending;
        if(growingDirection == true){
            tryNewFinal += 1;
        } else {
            tryNewFinal -= 1;
        }
        if(tryNewFinal < 0 || tryNewFinal > col){
            iMStillLookingForTheFinalPosition = false;
        } else {
            if(colsToDraw[rowNumber][tryNewFinal] == false){
                iMStillLookingForTheFinalPosition = false;
            }
        }
        if(iMStillLookingForTheFinalPosition == true){
            ending = tryNewFinal;
        }
    }
    if(starting == ending){
        growingDirection = proposeGrowingDirection;
    }
    //You are gonig to draw the wall, so here we take note we won't have to draw it again
    if(ending >= starting){
        for(int k = starting; k <= ending; k++){
            colsToDraw[rowNumber][k] = false;
        }
    } else {
        for(int k = starting; k >= ending; k--){
            colsToDraw[rowNumber][k] = false;
        }
    }
    //Adjust initial and final points
    int startingPoint = starting;
    int finalPoint = ending + 1;
    if(growingDirection == false){
        startingPoint = starting + 1;
        finalPoint = ending;
    }
    //Draw really the wall
    counterCreatedVertex += 4;
    keepVertex = false;//new change in this algorithm: the vertex now won't be keeped in every case because we have to create the norm
    if(keepVertex == true){
        counterCreatedVertex -= 2;
    }
    if(keepVertex == false){
        vPos.push_back(startingPoint); vPos.push_back(downPoint); vPos.push_back(rowNumber);
        vNorms.push_back(0); vNorms.push_back(0); vNorms.push_back(colsWallsDir[i][j]);
        vPos.push_back(startingPoint); vPos.push_back(upPoint); vPos.push_back(rowNumber);
        vNorms.push_back(0); vNorms.push_back(0); vNorms.push_back(colsWallsDir[i][j]);
    }
    vPos.push_back(finalPoint); vPos.push_back(downPoint); vPos.push_back(rowNumber);
    vNorms.push_back(0); vNorms.push_back(0); vNorms.push_back(colsWallsDir[i][j]);
    vPos.push_back(finalPoint); vPos.push_back(upPoint); vPos.push_back(rowNumber);
    vNorms.push_back(0); vNorms.push_back(0); vNorms.push_back(colsWallsDir[i][j]);
    vIdx.push_back(counterCreatedVertex-4); vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2);
    vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2); vIdx.push_back(counterCreatedVertex-1);
    //Find a next consecutive wall |
    int guardocolNumber = ending + 1;
    if(growingDirection == false){
        guardocolNumber = ending;
    }
    if(rowNumber > 0 && rowNumber <= row){
        if(rowsToDraw[guardocolNumber][rowNumber-1] == true && rowsToDraw[guardocolNumber][rowNumber] == false){
            rowsSequence(guardocolNumber,rowNumber-1,row,col,vPos,vNorms,vIdx, true, false);
            return;
        } else if(rowsToDraw[guardocolNumber][rowNumber-1] == false && rowsToDraw[guardocolNumber][rowNumber] == true){
            rowsSequence(guardocolNumber,rowNumber,row,col,vPos,vNorms,vIdx, true, true);
            return;
        }
    }
    if(rowNumber == 0){
        if(rowsToDraw[guardocolNumber][rowNumber] == true){
            rowsSequence(guardocolNumber,rowNumber,row,col,vPos,vNorms,vIdx, true, true);
            return;
        }
    }
    if(rowNumber == col+1){
        if(rowsToDraw[guardocolNumber][rowNumber-1] == true){
            rowsSequence(guardocolNumber,rowNumber-1,row,col,vPos,vNorms,vIdx, true, false);
            return;
        }
    }
    return;
}

// Draw the labyrinth
void LabyrinthSurvival::createMazeMesh(int row, int col, char **maze) {
    
    //variable declaration for wall positions
    bool rows[col+1][row];//Wall at shape |
    bool cols[row+1][col];//Wall at shape -
    for(int i = 0; i < col+1; i++){
        for(int j = 0; j < row; j++){
            rows[i][j] = false;
        }
    }
    for(int i = 0; i < row+1; i++){
        for(int j = 0; j < col; j++){
            cols[i][j] = false;
        }
    }
        
    //fix wall var to memorize wall's presence
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(maze[i][j] == '#'){
                if(j-1 >= 0){
                    if(maze[i][j-1] != '#'){
                        rows[j][i] = true;//wall in left
                        rowsWallsDir[j][i] = -1;//left
                    }
                } else {
                    rows[j][i] = true;//wall in left
                    rowsWallsDir[j][i] = -1;//left
                }
                if(j+1 < col){
                    if(maze[i][j+1] != '#'){
                        rows[j+1][i] = true;//wall in right
                        rowsWallsDir[j+1][i] = 1;//right
                    }
                } else {
                    rows[j+1][i] = true;//wall in right
                    rowsWallsDir[j+1][i] = 1;//right
                }
                if(i-1 >= 0){
                    if(maze[i-1][j] != '#'){
                        cols[i][j] = true;//wall in forward
                        colsWallsDir[i][j] = -1;//forward
                    }
                } else {
                    cols[i][j] = true;//wall in forward
                    colsWallsDir[i][j] = -1;//forward
                }
                if(i+1 < row){
                    if(maze[i+1][j] != '#'){
                        cols[i+1][j] = true;//wall in backward
                        colsWallsDir[i+1][j] = 1;//backward
                    }
                } else {
                    cols[i+1][j] = true;//wall in backward
                    colsWallsDir[i+1][j] = 1;//backward
                }
            }
        }
    }
    
    /*
    //Let's see the walls
    std::cout << "\n";
    for(int j = 0; j < row; j++){
        for(int i = 0; i < col+1; i++){
            if(rows[i][j] == true){
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    for(int i = 0; i < row+1; i++){
        for(int j = 0; j < col; j++){
            if(cols[i][j] == true){
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    */
    
    /*
    //Let's see the labyrinth
    int vLrow = row*2+1;
    int vLcol = col*2+1;
    bool letSSeeTheLabyrinth[vLrow][vLcol];//this var used just for debug to see visually the labyrinth
    for(int i = 0; i < vLrow; i++){
        for(int j = 0; j < vLcol; j++){
            letSSeeTheLabyrinth[i][j] = false;
            if(i%2 == 0 && j%2 == 0){
                letSSeeTheLabyrinth[i][j] = true;
            }
        }
    }
    for(int i = 0; i < col + 1; i++){
        for(int j = 0; j < row; j++){
            if(rows[i][j] == true){
                letSSeeTheLabyrinth[j*2+1][i*2] = true;
            }
        }
    }
    for(int i = 0; i < row + 1; i++){
        for(int j = 0; j < col; j++){
            if(cols[i][j] == true){
                letSSeeTheLabyrinth[i*2][j*2+1] = true;
            }
        }
    }
    for(int i = 0; i < vLrow; i++){
        for(int j = 0; j < vLcol; j++){
            if(letSSeeTheLabyrinth[i][j] == true){
                std::cout << "X";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    */
    
    //which walls I have to draw, copy them in rowsToDraw (| walls) and colsToDraw (- walls) vars
    for(int i = 0; i < col+1; i++){
        for(int j = 0; j < row; j++){
            rowsToDraw[i][j] = rows[i][j];
        }
    }
    for(int i = 0; i < row+1; i++){
        for(int j = 0; j < col; j++){
            colsToDraw[i][j] = cols[i][j];
        }
    }
    
    //Loop to run when there are walls to draw
    bool areThereAnyWallToDraw = true;
    while(areThereAnyWallToDraw){
        //check if any walls to draw already
        areThereAnyWallToDraw = false;
        for(int i = 0; i < col+1; i++){//Check if any | wall to draw
            for(int j = 0; j < row; j++){
                if(rowsToDraw[i][j] == true){
                    areThereAnyWallToDraw = true;
                }
            }
        }
        /*
        //This part of code is optional because, starting by drawing a | wall, we will follow the perimeter drawing also all - wall
        for(int i = 0; i < row+1; i++){//Check if any - wall to draw
            for(int j = 0; j < col; j++){
                if(colsToDraw[i][j] == true){
                    //areThereAnyWallToDraw = true;//
                }
            }
        }
        */
        if(areThereAnyWallToDraw == true){
            //START Here there are walls to be drawn
            
            //Search a wall sequence starting with a | wall
            for(int i = 0; i < col+1; i++){
                for(int j = 0; j < row; j++){
                    if(rowsToDraw[i][j] == true){
                        rowsSequence(i,j,row,col,vPos,vNorms,vIdx, false, true);//Draw a wall sequence starting with this | wall
                    }
                }
            }
            /*
            //Search a wall sequence starting with a - wall
            //This part of code is optional because, starting by drawing a | wall, we will follow the perimeter drawing also all - wall
            for(int i = 0; i < row+1; i++){
                for(int j = 0; j < col; j++){
                    if(colsToDraw[i][j] == true){
                        colsSequence(i,j,row,col,vPos,vNorms,vIdx, false, true);//Draw a wall sequence starting with this - wall
                    }
                }
            }
            */
            //END Here there are walls to draw
        }
    }
    
    /*
    //Add the floor
    counterCreatedVertex += 4;
    vPos.push_back(0.0f); vPos.push_back(downPoint); vPos.push_back(0.0f);
    vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
    vPos.push_back(col); vPos.push_back(downPoint); vPos.push_back(0.0f);
    vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
    vPos.push_back(0.0f); vPos.push_back(downPoint); vPos.push_back(row);
    vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
    vPos.push_back(col); vPos.push_back(downPoint); vPos.push_back(row);
    vNorms.push_back(0); vNorms.push_back(-1); vNorms.push_back(0);
    vIdx.push_back(counterCreatedVertex-4); vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2);
    vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2); vIdx.push_back(counterCreatedVertex-1);
    */
    /*
    //Add the ciel
    counterCreatedVertex += 4;
    vPos.push_back(0.0f); vPos.push_back(upWallPoint); vPos.push_back(0.0f);
    vNorms.push_back(0); vNorms.push_back(1); vNorms.push_back(0);
    vPos.push_back(col); vPos.push_back(upWallPoint); vPos.push_back(0.0f);
    vNorms.push_back(0); vNorms.push_back(1); vNorms.push_back(0);
    vPos.push_back(0.0f); vPos.push_back(upWallPoint); vPos.push_back(row);
    vNorms.push_back(0); vNorms.push_back(1); vNorms.push_back(0);
    vPos.push_back(col); vPos.push_back(upWallPoint); vPos.push_back(row);
    vNorms.push_back(0); vNorms.push_back(1); vNorms.push_back(0);
    vIdx.push_back(counterCreatedVertex-4); vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2);
    vIdx.push_back(counterCreatedVertex-3); vIdx.push_back(counterCreatedVertex-2); vIdx.push_back(counterCreatedVertex-1);
    */
    
}
