#define BIGNUM 99

//Positions hight and low floor and ciel
float puntoBasso = 0.0f;
float puntoMuroAlto = 2.0f;
float puntoAlto = 4.0f;

//Counter creted vertex
int contatoreVerticiCreati = 0;

bool righeDT[BIGNUM][BIGNUM];
bool colonneDT[BIGNUM][BIGNUM];

void sequenzaColonne(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<int> &vIdx, bool conservaVertici, bool proponiDirezioneCrescente);

void sequenzaRighe(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<int> &vIdx, bool conservaVertici, bool proponiDirezioneCrescente){
    //Delete the specific wall |
    int colonnaNumero = i;
    int iniziale = j;
    bool direzioneCrescente = true;
    if(iniziale > 0){
        if(righeDT[colonnaNumero][iniziale-1] == true){
            direzioneCrescente = false;
        }
    }
    int finale = iniziale;
    bool stoAncoraCercandoLaPosFinale = true;
    while(stoAncoraCercandoLaPosFinale == true){
        int provaNuovoFinale = finale;
        if(direzioneCrescente == true){
            provaNuovoFinale += 1;
        } else {
            provaNuovoFinale -= 1;
        }
        if(provaNuovoFinale < 0 || provaNuovoFinale > row){
            stoAncoraCercandoLaPosFinale = false;
        } else {
            if(righeDT[colonnaNumero][provaNuovoFinale] == false){
                stoAncoraCercandoLaPosFinale = false;
            }
        }
        if(stoAncoraCercandoLaPosFinale == true){
            finale = provaNuovoFinale;
        }
    }
    if(iniziale == finale){
        direzioneCrescente = proponiDirezioneCrescente;
    }
    if(finale >= iniziale){
        for(int k = iniziale; k <= finale; k++){
            righeDT[colonnaNumero][k] = false;
        }
    } else {
        for(int k = iniziale; k >= finale; k--){
            righeDT[colonnaNumero][k] = false;
        }
    }
    int puntoIniziale = iniziale;
    int puntoFinale = finale + 1;
    if(direzioneCrescente == false){
        puntoIniziale = iniziale + 1;
        puntoFinale = finale;
    }
    contatoreVerticiCreati += 4;
    if(conservaVertici == true){
        contatoreVerticiCreati -= 2;
    }
    if(conservaVertici == false){
        vPos.push_back(colonnaNumero); vPos.push_back(puntoBasso); vPos.push_back(puntoIniziale);
        vPos.push_back(colonnaNumero); vPos.push_back(puntoAlto); vPos.push_back(puntoIniziale);
    }
    vPos.push_back(colonnaNumero); vPos.push_back(puntoBasso); vPos.push_back(puntoFinale);
    vPos.push_back(colonnaNumero); vPos.push_back(puntoAlto); vPos.push_back(puntoFinale);
    vIdx.push_back(contatoreVerticiCreati-4); vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2);
    vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2); vIdx.push_back(contatoreVerticiCreati-1);
    //Cerco un consecutivo muro -
    int guardoRigaNumero = finale + 1;
    if(direzioneCrescente == false){
        guardoRigaNumero = finale;
    }
    if(colonnaNumero > 0 && colonnaNumero <= col){
        if(colonneDT[guardoRigaNumero][colonnaNumero-1] == true && colonneDT[guardoRigaNumero][colonnaNumero] == false){
            sequenzaColonne(guardoRigaNumero,colonnaNumero-1,row,col,vPos,vIdx, true, false);
            return;
        } else if(colonneDT[guardoRigaNumero][colonnaNumero-1] == false && colonneDT[guardoRigaNumero][colonnaNumero] == true){
            sequenzaColonne(guardoRigaNumero,colonnaNumero,row,col,vPos,vIdx, true, true);
            return;
        }
    }
    if(colonnaNumero == 0){
        if(colonneDT[guardoRigaNumero][colonnaNumero] == true){
            sequenzaColonne(guardoRigaNumero,colonnaNumero,row,col,vPos,vIdx, true, true);
            return;
        }
    }
    if(colonnaNumero == col+1){
        if(colonneDT[guardoRigaNumero][colonnaNumero-1] == true){
            sequenzaColonne(guardoRigaNumero,colonnaNumero-1,row,col,vPos,vIdx, true, false);
            return;
        }
    }
    return;
}

void sequenzaColonne(int i, int j, int row, int col, std::vector<float> &vPos, std::vector<int> &vIdx, bool conservaVertici, bool proponiDirezioneCrescente){
    //Elimino lo specifico muro -
    int rigaNumero = i;
    int iniziale = j;
    bool direzioneCrescente = true;
    if(iniziale > 0){
        if(colonneDT[rigaNumero][iniziale-1] == true){
            direzioneCrescente = false;
        }
    }
    int finale = iniziale;
    bool stoAncoraCercandoLaPosFinale = true;
    while(stoAncoraCercandoLaPosFinale == true){
        int provaNuovoFinale = finale;
        if(direzioneCrescente == true){
            provaNuovoFinale += 1;
        } else {
            provaNuovoFinale -= 1;
        }
        if(provaNuovoFinale < 0 || provaNuovoFinale > col){
            stoAncoraCercandoLaPosFinale = false;
        } else {
            if(colonneDT[rigaNumero][provaNuovoFinale] == false){
                stoAncoraCercandoLaPosFinale = false;
            }
        }
        if(stoAncoraCercandoLaPosFinale == true){
            finale = provaNuovoFinale;
        }
    }
    if(iniziale == finale){
        direzioneCrescente = proponiDirezioneCrescente;
    }
    if(finale >= iniziale){
        for(int k = iniziale; k <= finale; k++){
            colonneDT[rigaNumero][k] = false;
        }
    } else {
        for(int k = iniziale; k >= finale; k--){
            colonneDT[rigaNumero][k] = false;
        }
    }
    int puntoIniziale = iniziale;
    int puntoFinale = finale + 1;
    if(direzioneCrescente == false){
        puntoIniziale = iniziale + 1;
        puntoFinale = finale;
    }
    contatoreVerticiCreati += 4;
    if(conservaVertici == true){
        contatoreVerticiCreati -= 2;
    }
    if(conservaVertici == false){
        vPos.push_back(puntoIniziale); vPos.push_back(puntoBasso); vPos.push_back(rigaNumero);
        vPos.push_back(puntoIniziale); vPos.push_back(puntoAlto); vPos.push_back(rigaNumero);
    }
    vPos.push_back(puntoFinale); vPos.push_back(puntoBasso); vPos.push_back(rigaNumero);
    vPos.push_back(puntoFinale); vPos.push_back(puntoAlto); vPos.push_back(rigaNumero);
    vIdx.push_back(contatoreVerticiCreati-4); vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2);
    vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2); vIdx.push_back(contatoreVerticiCreati-1);
    //Cerco un consecutivo muro |
    int guardoColonnaNumero = finale + 1;
    if(direzioneCrescente == false){
        guardoColonnaNumero = finale;
    }
    if(rigaNumero > 0 && rigaNumero <= row){
        if(righeDT[guardoColonnaNumero][rigaNumero-1] == true && righeDT[guardoColonnaNumero][rigaNumero] == false){
            sequenzaRighe(guardoColonnaNumero,rigaNumero-1,row,col,vPos,vIdx, true, false);
            return;
        } else if(righeDT[guardoColonnaNumero][rigaNumero-1] == false && righeDT[guardoColonnaNumero][rigaNumero] == true){
            sequenzaRighe(guardoColonnaNumero,rigaNumero,row,col,vPos,vIdx, true, true);
            return;
        }
    }
    if(rigaNumero == 0){
        if(righeDT[guardoColonnaNumero][rigaNumero] == true){
            sequenzaRighe(guardoColonnaNumero,rigaNumero,row,col,vPos,vIdx, true, true);
            return;
        }
    }
    if(rigaNumero == col+1){
        if(righeDT[guardoColonnaNumero][rigaNumero-1] == true){
            sequenzaRighe(guardoColonnaNumero,rigaNumero-1,row,col,vPos,vIdx, true, false);
            return;
        }
    }
    return;
}

// Draw the labirinth
void LabirinthSurvival::createMazeMesh(int row, int col, char **maze) {
    
    //variable declaration for wall positions
    bool righe[col+1][row];//Wall at shape |
    bool colonne[row+1][col];//Wall at shape -
    for(int i = 0; i < col+1; i++){
        for(int j = 0; j < row; j++){
            righe[i][j] = false;
        }
    }
    for(int i = 0; i < row+1; i++){
        for(int j = 0; j < col; j++){
            colonne[i][j] = false;
        }
    }
    
    // TODO: Spiegas questa cosa - aggiungi qualche commento - refector delle variabili in inglese
    
    //fix wall var to memorize wall's presence
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(maze[i][j] == '#'){
                if(j-1 >= 0){
                    if(maze[i][j-1] != '#'){
                        righe[j][i] = true;//wall in left
                    }
                } else {
                    righe[j][i] = true;//wall in left
                }
                if(j+1 < col){
                    if(maze[i][j+1] != '#'){
                        righe[j+1][i] = true;//wall in right
                    }
                } else {
                    righe[j+1][i] = true;//wall in right
                }
                if(i-1 >= 0){
                    if(maze[i-1][j] != '#'){
                        colonne[i][j] = true;//wall in forward
                    }
                } else {
                    colonne[i][j] = true;//wall in forward
                }
                if(i+1 < row){
                    if(maze[i+1][j] != '#'){
                        colonne[i+1][j] = true;//wall in backward
                    }
                } else {
                    colonne[i+1][j] = true;//wall in backward
                }
            }
        }
    }
    
    /*
    //Vediamo i muri
    std::cout << "\n";
    for(int j = 0; j < row; j++){
        for(int i = 0; i < col+1; i++){
            if(righe[i][j] == true){
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
            if(colonne[i][j] == true){
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    */
    
    //Let's see the labirinth
    int vLrow = row*2+1;
    int vLcol = col*2+1;
    bool vediamoLabirinto[vLrow][vLcol];
    for(int i = 0; i < vLrow; i++){
        for(int j = 0; j < vLcol; j++){
            vediamoLabirinto[i][j] = false;
            if(i%2 == 0 && j%2 == 0){
                vediamoLabirinto[i][j] = true;
            }
        }
    }
    for(int i = 0; i < col + 1; i++){
        for(int j = 0; j < row; j++){
            if(righe[i][j] == true){
                vediamoLabirinto[j*2+1][i*2] = true;
            }
        }
    }
    for(int i = 0; i < row + 1; i++){
        for(int j = 0; j < col; j++){
            if(colonne[i][j] == true){
                vediamoLabirinto[i*2][j*2+1] = true;
            }
        }
    }
    /*
    for(int i = 0; i < vLrow; i++){
        for(int j = 0; j < vLcol; j++){
            if(vediamoLabirinto[i][j] == true){
                std::cout << "X";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    */
    
    //which walls I have to draw, copy them in righeDT and colonneDT vars
    //bool righeDT[col+1][row];//Muri a forma di | // diventata var globale
    //bool colonneDT[row+1][col];//Muri a forma di - // diventata var globale
    for(int i = 0; i < col+1; i++){
        for(int j = 0; j < row; j++){
            righeDT[i][j] = righe[i][j];
        }
    }
    for(int i = 0; i < row+1; i++){
        for(int j = 0; j < col; j++){
            colonneDT[i][j] = colonne[i][j];
        }
    }
    
    //Loop to run qwhen there are walls to draw
    bool ciSonoAncoraMuriDaTracciare = true;
    while(ciSonoAncoraMuriDaTracciare){
        //check if any walls to draw already
        ciSonoAncoraMuriDaTracciare = false;
        for(int i = 0; i < col+1; i++){
            for(int j = 0; j < row; j++){
                if(righeDT[i][j] == true){
                    ciSonoAncoraMuriDaTracciare = true;
                }
            }
        }
        /*
        for(int i = 0; i < row+1; i++){
            for(int j = 0; j < col; j++){
                if(colonneDT[i][j] == true){
                    //ciSonoAncoraMuriDaTracciare = true;//
                }
            }
        }
        */
        if(ciSonoAncoraMuriDaTracciare == true){
            //START Here there are walls to draw
            
            //ricerca sequenza di muri iniziante con un muro |
            for(int i = 0; i < col+1; i++){
                for(int j = 0; j < row; j++){
                    if(righeDT[i][j] == true){
                        //INIZIA sequenza righe da toglere | partendo da righeDT[i][j]
                        sequenzaRighe(i,j,row,col,vPos,vIdx, false, true);
                        //FINE sequenza righe da toglere | partendo da righeDT[i][j]
                    }
                }
            }
            /*
            //ricerca sequenza di muri iniziante con un muro -
            for(int i = 0; i < row+1; i++){
                for(int j = 0; j < col; j++){
                    if(colonneDT[i][j] == true){
                        //INIZIA sequenza colonne da toglere | partendo da colonneDT[i][j]
                        sequenzaColonne(i,j,row,col,vPos,vIdx, false, true);
                        //FINE sequenza colonne da toglere | partendo da colonneDT[i][j]
                    }
                }
            }
            */
            //END Here there are walls to draw
        }
    }
    
    //Add the wall
    contatoreVerticiCreati += 4;
    vPos.push_back(0.0f); vPos.push_back(puntoBasso); vPos.push_back(0.0f);
    vPos.push_back(col); vPos.push_back(puntoBasso); vPos.push_back(0.0f);
    vPos.push_back(0.0f); vPos.push_back(puntoBasso); vPos.push_back(row);
    vPos.push_back(col); vPos.push_back(puntoBasso); vPos.push_back(row);
    vIdx.push_back(contatoreVerticiCreati-4); vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2);
    vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2); vIdx.push_back(contatoreVerticiCreati-1);
    
    //Add the ciel
    contatoreVerticiCreati += 4;
    vPos.push_back(0.0f); vPos.push_back(puntoMuroAlto); vPos.push_back(0.0f);
    vPos.push_back(col); vPos.push_back(puntoMuroAlto); vPos.push_back(0.0f);
    vPos.push_back(0.0f); vPos.push_back(puntoMuroAlto); vPos.push_back(row);
    vPos.push_back(col); vPos.push_back(puntoMuroAlto); vPos.push_back(row);
    vIdx.push_back(contatoreVerticiCreati-4); vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2);
    vIdx.push_back(contatoreVerticiCreati-3); vIdx.push_back(contatoreVerticiCreati-2); vIdx.push_back(contatoreVerticiCreati-1);
    
}
