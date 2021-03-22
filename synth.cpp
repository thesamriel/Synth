//#include "rtaudio/RtAudio.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// #include <X11/Xlib.h>
//#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <array>
#include <algorithm>

#include "SoundPlayer.h"

#define KEY_LEFT 75
#define KEY_RIGHT 77
// zsxcfvgbnjmk,l./
std::array<int, 16> NOTE_KEYS = {90, 83, 88, 67, 70, 86, 71, 66, 78, 73, 77, 75, 44, 76, 46, 47};
bool keys[1024];

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

void processInput(SoundPlayer *soundMaker)
{
    for (auto k=0; k<NOTE_KEYS.size(); k++)
    {
        // note turned on
        if (!frequs[k].noteOn && keys[NOTE_KEYS[k]])
        {
            frequs[k].noteOn = true;
            frequs[k].noteStart(soundMaker->getStreamTime());
        }
        //note turned off
        else if (frequs[k].noteOn && !keys[NOTE_KEYS[k]])
        {
            frequs[k].noteOn = false;
            frequs[k].noteEnd(soundMaker->audio.getStreamTime());
        }

    }

    if (keys[GLFW_KEY_RIGHT])
    {
        soundMaker->setWaveType(abs((soundMaker->getWaveType() + 1) % 5));
        std::cout << soundMaker->getWaveType() << std::endl;
        keys[GLFW_KEY_RIGHT] = false;
    }
    if (keys[GLFW_KEY_LEFT])
    {
        soundMaker->setWaveType(abs((soundMaker->getWaveType() - 1) % 5));
        std::cout << soundMaker->getWaveType() << std::endl;
        keys[GLFW_KEY_LEFT] = false;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        { glfwSetWindowShouldClose(window, true); }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        { keys[key] = true; }
        else if (action == GLFW_RELEASE)
        { keys[key] = false; }
    }
}




int main()
{
    

	// Display a keyboard
	std::cout << std::endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << std::endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << std::endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << std::endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << std::endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << std::endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << std::endl << std::endl;


    //std::cout << std::this_thread::get_id() << std::endl;

    SoundPlayer *soundMaker = new SoundPlayer;
    
    soundMaker->setWaveFunction(&oscillator);
    std::cout << "before loop" << std::endl;
    soundMaker->startStream();
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "SYNTH", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    glClearColor(140/255, 205.0/255, 220.0/255, 1.0);
    
    while(!glfwWindowShouldClose(window))
    {
        processInput(soundMaker);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    delete soundMaker;

    return 0;
}
