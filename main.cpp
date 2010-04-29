// includes from OpenEngine base
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Resources/Tex.h>
#include <Resources/ResourceManager.h>
#include <Resources/DirectoryManager.h>
#include <Utils/Timer.h>
#include <Utils/Convert.h>
#include <Math/RandomGenerator.h>

// includes from OpenEngine extensions
#include <Resources/FreeImage.h>
#include <Utils/TextureTool.h>
#include <Utils/PerlinNoise.h>

// other includes
#include <stdlib.h>
#include <limits>

using namespace OpenEngine;
using namespace Logging;
using namespace Utils;

//typedef double REAL;

//#define DEBUG_PRINT

static void Save(FloatTexture2DPtr tex, std::string filename) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    //unsigned int c = tex->GetChannels();
    UCharTexture2DPtr output(new UCharTexture2D(w,h,1));
    
    unsigned int k = 0;
    REAL min = numeric_limits<REAL>::max();
    REAL max = numeric_limits<REAL>::min();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            REAL v = *(tex->GetPixel(x,y));
            if (v<min) { 
                min = v;
                k++;
                //logger.info << "------> " << x << ":" << y << "= " << v << logger.end;
            }
            if (v>max) max = v;
            //if (v<-1000.0f) 
        }
    }
    //logger.info << "min:" << min << " max:" << max << " k:" << k << logger.end;
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            *(output->GetPixel(x,y)) = ((*(tex->GetPixel(x,y))-min)/max)* 255;
        }
    }

    TextureTool::DumpTexture(output,filename +".png");
    TextureTool::DumpTexture(tex,filename +".exr");
}

void Threshold(FloatTexture2DPtr tex, REAL threshold) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            if(*(tex->GetPixel(x,y)) < threshold)
                *(tex->GetPixel(x,y)) = 0;
        }
    }
}

void CloudExpCurve(FloatTexture2DPtr tex) {
    //unsigned int CloudCover = 85; // 0-255 =density
    //REAL CloudSharpness = 0.5; //0-1 =sharpness
    REAL CloudCover = 0.215; // 0-255 =density
    REAL CloudSharpness = 10; //0-1 =sharpness

    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            /*
            *(tex->GetPixel(x,y)) = *(tex->GetPixel(x,y)) - CloudCover;
            if(*(tex->GetPixel(x,y)) < 0)
                *(tex->GetPixel(x,y)) = 0;
            *(tex->GetPixel(x,y)) = 255 - (pow(CloudSharpness , *(tex->GetPixel(x,y)) ) * 255);
*/
            *(tex->GetPixel(x,y)) = *(tex->GetPixel(x,y)) - CloudCover;
            *(tex->GetPixel(x,y)) = 
                1.0 - exp( -CloudSharpness * *(tex->GetPixel(x,y)) );
            if(*(tex->GetPixel(x,y)) < 0)
                *(tex->GetPixel(x,y)) = 0;
        }
    }
}

int main(int argc, char** argv) {
    // timer to mesure execution time
    Utils::Timer timer;
    timer.Start();

    // create a logger to std out
    StreamLogger* stdlog = new StreamLogger(&std::cout);
    Logger::AddLogger(stdlog);

    //unsigned int resolution = 64,
    //unsigned int bandwidth = 128,
    //float mResolution = 2,
    //float mBandwidth = 0.5,
    //unsigned int smooth = 10,
    //unsigned int layers = 4,
    //unsigned int seed = 0
    //FloatTexture2DPtr output = PerlinNoise::Generate(64, 128, 2, 0.5, 10, 4, 0);

    FloatTexture2DPtr output = PerlinNoise::Generate(512, 512, 128, 0.5, 1, 10, 5, 0);


    //Threshold(output,80);
    //CloudExpCurve(output);

    PerlinNoise::Smooth(output,20);
    PerlinNoise::Normalize(output,0,1); 
    CloudExpCurve(output);
    Save(output, "output");

    logger.info << "execution time: " << timer.GetElapsedTime() << logger.end;
    return EXIT_SUCCESS;
}
