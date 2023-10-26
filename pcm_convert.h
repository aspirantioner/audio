#ifndef _PCM_CONVERT_H
#define _PCM_CONVERT_H

#pragma pack(1)

#include <alsa/asoundlib.h>

const char g_chunkid[4] = {'R', 'I', 'F', 'F'};
const char g_format[4] = {'W', 'A', 'V', 'E'};
const char g_subchunk1id[4] = {'f', 'm', 't', ' '};
const unsigned int g_subchunk1size = 16;
const char g_subchunk2id[4] = {'d', 'a', 't', 'a'};

typedef enum
{
    FMT_U8,
    FMT_S8,
    FMT_S16,
    FMT_U16,
    FMT_S32,
    FMT_U32,
    FMT_FLOAT,
    FMT_64FLOAT
} PCM_FORMAT;

typedef struct
{
    unsigned char chunck_id[4];
    unsigned int chunk_size;
    unsigned char format[4];
    unsigned char sub_chunck1_id[4];
    unsigned int sub_chunck1_size;
    unsigned short audio_format;
    unsigned short num_channels;
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned short block_align;
    unsigned short bits_per_sample;
    unsigned char sub_chunck2_id[4];
    unsigned int sub_chunck2_size;
} WavHeader;

void WavHeaderSetParama(WavHeader *wav_header_p, unsigned short audio_format, unsigned short num_channels, unsigned int sample_rate, unsigned short bit_depth, unsigned int pcm_data_size);

#define SWAP(a, b)       \
    {                    \
        typeof(a) c = b; \
        b = a;           \
        a = c;           \
    }

#define PCM_PTR_CHANGE(pcm_ptr, offset1, offset2, pcm_format)                                     \
    {                                                                                             \
        switch (pcm_format)                                                                       \
        {                                                                                         \
        case FMT_S8:                                                                              \
            SWAP(*((char *)pcm_ptr + offset1), *((char *)pcm_ptr + offset2));                     \
            break;                                                                                \
        case FMT_U8:                                                                              \
            SWAP(*((unsigned char *)pcm_ptr + offset1), *((unsigned char *)pcm_ptr + offset2));   \
            break;                                                                                \
        case FMT_S16:                                                                             \
            SWAP(*((short *)pcm_ptr + offset1), *((short *)pcm_ptr + offset2));                   \
            break;                                                                                \
        case FMT_U16:                                                                             \
            SWAP(*((unsigned short *)pcm_ptr + offset1), *((unsigned short *)pcm_ptr + offset2)); \
            break;                                                                                \
        case FMT_S32:                                                                             \
            SWAP(*((int *)pcm_ptr + offset1), *((int *)pcm_ptr + offset2));                       \
            break;                                                                                \
        case FMT_U32:                                                                             \
            SWAP(*((unsigned int *)pcm_ptr + offset1), *((unsigned int *)pcm_ptr + offset2));     \
            break;                                                                                \
        case FMT_FLOAT:                                                                           \
            SWAP(*((float *)pcm_ptr + offset1), *((float *)pcm_ptr + offset2));                   \
            break;                                                                                \
        case FMT_64FLOAT:                                                                         \
            SWAP(*((double *)pcm_ptr + offset1), *((double *)pcm_ptr + offset2));                 \
            break;                                                                                \
        default:                                                                                  \
            break;                                                                                \
        }                                                                                         \
    }

#define VOLUME_CHANGE(pcm_ptr, length, times) \
    for (int i = 0; i < length; i++)          \
    {                                         \
        pcm_ptr[i] *= times;                  \
    }

#define SAMPLE_FORMAT_CAHNGE(pcm_ptr, length, new_pcm_ptr) \
    for (int i = 0; i < length; i++)                       \
    {                                                      \
        new_pcm_ptr[i] = pcm_ptr[i];                       \
    }

void PcmToNonIntervaled(void *pcm_ptr, int left, int leftnum, int right, int rightnum, PCM_FORMAT format);

#endif