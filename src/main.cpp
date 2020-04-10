/*
    main.cpp

    Copyright (C) 2013 by Don Cross  -  http://cosinekitty.com/raytrace

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the author be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
       distribution.

    -------------------------------------------------------------------------

    Main source file for my demo 3D ray-tracing image maker.
*/

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <vector>
#include "algebra.h"
#include "chessboard.h"
#include "lodepng.h"

constexpr int threads = 1;

using namespace Imager;
using namespace std::chrono_literals;

void prepareScene(Scene & scene);
void saveImage(std::vector<unsigned char> & rgbaBuffer, size_t pixelsWide,
                size_t pixelsHigh);

int main()
{
    Scene scene(Color(0.0, 0.0, 0.0));

    prepareScene(scene);

    const char* filename = "chessboard.png";
    const size_t zoom = 4.5;
    const size_t antiAliasFactor = 3;

/* --- SINGLE THREAD start -- */

    auto start1 = std::chrono::high_resolution_clock::now();
    auto buffer = scene.SaveImage(600, 360, zoom, antiAliasFactor);
    saveImage(buffer, 600, 360);
    //scene.SaveImage(0, 600, 0, 360, zoom, antiAliasFactor);

    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> difference1 = end1 - start1;

    std::cout << "Multithreaded version with " << threads << " thread(s) = " << difference1.count() << " ms" << std::endl;

/* --- SINGLE THREAD end -- */

    // std::vector<std::future<void>> futures(threads);
    
    // for (size_t i = 0; i < threads; i++)
    //     futures[i] = std::async(std::launch::async, &Scene::SaveImage, &scene, filename, 600, 360, zoom, antiAliasFactor);

    // for (size_t i = 0; i < threads; i++)
    //     futures[i] = std::async(std::launch::async, &Scene::SaveImage, &scene, filename, i * 100, i * 60, zoom, antiAliasFactor);

    // std::for_each(futures.begin(), futures.end(), [](std::future<void> & obj){ obj.get(); });   

    return 0;
}
 
void prepareScene(Scene & scene)
{
    ChessBoard* board = new ChessBoard(
        4.00, 
        1.00, 
        1.00,
        0.25,
        Color(0.75, 0.70, 0.10),
        Color(0.30, 0.30, 0.40),
        Color(0.50, 0.30, 0.10));

    board->Move(-0.35, 0.1, -20.0);
    board->RotateZ(+11.0);
    board->RotateX(-62.0);

    scene.AddSolidObject(board);

    struct sphere_info
    {
        Vector center;
        double radius;
    };

    const sphere_info sinfo[] = 
    {
        { Vector(-1.8, 0.0, -17.0), 0.72 },
        { Vector( 0.0, 0.0, -17.0), 0.72 },
        { Vector(+1.8, 0.0, -17.0), 0.72 }
    };

    const size_t numSpheres = sizeof(sinfo) / sizeof(sinfo[0]);
    for (size_t i=0; i < numSpheres; ++i)
    {
        Sphere* sphere = new Sphere(sinfo[i].center, sinfo[i].radius);
        sphere->SetOpacity(0.17);
        sphere->SetMatteGlossBalance(0.95, Color(0.4,0.5,0.7), Color(0.8,1.0,0.7));
        scene.AddSolidObject(sphere);
    }

    scene.AddLightSource(LightSource(Vector(-45.0, +25.0, +50.0), Color(0.4, 0.4, 0.1, 1.0)));
    scene.AddLightSource(LightSource(Vector( +5.0, +90.0, +40.0), Color(0.5, 0.5, 1.5, 1.0)));
    scene.AddLightSource(LightSource(Vector(-25.0, +30.0, +40.0), Color(0.3, 0.2, 0.1, 1.0)));
}

void saveImage(std::vector<unsigned char> & rgbaBuffer, size_t pixelsWide,
                size_t pixelsHigh)
{
    const char* filename = "chessboard.png";
    // Write the PNG file
    const unsigned error = lodepng::encode(
        filename, 
        rgbaBuffer, 
        pixelsWide, 
        pixelsHigh);

    // If there was an encoding error, throw an exception.
    if (error != 0)
    {
        std::string message = "PNG encoder error: ";
        message += lodepng_error_text(error);
        throw ImagerException(message.c_str());
    }
}