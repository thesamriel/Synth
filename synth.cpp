//#include "rtaudio/RtAudio.h"
#include <X11/Xlib.h>
//#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
//#include <thread>

#include "SoundPlayer.h"

#define KEY_LEFT 75
#define KEY_RIGHT 77

Display *display;
Window window;
XEvent report;


struct EnvelopeADSR
{
    double attackTime;
    double decayTime;
    double startAmplitude;
    double sustainAmplitude;
    double releaseTime;
    double onTime;
    double offTime;
    bool noteOn;

    EnvelopeADSR()
    {
        attackTime = 0.01;
        decayTime = 0.01;
        startAmplitude = 1.0;
        sustainAmplitude = 0.9;
        releaseTime = 0.20;
        noteOn = false;
        onTime = 0.0;
        offTime = 0.0;
    }

    void noteStart(double timeOn){
        onTime = timeOn;
        noteOn = true;
    }

    void noteEnd(double timeOff){
        offTime = timeOff;
        noteOn = false;
    }

    double getAmplitude(double time)
    {
        double amplitude = 0.0;
        double lifeTime = time - onTime;
        
        if(noteOn) {
            if (lifeTime <= attackTime) {
                amplitude = lifeTime * (startAmplitude/attackTime);
            }
            else if (lifeTime > attackTime && lifeTime <= attackTime + decayTime) {
                amplitude = (lifeTime - attackTime) * ((sustainAmplitude-startAmplitude)/decayTime) + startAmplitude;
            }
            else {
               amplitude = sustainAmplitude;  
            }
            
        }
        else {
            if (offTime != 0.0) {
            amplitude = (time - offTime) * (-sustainAmplitude/releaseTime) + sustainAmplitude;
            }
        }

        if (amplitude <= 0.0001) {amplitude =0.0;}
        return amplitude;
    }
};

std::vector<EnvelopeADSR> frequs (16, EnvelopeADSR());

void checkKeys(SoundPlayer *soundMaker)
{
    unsigned short retriggered = 0;
    XEvent nextEv;
    std::string notes = "zsxcfvgbnjmk,l./"; 
    //char notes[] = {'z','s','x','c','f','v','g','b','n','j','m','k',',','l','.','/'};
    //char *notes = "zsxcfvgbnjmk,l./";
    while (1) {
        XNextEvent(display, &report);
        //std::cout << "notes0 after event poll " << (sizeof(notes)/sizeof(*notes)) << std::endl;
        switch (report.type) {
            case KeyPress:
                std::cout << "key #" << (char) (XLookupKeysym(&report.xkey, 0))<< " was pressed.\n";
                for (int k=0; k<16; k++) {
                    if (notes[k] == (char) XLookupKeysym(&report.xkey, 0)) {
                        frequs[k].noteOn = true;
                        frequs[k].noteStart(soundMaker->getStreamTime());
                        std::cout << "note on time: " << frequs[k].onTime << std::endl;
                    }
                }
                if ((char)XLookupKeysym(&report.xkey, 0) == 'S') {
                    soundMaker->setWaveType(abs((soundMaker->getWaveType() + 1) % 5));
                    std::cout << soundMaker->getWaveType() << std::endl;
                }
                if ((char)XLookupKeysym(&report.xkey, 0) == 'Q') {
                    soundMaker->setWaveType(abs((soundMaker->getWaveType() - 1) % 5));
                }                
                if ((char)XLookupKeysym(&report.xkey, 0) == 'q') {return;}
                
                break;
            case KeyRelease:
                retriggered = 0;
                if (XEventsQueued(display, QueuedAfterReading)) {
                    XPeekEvent(display, &nextEv);
                    if (nextEv.type == KeyPress && nextEv.xkey.time == report.xkey.time && nextEv.xkey.keycode == report.xkey.keycode) 
                    {
                        // delete retriggered KeyPress event
                        XNextEvent(display, &report);
                        retriggered = 1;
                    }
                }

                if (!retriggered) {
                    for (int k=0; k<16; k++) {
                        if (notes[k] == (char) XLookupKeysym(&report.xkey, 0)) {
                            frequs[k].noteOn = false;
                            frequs[k].noteEnd(soundMaker->audio.getStreamTime());
                        }
                    }
                    std::cout << "key #" << (long) XLookupKeysym (&report.xkey, 0) << " was released.\n";
                }
                break;
        }
        // std::cout << frequ << std::endl;
    }
    return;
}

double oscillator(double xtime, int osc = OSC_SINE) 
{
    double output = 0.0;
    int fcounter = 0;
    double f = 0.0;
    double amp;
    for (size_t i=0; i < frequs.size(); i++)
    {
        f = 220*pow(pow(2.0, 1.0/12.0), i);
        amp = frequs[i].getAmplitude(xtime);
        if (amp > 0.0) {
            switch (osc)
            {
            case OSC_SINE:
                output += amp * sin(f * 2 * M_PI * xtime);
                break;
            case OSC_SQUARE:
                output += amp * (sin(f * 2 * M_PI * xtime) > 0 ? 1.0 : -1.0);
                break;
            case OSC_TRIANGLE:
                output += amp * asin(sin(f * 2 * M_PI * xtime)) * (2.0/M_PI);
                break;
            case OSC_SAW:
                output += amp * ((2.0/M_PI) * (f * M_PI * fmod(xtime, 1.0/f) - (2.0/M_PI)));
                break;
            case OSC_NOISE:
                return 2.0 * ((double) rand() / (double) RAND_MAX) -1.0;
            default:
                break;
            }
            fcounter++;
        }

    }
    //std::cout << output << std::endl;
    //if (fcounter == 0) { return output;}
    //else { return output / (double) fcounter;}
    return output*0.2;
}



int main()
{
    display = XOpenDisplay(NULL);
    window = XCreateSimpleWindow(display, RootWindow (display, 0), 1, 1, 500, 500,
        0, BlackPixel (display, 0), BlackPixel (display, 0));
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
    XMapWindow(display, window);
    XFlush(display);

	// Display a keyboard
	std::cout << std::endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << std::endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << std::endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << std::endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << std::endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << std::endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << std::endl << std::endl;


    //std::cout << std::this_thread::get_id() << std::endl;

    SoundPlayer soundMaker;
    soundMaker.setWaveFunction(&oscillator);
    soundMaker.startStream();
    checkKeys(&soundMaker);

    return 0;
}
