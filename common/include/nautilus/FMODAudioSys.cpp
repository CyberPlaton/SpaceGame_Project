#include"FMODAudioSys.h"

namespace nautilus {

	namespace audio {

		FMODAudioSystem* FMODAudioSystem::g_pAudioSystem = 0;


		bool FMODAudioSystem::startUp() {
			FMOD_RESULT r = FMOD::System_Create(&pFMODSystem);
			if (r != FMOD_OK) return false;
			pFMODSystem->init(1, FMOD_DEFAULT, NULL);
			return true;
		}


		void FMODAudioSystem::shutDown() {
			// Releasing resources
			for (auto it : m_mSoundMap) {
				it.second->release();
			}

			pFMODSystem->release();
			pFMODSystem = 0;
		};

		FMOD::Channel* FMODAudioSystem::_getChannel(FMOD::Sound& associatedSound) {

			return(m_mChannelMap.at(&associatedSound));
		}


		FMOD::Channel* FMODAudioSystem::_getChannel(std::string soundName) {

			return(m_mChannelMap.at(m_mSoundMap.at(soundName)));

		}


		bool FMODAudioSystem::_makeChannelGroup(std::string name) {

			FMOD::ChannelGroup* g = 0;
			FMOD_RESULT r = pFMODSystem->createChannelGroup(name.c_str(), &g);
			if (r == FMOD_OK) {
				m_mChannelGroupMap[name] = g;
				return true;
			}
			else {
				return false;
			}
		}


		bool FMODAudioSystem::_addToChannelGroup(FMOD::Channel& channel, std::string groupName) {

			FMOD_RESULT r = channel.setChannelGroup(m_mChannelGroupMap.at(groupName));
			if (r == FMOD_OK) {
				return true;
			}
			else {
				return false;
			}
		}


		void FMODAudioSystem::_removeChannelGroup(std::string groupName) {

			m_mChannelGroupMap.erase(groupName);

		}


		void FMODAudioSystem::_removeChannelFromGroup(std::string groupName, FMOD::Channel& channel) {

			// Bit harder function -> research on FMOD documentation needed.

		}



		bool FMODAudioSystem::_createReverb3D(FMOD_VECTOR& positionVec, float minDist, float maxDist, FMOD_REVERB_PROPERTIES& props, std::string reverbName) {

			FMOD::Reverb3D* reverb = 0;
			FMOD_RESULT r = pFMODSystem->createReverb3D(&reverb);
			if (r == FMOD_OK) {

				if (m_sReverbInstances > 4) return false;
				pFMODSystem->setReverbProperties(m_sReverbInstances++, &props);

				reverb->set3DAttributes(&positionVec, minDist, maxDist);

				m_mReverbMap[reverbName] = reverb;
				return true;
			}
			else {
				return false;
			}
		}


		void FMODAudioSystem::_activateReverb(std::string name) {

			m_mReverbMap[name]->setActive(true);
		}


		void FMODAudioSystem::_deactivateReverb(std::string name) {

			m_mReverbMap[name]->setActive(false);
		}


		FMOD_REVERB_PROPERTIES* FMODAudioSystem::_combineFMOD_REVERB_PROPERTIES(FMOD_REVERB_PROPERTIES& X, FMOD_REVERB_PROPERTIES& Y) {

			FMOD_REVERB_PROPERTIES XY = {
			(X.DecayTime + Y.DecayTime) / 2,   (X.EarlyDelay + Y.EarlyDelay) / 2,
			(X.LateDelay + Y.LateDelay) / 2,   (X.HFReference + Y.HFReference) / 2,
			(X.HFDecayRatio + Y.HFDecayRatio) / 2,   (X.Diffusion + Y.Diffusion) / 2,
			(X.Density + Y.Density) / 2,   (X.LowShelfFrequency + Y.LowShelfFrequency) / 2,
			(X.LowShelfGain + Y.LowShelfGain) / 2,   (X.HighCut + Y.HighCut) / 2,
			(X.EarlyLateMix + Y.EarlyLateMix) / 2,   (X.WetLevel + Y.WetLevel) / 2
			};
			return(&XY);
		}


		bool FMODAudioSystem::_createDSP(FMOD_DSP_TYPE type, FMOD::DSP* dsp, FMOD::Channel& channel) {

			FMOD_RESULT r = pFMODSystem->createDSPByType(type, &dsp);
			if (r == FMOD_OK) {
				channel.addDSP(0, dsp);
				m_mDSPMap[&channel] = dsp;

				return true;
			}
			else {
				return false;
			}
		}

		void FMODAudioSystem::_removeDSP(FMOD::Channel& channel) {

			m_mDSPMap[&channel] = 0;
		}


		FMOD::Sound* FMODAudioSystem::_loadSound(std::string soundName, unsigned int mode) {

			FMOD::Sound* s = 0;
			FMOD_RESULT r = pFMODSystem->createSound(soundName.c_str(), mode, 0, &s);
			if (r == FMOD_OK) {
				// Store sound with path in SoundMap
				m_mSoundMap.insert(std::make_pair(soundName, s));
				return s;
			}
			else {
				return 0;
			}
		}


		FMOD::Sound* FMODAudioSystem::_streamSound(std::string soundName, unsigned int mode) {

			// Make sure we dont append the same file twice
			SoundMap::iterator it = m_mSoundMap.find(soundName);

			if (it != m_mSoundMap.end()) {
				_playSound(soundName);
				return 0;
			}


			FMOD::Sound* s;
			FMOD_RESULT r = pFMODSystem->createStream(soundName.c_str(), mode, 0, &s);
			if (r == FMOD_OK) {

				// Store sound with path in SoundMap
				m_mSoundMap.insert(std::make_pair(soundName, s));
				return s;
			}
			else {
				return 0;
			}
		}

		FMOD::Channel* FMODAudioSystem::_playSound(FMOD::Sound& sound) {

			FMOD::Channel* c = 0;
			FMOD_RESULT r = pFMODSystem->playSound(&sound, NULL, false, &c);
			if (r == FMOD_OK) {


				// Make sure we dont append the same file twice
				ChannelMap::iterator it = m_mChannelMap.find(&sound);
				if (it != m_mChannelMap.end()) {
					return 0;
				}


				// Save sound to a specific channel
				m_mChannelMap.insert(std::make_pair(&sound, c));
				return c;
			}
			else {
				return 0;
			}
		}


		FMOD::Channel* FMODAudioSystem::_playSound(std::string soundName) {

			SoundMap::iterator it = m_mSoundMap.find(soundName);

			if (it == m_mSoundMap.end()) return 0;


			FMOD::Channel* c = 0;

			FMOD_RESULT r = pFMODSystem->playSound(it->second, NULL, false, &c);
			if (r == FMOD_OK) {

				// Save sound to a specific channel
				m_mChannelMap.insert(std::make_pair(it->second, c));
				return c;
			}
			else {
				return 0;
			}
		}



		FMOD_RESULT FMODAudioSystem::_pauseSound(FMOD::Channel* channel) {

			return(channel->setPaused(true));
		}


		FMOD_RESULT FMODAudioSystem::_pauseSound(int channelID) {

			FMOD::Channel* c = 0;
			FMOD_RESULT r = pFMODSystem->getChannel(channelID, &c);
			if (r != FMOD_OK) return r;
			c->setPaused(true);
		}


		FMOD_RESULT FMODAudioSystem::_setVolume(FMOD::Channel* channel, float vol) {

			return(channel->setVolume(vol));
		}
	}
}