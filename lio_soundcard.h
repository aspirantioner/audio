#ifndef _LIO_SOUNDCARD
#define _LIO_SOUNDCARD

#include <alsa/asoundlib.h>

typedef union
{
    void *rw_buffer;
    void **rw_buffer_array;
} rw_buffer;
typedef struct
{
    snd_pcm_t *handle;
    int sample_rate;                // 采样率
    unsigned long period_size;      // 周期大小(帧数大小)
    int pcm_rw_access;              // 样本存放方式
    int pcm_format;                 // 样本格式
    unsigned int channel_num;       // 通道数
    unsigned long read_buffer_size; // 内存大小
    rw_buffer rw_buf;
    int bit_depth;   // 位深度
    int device_type; // 设备类型
} LioSoundCard;

/*配置音频设备*/
void LioSoundCardInit(LioSoundCard *soundcard_p, int device_type, int sample_rate, unsigned long period_size, int pcm_rw_access, int pcm_format, int channel_num);

/*从设备中取出一个周期数据*/
void LioSoundCardFetchFrame(LioSoundCard *soundcard_p);

/*写入一周期带播放数据*/
void LioSoundCardPlayFrame(LioSoundCard *soundcard_p);

/*关闭音频设备*/
void LioSoundCardClose(LioSoundCard *soundcard_p);

#endif