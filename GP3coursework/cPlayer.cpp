#include "cPlayer.h"

cPlayer::cPlayer() : cModel()
{

}

void cPlayer::attachInputMgr(cInputMgr* inputMgr)
{
	m_InputMgr = inputMgr;
}

void cPlayer::update(float elapsedTime)
{
	float fireTime = 0;

	if (m_InputMgr->isKeyDown(VK_RIGHT))
	{
		translationX -= 5.0f;
	}
	if (m_InputMgr->isKeyDown(VK_LEFT))
	{
		translationX += 5.0f;
	}
	if (m_InputMgr->isKeyDown(VK_UP))
	{
		translationY -= 5.0f;
	}
	if (m_InputMgr->isKeyDown(VK_DOWN))
	{
		translationY += 5.0f;
	}

	if (m_InputMgr->isKeyDown(VK_SPACE) && fireTime == 0)
	{
		fireTime += 1;

		glm::vec3 mdlLaserDirection;
		mdlLaserDirection.x = -(float)glm::sin(glm::radians(this->getRotation()));
		mdlLaserDirection.y = 0.0f;
		mdlLaserDirection.z = (float)glm::cos(glm::radians(this->getRotation()));
		mdlLaserDirection *= -1;

		// Add new bullet sprite to the vector array
		theTardisLasers.push_back(new cLaser);
		int numLasers = theTardisLasers.size() - 1;
		theTardisLasers[numLasers]->setDirection(mdlLaserDirection);
		theTardisLasers[numLasers]->setRotation(0.0f);
		theTardisLasers[numLasers]->setScale(glm::vec3(1, 1, 1));
		theTardisLasers[numLasers]->setSpeed(5.0f);
		theTardisLasers[numLasers]->setPosition(this->getPosition() + mdlLaserDirection);
		theTardisLasers[numLasers]->setIsActive(true);
		//theTardisLasers[numLasers]->setMdlDimensions(theLaser.getModelDimensions());
		theTardisLasers[numLasers]->update(elapsedTime);
		// play the firing sound
		m_SoundMgr->getSnd("Shot")->playAudio(AL_TRUE);

	}

	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cLaser*>::iterator laserIterartor = theTardisLasers.begin(); laserIterartor != theTardisLasers.end(); ++laserIterartor)
	{
		(*laserIterartor)->update(elapsedTime);
		for (vector<cEnemy*>::iterator enemyIterator = theEnemy.begin(); enemyIterator != theEnemy.end(); ++enemyIterator)
		{
			if ((*enemyIterator)->SphereSphereCollision((*laserIterartor)->getPosition(), (*laserIterartor)->getMdlRadius()))
			{
				// if a collision set the bullet and spaceship to false
				(*enemyIterator)->setIsActive(false);
				(*laserIterartor)->setIsActive(false);
				// play the explosion sound.
				m_SoundMgr->getSnd("Explosion")->playAudio(AL_TRUE);
			}
		}
	}

	vector<cLaser*>::iterator laserIterartor = theTardisLasers.begin();
	while (laserIterartor != theTardisLasers.end())
	{
		if ((*laserIterartor)->isActive() == false)
		{
			laserIterartor = theTardisLasers.erase(laserIterartor);
		}
		else
		{
			++laserIterartor;
		}
	}

	vector<cEnemy*>::iterator enemyIterartor = theEnemy.begin();
	while (enemyIterartor != theEnemy.end())
	{
		if ((*enemyIterartor)->isActive() == false)
		{
			enemyIterartor = theEnemy.erase(enemyIterartor);
		}
		else
		{
			++enemyIterartor;
		}
	}

	// Find out what direction we should be thrusting, using rotation.
	glm::vec3 mdlVelocityAdd;

	mdlVelocityAdd.x = 0.0f;   // Remember to adjust for radians (float)glm::sin(glm::radians(m_mdlRotation)
	mdlVelocityAdd.y = 0.0f;
	mdlVelocityAdd.z = 0.0f;

	m_mdlRotation -= rotationAngle;

	if ((translationX != 0) && (translationY == 0))
	{
		mdlVelocityAdd.x = -(float)glm::sin(glm::radians(100.f));
		mdlVelocityAdd *= translationX;
	}
	else if ((translationY != 0) && (translationX == 0))
	{
		mdlVelocityAdd.y = -(float)glm::sin(glm::radians(100.f));;
		mdlVelocityAdd *= translationY;
	}
	else
	{
		mdlVelocityAdd.x = 0.0f;
		mdlVelocityAdd.y = 0.0f;
		mdlVelocityAdd.z = 0.0f;
		mdlVelocityAdd *= 0;
	}

	m_mdlDirection += mdlVelocityAdd;

	m_mdlPosition += m_mdlDirection * m_mdlSpeed *elapsedTime;
	m_mdlDirection *= 0.95f;

	rotationAngle = 0;
	translationX = 0;
	translationY = 0;
	translationZ = 0;
}

cPlayer::~cPlayer()
{

}