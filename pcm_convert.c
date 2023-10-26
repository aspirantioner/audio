#include "pcm_convert.h"

void WavHeaderSetParama(WavHeader *wav_header_p, unsigned short audio_format, unsigned short num_channels, unsigned int sample_rate, unsigned short bit_depth, unsigned int pcm_data_size)
{
    strncpy(wav_header_p->chunck_id, g_chunkid, 4);
    strncpy(wav_header_p->format, g_format, 4);
    strncpy(wav_header_p->sub_chunck1_id, g_subchunk1id, 4);
    wav_header_p->sub_chunck1_size = g_subchunk1size;
    strncpy(wav_header_p->sub_chunck2_id, g_subchunk2id, 4);
    wav_header_p->chunk_size = pcm_data_size + 40;
    wav_header_p->audio_format = audio_format;
    wav_header_p->num_channels = num_channels;
    wav_header_p->sample_rate = sample_rate;
    wav_header_p->byte_rate = bit_depth * num_channels * sample_rate;
    wav_header_p->block_align = num_channels * bit_depth;
    wav_header_p->bits_per_sample = bit_depth * 8;
    wav_header_p->sub_chunck2_size = pcm_data_size;
}

void PcmToNonIntervaled(void *pcm_ptr, int left, int leftnum, int right, int rightnum, PCM_FORMAT format)
{
    double tmp;
    if (leftnum > 2)
    {
        int len = left % 2 == 0 ? leftnum - leftnum / 2 : leftnum / 2;
        PcmToNonIntervaled(pcm_ptr, left, len, left + len, leftnum - len, format);
        // for (int i = 0; i < leftnum; i++)
        // {
        //     printf("%d\t", pcm_ptr[left + i]);
        // }
        // printf("\n");
    }
    if (rightnum > 2)
    {
        int len = right % 2 == 0 ? rightnum - rightnum / 2 : rightnum / 2;
        PcmToNonIntervaled(pcm_ptr, right, len, right + len, rightnum - len, format);
        // for (int i = 0; i < rightnum; i++)
        // {
        //     printf("%d\t", pcm_ptr[right + i]);
        // }
        // printf("\n");
    }
    if (leftnum == 2)
    {
        if (left % 2 == 1)
        {

            PCM_PTR_CHANGE(pcm_ptr, 0, 1, format);
        }
    }
    if (rightnum == 2)
    {
        if (right % 2 == 1)
        {
            PCM_PTR_CHANGE(pcm_ptr, 0, 1, format);
        }
    }
    // printf("fenzhi qian!\n");
    // for (int i = 0; i < leftnum; i++)
    // {
    //     printf("%d\t", pcm_ptr[left + i]);
    // }
    // printf("\n");
    // for (int i = 0; i < rightnum; i++)
    // {
    //     printf("%d\t", pcm_ptr[right + i]);
    // }
    // printf("\n");
    int sort_num = leftnum > rightnum ? leftnum / 2 : rightnum / 2;
    if (leftnum == rightnum && left % 2 == 1 && leftnum % 2 == 1)
    {
        sort_num++;
    }
    for (int i = 0; i < sort_num; i++)
    {
        PCM_PTR_CHANGE(pcm_ptr, right + i, leftnum - sort_num + i + left, format);
    }
    // for (int i = 0; i < leftnum + rightnum; i++)
    // {
    //     printf("%d\t", pcm_ptr[left + i]);
    // }
    // printf("\n");
}

int main(void)
{

    FILE *out_fp = fopen("test_new.pcm", "wb");
    FILE *in_fp = fopen("test.pcm", "rb");
    // fseek(pcm_fp, 0, SEEK_END);
    // int file_length = ftell(pcm_fp);
    // WavHeader wav_header;
    // WavHeaderSetParama(&wav_header, 1, 2, 44100, 2, file_length);
    // fwrite(&wav_header, sizeof(WavHeader), 1, wav_fp);
    float buff[2 * 1024];
    // fseek(pcm_fp, 0, SEEK_SET);
    while (fread(buff, sizeof(float), 2048, in_fp))
    {
        PcmToNonIntervaled(buff, 0, 1024, 1024, 1024, FMT_FLOAT);
        fwrite(buff, sizeof(float), 2048, out_fp);
    };
    // fclose(wav_fp);
    // fclose(pcm_fp);
    return 0;
}