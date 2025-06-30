#include "AudioSystem.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(int numChannels)
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("Mix OpenAudio Failed! %s", Mix_GetError());
	}

    Mix_AllocateChannels(numChannels);
	mChannels.resize(numChannels, SoundHandle::Invalid);
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
    for (auto& [str, sound] : mSounds) {
		Mix_FreeChunk(sound);
	}
	mSounds.clear();
	Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
    for (size_t i = 0; i < mChannels.size(); ++i) {
		if (!Mix_Playing(static_cast<int>(i))) {
			// canal livre — remove do mapa e reseta
			SoundHandle sh = mChannels[i];
			if (sh != SoundHandle::Invalid) {
				mHandleMap.erase(sh);
				mChannels[i] = SoundHandle::Invalid;
			}
		}
	}

}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping)
{
    // Get the sound with the given name
    Mix_Chunk *sound = GetSound(soundName);
	if (!sound) return SDL_Log("Invalid Sound!"), SoundHandle::Invalid;

    int availableChannel = -1;
	for (int i = 0; i < static_cast<int>(mChannels.size()); ++i) {
		if (!mChannels[i].IsValid() || !Mix_Playing(i)) {
			availableChannel = i;
			break;
		}
	}
	
	if (availableChannel == -1) {
		for (auto& [handle, info] : mHandleMap) {
			if (info.mSoundName == soundName) {
				availableChannel = info.mChannel;
				StopSound(handle);
				break;
			}
		}
	}

    if (availableChannel == -1) {
		for (auto& [handle, info] : mHandleMap) {
			if (info.mIsLooping) {
				availableChannel = info.mChannel;
				StopSound(handle);
				break;
			}
		}
	}

    if (availableChannel == -1 && !mHandleMap.empty()) {
		auto oldest = mHandleMap.begin();
		availableChannel = oldest->second.mChannel;
		StopSound(oldest->first);
	}

	if (availableChannel == -1) {
		SDL_Log("No available channels for sound: %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

    SoundHandle newHandle = ++mLastHandle;

    HandleInfo newInfo = {
		.mSoundName = soundName,
		.mChannel = availableChannel,
		.mIsLooping = looping,
		.mIsPaused = false
	};
	mHandleMap[newHandle] = newInfo;
	mChannels[availableChannel] = newHandle;
    int loops = looping ? -1 : 0;
	if (Mix_PlayChannel(availableChannel, sound, loops) == -1) {
		SDL_Log("Failed to play sound: %s", Mix_GetError());
		mHandleMap.erase(newHandle);
		mChannels[availableChannel] = SoundHandle::Invalid;
		return SoundHandle::Invalid;
	}

    return mLastHandle;
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(SoundHandle sound)
{
    auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end()) {
		SDL_Log("The sound not exists in the map!");
		return;
	}
	Mix_HaltChannel(it->second.mChannel);
	mHandleMap.erase(it);
	mChannels[it->second.mChannel].Reset();
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(SoundHandle sound)
{
    auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end()) {
		SDL_Log("Error when try to pause the sound!");
		return;
	}
	if (not it->second.mIsPaused) {
		Mix_Pause(it->second.mChannel);
		it->second.mIsPaused = true;
	}
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(SoundHandle sound)
{
    auto it = mHandleMap.find(sound);
	if (it == mHandleMap.end()) {
		SDL_Log("Error when try to resume the sound!");
		return;
	}
	if (it->second.mIsPaused) {
		Mix_Resume(it->second.mChannel);
		it->second.mIsPaused = false;
	}

}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        return SoundState::Stopped;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        return SoundState::Paused;
    }

	return SoundState::Playing;
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
    Mix_HaltChannel(-1);

    for(auto & mChannel : mChannels)
    {
        mChannel.Reset();
    }

    mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
	{
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
	std::string fileName = "../Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i].IsValid())
			{
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end())
				{
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
							hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
							mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}
