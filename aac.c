#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fdk-aac/aacenc_lib.h>
#include <string.h>
#include <errno.h>

#define OUTPUT_BIT_RATE 128 * 1024
#define OUTPUT_CHANNELS 2
#define OUTPUT_SAMPLE_RATE 44100
#define INPUT_FILE "../lio_ffmpeg/audio.pcm"
#define OUTPUT_FILE "../lio_ffmpeg/audio.aac"

int main()
{
    // 创建编码器句柄
    HANDLE_AACENCODER encoderHandle;
    if (aacEncOpen(&encoderHandle, 0x01, 0x00) != AACENC_OK)
    {
        fprintf(stderr, "Failed to open AAC encoder\n");
        return -1;
    }

    // 设置编码器参数
    if (aacEncoder_SetParam(encoderHandle, AACENC_AOT, AOT_AAC_LC) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_SBR_MODE, AOT_NULL_OBJECT) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_SAMPLERATE, OUTPUT_SAMPLE_RATE) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_BITRATE, OUTPUT_BIT_RATE) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_BITRATEMODE, 0) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_CHANNELMODE, MODE_2) != AACENC_OK ||
        aacEncoder_SetParam(encoderHandle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK)
    {
        fprintf(stderr, "Failed to set AAC encoder parameters\n");
        aacEncClose(&encoderHandle);
        return -1;
    }

    // 初始化编码器实例
    if (aacEncEncode(encoderHandle, NULL, NULL, NULL, NULL) != AACENC_OK)
    {
        printf("aacEncEncode test ERROR from aacEncInit\n");
        return -1;
    }

    // 打开输入文件
    FILE *inputFile = fopen(INPUT_FILE, "rb");
    if (!inputFile)
    {
        fprintf(stderr, "Failed to open input file\n");
        aacEncClose(&encoderHandle);
        return -1;
    }

    // 打开输出文件
    FILE *outputFile = fopen(OUTPUT_FILE, "wb");
    if (!outputFile)
    {
        fprintf(stderr, "Failed to open output file\n");
        aacEncClose(&encoderHandle);
        return -1;
    }

    // 获取一次编码需要的帧长度
    UINT frame_size = aacEncoder_GetParam(encoderHandle, AACENC_GRANULE_LENGTH);

    // 创建输入PCM数据缓冲区
    INT_PCM inputBuffer[frame_size * OUTPUT_CHANNELS];

    // 创建输出AAC数据缓冲区
    UINT outputBuffer[frame_size * OUTPUT_CHANNELS * sizeof(INT_PCM)];

    // 编码输入输出参数设置
    AACENC_BufDesc inputBufDesc;
    AACENC_BufDesc outputBufDesc;
    AACENC_InArgs inputArgs;
    AACENC_OutArgs outputArgs;

    void *inBuffer[] = {inputBuffer};           // 各种输入数据的存放区
    INT inBufferIds[] = {IN_AUDIO_DATA};        // 不包含辅助数据
    INT inBufferSize[] = {sizeof(inputBuffer)}; // 元素总数
    INT inBufferElSize[] = {sizeof(INT_PCM)};   // 单个元素数据大小

    inputBufDesc.numBufs = sizeof(inBuffer) / sizeof(void *);
    inputBufDesc.bufs = (void **)&inBuffer;
    inputBufDesc.bufferIdentifiers = inBufferIds;
    inputBufDesc.bufSizes = inBufferSize;
    inputBufDesc.bufElSizes = inBufferElSize;

    void *outBuffer[] = {outputBuffer};
    INT outBufferIds[] = {OUT_BITSTREAM_DATA};
    INT outBufferSize[] = {sizeof(outputBuffer)};
    INT outBufferElSize[] = {sizeof(UINT)};

    outputBufDesc.numBufs = sizeof(outBuffer) / sizeof(void *); // 数据类型数量（此处仅包含代编码数据不含辅助数据等类型，故设置为1）
    outputBufDesc.bufs = outBuffer;
    outputBufDesc.bufferIdentifiers = outBufferIds;
    outputBufDesc.bufSizes = outBufferSize;
    outputBufDesc.bufElSizes = outBufferElSize;

    inputArgs.numAncBytes = 0;                             // 辅助数据字节数
    inputArgs.numInSamples = frame_size * OUTPUT_CHANNELS; // 编码输入字节总数

    // 编码循环
    while (fread(inputBuffer, sizeof(INT_PCM), frame_size * OUTPUT_CHANNELS, inputFile))
    {
        int err = aacEncEncode(encoderHandle, &inputBufDesc, &outputBufDesc, &inputArgs, &outputArgs);
        if (err != AACENC_OK)
        {
            perror("Failed to encode PCM frame\n");
            break;
        }

        // 写入输出文件
        fwrite(outputBuffer, 1, outputArgs.numOutBytes, outputFile);
    }

    // 关闭输出文件
    fclose(outputFile);
    fclose(inputFile);
    // 关闭编码器
    aacEncClose(&encoderHandle);

    return 0;
}