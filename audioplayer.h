#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QHash>
extern "C"
{
#include <al.h>
#include <alc.h>
}


class AudioPlayer
{
private:
    enum { MAXCACHEQUEUESIZE = 36 };
    struct UserAudioInfo
    {
        ALuint m_source;
        ALuint m_buffers[MAXCACHEQUEUESIZE];
        std::list<ALuint> m_bufferQueue;
    };

public:
    AudioPlayer();
    ~AudioPlayer();
    void addUser(uint32_t id);
    bool isPlaying();
    bool setCurrentUser(uint32_t id);
    void delUser(uint32_t);
    void play(const void* data, int dataSize, int freq, ALenum format);
    void setVolume(float volume);
    float getVolume();


private:
    QHash<uint32_t, UserAudioInfo> m_UserMap;
    ALCcontext * m_context;
    ALCdevice* m_device;
    QHash<uint32_t, UserAudioInfo>::iterator it;
    uint32_t m_CurrentUser;

private:
    bool init();
    void recycle();
    void resume();
    ALint playState();
    void clear(const QHash<uint32_t, UserAudioInfo>::iterator &it);
};

#endif // AUDIOPLAYER_H
