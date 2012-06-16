#include "ovavrdCHandballBCI.h"

#include <iostream>

template <typename T> const T _abs_(const T& t) { return t<0?-t:t; }
template <typename T> const T _min_(const T& t1, const T& t2) { return t1<t2?t1:t2; }
template <typename T> const T _max_(const T& t1, const T& t2) { return t1<t2?t2:t1; }
template <typename T> const T _crop_(const T& t1, const T& t, const T& t2) { return _min_(_max_(t1, t), t2); }

using namespace OpenViBEVRDemos;
using namespace Ogre;

static const float g_fViscosity=0.003f;
static const float g_fGrowScoreingSpeed=0.1f;
static const float g_fEpsilon=1E-5f;

static const int HISTORY_SIZE = 5;
static const float GOAL_DISTANCE = 6.0f;

CHandballBCI::CHandballBCI() : COgreVRApplication()
{
	// scores
	m_iGoalScore=0;
	m_iLeftScore=0;
	m_iRightScore=0;
	m_iTrialRightCount = 0;
	m_iTrialLeftCount = 0;
	// the classification score
	m_iClassificationScore=0;

	// The current and previous phases
	m_iPhase=Phase_Rest;
	m_iLastPhase=Phase_Rest;

	// Feedback from analog VRPN server
	m_dFeedback=0;
	m_dLastFeedback=0;
	m_dMaxFeedback=0;
	m_dMinFeedback=0;

	m_iMark = Mark_None;
	m_iLastMark = Mark_None;
	m_bGoalMarkedAtThisPhase = false;
	m_bShowCross = false;
	m_bShowClue = false;
	m_bShowFeedback = false;

	m_fBallSpeed=0;
	m_fBallPosition=0;
	m_fLastBallPosition=0;
	m_fBallOrientation=0;
}

bool CHandballBCI::initialise()
{
	// taken from gymnasium.scene + transformations from demo.omk
	
	//----------- LIGHTS -------------//
	m_poSceneManager->setAmbientLight(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
	m_poSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);

	Ogre::Light* l_poLight1 = m_poSceneManager->createLight("Light1");
	//SceneNode *l_poLight1Node = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "Light1Node" );
	l_poLight1->setPosition(8,6,-5);
	l_poLight1->setSpecularColour(1,1,1);
	l_poLight1->setDiffuseColour(1,1,1);
	
	//----------- CAMERA -------------//
	m_poCamera->setNearClipDistance(0.01f);
	m_poCamera->setFarClipDistance(2000.0f);
	m_poCamera->setFOVy(Radian(Degree(100)));
	m_poCamera->setProjectionType(PT_PERSPECTIVE);

	m_poCameraNode->setPosition(5.5f,0.9f,0.f);
	m_poCameraNode->yaw(Radian(Math::PI/2.f));
	
	this->initSceneGymnasium();
	this->initSceneAds();
	this->initSceneGoals();
	this->initSceneCrossArrowAndFeedback();
	this->initSceneBalls();

	return true;
}


bool CHandballBCI::process(double timeSinceLastProcess)
{

	//------------ VRPN --------------//
	// The button VRPN server must use 6 switches, inputs from the stim scenario (lua):
	/*

	[ GET READY | | ACTIVE | REST |~| MARK LEFT | MARK RIGHT | NO INSTRUCTION ]
		
	*/
	while(!m_poVrpnPeripheral->m_vButton.empty())
	{
		std::pair < int, int >& l_rVrpnButtonState=m_poVrpnPeripheral->m_vButton.front();

		if(l_rVrpnButtonState.second)
		{
			switch(l_rVrpnButtonState.first)
			{
				case 0:
					m_iPhase=Phase_GetReady;
					m_bShowCross = true;
					m_bShowClue = false;
					m_bShowFeedback = false;
					break;

				case 1:
					m_iPhase=Phase_Active;
					m_bShowCross = true;
					m_bShowClue = false;
					m_bShowFeedback = true;
					break;

				case 2:
					m_iPhase=Phase_Rest;
					m_bShowCross = false;
					m_bShowClue = false;
					m_bShowFeedback = false;
					break;
				//------------Instructions ---------------
				case 3:
					m_iMark=Mark_Left;
					m_bShowClue = true;
					m_iTrialLeftCount++;
					break;

				case 4:
					m_iMark=Mark_Right;
					m_bShowClue = true;
					m_iTrialRightCount++;
					break;

				case 5:
					m_iMark=Mark_None;
					break;

				default:
					break;
			}
		}

		m_poVrpnPeripheral->m_vButton.pop_front();
	}

	if(!m_poVrpnPeripheral->m_vAnalog.empty())
	{
		std::list < double >& l_rVrpnAnalogState=m_poVrpnPeripheral->m_vAnalog.front();

		//we take the last value from the server
		double l_dAnalog=*(l_rVrpnAnalogState.begin());

		//we updtae the max and min values
		// A. VLG : we should compute the min/max only in the ACTIVE phase
		// but if so, the first feedback value is infinite, resulting in a strange ball placement in the first active phase.
		if(m_iPhase == Phase_Active || m_iPhase == Phase_GetReady)
		{
			if(l_dAnalog>m_dMaxFeedback) m_dMaxFeedback=l_dAnalog;
			if(l_dAnalog<m_dMinFeedback) m_dMinFeedback=l_dAnalog;
		}
		
		//maximum of the absolute values
		
		double l_dAbsoluteMinMax=_max_(_abs_(m_dMaxFeedback), _abs_(m_dMinFeedback));

		//we update the history, keeping only the last HISTORY_SIZE values
		m_vAnalogHistory.push_back(l_dAnalog);
		if(m_vAnalogHistory.size() > HISTORY_SIZE)
		{
			m_vAnalogHistory.pop_front();
		}

		// we compute the mean value of the analog history
		double l_dAnalogMean=0;
		std::list < double >::const_iterator i;
		for(i=m_vAnalogHistory.begin(); i!=m_vAnalogHistory.end(); i++)
		{
			l_dAnalogMean+=*i;
		}
		l_dAnalogMean/=m_vAnalogHistory.size();

		// and we divide it by the absolute max value, to get a value between 0 and 1
		l_dAnalogMean/=(l_dAbsoluteMinMax?l_dAbsoluteMinMax:g_fEpsilon);

		// we update the feedback value that will be used to move the ball
		m_dLastFeedback = m_dFeedback;
		m_dFeedback=l_dAnalogMean;

		m_poVrpnPeripheral->m_vAnalog.pop_front();
	}


	//--------------------------//
	SceneNode * l_poCrossNode = m_poSceneManager->getSceneNode("CrossNode");
	SceneNode * l_poLeftArrowNode = m_poSceneManager->getSceneNode("LeftArrowNode");
	SceneNode * l_poRightArrowNode = m_poSceneManager->getSceneNode("RightArrowNode");
	SceneNode * l_poFeedbackNode = m_poSceneManager->getSceneNode("FeedbackNode");
	SceneNode * l_poActiveBallPivot = m_poSceneManager->getSceneNode("ActiveBallPivot");
	SceneNode * l_poPassiveBallPivot = m_poSceneManager->getSceneNode("PassiveBallPivot");
	SceneNode * l_poGetReadyBallPivot = m_poSceneManager->getSceneNode("GetReadyBallPivot");
	SceneNode * l_poBallCommonPivot = m_poSceneManager->getSceneNode("BallCommonNode");
	std::list < double >::const_iterator ii;
				
	// update for each phase change
	if(m_iPhase!=m_iLastPhase)
	{
		switch(m_iPhase)
		{
			case Phase_Rest:
				std::cout << "### PHASE REST ###" <<std::endl;
				if(m_iLastMark==Mark_Left && m_fBallPosition<0) { m_iLeftScore++;}
				if(m_iLastMark==Mark_Right&& m_fBallPosition>0) { m_iRightScore++;}
				
				std::cout << "- Current Score: " <<std::endl;
				std::cout << "--- GOAL:  " <<m_iGoalScore<<"/"<<(m_iTrialRightCount+m_iTrialLeftCount)<<std::endl;
				std::cout << "--- LEFT:  " <<m_iLeftScore<<"/"<<m_iTrialLeftCount<<std::endl;
				std::cout << "--- RIGHT: " <<m_iRightScore<<"/"<<m_iTrialRightCount<<std::endl;
				std::cout << "--- CLASSIFICATION: " <<m_iClassificationScore<<std::endl;
				//std::cout << "### ScOrE StAtUs = GoAl:" << m_iGoalScore << " SiDe:" << m_iSideScore << " ClAsSiFiCaTiOn:" << m_iClassificationScore << " LoOpCoUnT: " <<   << "\n";
				
				l_poCrossNode->setVisible(false);
				l_poLeftArrowNode->setVisible(false);
				l_poRightArrowNode->setVisible(false);
				l_poFeedbackNode->setVisible(false);

				l_poGetReadyBallPivot->setVisible(false);
				l_poPassiveBallPivot->setVisible(true);
				l_poActiveBallPivot->setVisible(false);

				m_fBallPosition=0;
				m_fBallOrientation = 0;
				m_fBallSpeed=0;
				l_poBallCommonPivot->setPosition(0,l_poBallCommonPivot->getPosition().y,m_fBallPosition);
		
				break;
			case Phase_GetReady:
				std::cout << "### PHASE GET READY ###" <<std::endl;
				l_poCrossNode->setVisible(true, false);
				l_poLeftArrowNode->setVisible(false);
				l_poRightArrowNode->setVisible(false);
				l_poFeedbackNode->setVisible(false);
				
				l_poGetReadyBallPivot->setVisible(true);
				l_poPassiveBallPivot->setVisible(false);
				l_poActiveBallPivot->setVisible(false);

				break;

			
			case Phase_Active:
				std::cout << "### PHASE ACTIVE ###" <<std::endl;
				l_poCrossNode->setVisible(true);
				l_poLeftArrowNode->setVisible(false);
				l_poRightArrowNode->setVisible(false);
				l_poFeedbackNode->setVisible(true);

				l_poGetReadyBallPivot->setVisible(false);
				l_poPassiveBallPivot->setVisible(false);
				l_poActiveBallPivot->setVisible(true);

				/*std::cout<<"Current analog history : ";
				for(ii=m_vAnalogHistory.begin(); ii!=m_vAnalogHistory.end(); ii++)
				{
					std::cout << (*ii) << " ";
				}
				std::cout << std::endl;*/

				break;
		}
	}

	
	// update for each phase (loop)
	switch(m_iPhase)
	{
		case Phase_Rest:
			break;

		case Phase_GetReady:
			l_poLeftArrowNode->setVisible(m_bShowClue ? (m_iMark == Mark_Left) : false);
			l_poRightArrowNode->setVisible(m_bShowClue ? (m_iMark == Mark_Right) : false);
			break;

		case Phase_Active:
			m_fBallSpeed=-m_dFeedback * 0.1f;
			if(m_iMark==Mark_Left && m_fBallPosition==-GOAL_DISTANCE && !m_bGoalMarkedAtThisPhase) { m_iGoalScore++; m_bGoalMarkedAtThisPhase=true; }
			if(m_iMark==Mark_Right&& m_fBallPosition== GOAL_DISTANCE && !m_bGoalMarkedAtThisPhase) { m_iGoalScore++; m_bGoalMarkedAtThisPhase=true; }
			if(m_iMark==Mark_Left && m_dFeedback<0) { m_iClassificationScore++;}
			if(m_iMark==Mark_Right&& m_dFeedback>0) { m_iClassificationScore++;}

			break;
	}

	m_fBallOrientation+=m_fBallSpeed;
	m_fBallPosition+=m_fBallSpeed;
	m_fBallPosition=_crop_(-GOAL_DISTANCE, m_fBallPosition, GOAL_DISTANCE);
	m_fBallSpeed*=(1.f-g_fViscosity);

	double l_dFeedbackScale = m_dFeedback;
	if(l_dFeedbackScale <= 0 && l_dFeedbackScale > -g_fEpsilon) l_dFeedbackScale=-g_fEpsilon;
	if(l_dFeedbackScale >= 0 && l_dFeedbackScale <  g_fEpsilon) l_dFeedbackScale= g_fEpsilon;

	// -------------------------------------------------------------------------------
	// Object displacement stuffs
	//BALL FEEDBACK
	if(m_iPhase==Phase_Active)
	{
		l_poBallCommonPivot->setPosition(0,l_poBallCommonPivot->getPosition().y,m_fBallPosition);
		//l_poBallCommonPivot->setOrientation(1,m_fBallOrientation*100.f, 0,0 ); // *4
		//l_poActiveBallPivot->pitch(Radian(m_fBallOrientation/20.),Node::TS_PARENT);
		
		float div = (m_fBallOrientation*4)/(2*Math::PI);
		div = Math::Floor(div);

		Entity * l_poPassiveBallEntity = m_poSceneManager->getEntity("passive-ball");
		float alpha = (m_fBallPosition - m_fLastBallPosition) / l_poPassiveBallEntity->getBoundingRadius();
		
		/*printf("m_fBallOrientation=%f\n",(m_fBallOrientation*4));
		printf("div=%i\n",div);
		float final = (m_fBallOrientation*4)-(div*2*Math::PI);
		printf("final=%f\n",final);*/
		
		l_poActiveBallPivot->pitch(Radian(alpha),Node::TS_PARENT);
		//l_poActiveBallPivot->setOrientation(1,(m_fBallOrientation*4)/*-(div*2*Math::PI)*/,0,0 ); // *4
		//l_poActiveBallPivot->setOrientation(1,m_fBallOrientation*4,0,0);
	}

	// GRAZ FEEDBACK
	if(m_iPhase==Phase_Active)
	{
		l_poFeedbackNode->setVisible(true);
		l_poFeedbackNode->setScale(0.25f,0.25f,_abs_(l_dFeedbackScale));
		l_poFeedbackNode->setPosition(3.499f,0.20f,-l_dFeedbackScale/2.f);
	}
	else
	{
		l_poFeedbackNode->setVisible(false);
	}
	

	

	// -------------------------------------------------------------------------------
	// End of computation

	m_dLastFeedback=m_dFeedback;
	m_fLastBallPosition = m_fBallPosition;
	m_iLastPhase=m_iPhase;
	m_iLastMark=m_iMark;

	return m_bContinue;
}


void CHandballBCI::initSceneGymnasium()
{
	//----------- GYMNASIUM -------------//
	SceneNode * l_poGymnasiumPivot = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "GymnasiumPivot" );
	SceneNode * l_poGymnasium = l_poGymnasiumPivot->createChildSceneNode("Gymnasium");
	SceneNode * l_poGymnasiumNode = l_poGymnasium->createChildSceneNode("GymnasiumNode");
	
	// GYMNASIUM
	Entity * l_poGymnasiumNodeEntity = m_poSceneManager->createEntity( "GymnasiumNodeEntity", "gymnasiumNode.mesh" );
	l_poGymnasiumNodeEntity->setCastShadows(false);
	l_poGymnasiumNodeEntity->getSubEntity(0)->setMaterialName("gymnasium-surface06");
	l_poGymnasiumNodeEntity->getSubEntity(1)->setMaterialName("gymnasium-surface10");
	l_poGymnasiumNodeEntity->getSubEntity(2)->setMaterialName("gymnasium-surface09");
	l_poGymnasiumNode->attachObject(l_poGymnasiumNodeEntity);

	// GYM PUB
	Entity * l_poGymnasiumPublicityEntity = m_poSceneManager->createEntity( "GymnasiumPublicity", "gymnasium-publicity.mesh" );
	l_poGymnasiumPublicityEntity->setCastShadows(false);
	l_poGymnasiumPublicityEntity->getSubEntity(0)->setMaterialName("gymnasium-surface11");
	l_poGymnasiumPublicityEntity->getSubEntity(1)->setMaterialName("gymnasium-surface12");
	l_poGymnasiumNode->attachObject(l_poGymnasiumPublicityEntity);

	//DOORS
	SceneNode * l_poDoor01Node = l_poGymnasiumNode->createChildSceneNode("door-01-node");
	l_poDoor01Node->setPosition(26,1,-24); 
	l_poDoor01Node->setOrientation(0.923879f,0,-0.382684f,0);
	Entity * l_poDoor01Entity = m_poSceneManager->createEntity( "door-01-entity", "door-01.mesh" );
	l_poDoor01Entity->setCastShadows(false);
	l_poDoor01Entity->getSubEntity(0)->setMaterialName("gymnasium-surface07");
	l_poDoor01Entity->getSubEntity(1)->setMaterialName("gymnasium-surface08");
	l_poDoor01Node->attachObject(l_poDoor01Entity);

	SceneNode * l_poDoor02Node = l_poGymnasiumNode->createChildSceneNode("door-02-node");
	l_poDoor02Node->setPosition(26,1,4);
	l_poDoor02Node->setOrientation(0.923879f,0,0.382684f,0);
	Entity * l_poDoor02Entity = m_poSceneManager->createEntity( "door-02-entity", "door-02.mesh" );
	l_poDoor02Entity->setCastShadows(false);
	l_poDoor02Entity->getSubEntity(0)->setMaterialName("gymnasium-surface07");
	l_poDoor02Entity->getSubEntity(1)->setMaterialName("gymnasium-surface08");
	l_poDoor02Node->attachObject(l_poDoor02Entity);

	SceneNode * l_poDoor03Node = l_poGymnasiumNode->createChildSceneNode("door-03-node");
	l_poDoor03Node->setPosition(-26,2.5,4);
	l_poDoor03Node->setOrientation(0.923879f,0,-0.382684f,0);
	Entity * l_poDoor03Entity = m_poSceneManager->createEntity( "door-03-entity", "door-03.mesh" );
	l_poDoor03Entity->setCastShadows(false);
	l_poDoor03Entity->getSubEntity(0)->setMaterialName("gymnasium-surface08");
	l_poDoor03Entity->getSubEntity(1)->setMaterialName("gymnasium-surface07");
	l_poDoor03Node->attachObject(l_poDoor03Entity);

	SceneNode * l_poDoor04Node = l_poGymnasiumNode->createChildSceneNode("door-04-node");
	l_poDoor04Node->setPosition(-26,1,-24);
	l_poDoor04Node->setOrientation(0.923879f,0,0.382684f,0);
	Entity * l_poDoor04Entity = m_poSceneManager->createEntity( "door-04-entity", "door-04.mesh" );
	l_poDoor04Entity->setCastShadows(false);
	l_poDoor04Entity->getSubEntity(0)->setMaterialName("gymnasium-surface07");
	l_poDoor04Entity->getSubEntity(1)->setMaterialName("gymnasium-surface08");
	l_poDoor04Node->attachObject(l_poDoor04Entity);

	//TABLE
	SceneNode * l_poTableNode = l_poGymnasiumNode->createChildSceneNode("Table");
	l_poTableNode->setPosition(-2.475f,0.36f,-21.5f);
	l_poTableNode->setOrientation(0.707105f,0.f,0.707108f,0.f);
	Entity * l_poTableEntity = m_poSceneManager->createEntity( "Table", "table.mesh" );
	l_poTableEntity->setCastShadows(false);
	l_poTableEntity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poTableEntity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorBunrakuM2S01");
	l_poTableEntity->getSubEntity(2)->setMaterialName("gymnasium-surface13");
	l_poTableNode->attachObject(l_poTableEntity);

	

	//TRANSFORM FROM demo.omk
	l_poGymnasiumPivot->translate(-10,0,0);
	l_poGymnasiumPivot->rotate(Vector3::UNIT_Y, Radian(-1.570796327f));
}

void CHandballBCI::initSceneAds()
{
	SceneNode* l_poGymnasiumNode = m_poSceneManager->getSceneNode("GymnasiumNode");
	// WALL Ads
	Entity * l_poWallPublicity01Entity = m_poSceneManager->createEntity( "WallPublicity01", "wall-publicity-01.mesh" );
	l_poWallPublicity01Entity->setCastShadows(false);
	l_poWallPublicity01Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity01Entity);

	Entity * l_poWallPublicity02Entity = m_poSceneManager->createEntity( "WallPublicity02", "wall-publicity-02.mesh" );
	l_poWallPublicity02Entity->setCastShadows(false);
	l_poWallPublicity02Entity->getSubEntity(0)->setMaterialName("gymnasium-surface15");
	l_poGymnasiumNode->attachObject(l_poWallPublicity02Entity);

	Entity * l_poWallPublicity03Entity = m_poSceneManager->createEntity( "WallPublicity03", "wall-publicity-03.mesh" );
	l_poWallPublicity03Entity->setCastShadows(false);
	l_poWallPublicity03Entity->getSubEntity(0)->setMaterialName("gymnasium-surface02");
	l_poGymnasiumNode->attachObject(l_poWallPublicity03Entity);

	Entity * l_poWallPublicity04Entity = m_poSceneManager->createEntity( "WallPublicity04", "wall-publicity-04.mesh" );
	l_poWallPublicity04Entity->setCastShadows(false);
	l_poWallPublicity04Entity->getSubEntity(0)->setMaterialName("gymnasium-surface03");
	l_poGymnasiumNode->attachObject(l_poWallPublicity04Entity);

	Entity * l_poWallPublicity06Entity = m_poSceneManager->createEntity( "WallPublicity06", "wall-publicity-06.mesh" );
	l_poWallPublicity06Entity->setCastShadows(false);
	l_poWallPublicity06Entity->getSubEntity(0)->setMaterialName("gymnasium-surface04");
	l_poGymnasiumNode->attachObject(l_poWallPublicity06Entity);

	Entity * l_poWallPublicity07Entity = m_poSceneManager->createEntity( "WallPublicity07", "wall-publicity-07.mesh" );
	l_poWallPublicity07Entity->setCastShadows(false);
	l_poWallPublicity07Entity->getSubEntity(0)->setMaterialName("gymnasium-surface05");
	l_poGymnasiumNode->attachObject(l_poWallPublicity07Entity);

	Entity * l_poWallPublicity08Entity = m_poSceneManager->createEntity( "WallPublicity08", "wall-publicity-08.mesh" );
	l_poWallPublicity08Entity->setCastShadows(false);
	l_poWallPublicity08Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity08Entity);

	Entity * l_poWallPublicity09Entity = m_poSceneManager->createEntity( "WallPublicity09", "wall-publicity-09.mesh" );
	l_poWallPublicity09Entity->setCastShadows(false);
	l_poWallPublicity09Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity09Entity);

	Entity * l_poWallPublicity10Entity = m_poSceneManager->createEntity( "WallPublicity10", "wall-publicity-10.mesh" );
	l_poWallPublicity10Entity->setCastShadows(false);
	l_poWallPublicity10Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity10Entity);

	Entity * l_poWallPublicity11Entity = m_poSceneManager->createEntity( "WallPublicity11", "wall-publicity-11.mesh" );
	l_poWallPublicity11Entity->setCastShadows(false);
	l_poWallPublicity11Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity11Entity);

	Entity * l_poWallPublicity12Entity = m_poSceneManager->createEntity( "WallPublicity12", "wall-publicity-12.mesh" );
	l_poWallPublicity12Entity->setCastShadows(false);
	l_poWallPublicity12Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity12Entity);

	Entity * l_poWallPublicity13Entity = m_poSceneManager->createEntity( "WallPublicity13", "wall-publicity-13.mesh" );
	l_poWallPublicity13Entity->setCastShadows(false);
	l_poWallPublicity13Entity->getSubEntity(0)->setMaterialName("gymnasium-surface14");
	l_poGymnasiumNode->attachObject(l_poWallPublicity13Entity);

	// more ads
	SceneNode * l_poPublicity01Node = l_poGymnasiumNode->createChildSceneNode("publicity-01-node");
	l_poPublicity01Node->setPosition(-22.5f,0.01f,-10.f);
	l_poPublicity01Node->setOrientation(-3.65178e-006f,0.f,1.f,0.f);
	Entity * l_poPublicity01Entity = m_poSceneManager->createEntity( "publicity-01-entity", "publicity-01.mesh" );
	l_poPublicity01Entity->setCastShadows(false);
	l_poPublicity01Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsorBunrakuM2S01");
	l_poPublicity01Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity01Node->attachObject(l_poPublicity01Entity);

	SceneNode * l_poPublicity02Node = l_poGymnasiumNode->createChildSceneNode("publicity-02-node");
	l_poPublicity02Node->setPosition(-22.5f,0.01f,-10.f);
	l_poPublicity02Node->setOrientation(0.707105f,0.707108f,0.f,0.f);
	Entity * l_poPublicity02Entity = m_poSceneManager->createEntity( "publicity-02-entity", "publicity-02.mesh" );
	l_poPublicity02Entity->setCastShadows(false);
	l_poPublicity02Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity02Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorBunrakuM2S01");
	l_poPublicity02Node->attachObject(l_poPublicity02Entity);

	SceneNode * l_poPublicity03Node = l_poGymnasiumNode->createChildSceneNode("publicity-03-node");
	l_poPublicity03Node->setPosition(13.75f,0.36f,1.5f); 
	l_poPublicity03Node->setOrientation(0.707105f,0.f,-0.707108f,0.f);
	Entity * l_poPublicity03Entity = m_poSceneManager->createEntity( "publicity-03-entity", "publicity-03.mesh" );
	l_poPublicity03Entity->setCastShadows(false);
	l_poPublicity03Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity03Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorIrisa01");
	l_poPublicity03Node->attachObject(l_poPublicity03Entity);

	SceneNode * l_poPublicity04Node = l_poGymnasiumNode->createChildSceneNode("publicity-04-node");
	l_poPublicity04Node->setPosition(13.75f,0.36f,1.5f);
	l_poPublicity04Node->setOrientation(0.707105f,0.f,-0.707108f,0.f);
	Entity * l_poPublicity04Entity = m_poSceneManager->createEntity( "publicity-04-entity", "publicity-04.mesh" );
	l_poPublicity04Entity->setCastShadows(false);
	l_poPublicity04Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity04Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorIrisa01");
	l_poPublicity04Node->attachObject(l_poPublicity04Entity);

	SceneNode * l_poPublicity05Node = l_poGymnasiumNode->createChildSceneNode("publicity-05-node");
	l_poPublicity05Node->setPosition(13.75f,0.36f,-21.5f);
	l_poPublicity05Node->setOrientation(0.707105f,0.f,0.707108f,0.f);
	Entity * l_poPublicity05Entity = m_poSceneManager->createEntity( "publicity-05-entity", "publicity-05.mesh" );
	l_poPublicity05Entity->setCastShadows(false);
	l_poPublicity05Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity05Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorIrisa01");
	l_poPublicity05Node->attachObject(l_poPublicity05Entity);

	SceneNode * l_poPublicity06Node = l_poGymnasiumNode->createChildSceneNode("publicity-06-node");
	l_poPublicity06Node->setPosition(-13.75f,0.01f,-21.f);
	l_poPublicity06Node->setOrientation(0.707105f,0.f,0.707108f,0.f);
	Entity * l_poPublicity06Entity = m_poSceneManager->createEntity( "publicity-06-entity", "publicity-06.mesh" );
	l_poPublicity06Entity->setCastShadows(false);
	l_poPublicity06Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity06Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsorIrisa01");
	l_poPublicity06Node->attachObject(l_poPublicity06Entity);

	SceneNode * l_poPublicity07Node = l_poGymnasiumNode->createChildSceneNode("publicity-07-node");
	l_poPublicity07Node->setPosition(-1.96695e-006f,0.71f,1.5f);
	l_poPublicity07Node->setOrientation(0.5f,0.5f,-0.5f,0.5f);
	Entity * l_poPublicity07Entity = m_poSceneManager->createEntity( "publicity-07-entity", "publicity-07.mesh" );
	l_poPublicity07Entity->setCastShadows(false);
	l_poPublicity07Entity->getSubEntity(0)->setMaterialName("gymnasium-MaterialSponsorIrisa01");
	l_poPublicity07Entity->getSubEntity(1)->setMaterialName("gymnasium-MaterialSponsor01");
	l_poPublicity07Node->attachObject(l_poPublicity07Entity);

}
void CHandballBCI::initSceneGoals()
{
	//----------- LEFT GOAL -------------//
	SceneNode * l_poLeftGoalPivot = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "LeftGoalPivot" );
	SceneNode * l_poLeftGoal = l_poLeftGoalPivot->createChildSceneNode("LeftGoal");
	SceneNode * l_poLeftGoalNode = l_poLeftGoal->createChildSceneNode("LeftGoalNode");

	Entity * l_poLeftGoalEntity = m_poSceneManager->createEntity( "LgoalNode", "goalNode.mesh" );
	l_poLeftGoalEntity->setCastShadows(true);
	l_poLeftGoalEntity->getSubEntity(0)->setMaterialName("goal-surface02");
	l_poLeftGoalEntity->getSubEntity(1)->setMaterialName("goal-surface01");
	l_poLeftGoalNode->attachObject(l_poLeftGoalEntity);

	Entity * l_poLeftGoalBorderEntity = m_poSceneManager->createEntity( "Lgoal-border", "goal-border.mesh" );
	l_poLeftGoalBorderEntity->setCastShadows(true);
	l_poLeftGoalBorderEntity->getSubEntity(0)->setMaterialName("goal-frontcol01");
	l_poLeftGoalNode->attachObject(l_poLeftGoalBorderEntity);

	Entity * l_poLeftGoalNetEntity = m_poSceneManager->createEntity( "Lgoal-net", "goal-net.mesh" );
	l_poLeftGoalNetEntity->setCastShadows(true);
	l_poLeftGoalNetEntity->getSubEntity(0)->setMaterialName("goal-foregrou01");
	l_poLeftGoalNode->attachObject(l_poLeftGoalNetEntity);
	
	//TRANSFORM FROM demo.omk
	l_poLeftGoalPivot->translate(0,0,-5.92f);
	l_poLeftGoalPivot->rotate(Vector3::UNIT_Y, Radian(-1.570796327f));
	
	//----------- RIGHT GOAL -------------//
	SceneNode * l_poRightGoalPivot = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "RightGoalPivot" );
	SceneNode * l_poRightGoal = l_poRightGoalPivot->createChildSceneNode("RightGoal");
	SceneNode * l_poRightGoalNode = l_poRightGoal->createChildSceneNode("RightGoalNode");

	Entity * l_poRightGoalEntity = m_poSceneManager->createEntity( "RgoalNode", "goalNode.mesh" );
	l_poRightGoalEntity->setCastShadows(true);
	l_poRightGoalEntity->getSubEntity(0)->setMaterialName("goal-surface02");
	l_poRightGoalEntity->getSubEntity(1)->setMaterialName("goal-surface01");
	l_poRightGoalNode->attachObject(l_poRightGoalEntity);

	Entity * l_poRightGoalBorderEntity = m_poSceneManager->createEntity( "Rgoal-border", "goal-border.mesh" );
	l_poRightGoalBorderEntity->setCastShadows(true);
	l_poRightGoalBorderEntity->getSubEntity(0)->setMaterialName("goal-frontcol01");
	l_poRightGoalNode->attachObject(l_poRightGoalBorderEntity);

	Entity * l_poRightGoalNetEntity = m_poSceneManager->createEntity( "Rgoal-net", "goal-net.mesh" );
	l_poRightGoalNetEntity->setCastShadows(true);
	l_poRightGoalNetEntity->getSubEntity(0)->setMaterialName("goal-foregrou01");
	l_poRightGoalNode->attachObject(l_poRightGoalNetEntity);
	
	//TRANSFORM FROM demo.omk
	l_poRightGoalPivot->translate(0,0,5.92f);
	l_poRightGoalPivot->rotate(Vector3::UNIT_Y, Radian(1.570796327f));

}
void CHandballBCI::initSceneBalls()
{
	//----------- ACTIVE BALL -------------//
	SceneNode * l_poBallCommonNode = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "BallCommonNode" );
	
	SceneNode * l_poActiveBallPivot = l_poBallCommonNode->createChildSceneNode( "ActiveBallPivot" );
	SceneNode * l_poActiveBall = l_poActiveBallPivot->createChildSceneNode("ActiveBall");
	SceneNode * l_poActiveBallNode = l_poActiveBall->createChildSceneNode("ActiveBallNode");

	Entity * l_poActiveBallEntity = m_poSceneManager->createEntity( "active-ball", "active-ballNode.mesh" );
	l_poActiveBallEntity->setCastShadows(true);
	l_poActiveBallEntity->getSubEntity(0)->setMaterialName("active-ball-active-ball-material");
	l_poActiveBallNode->attachObject(l_poActiveBallEntity);

	//----------- GetReady BALL -------------//
	SceneNode * l_poGetReadyBallPivot = l_poBallCommonNode->createChildSceneNode( "GetReadyBallPivot" );
	SceneNode * l_poGetReadyBall = l_poGetReadyBallPivot->createChildSceneNode("GetReadyBall");
	SceneNode * l_poGetReadyBallNode = l_poGetReadyBall->createChildSceneNode("GetReadyBallNode");

	Entity * l_poGetReadyBallEntity = m_poSceneManager->createEntity( "get-ready-ball", "get-ready-ballNode.mesh" );
	l_poGetReadyBallEntity->setCastShadows(true);
	l_poGetReadyBallEntity->getSubEntity(0)->setMaterialName("get-ready-ball-get-ready-ball-material");
	l_poGetReadyBallNode->attachObject(l_poGetReadyBallEntity);

	//----------- Passive BALL -------------//
	SceneNode * l_poPassiveBallPivot = l_poBallCommonNode->createChildSceneNode( "PassiveBallPivot" );
	SceneNode * l_poPassiveBall = l_poPassiveBallPivot->createChildSceneNode("PassiveBall");
	SceneNode * l_poPassiveBallNode = l_poPassiveBall->createChildSceneNode("PassiveBallNode");

	Entity * l_poPassiveBallEntity = m_poSceneManager->createEntity( "passive-ball", "passive-ballNode.mesh" );
	l_poPassiveBallEntity->setCastShadows(true);
	l_poPassiveBallEntity->getSubEntity(0)->setMaterialName("passive-ball-surface01");
	l_poPassiveBallNode->attachObject(l_poPassiveBallEntity);

	l_poBallCommonNode->translate(0,l_poPassiveBallEntity->getBoundingRadius(),0);

	l_poGetReadyBallPivot->setVisible(false);
	l_poPassiveBallPivot->setVisible(true);
	l_poActiveBallPivot->setVisible(false);
}
void CHandballBCI::initSceneCrossArrowAndFeedback()
{

	//----------- CROSS -------------//
	SceneNode * l_poCrossCommonNode = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "CrossCommonNode" );
	
	SceneNode * l_poCrossNode = l_poCrossCommonNode->createChildSceneNode( "CrossNode" );
	Entity * l_poCrossEntity = m_poSceneManager->createEntity( "cross", "crossNode.mesh" );
	l_poCrossEntity->setCastShadows(false);
	l_poCrossEntity->getSubEntity(0)->setMaterialName("cross-cross-material");
	l_poCrossNode->attachObject(l_poCrossEntity);
	m_fCrossSize = l_poCrossEntity->getBoundingRadius()*2;
	l_poCrossNode->setVisible(false);

	//----------- RIGHT ARROW -------------//
	SceneNode * l_poRArrowNode = l_poCrossCommonNode->createChildSceneNode( "RightArrowNode" );
	Entity * l_poRArrowEntity = m_poSceneManager->createEntity( "rightArrow", "basic-arrowNode.mesh" );
	l_poRArrowEntity->setCastShadows(false);
	l_poRArrowEntity->getSubEntity(0)->setMaterialName("basic-arrow-basic-arrow-material");
	l_poRArrowNode->attachObject(l_poRArrowEntity);
	l_poRArrowNode->roll(Radian(Math::PI));
	l_poRArrowNode->translate(0,0,0.001f);
	l_poRArrowNode->setVisible(false);


	//----------- LEFT ARROW -------------//
	SceneNode * l_poLArrowNode = l_poCrossCommonNode->createChildSceneNode( "LeftArrowNode" );
	Entity * l_poLArrowEntity = m_poSceneManager->createEntity( "leftArrow", "basic-arrowNode.mesh" );
	l_poLArrowEntity->setCastShadows(false);
	l_poLArrowEntity->getSubEntity(0)->setMaterialName("basic-arrow-basic-arrow-material");
	l_poLArrowNode->attachObject(l_poLArrowEntity);
	l_poLArrowNode->translate(0,0,0.001f);
	l_poLArrowNode->setVisible(false);

	l_poCrossCommonNode->yaw(Radian(Math::PI/2.f));
	l_poCrossCommonNode->roll(Radian(Math::PI/2.f));
	l_poCrossCommonNode->translate(3.5,0.20,0);
	l_poCrossCommonNode->setScale(0.35,0.35,0.35);

	//----------- FEEDBACK -------------//
	SceneNode * l_poFeedbackNode = m_poSceneManager->getRootSceneNode()->createChildSceneNode( "FeedbackNode" );
	Entity * l_poFeedbackEntity = m_poSceneManager->createEntity( "feedback", "plane-cyanNode.mesh" );
	l_poFeedbackEntity->setCastShadows(false);
	l_poFeedbackEntity->getSubEntity(0)->setMaterialName("plane-cyan-plane-material");
	l_poFeedbackNode->attachObject(l_poFeedbackEntity);
	l_poFeedbackNode->roll(Radian(-Math::PI/2.f));
	l_poFeedbackNode->setScale(0.25f,0.25f,2.59f);
	l_poFeedbackNode->setPosition(3.499f,0.20f,-m_fCrossSize*l_poFeedbackNode->getScale().z/2);
	
	

	
	l_poFeedbackNode->setVisible(false);
}