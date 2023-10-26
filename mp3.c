#include <stdio.h>
#include <stdlib.h>
#include <lame/lame.h>

int main(int argc, char *argv[])
{
    // 输入和输出文件名
    const char *inputFile = "test.pcm";
    const char *outputFile = "output.mp3";

    // 打开输入文件
    FILE *wavFile = fopen(inputFile, "rb");
    if (!wavFile)
    {
        printf("无法打开输入文件\n");
        return 1;
    }

    // 创建输出文件
    FILE *mp3File = fopen(outputFile, "wb");
    if (!mp3File)
    {
        printf("无法创建输出文件\n");
        return 1;
    }

    // 初始化LAME库编码器
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, 44100);  // 输入采样率
    lame_set_out_samplerate(lame, 44100); // 输出采样率
    lame_set_num_channels(lame, 2);       // 声道数
    lame_set_brate(lame, 128);            // 比特率（以kbps为单位）

    if (lame_init_params(lame) == -1)
    {
        perror("lame set error");
        exit(0);
    };

    // 初始化LAME库
    if (lame_init_params(lame) < 0)
    {
        printf("无法初始化LAME库\n");
        return 1;
    }

    // 缓冲区大小
    const int PCM_SIZE = 1024;

    const int MP3_SIZE = 2 * sizeof(short) * PCM_SIZE > 7200 ? 2 * sizeof(int) * PCM_SIZE : 7200;

    // 分配缓冲区
    short pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];

    // 读取和编码音频数据
    int read, write;
    do
    {
        read = fread(pcm_buffer, 2 * sizeof(short), PCM_SIZE, wavFile);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite(mp3_buffer, write, 1, mp3File);
    } while (read != 0);

    // 清理和关闭文件
    lame_close(lame);
    fclose(wavFile);
    fclose(mp3File);

    printf("音频编码完成\n");

    return 0;
}
