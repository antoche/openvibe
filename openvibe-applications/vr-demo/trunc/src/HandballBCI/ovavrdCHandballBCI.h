#ifndef __OpenViBEApplication_CHandballBCI_H__
#define __OpenViBEApplication_CHandballBCI_H__

#include "../ovavrdCOgreVRApplication.h"

namespace OpenViBEVRDemos {

	/**
	 * \class CHandballBCI
	 * \author Laurent Bonnet (INRIA/IRISA)
	 * \date 2010-02-16
	 * \brief Ogre application for the Handball application.
	 *
	 * \details The HandballBCI application is based on motor imagery of the hands. 
	 * The 3d scene is a gymnasium, with 2 handball goals.
	 * The subject has to move his right/left hand to move a ball in the right/left goal.
	 * TODO: full documentation.
	 */
	class CHandballBCI : public COgreVRApplication
	{
		public:

			CHandballBCI();

		private:

			virtual bool initialise(void);
			virtual void initSceneGymnasium();
			virtual void initSceneAds();
			virtual void initSceneGoals();
			virtual void initSceneBalls();
			virtual void initSceneCrossArrowAndFeedback();

			//virtual bool initGUI(void);
	
			virtual bool process(double timeSinceLastProcess);	
			
			int m_iGoalScore;      // goal !
			int m_iLeftScore;      // ball successfully sent on the left side (but maybe no goal)
			int m_iRightScore;     // ball successfully sent on the left side (but maybe no goal)

			//for stats
			int m_iTrialRightCount;
			int m_iTrialLeftCount;
			
			int m_iClassificationScore;
			
			int m_iPhase;
			int m_iLastPhase;
			
			double m_dFeedback;
			double m_dLastFeedback;

			double m_dMaxFeedback;
			double m_dMinFeedback;
			std::list < double > m_vAnalogHistory;

			int m_iMark;
			int m_iLastMark;
			bool m_bGoalMarkedAtThisPhase;

			float m_fBallSpeed;
			float m_fBallPosition;
			float m_fLastBallPosition;
			float m_fBallOrientation;

			float m_fCrossSize;
			
			bool m_bShowCross;
			bool m_bShowClue;
			bool m_bShowFeedback;

			enum
			{
				Phase_GetReady,
				Phase_Active,
				Phase_Rest,
			} EPhase;

			enum
			{
				Mark_Right,
				Mark_Left,
				Mark_None,
			} EMark;
	};
};
#endif //__OpenViBEApplication_CHandballBCI_H__