#ifndef AUDIOCONVERTERINTERNAL_H
#define AUDIOCONVERTERINTERNAL_H
#include "AudioConverter.h"
#include <stdint.h>

extern "C" {
#include <libavresample/avresample.h>
#include <libavcodec/avcodec.h>
}

class AudioConverter
{
	AudioConverter(const AudioStreamBasicDescription* inSourceFormat, const AudioStreamBasicDescription* inDestinationFormat);
public:
	static OSStatus create(const AudioStreamBasicDescription* inSourceFormat, const AudioStreamBasicDescription* inDestinationFormat, AudioConverter** out);
	~AudioConverter();

	void flush();
	UInt32 frameSize() const { return m_sourceFormat.mBytesPerFrame; }
	
	OSStatus fillComplex(AudioConverterComplexInputDataProc dataProc, void* opaque, UInt32* ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription* outPacketDescription);
	
	OSStatus setProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData);
	OSStatus getProperty(AudioConverterPropertyID inPropertyID, UInt32 *ioPropertyDataSize, void *outPropertyData);
private:
	static enum AVCodecID CACodecToAV(const AudioStreamBasicDescription* desc);
	static enum AVSampleFormat CACodecSampleFormat(const AudioStreamBasicDescription* desc);
	static uint32_t CAChannelCountToLayout(UInt32 numChannels);
	
	// Requests more data from the input callback
	OSStatus feedInput(AudioConverterComplexInputDataProc dataProc, void* opaque, UInt32& numDataPackets);
	
	// Setups the resampler context based on the PCM format in the frame
	void setupResampler(const AVFrame* frame);
	
	void allocateBuffers();
	void initEncoder();
private:
	AudioStreamBasicDescription m_sourceFormat, m_destinationFormat;
	UInt32 m_inputChannelLayout, m_outputChannelLayout;
	AVCodecContext* m_decoder;
	AVCodecContext* m_encoder;
	AVPacket m_avpkt;
	AVFrame* m_audioFrame;
	AVAudioResampleContext* m_resampler = nullptr;
	UInt32 m_outBitRate = 128000;
	bool m_encoderInitialized = false;
	AVCodec* m_codecIn = nullptr;
	AVCodec* m_codecOut = nullptr;
};

#endif


