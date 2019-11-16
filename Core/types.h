#ifndef MYENGINE_TYPES_H
#define MYENGINE_TYPES_H

#include <stdio.h>
#include <stdarg.h>

//unsigned int GetTickCount();

class log
{
public:
    static void write( const char * str, ... )
    {
        char buffer[1024];
        for (unsigned int i = 0; i < 1024; ++i)
        {
            buffer[i] = 0;
        }

        va_list			args;
        
        va_start( args, str );
        vsprintf( buffer, str, args );
        va_end( args );

        printf("%s\n", buffer);
//        char output[1024];
//        sprintf(output, "time: %f, log: %s\n", GetTickCount() / 1000.f, buffer);
//#ifndef WIN32
//        printf(output);
//#else
//		OutputDebugStringA(output);
//		if (GetKeyState(VK_MENU) < 0)
//			MessageBoxA(0, output, "message:", MB_OK);
//#endif//WIN32
    }
};

#ifndef WIN32
#ifdef _DEBUG
class TimeProfiler
{
private:
    struct timeval _startTime;
    long _min;
public:
    TimeProfiler(long min);
    ~TimeProfiler();
};

#define TIME_TEST(a) TimeProfiler __profiler(a);
#else
#define TIME_TEST(a) 
#endif//_DEBUG
#else
#define TIME_TEST(a) 
#endif//WIN32

#ifndef M_PI
#define M_PI	3.14159265358979323846f
#define M_PI_2	1.57079632679489661923f
#define M_PI_4	0.785398163397448309616f
#define M_1_PI	0.318309886183790671538f
#define M_2_PI	0.636619772367581343076f
#endif

#ifndef SCALE_BOX2D 
#define SCALE_BOX2D 60
#endif

//Sound *SoundEffect(const char *id);
//void SoundEffectRnd(const char *id, int var, int base = 1);
//void SoundEffectVar(const char *id);

#define INTERFACE_XML "sets.xml"

#ifdef WIN32
	#include <jansson\jansson.h>
	#define usleep Sleep
#else
	#include <unistd.h>
#endif

//extern std::string globalDigitsIni;


#endif//MYENGINE_TYPES_H
