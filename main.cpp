#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cBkGround.h"
#include "cSprite.h"
#include "cSoundMgr.h"
#include "cFontMgr.h"
#include "asteroidsGame.h"
#include "cButton.h"
#include "cFileHandler.h"
int score = 0; //starting player score 

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR cmdLine,
	int cmdShow)
{

	//Set our window settings
	const int windowWidth = 1024;
	const int windowHeight = 768;
	const int windowBPP = 16;

	//This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

	//The example OpenGL code
	windowOGL theOGLWnd;

	//Attach our the OpenGL window
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
	int score = 0;
	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);


	/* initialize random seed: */
	srand((unsigned int)time(NULL));

	// Create vector array of textures
	LPCSTR texturesToUse[] = { "Images\\enemycar.png", "Images\\enemycar.png", "Images\\enemycar.png", "Images\\enemycar.png", "Images\\bullet.png" };
	for (int tCount = 0; tCount < 5; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(texturesToUse[tCount]);
	}

	// Create vector array of background textures
	vector<cTexture*> textureBkgList;
	LPCSTR bkgTexturesToUse[] = { "Images/mainroad.jpg", "Images/mainmenu.jpg", "Images/end.jpg" };
	for (int tCount = 0; tCount < 3; tCount++)
	{
		textureBkgList.push_back(new cTexture());
		textureBkgList[tCount]->createTexture(bkgTexturesToUse[tCount]);
	}

	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgList[0]->getTexture());
	spriteBkgd.setTextureDimensions(textureBkgList[0]->getTWidth(), textureBkgList[0]->getTHeight());

	cBkGround spriteStartBkgd;
	spriteStartBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStartBkgd.setTexture(textureBkgList[1]->getTexture());
	spriteStartBkgd.setTextureDimensions(textureBkgList[1]->getTWidth(), textureBkgList[1]->getTHeight());

	cBkGround spriteEndBkgd;
	spriteEndBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteEndBkgd.setTexture(textureBkgList[2]->getTexture());
	spriteEndBkgd.setTextureDimensions(textureBkgList[2]->getTWidth(), textureBkgList[2]->getTHeight());
	cTexture textureBkgd;

	// Create vector array of button textures
	vector<cTexture*> btnTextureList;
	LPCSTR btnTexturesToUse[] = { "Images/Buttons/exit.png", "Images/Buttons/instructionsBtn.png", "Images/Buttons/loadBtn.png", "Images/Buttons/play.png", "Images/Buttons/save.png", "Images/Buttons/settingsBtn.png" };
	for (int tCount = 0; tCount < 6; tCount++)
	{
		btnTextureList.push_back(new cTexture());
		btnTextureList[tCount]->createTexture(btnTexturesToUse[tCount]);
	}

	cButton saveButton;
	saveButton.attachInputMgr(theInputMgr);
	saveButton.setTexture(btnTextureList[4]->getTexture());
	saveButton.setTextureDimensions(btnTextureList[4]->getTWidth(), btnTextureList[4]->getTHeight());

	cButton exitButton;
	exitButton.attachInputMgr(theInputMgr);
	exitButton.setTexture(btnTextureList[0]->getTexture());
	exitButton.setTextureDimensions(btnTextureList[0]->getTWidth(), btnTextureList[0]->getTHeight());

	cButton playButton;
	playButton.attachInputMgr(theInputMgr);
	playButton.setTexture(btnTextureList[3]->getTexture());
	playButton.setTextureDimensions(btnTextureList[3]->getTWidth(), btnTextureList[3]->getTHeight());

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/mainmusic.wav", "Audio/enginestart.wav", "Audio/cargone.wav" };
	// first [0] is background main game theme souned
	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);


	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[2] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };

	theFontMgr->addFont("digital", gameFonts[0], 24);
	theFontMgr->addFont("space", gameFonts[1], 24);

	// Create vector for enemycar objects.

	for (int astro = 0; astro < 30; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos(glm::vec2(800.0f / (rand() % 5 + 1), 600.0f / (rand() % 5 + 1)));    // (glm::vec2(800.0f / (rand() % 5 + 1), 600.0f / (rand() % 5 + 1)))
		theAsteroids[astro]->setSpriteTranslation(glm::vec2(5.0f, 5.0f));  // (glm::vec2((200.0f / 5.0f), (rand() % 4 + 1)))
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
		theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
		theAsteroids[astro]->setSpriteCentre();
		theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
		theAsteroids[astro]->setActive(true);
		theAsteroids[astro]->setMdlRadius();
	}
	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\playercar.png");
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); // Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(512.0f, 500.0f));
	rocketSprite.setTexture(rocketTxt.getTexture());
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth(), rocketTxt.getTHeight());
	rocketSprite.setSpriteCentre();
	rocketSprite.setRocketVelocity(glm::vec2(0.0f, 0.0f));

	// Attach sound manager to player sprite
	rocketSprite.attachSoundMgr(theSoundMgr);


	// include an exit button

	cFileHandler theFile("Data/usermap.dat");
	string mapData;

	string outputMsg; //create message text to inform player of controls
	string strMsg[] = { "ARROW KEYS to MOVE and SPACE to SHOOT" ,"Shoot enemies to win", "Bumper Cars", "Nice driving!","Thanks for playing!" };

	gameState theGameState = MENU;
	btnTypes theBtnType = EXIT;

	//This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
	{
		pgmWNDMgr->processWNDEvents(); //Process any window events

									   //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (theGameState) //switch statement to control game state
		{
		case MENU:
			// create menu game state (menu screen)
			spriteStartBkgd.render();
			//place buttons and render them
			playButton.setSpritePos(glm::vec2(500.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(500.0f, 375.0f));
			playButton.render();
			exitButton.render();
			//If play is pressed change the gamer to the playing screen
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update(elapsedTime);

			// Show lines "Bumper Cars"
			outputMsg = strMsg[2];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));
			outputMsg = strMsg[0];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(100, 100, 0.0f));
			outputMsg = strMsg[1];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(100, 150, 0.0f));
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
			break;
		case PLAYING:
			// create playing state (main game)
			spriteBkgd.render();
			{
				//create enemies on screen 
				vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
				while (asteroidIterator != theAsteroids.end())
				{
					if ((*asteroidIterator)->isActive() == false)
					{
						//destroy enemy sprite and increase score by 1.
						asteroidIterator = theAsteroids.erase(asteroidIterator);
						score++;
					}
					else
					{
						(*asteroidIterator)->update(elapsedTime);
						(*asteroidIterator)->render();
						++asteroidIterator;
					}
				}
			}
			//theAsteroids[0]->render();
			rocketSprite.update(elapsedTime);
			rocketSprite.update(elapsedTime);
			rocketSprite.render();
			// draw Score and score number
			score = rocketSprite.updater(score);
			theFontMgr->getFont("space")->printText("Bumper Cars", FTPoint(800.0f, 750.0f, 0.0f));
			theFontMgr->getFont("space")->printText("Score", FTPoint(200.0f, 550.0f, 0.0f));
			outputMsg = to_string(score);
			theFontMgr->getFont("space")->printText(outputMsg.c_str(), FTPoint(350.0f, 550.0, 0.0f));
			// render button and reset clicked to false;
			exitButton.setSpritePos(glm::vec2(940.0f, 575.0f));
			exitButton.render();
			theGameState = exitButton.update(theGameState, END);
			outputMsg = strMsg[2];
			theFontMgr->getFont("space")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));

			break;


		case END:
			//end game state (game over screen)
			spriteEndBkgd.render();

			playButton.setClicked(false);
			exitButton.setClicked(false);
			// show play button and exit button
			playButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 375.0f));
			playButton.render();
			exitButton.render();
			// if play button is pressed, return to play screen
			theGameState = playButton.update(theGameState, PLAYING);
			exitButton.update(theGameState, END);
			//display end screen text messages
			outputMsg = strMsg[2];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(10, 15, 0.0f));
			outputMsg = strMsg[3];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(100, 100, 0.0f));
			outputMsg = strMsg[4];
			theFontMgr->getFont("digital")->printText(outputMsg.c_str(), FTPoint(100, 150, 0.0f));
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}
			break;

		}

		pgmWNDMgr->swapBuffers();
		theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER);

	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

	return 0; //Return success

}
