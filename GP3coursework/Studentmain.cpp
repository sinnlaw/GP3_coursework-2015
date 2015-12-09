#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


//#include <windows.h>
#include "GameConstants.h"
#include "windowOGL.h"
#include "cWNDManager.h"
#include "cColours.h"
#include "cShapes.h"
#include "cPyramid.h"
#include "cCube.h"
#include "cSphere.h"
#include "cMaterial.h"
#include "cLight.h"
#include "cStarfield.h"
#include "cFontMgr.h"
#include "cCamera.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cModelLoader.h"
#include "cModel.h"
#include "cPlayer.h"
#include "cEnemy.h"
#include "cLaser.h"
#include "tardisWarsGame.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

	//Set current sence
	int currentPointer = 0;
	int currentSence = 0;

	//Setup different camera
	int currentCamera = 0;
	float cameraPosX = 0.0f;

    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;



    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
	
	//The example OpenGL code
    windowOGL theOGLWnd;

    //Attach our example to our window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);


    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	// Create Texture map
	cTexture skyShipTexture;
	skyShipTexture.createTexture("Models/skyShip/Maps/vj2c.bmp");
	cTexture rockTexture;
	rockTexture.createTexture("Models/stone/Models/RockSmooth0076_4_L.png");
//	cTexture spaceShipTexture;
//	spaceShipTexture.createTexture("Models/SpaceShip/sh3.jpg");
//	cTexture laserTexture;
//	laserTexture.createTexture("Models/AIM120DMissile/texture.png");
	cTexture starTexture;
	starTexture.createTexture("Images/star.png");

	// the starfield
	cStarfield theStarField(starTexture.getTexture(), glm::vec3(50.0f, 50.0f, 50.0f));

	// Create Materials for lights
	cMaterial sunMaterial(lightColour4(0.0f, 0.0f, 0.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0, 0, 0, 1.0f), 5.0f);

	// Create Light
	cLight sunLight(GL_LIGHT0, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, 20, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight lfLight(GL_LIGHT1, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight rfLight(GL_LIGHT2, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(-30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight cbLight(GL_LIGHT3, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, -100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	//Define Ambient light for scene
	GLfloat g_Ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_Ambient);

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[4] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/doctor_who.ttf", "Fonts/seeds.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);
	theFontMgr->addFont("titleFont", gameFonts[3], 50);
	theFontMgr->addFont("seedsFont", gameFonts[3], 26);

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);

	cCamera theCamera;
	cCamera the2ndCamera;

		// Create a camera
		theCamera.setTheCameraPos(glm::vec3(cameraPosX, 50.0f, 50.0f));
		theCamera.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
		theCamera.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)); // pointing upwards in world space
		theCamera.setTheCameraAspectRatio(windowWidth, windowHeight);
		theCamera.setTheProjectionMatrix(45.0f, theCamera.getTheCameraAspectRatio(), 0.1f, 300.0f);
		theCamera.update();

		//Create the 3rd person camera
		the2ndCamera.setTheCameraPos(glm::vec3(0.0f, 0.0f, 50.0f));
		the2ndCamera.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
		the2ndCamera.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f)); // pointing upwards in world space
		the2ndCamera.setTheCameraAspectRatio(windowWidth, windowHeight);
		the2ndCamera.setTheProjectionMatrix(45.0f, theCamera.getTheCameraAspectRatio(), 0.1f, 300.0f);
		the2ndCamera.update();

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	// Model
	cModelLoader skyShipMdl;
	skyShipMdl.loadModel("Models/skyShip/starship.obj", skyShipTexture); // Player
	
	cModelLoader doctorWhoMdl;
	doctorWhoMdl.loadModel("Models/Tardis.obj", skyShipTexture);

	cModelLoader enemyMdl;
	enemyMdl.loadModel("Models/stone/Models/stone.obj", rockTexture); // Enemy
	//enemyMdl.loadModel("Models/Tardis.obj", skyShipTexture);
	
	cModelLoader theLaser;
	theLaser.loadModel("Models/AIM120DMissile/AIM120D.obj", rockTexture);

	//cModelLoader largeShipMdl;
	//largeShipMdl.loadModel("Models/sof/sof_", skyShipTexture);

	for (int loop = 0; loop < 10; loop++)
	{
		theEnemy.push_back(new cEnemy);
  		theEnemy[loop]->randomise();
		theEnemy[loop]->setMdlDimensions(enemyMdl.getModelDimensions());
		theEnemy[loop]->setScale(glm::vec3(2, 2, 2));
	}

	cPlayer thePlayer;
	thePlayer.initialise(glm::vec3(0, 0, 0), 180.0f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0, 0, 0), 5.0f, true);	//this is for skyship
	//thePlayer.initialise(glm::vec3(0, 0, 0), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, 0, 0), 5.0f, true);	//this is for Tardis
	thePlayer.setMdlDimensions(skyShipMdl.getModelDimensions());
	thePlayer.attachInputMgr(theInputMgr);
	thePlayer.attachSoundMgr(theSoundMgr);

	float tCount = 0.0f;
	string outputMsg;

//	theSoundMgr->getSnd("Theme")->playAudio(AL_LOOPING);

	std::vector<cLaser*> laserList;
	std::vector<cLaser*>::iterator index;

   //This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
	{

		pgmWNDMgr->processWNDEvents(); //Process any window events

		//We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		// Lab code goes here
		glClearColor(0.045f, 0.045f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		theOGLWnd.initOGL(windowWidth, windowHeight);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (currentCamera == 0)
		{
			theCamera.setTheCameraPos(glm::vec3(thePlayer.getPosition().x, 50.0f, 50.0f));
			theCamera.setTheCameraLookAt(glm::vec3(thePlayer.getPosition().x, 0.0f, 0.0f));
			theCamera.update();
			glLoadMatrixf((GLfloat*)&theCamera.getTheViewMatrix());
		}
		if (currentCamera == 1)
		{
			glLoadMatrixf((GLfloat*)&the2ndCamera.getTheViewMatrix());
		}



		if (currentSence == 0)
		{

			if (GetAsyncKeyState(VK_DOWN))
			{
				currentPointer = 1;
			}
			else if (GetAsyncKeyState(VK_UP))
			{
				currentPointer = 0;
			}
			
			if (GetAsyncKeyState(VK_SPACE))
			{
				if (currentPointer == 0)
				{
					currentSence = 1;
					Sleep(1.0f);
				}
				else if (currentPointer == 1)
				{
					theOGLWnd.shutdown(); //Free any resources
					pgmWNDMgr->destroyWND(); //Destroy the program window

					return 0; //Return success
				}

			}

			outputMsg = to_string(theEnemy.size());

			glPushMatrix();
			theOGLWnd.setOrtho2D(windowWidth, windowHeight);
			theFontMgr->getFont("titleFont")->printText("THE GAME", FTPoint(400, 250, 0.0f), colour3f(1.0f, 1.0f, 1.0f));
			if (currentPointer == 0)
			{
				theFontMgr->getFont("seedsFont")->printText("Start", FTPoint(450, 400, 0.0f), colour3f(0.5f, 1.0f, 0.5f));
				theFontMgr->getFont("seedsFont")->printText("End", FTPoint(450, 450, 0.0f), colour3f(0.0f, 0.5f, 0.0f));
			}
			else if (currentPointer == 1)
			{
				theFontMgr->getFont("seedsFont")->printText("Start", FTPoint(450, 400, 0.0f), colour3f(0.0f, 0.5f, 0.0f));
				theFontMgr->getFont("seedsFont")->printText("End", FTPoint(450, 450, 0.0f), colour3f(0.5f, 1.0f, 0.5f));
			}
			glPopMatrix();

		}

		if (currentSence == 1)
		{

			if (GetAsyncKeyState(VK_TAB))
			{	currentCamera = 1;	}
			else{ currentCamera = 0; }

			theStarField.render(0.0f);
			sunMaterial.useMaterial();
			sunLight.lightOn();
			lfLight.lightOn();
			rfLight.lightOn();
			cbLight.lightOn();

			//start render obj
			//theLaser.renderMdl(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1, 1, 1));

			for (vector<cEnemy*>::iterator enemyIterator = theEnemy.begin(); enemyIterator != theEnemy.end(); ++enemyIterator)
			{
				if ((*enemyIterator)->isActive())
				{
					enemyMdl.renderMdl((*enemyIterator)->getPosition(), (*enemyIterator)->getRotation(), (*enemyIterator)->getScale());
					(*enemyIterator)->update(elapsedTime);
				}
			}

			skyShipMdl.renderMdl(thePlayer.getPosition(), thePlayer.getRotation(), thePlayer.getScale());
			thePlayer.update(elapsedTime);


					for (vector<cLaser*>::iterator laserIterartor = theTardisLasers.begin(); laserIterartor != theTardisLasers.end(); ++laserIterartor)
					{
						if ((*laserIterartor)->isActive())
						{
							theLaser.renderMdl((*laserIterartor)->getPosition(), (*laserIterartor)->getRotation(), (*laserIterartor)->getScale());
							(*laserIterartor)->update(elapsedTime);
						}
					}

			//outputMsg = to_string(theEnemy.size()); // convert float to string

			glPushMatrix();
			theOGLWnd.setOrtho2D(windowWidth, windowHeight);
			theFontMgr->getFont("seedsFont")->printText("THE GAME", FTPoint(10, 50, 0.0f), colour3f(255.0f, 255.0f, 255.0f));
			theFontMgr->getFont("seedsFont")->printText(outputMsg.c_str(), FTPoint(850, 50, 0.0f), colour3f(255.0f, 255.0f, 0.0f)); // uses c_str to convert string to LPCSTR
			glPopMatrix();
		}

		pgmWNDMgr->swapBuffers();

		tCount += elapsedTime;

		//Clear key buffers
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
