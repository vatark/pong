#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 780

typedef enum {up, down, stand}direction;

typedef struct game{
  SDL_Window *g_pWindow;
  SDL_Renderer *g_pRenderer;
  SDL_Texture *g_ptexture;
  SDL_Surface *g_psurface;
}game;

typedef struct gameState{
  int g_bRunning;
}gameState;

typedef struct coordonnees{
  double x;
  double y;
  direction direction;
}coordonnees;

typedef struct font{
  TTF_Font *g_font;
}font;

//prototypes
int init(char *title, int xpos,int ypos,int width, int height,int flags,game *myGame, font *mFont);
void destroy(game *myGame, font *mFont);
void handleEvents(gameState *state,coordonnees *dep,coordonnees *dep2);
void delay(unsigned int frameLimit);
void render(game *myGame);
void drawInRenderer(game *myGame);
void drawInSurface(game *myGame);
void renderTexture(game *myGame,coordonnees *dep,coordonnees *dep2, coordonnees *dep3 );
void directionBalle(int *angle, coordonnees *dep3);
void calculAngle(int *angle, coordonnees *dep, coordonnees *dep2, coordonnees *dep3);

void writeSDL(game *myGame,font mFont, SDL_Surface *texte, char points[5]);

int main(int argc, char *argv[])
{
  srand(time(NULL));

  game myGame;
  gameState state;

  unsigned int frameLimit = SDL_GetTicks() + 16;
  coordonnees dep;
  coordonnees dep2;
  coordonnees dep3;
  
  int choix=0;
  int direction=5;
  int balleAngle = rand()%(360-1) + 1;

  SDL_Surface texte;
  font mFont;
  char tPoints[5] = "";
  int points1, points2;

  dep.y = SCREEN_HEIGHT/2;
  dep2.y = SCREEN_HEIGHT/2;
  dep.direction = stand;
  dep2.direction = stand;
  dep3.x = SCREEN_WIDTH/2;
  dep3.y = SCREEN_HEIGHT/2;
  
  
  state.g_bRunning=1;
  
  //Create texture for drawing in texture or load picture
  myGame.g_ptexture=SDL_CreateTexture(myGame.g_pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,SCREEN_WIDTH,SCREEN_HEIGHT);


  
  // while(state.g_bRunning){
  init("Pong",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN,&myGame,&mFont);
  myGame.g_ptexture=SDL_CreateTexture(myGame.g_pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,SCREEN_WIDTH,SCREEN_HEIGHT);


  points1 = 0;
  points2 = 0;
  while(state.g_bRunning){
      sprintf(tPoints, "%d : %d", points1, points2);
      
      writeSDL(&myGame,mFont, &texte, tPoints);
      handleEvents(&state,&dep,&dep2);
      renderTexture(&myGame, &dep, &dep2, &dep3);
      calculAngle(&balleAngle, &dep, &dep2, &dep3);
      directionBalle(&balleAngle, &dep3);

      if (dep3.x >= SCREEN_WIDTH) {
	points1 ++;
	dep3.x = SCREEN_WIDTH/2;
	dep3.y = SCREEN_HEIGHT/2;
      }

      if (dep3.x+10 <= 0) {
	points2 ++;
	dep3.x = SCREEN_WIDTH/2;
	dep3.y = SCREEN_HEIGHT/2;
      }
      

    system ("clear");
    
    
    // Gestion des 60 fps (1000ms/60 = 16.6 -> 16
    delay(frameLimit);
    frameLimit = SDL_GetTicks() + 16;
  }

  destroy(&myGame, &mFont);

  TTF_Quit();
  SDL_Quit();
  
  return 0;
}


int init(char *title, int xpos,int ypos,int width, int height,int flags,game *myGame, font *mFont){


    myGame->g_pWindow=NULL;
    myGame->g_pRenderer=NULL;
    myGame->g_psurface=NULL;
    myGame->g_ptexture=NULL;


    //initialize SDL

    if(SDL_Init(SDL_INIT_EVERYTHING)>=0)
    {
            //if succeeded create our window
            myGame->g_pWindow=SDL_CreateWindow(title,xpos,ypos,width,height,flags);
            //if succeeded create window, create our render
            if(myGame->g_pWindow!=NULL){
                myGame->g_pRenderer=SDL_CreateRenderer(myGame->g_pWindow,-1,SDL_RENDERER_PRESENTVSYNC);
            }
    }else{
        return 0;
    }

    if(TTF_Init() == -1)
      {
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
      }
    
    mFont->g_font=TTF_OpenFont("./assets/fonts/Pixeled.ttf",65);

    return 1;
}

void writeSDL(game *myGame,font mFont, SDL_Surface *texte, char points[5]) {
  
        SDL_Color fontColor={255,255,255};

        texte=TTF_RenderText_Solid(mFont.g_font, points, fontColor);//Charge la police
  
        if(texte){
                //Définition du rectangle dest pour blitter la chaine
               SDL_Rect rectangle;
                rectangle.x=SCREEN_WIDTH/3;//debut x
                rectangle.y=SCREEN_HEIGHT/3;//debut y
                rectangle.w=400; //Largeur
                rectangle.h=200; //Hauteur
		

	         myGame->g_ptexture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,texte); // Préparation de la texture pour la chaine
                 SDL_FreeSurface(texte); // Libération de la ressource occupée par le sprite

                 if(myGame->g_ptexture){

		   // SDL_RenderCopy(myGame->g_pRenderer,myGame->g_ptexture,NULL,&rectangle); // Copie du sprite grâce au SDL_Renderer
		   //      SDL_RenderPresent(myGame->g_pRenderer); // Affichage
                 }
                 else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }



        }else{
            fprintf(stdout,"Échec de creation surface pour chaine (%s)\n",SDL_GetError());
        }

}

void renderTexture(game *myGame,coordonnees *dep,coordonnees *dep2, coordonnees *dep3) {

  //Définition du filet à dessiner
  int i = 0;

  SDL_Rect filet;
  filet.y = 0;
  while (filet.y <= SCREEN_HEIGHT) {
    SDL_SetRenderDrawColor(myGame->g_pRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    
    filet.w=10; //Largeur
    filet.h=30; //Hauteur
    
    filet.x = SCREEN_WIDTH / 2 - filet.w / 2;
    filet.y = i * (filet.h + 20);
    
    SDL_RenderFillRect(myGame->g_pRenderer,&filet);
    i++;
  }

  //Définition des bords à dessiner
  SDL_Rect bordHaut;
  bordHaut.x=0;
  bordHaut.y=0;
  bordHaut.w=SCREEN_WIDTH;
  bordHaut.h=10;

  SDL_Rect bordBas;
  bordBas.x=0;
  bordBas.y=SCREEN_HEIGHT - 10;
  bordBas.w=SCREEN_WIDTH;
  bordBas.h=10;
  
  //Définition du rectangle 1 a dessiner
  SDL_Rect rectangle;
  rectangle.x=10;//debut x
  rectangle.y=dep->y;//debut y
  rectangle.w=10; //Largeur
  rectangle.h=100; //Hauteur
  
  
  //Définition du rectangle 2 a dessiner
  SDL_Rect rectangle2;
  rectangle2.x=SCREEN_WIDTH-20;//debut x
  rectangle2.y=dep2->y;//debut y
  rectangle2.w=10; //Largeur
  rectangle2.h=100; //Hauteur

  //Définition de la balle à dessiner
  SDL_Rect balle;
  balle.x=dep3->x;//debut x
  balle.y=dep3->y;//debut y
  balle.w = 10; //Largeur
  balle.h = 10; //Hauteur
  
  //Draw in texture
  SDL_SetRenderDrawColor(myGame->g_pRenderer,255,255,255,255);
  SDL_SetRenderTarget(myGame->g_pRenderer, myGame->g_ptexture); //on modifie la texture
  
  SDL_RenderFillRect(myGame->g_pRenderer, &rectangle);
  SDL_RenderFillRect(myGame->g_pRenderer, &rectangle2);
  SDL_RenderFillRect(myGame->g_pRenderer, &balle);
  SDL_RenderFillRect(myGame->g_pRenderer, &bordHaut);
  SDL_RenderFillRect(myGame->g_pRenderer, &bordBas);

  
  SDL_SetRenderTarget(myGame->g_pRenderer, NULL);// Dorénavent, on modifie à nouveau le renderer
  
  SDL_Rect position;//position de la texture dans screen
  position.x = 0;
  position.y = 0;
  SDL_QueryTexture(myGame->g_ptexture, NULL, NULL, &position.w, &position.h);
  SDL_RenderCopy(myGame->g_pRenderer,myGame->g_ptexture,NULL,&position);
  

  
  SDL_RenderPresent(myGame->g_pRenderer);


  
  SDL_SetRenderDrawColor(myGame->g_pRenderer,0,0,0,255);
  SDL_DestroyTexture(myGame->g_ptexture);
  SDL_RenderClear(myGame->g_pRenderer);
  
}

void destroy(game *myGame, font *mFont){
  if(mFont->g_font!=NULL){
    TTF_CloseFont(mFont->g_font); /* Doit être avant TTF_Quit() */
    mFont->g_font=NULL;
  }
    
  //Destroy render
  if(myGame->g_pRenderer!=NULL)
    SDL_DestroyRenderer(myGame->g_pRenderer);
  
  //Destroy texture
  if(myGame->g_ptexture!=NULL)
    SDL_DestroyTexture(myGame->g_ptexture);
  
  //Destroy surface
  if(myGame->g_psurface!=NULL)
    SDL_FreeSurface(myGame->g_psurface);
  
  //Destroy window
  if(myGame->g_pWindow!=NULL)
    SDL_DestroyWindow(myGame->g_pWindow);
  
}


void handleEvents(gameState *state,coordonnees *dep,coordonnees *dep2){

  SDL_Event event;
  
  if(SDL_PollEvent(&event)){
    switch(event.type){
    case SDL_QUIT:
      state->g_bRunning=0;break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
	{
	case SDLK_UP:
	  dep2->direction = up;
	  break;
	case SDLK_DOWN:
	  dep2->direction = down;
	  break;
	case SDLK_z:
	  dep->direction = up;
	  break;
	case SDLK_s:
	  dep->direction = down;
	  break;
	}
      break;
      
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_UP:
	if (dep2->direction == up) {
	  dep2->direction = stand;
	}break;
      case SDLK_DOWN:
	if (dep2->direction == down) {
	  dep2->direction = stand;
	}break;
      case SDLK_z:
	if (dep->direction == up) {
	  dep->direction = stand;
	}break;
      case SDLK_s:
	if (dep->direction == down) {
	  dep->direction = stand;
	}break;      
      }
      break;
      
    default:break;
      
    }

    
  }

  switch (dep2->direction){
    case up:
      if (dep2->y > 10) {
	dep2->y-=10;
      }
      break;
    case down:
      if (dep2->y < SCREEN_HEIGHT - 110) {
	dep2->y+=10;
      }
      break;
    }


    switch (dep->direction){
    case up:
      if (dep->y > 10) {
	dep->y-=10;
      }
      break;
    case down:
      if (dep->y < SCREEN_HEIGHT - 110) {
	dep->y+=10;
      }
      break;
    }
}

void directionBalle(int *angle, coordonnees *dep3) {  
  dep3->x += 5 * sin((*angle*3.14)/180);
  dep3->y += 5 * cos((*angle*3.14)/180);
}

void calculAngle(int *angle, coordonnees *dep, coordonnees *dep2, coordonnees *dep3) {  
  //Si la balle touche les bords de l'écran
  if((dep3->y+10 >= SCREEN_HEIGHT-10)
     ||(dep3->y <= 10))
    {
      *angle = *angle + (90 - *angle)*2;
    }
  
  //Si la balle touche la raquette gauche
  if((dep3->x <= 20)      // trop à droite
     && (dep3->y <= dep->y + 100) // trop en bas
     && (dep3->y + 10 >= dep->y))  // trop en haut
    {
      *angle = *angle + (180 - *angle) * 2;
    }

  //Si la balle touche la raquette droite
  if((dep3->x+10 >= SCREEN_WIDTH-20)      // trop à droite
     && (dep3->x+10 <= SCREEN_WIDTH)
     && (dep3->y <= dep2->y + 100) // trop en bas
     && (dep3->y + 10 >= dep2->y))  // trop en haut
    {
      *angle = *angle + (180 - *angle)*2;
    }
}

void delay(unsigned int frameLimit)
{
    // Gestion des 60 fps (images/seconde)
    unsigned int ticks = SDL_GetTicks();

    if (frameLimit < ticks)
    {
        return;
    }

    if (frameLimit > ticks + 16)
    {
        SDL_Delay(16);
    }

    else
    {
        SDL_Delay(frameLimit - ticks);
    }
}
