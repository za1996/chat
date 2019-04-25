#include "audioplayer.h"

AudioPlayer::AudioPlayer() :
    m_device(nullptr),
    m_context(nullptr),
    m_CurrentUser(0)
{
    init();
}

AudioPlayer::~AudioPlayer()
{
    for(auto it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
    {
        clear(it);
    }
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}
//init function
bool AudioPlayer::init()
{
    m_device = alcOpenDevice(nullptr);
    if (m_device == nullptr)
    {
//        LOG_ERROR("Create device failed!\n");
        return false;
    }

    m_context = alcCreateContext(m_device, nullptr);
    if (m_context == nullptr)
    {
//        LOG_ERROR("Create context failed!\n");
        return false;
    }

    if(alcMakeContextCurrent(m_context) != ALC_TRUE)
    {
        return false;
    }

//    if (CheckALError("CGEALPlayBack::init"))
//        return false;


    return true;
}

//public fun
void AudioPlayer::play(const void *data, int dataSize, int freq, ALenum format)
{
    if(m_CurrentUser)
    {
        recycle();

        if(data && !((*it).m_bufferQueue.empty()))
        {
            ALuint buffer = (*it).m_bufferQueue.front();
            (*it).m_bufferQueue.pop_front();
            alBufferData(buffer, format, data, dataSize, freq);
            alSourceQueueBuffers((*it).m_source, 1, &buffer);
        }

        resume();
    }
}

void AudioPlayer::addUser(uint32_t id)
{
    UserAudioInfo info;
    alGenSources(1, &info.m_source);
    alGenBuffers(MAXCACHEQUEUESIZE, info.m_buffers);

    for(auto buf : info.m_buffers)
    {
        info.m_bufferQueue.push_back(buf);
    }

    m_UserMap.insert(id, info);
}

bool AudioPlayer::isPlaying()
{
    return playState() == AL_PLAYING;
}
bool AudioPlayer::setCurrentUser(uint32_t id)
{
    bool state = false;
    if(m_CurrentUser == id) return true;
    it = m_UserMap.find(id);
    if(it != m_UserMap.end())
    {
        m_CurrentUser = id;
        state = true;
    }
    else
    {
        m_CurrentUser = 0;
    }
    return state;
}

void AudioPlayer::delUser(uint32_t id)
{
    auto it = m_UserMap.find(id);
    if(it != m_UserMap.end())
    {
        clear(it);
        m_UserMap.erase(it);
    }
    if(m_CurrentUser == id)
    {
        m_CurrentUser = 0;
    }
}

void AudioPlayer::setVolume(float volume)
{
    alSourcef((*it).m_source, AL_GAIN, volume);
}

float AudioPlayer::getVolume()
{
    float volume;
    alGetSourcef((*it).m_source, AL_GAIN, &volume);
    return volume;
}

//private fun
ALint AudioPlayer::playState()
{
    ALint playState = 0;
    if(m_CurrentUser)
    {
        alGetSourcei((*it).m_source, AL_SOURCE_STATE, &playState);
        return playState;
    }
    return -1;
}

void AudioPlayer::recycle()
{
    ALint procBuffNum;
    if(m_CurrentUser)
    {
        alGetSourcei((*it).m_source, AL_BUFFERS_PROCESSED, &procBuffNum);
        if(procBuffNum > 0)
        {
            ALuint buffers[MAXCACHEQUEUESIZE];
            alSourceUnqueueBuffers((*it).m_source, procBuffNum, buffers);
            for(int i = 0; i != procBuffNum; ++i)
            {
                (*it).m_bufferQueue.push_back(buffers[i]);
            }
        }
    }
}

void AudioPlayer::resume()
{
    if(m_CurrentUser && !isPlaying())
    {
        ALint bufferQueued;
        alGetSourcei((*it).m_source, AL_BUFFERS_QUEUED, &bufferQueued);
        if(bufferQueued != 0)
        {
            alSourcePlay((*it).m_source);
        }
    }
}

void AudioPlayer::clear(const QHash<uint32_t, UserAudioInfo>::iterator &it)
{
    alDeleteSources(1, &((*it).m_source));
    alDeleteBuffers(MAXCACHEQUEUESIZE, (*it).m_buffers);
    (*it).m_bufferQueue.clear();
}
