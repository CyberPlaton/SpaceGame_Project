#pragma once
#ifndef BASE_H
#define BASE_H

// FOR AUDIO
#include"common/include/FMOD/core/inc/fmod.hpp"
#include"common/include/FMOD/core/inc/fmod_errors.h"

#include"common/include/FMOD/fsbank/inc/fsbank.h"
#include"common/include/FMOD/fsbank/inc/fsbank_errors.h"

#include"common/include/FMOD/studio/inc/fmod_studio.hpp"

#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "fsbank_vc.lib")



namespace nautilus {

	namespace audio {



	}

}



// FOR GRAPHICS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<iostream>

#include"common/include/GL/glew.h"
#include"common/include/GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include"GLFW/glfw3native.h" // For accessing HWND handle from GLFWwindow.

#include"common/include/glm/glm.hpp"
#include"common/include/glm/gtc/matrix_transform.hpp"
#include"common/include/glm/gtx/quaternion.hpp"
#include"common/include/glm/gtc/type_ptr.hpp"

#include"common/include/yaml-cpp/yaml.h"
#include"common/include/entt/entt.hpp"

#include<memory>
#include<vector>
#include<utility>
#include<random>
#include<assert.h>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include<map>
#include<chrono>
#include<array>



#include"ImGuiBase.h"


namespace nautilus {


    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}



namespace nautilus {

	namespace graphics {


        struct Pos_Color_Tex {
            glm::vec3 m_Position;
            glm::vec4 m_Color;
            glm::vec2 m_TexCoords;
        };



        float common_lerp(float x, float y, float t);

        class Random {
        public:
            static void Init()
            {
                s_RandomEngine.seed(std::random_device()());
            }

            // Function returns a random number between 0 and 1.
            static float Float()
            {
                return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
            }


            static float AlternatingOne() {

                if (s_Distribution(s_RandomEngine) % 2 == 0) {
                    return -1.0f;
                }
                else {
                    return 1.0f;
                }
            }

        private:
            static std::mt19937 s_RandomEngine;
            static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
        };



	}

}


// FOR NETWORKING
#include"common/include/olcPGEX_Network.h"


namespace nautilus {

	namespace network {

	}
}



// FOR CORE
// LUA
extern "C" {
#include"lua.hpp"
}

#include<chrono>
#include<thread>
#include<omp.h>


#define COMPARE_STRINGS(x, y) strcmp(x.c_str(), y) // Utility. Comparing strings.
#define COMPARE_STRINGS_2(x, y) strcmp(x.c_str(), y.c_str()) // Utility. Comparing strings.

namespace nautilus {

    namespace core {



    }

}





namespace colors {
    enum {
        BLACK = 0,
        DARKBLUE = FOREGROUND_BLUE,
        DARKGREEN = FOREGROUND_GREEN,
        DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
        DARKRED = FOREGROUND_RED,
        DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
        DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
        DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        GRAY = FOREGROUND_INTENSITY,
        BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
        GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
        CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
        RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
        MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
        YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
        WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    };
}



inline std::ostream& blue(std::ostream& s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
        | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& red(std::ostream& s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& green(std::ostream& s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& yellow(std::ostream& s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& white(std::ostream& s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return s;
}

struct color {
    color(WORD attribute) :m_color(attribute) {};
    WORD m_color;
};

template <class _Elem, class _Traits>
std::basic_ostream<_Elem, _Traits>&
operator<<(std::basic_ostream<_Elem, _Traits>& i, const color& c)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout, c.m_color);
    return i;
}







namespace nautilus {
    

    class HRTimer {
    public:
        HRTimer(){}
        ~HRTimer(){}


        // Start counting time from this point.
        // Initial starting of the timer.
        //
        void startTimer() {
            m_StartTimePoint = std::chrono::high_resolution_clock::now();
        }

        // Get elapsed time since last start.
        // Returns the elapsed time since function "startTimer" was called.
        // In order to reset the counter, call "startTimer".
        //
        // Elapsed time is in milliseconds.
        //
        double getElapsedTime() {

            auto endTimePoint = std::chrono::high_resolution_clock::now();

            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

            auto duration = end - start;
            return duration / 1000; // Duration in milliseconds
        }


    protected:

        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;

    private:

    };



    // Cross-platform sleep function
    // See: https://stackoverflow.com/questions/4184468/sleep-for-milliseconds
    void sleepcp(int milliseconds);


    class FPSTimer : public HRTimer {
    public:
        FPSTimer();
            

        // Functions to be called on Framestart and Frameend.
        // "endFrame()" calculates frames and if one second elapsed stores how many frames we could draw in one second,
        // thus FPS.
        //
        // Further, if framegoverning is turned on, the "endFrame()" function will govern the fps.
        //
        void startFrame() { startTimer(); }
        
        
        void endFrame();


        double getFrametime();
        double getAverageFrametime();

        double getMillisecondsPerFrame();
        double getAverageMillisecondsPerFrame();


        double getFPS();
        double getAverageFPS();

        void toggleFPSGoverning() { m_GovernFPS = !m_GovernFPS; }

        // Setting the target frames per second.
        // E.g. for setting 60 FPS, just call "setFpsGoverning(60)".
        //
        void setFpsGoverning(double fps) { m_TargetFPS = fps; }


    private:

        double m_TargetFPS = 0.0;
        bool m_GovernFPS = false;


        // Storing how many milliseconds we need to process one frame.
        // Example value: 16.6
        std::deque<double> m_FrametimeQueue;


    private:


        void _governFPS(double dt);

    };



}




#endif //BASE_H