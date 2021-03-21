
#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <rtaudio/RtAudio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

#define OSC_SINE 0
#define OSC_SQUARE 1
#define OSC_TRIANGLE 2
#define OSC_SAW 3
#define OSC_NOISE 4

static double(*waveFunction)(double, int) = NULL;
static int waveType = OSC_TRIANGLE;

typedef struct {
    unsigned int sampleRate;
    unsigned int bufferFrames;
    unsigned int channels;
    double nextTime;
}CallbackData;

class SoundPlayer
{
    public:
        RtAudio audio;
        CallbackData data;
        
        unsigned int devId;
        unsigned int bufFrames;
        unsigned int sRate;
        unsigned int chn;

        SoundPlayer(int deviceId = 0, unsigned int channels = 2, unsigned int sampleRate = 0,
                    unsigned int bufferFrames = 256)
        {
            // audio = RtAudio();
            audio.showWarnings(true);
            if (audio.getDeviceCount() < 1) {
                std::cout << "No audio device detected!" << std::endl;
                exit(0);
            }

            if (deviceId == -1) {
                devId = audio.getDefaultOutputDevice();
            }
            else { devId = deviceId; }

            RtAudio::DeviceInfo devInfo = audio.getDeviceInfo(devId);
            if (channels > devInfo.outputChannels) {
                chn = 1;
            }
            else { chn = channels; }
            
            std::vector<unsigned int> sampleRates = devInfo.sampleRates;
            if (std::find(sampleRates.begin(), sampleRates.end(), sampleRate) == sampleRates.end()) {
                sRate = devInfo.preferredSampleRate;
            }
            else { sRate = sampleRate;}

            bufFrames = bufferFrames;
        }

        ~SoundPlayer()
        {
            stopStream();
        }

 


        std::vector<std::string> getSoundDevs()
        {
            unsigned int deviceCount = audio.getDeviceCount();
            std::vector<std::string> devices;
            for (size_t n=0; n<deviceCount; n++){
                if (audio.getDeviceInfo(n).probed){
                    devices.push_back("ID: " + std::to_string(n) + " Name: " + audio.getDeviceInfo(n).name);
                }
            }
            return devices;
        }

        void startStream() 
        {
            RtAudio::StreamParameters params;
            params.deviceId = devId;
            params.nChannels = chn;
            params.firstChannel = 0;


            try {
                audio.openStream( &params, NULL, RTAUDIO_FLOAT64, 
                                    sRate, &bufFrames, callbackFunct, &data);
            data = {
                sRate,  // sample Rate
                bufFrames,  // buffer Frames
                chn,        // channels
                0.0   // next time Valuess
            };

                audio.startStream();
            }
            catch (RtAudioError& e) {
                e.printMessage();
                exit(0);
            }            
        }
        void stopStream() 
        {
            std::cout << "classs channels " << chn <<std::endl;
            if (audio.isStreamRunning()) {
                try{
                    audio.stopStream();
                }
                catch(RtAudioError& e) {
                    e.printMessage();
                }                
            }

            if (audio.isStreamOpen()){
                audio.closeStream();
            }          
        }

        /* Set a wave function that will be used in the audio callback
        *
        * @param *func Function pointer that takes a time double value 
        *   and returns a double value */
        void setWaveFunction(double(*func)(double, int)) 
        {
            waveFunction = func;
        }

        void setWaveType(int wave)
        {
            waveType = wave;
        }
        int getWaveType()
        {
            return waveType;
        }

        double getStreamTime()
        {
            return data.nextTime;
        }

        static int callbackFunct( void *outputBuffer, void */*inputBuffer*/, unsigned int nBufferFrames,
                double /*streamTime*/, RtAudioStreamStatus status, void *userdata)
        {
            unsigned int i, j;
            CallbackData *data = (CallbackData*) userdata;;
            unsigned int channels = data->channels;
            double *buffer = (double *) outputBuffer;
            double timestep = 1.0 / data->sampleRate;
            double bufferStartTime = data->nextTime;
            data->nextTime +=  nBufferFrames * timestep; // save next time (=x) value, buffer has y values!!

            if ( status ) {
                std::cout << "Stream underflow detected!" << std::endl;
            }

            for ( i=0; i<nBufferFrames; i++ ) {
                for( j=0; j<channels; j++) {
                    *buffer++ = waveFunction(bufferStartTime + (i * timestep), waveType);
                }
            }
            
            //frameCounter += nBufferFrames;
            //if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
            return 0;
        }

};

#endif