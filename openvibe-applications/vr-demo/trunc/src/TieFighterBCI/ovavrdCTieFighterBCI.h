#ifndef __OpenViBEApplication_CTieFighterBCI_H__
#define __OpenViBEApplication_CTieFighterBCI_H__

#include "../ovavrdCOgreVRApplication.h"

namespace OpenViBEVRDemos {

	/**
	 * \class CTieFighterBCI
	 * \author Laurent Bonnet (INRIA/IRISA)
	 * \date 2010-02-16
	 * \brief Ogre application for the application "Use-The-Force".
	 *
	 * \details The TieFighterBCI application is based on motor imagery of the feet. 
	 * The 3D scene is a hangar, with few barrels stored. 
	 * A spaceship (tie-fighter) is parked on the center of the screen.
	 * The subject has to move his feet to lift the spaceship.
	 * The feedback is computed according to the bounce of beta 
	 * detected in the EEG, after the movement.
	 * 
	 * Each trial has 3 steps:
	 * - Rest: the subject has to stand still.
	 * - Move: the subject moves his feet.
	 * - NoMove: the subject stops.
	 * The subject scores if the spaceship is lifted in the NoMove phase.
	 */
	class CTieFighterBCI : public COgreVRApplication
	{
		public:

			/**
			* \brief constructor.
			*/
			CTieFighterBCI(std::string s_localization="eng");

		private:

			/**
			* \brief Initializes the scene, camera, lights and GUI.
			* \return \em true if the scene is successfully set up.
			*/
			virtual bool initialise(void);
			
			//----- SCENE COMPONENTS -----//
			/**
			* \brief Loads the GUI.
			*/
			void loadGUI(void);
			/**
			* \brief Loads the hangar.
			*/
			void loadHangar(void);
			/**
			* \brief Populates the hangar with fixed barrels.
			*/
			void loadHangarBarrels(void);
			
			bool m_bVador; //!< Tells if whether or not Dark Vador model should be added in the scene.
			/**
			* \brief Loads Dark Vador model.
			*/
			void loadDarkVador(void);
			/**
			* \brief Loads the movable spaceship.
			*/
			void loadTieFighter(void);
			/**
			* \brief Loads the movable barrels in front of the ship.
			*/
			void loadTieBarrels(void);
			
			/**
			* \brief Lifts the barrels and spaceship according to the feedback received from the analog server.
			*/
			virtual bool process(double timeSinceLastProcess);

			virtual void processStageFreetime(double timeSinceLastProcess);
			
			int m_iScore;                      //!<Current score.
			int m_iAttemptCount;               //!<Current attempt count.
			
			int m_iPhase;                      //!<Current phase (Rest, Move, NoMove).
			int m_iLastPhase;                  //!<Previous phase.
		
			int m_iStage;                      //!<Current stage (Baseline, FreetimeReal, FreetimeImaginary, Statistics).

			double m_dFeedback;                //!<The current feedback value received from the VRPN server.
				
			double m_dLastFeedback;            //!<Previous feedback value.
			bool m_bShouldScore;               //!<Tells if the subject is in condition of scoring.

			float m_fTieHeight;                //!<Current spaceship height in the scene.
			Ogre::Vector3 m_vTieOrientation;   //!<Current Orientation of the tie-fighter.

			double m_dMinimumFeedback;         //!<Minimum feedback value ever received.
			
			std::vector<float> m_vfSmallObjectHeight;              //!<Current mini-barrels height in the scene.
			std::vector<Ogre::Vector3> m_voSmallObjectOrientation; //!<Current Orientation of the mini-barrels.

			double m_dStat_TieFighterLiftTime;
			bool m_bShouldIncrementStat;
			
			/**
			* \brief Phases enumeration.
			*/
			enum
			{
				Phase_Rest,   //!< The subject stands still.
				Phase_Move,   //!< The subject moves his feet.
				Phase_NoMove, //!< The subject stops the movement.
			};

			/**
			* \brief Stage enumeration.
			*/
			enum
			{
				Stage_Baseline,          //!< The subject stands still.
				Stage_FreetimeReal,      //!< The subject iterates 10 times with real movements
				Stage_FreetimeImaginary, //!< The subject iterates 10 times with imaginary movements
				Stage_Statistics,        //!< Experiment is over, uninitialize() is called.
			};

			/** 
			* \brief Key pressed callback, launched when a key is pressed.
			* \return \em true if the rendering engine should continue.
			*/
			bool keyPressed(const OIS::KeyEvent& evt);

			double m_dBetaOffset;
			int m_iBetaOffsetPercentage;

			/****************
			* Localization. *
			*****************/
			std::map<std::string,std::string> m_mLocalizedFilenames;
			std::string m_sLocalization;
		
	};
};
#endif //__OpenViBEApplication_CTieFighterBCI_H__