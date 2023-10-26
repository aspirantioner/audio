#include "lio_soundcard.h"

void LioSoundCardInit(LioSoundCard *soundcard_p, int device_type, int sample_rate, unsigned long period_size, int pcm_rw_access, int pcm_format, int channel_num)
{

    /*配置设备参数*/
    soundcard_p->pcm_format = pcm_format;
    soundcard_p->pcm_rw_access = pcm_rw_access;
    soundcard_p->device_type = device_type;

    /*打开设备*/
    int retValue = snd_pcm_open(&(soundcard_p->handle), "default", device_type, 0);
    if (retValue < 0)
    {
        perror("open pcm error");
        exit(0);
    }

    /*配置相关参数*/
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(soundcard_p->handle, params);                                // 初始化设备设置参数
    if (snd_pcm_hw_params_set_access(soundcard_p->handle, params, pcm_rw_access) != 0) // 使用读写传输,数据以交错方式存储
    {
        perror("set rw access error");
        exit(0);
    };
    if (snd_pcm_hw_params_set_format(soundcard_p->handle, params, pcm_format) != 0) // 设置采样位数及大小端
    {
        perror("set format error");
        exit(0);
    };
    if (snd_pcm_hw_params_set_channels(soundcard_p->handle, params, channel_num) != 0) // 设置采集声道数
    {
        perror("set channel nums error");
        exit(0);
    };
    if (snd_pcm_hw_params_set_rate_near(soundcard_p->handle, params, &sample_rate, 0) != 0) // 设置采样率
    {
        perror("set sample rate error");
        exit(0);
    };
    if (snd_pcm_hw_params_set_period_size_near(soundcard_p->handle, params, &period_size, 0) != 0) // 调整周期值,dir>0向上调整m,反之向下调整
    {
        perror("set period size error");
        exit(0);
    };

    retValue = snd_pcm_hw_params(soundcard_p->handle, params); // 为以上设置进行配置
    if (retValue < 0)
    {
        perror("set pcm params error");
        exit(0);
    }

    /*得到音频设备信息*/
    snd_pcm_hw_params_get_period_size(params, &(soundcard_p->period_size), 0);
    snd_pcm_hw_params_get_rate(params, &(soundcard_p->sample_rate), 0);
    snd_pcm_hw_params_get_channels(params, &(soundcard_p->channel_num));
    soundcard_p->bit_depth = snd_pcm_hw_params_get_sbits(params) / 8;
    soundcard_p->read_buffer_size = soundcard_p->period_size * channel_num * soundcard_p->bit_depth; // 一次读取数据大小 == 周期大小*帧大小(通道数*位深度)
    printf("set params ok!\nsample bit depth is %d;\nchannel num is %u;\nrate size is %d;\nperiod size is %lu;\nbuff size is %lu;\n", soundcard_p->bit_depth, soundcard_p->channel_num, soundcard_p->sample_rate, soundcard_p->period_size, soundcard_p->read_buffer_size);

    soundcard_p->rw_buf.rw_buffer = malloc(soundcard_p->read_buffer_size);
    if (soundcard_p->rw_buf.rw_buffer == NULL)
    {
        perror("malloc error");
        exit(0);
    }

    if (soundcard_p->pcm_rw_access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
        soundcard_p->rw_buf.rw_buffer_array = malloc(sizeof(void *) * soundcard_p->channel_num);
        if (soundcard_p->rw_buf.rw_buffer_array == NULL)
        {
            perror("malloc error");
            exit(0);
        }
        for (int i = 0; i < soundcard_p->channel_num; i++)
        {
            soundcard_p->rw_buf.rw_buffer_array[i] = soundcard_p->rw_buf.rw_buffer + i * soundcard_p->period_size * soundcard_p->bit_depth;
        }
    }
    return;
}

void LioSoundCardFetchFrame(LioSoundCard *soundcard_p)
{

    /*取出音频数据*/
    static int retValue;
    if (soundcard_p->pcm_rw_access == SND_PCM_ACCESS_RW_INTERLEAVED)
    {
        retValue = snd_pcm_readi(soundcard_p->handle, soundcard_p->rw_buf.rw_buffer, soundcard_p->period_size);
    }
    else
    {
        retValue = snd_pcm_readn(soundcard_p->handle, soundcard_p->rw_buf.rw_buffer_array, soundcard_p->period_size);
    }
    if (retValue == -EPIPE)
    {
        perror("overrun occured!\n");
        snd_pcm_prepare(soundcard_p->handle);
    }

    else if (retValue < 0)
    {
        perror("read error");
        exit(0);
    }

    else if (retValue != soundcard_p->period_size)
    {
        perror("read less than frames");
        exit(0);
    }

    return;
}

void LioSoundCardPlayFrame(LioSoundCard *soundcard_p)
{
    // 写入音频数据到PCM设备
    static int retValue;
    if (soundcard_p->pcm_rw_access == SND_PCM_ACCESS_RW_INTERLEAVED)
    {
        retValue = snd_pcm_writei(soundcard_p->handle, soundcard_p->rw_buf.rw_buffer, soundcard_p->period_size);
    }
    else
    {
        retValue = snd_pcm_writen(soundcard_p->handle, soundcard_p->rw_buf.rw_buffer_array, soundcard_p->period_size);
    }
    if (retValue == -EPIPE)
    {
        // 溢出错误，重新恢复PCM设备
        perror("overrun");
        snd_pcm_prepare(soundcard_p->handle);
    }
    else if (retValue < 0)
    {
        printf("无法写入PCM设备: %s\n", snd_strerror(retValue));
        exit(0);
    }
}

void LioSoundCardClose(LioSoundCard *soundcard_p)
{
    snd_pcm_close(soundcard_p->handle);
    free(soundcard_p->rw_buf.rw_buffer);
    if (soundcard_p->pcm_rw_access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
        free(soundcard_p->rw_buf.rw_buffer_array);
    }
    return;
}

// int main(void)
// {
//     LioSoundCard lio_card;
//     LioSoundCardInit(&lio_card, SND_PCM_STREAM_CAPTURE, 44100, 1024, SND_PCM_ACCESS_RW_INTERLEAVED, SND_PCM_FORMAT_S16_LE, 2);
//     FILE *fp = fopen("test.pcm", "wb");
//     while (1)
//     {
//         LioSoundCardFetchFrame(&lio_card);
//         // for (int i = 0; i < 2; i++)
//         // {
//         //     fwrite(lio_card.rw_buf.rw_buffer_array[i], lio_card.read_buffer_size / 2, 1, fp);
//         // }
//         fwrite(lio_card.rw_buf.rw_buffer, lio_card.read_buffer_size, 1, fp);
//         // LioSoundCardPlayFrame(&lio_card);
//     }
//     // snd_pcm_drain(lio_card.handle);
//     LioSoundCardClose(&lio_card);
//     return 0;
// }