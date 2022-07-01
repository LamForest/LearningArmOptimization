#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"
#include "boxFilter.h"
#include <random>
#include <iostream>
#include <vector>
#include <chrono>

static void print(float *input, int height, int width) {
  for (int h = 10; h < 11; ++h) {
    int height_sift = h * width;
    std:: cout << std::endl;
    for (int w = 0; w < 50; ++w) {
      std::cout << input[height_sift + w] << " ";
    }
  }
  std::cout << std::endl;
};

static int loop = 10;
static int height = 2000;
static int width = 2000;
static int radius = 5;
static int printMat = 1;

// TEST(netTest, org_boxfilter)
// {
//     std::vector<float> input;
//     std::vector<float> output;

//     int size = height * width;

//     input.resize(size);
//     output.resize(size);

//     std::random_device rd;
//     std::mt19937 gen(0);
//     std::uniform_real_distribution<> dis(-2.0, 2.0);

//     for (int i = 0; i < size; ++i) {
//         input[i] = dis(gen);
//     }

//     for (int i = 0; i < size; ++i) {
//         output[i] = 0;
//     }

//     BoxFilter boxFilter;
//     boxFilter.init(height, width, radius);

//     float avgTime = 0;
//     float tmp;
//     for (int i = 0; i < loop; ++i) {
//         auto startClock = std::chrono::system_clock::now();
//         boxFilter.filter(&input[0], radius, height, width, &output[0]);
//         auto endClock = std::chrono::system_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
//         tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
//         avgTime += tmp;
//         std::cout << "          [" << i << "]" << " BoxFilfer Cost time: " << tmp << "ms" << std::endl;

//     }
//     std::cout << "\n          BoxFilfer Average Cost time: " << avgTime / loop << "ms" << std::endl;


//     if (printMat == 1) {
//         std::cout << "result: " << std::endl;
//         print(&output[0], height, width);
//     }
// }

TEST(netTest, fast_boxfilter)
{
    std::vector<float> input;
    std::vector<float> output;

    int size = height * width;

    input.resize(size);
    output.resize(size);

    std::random_device rd;
    std::mt19937 gen(0);
    std::uniform_real_distribution<> dis(-2.0, 2.0);

    for (int i = 0; i < size; ++i) {
        input[i] = dis(gen);
    }

    for (int i = 0; i < size; ++i) {
        output[i] = 0;
    }

    BoxFilter boxFilter;
    boxFilter.init(height, width, radius);

    float avgTime = 0;
    float tmp;
    for (int i = 0; i < loop; ++i) {
        auto startClock = std::chrono::system_clock::now();
        boxFilter.fastFilter(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Fast BoxFilfer Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          Fast BoxFilfer Average Cost time: " << avgTime / loop << "ms" << std::endl;


    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }
}

// TEST(netTest, fast_boxfilter_SAT_test){
//     const int N = 16;
//     // float input[N] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
//     std::vector<float> input(N, 1);
//     std::vector<float> output(N, 0);
//     float cachePtr[N];

//     int height = 4, width = 4;
//     int radius = 1;

//     for(int h = 0; h < height; ++h){
//         for(int w = 0; w < width; ++w){
//         int cur_offset = h * width + w;
//         float cur = 0;
//         if(h > 0){
//             cur += cachePtr[cur_offset - width];
//         }
//         if(w > 0){
//             cur += cachePtr[cur_offset - 1];
//         }
//         if(w > 0 && h > 0){
//             cur -= cachePtr[cur_offset - width - 1];
//         }
//         cur += input[cur_offset];
//         cachePtr[cur_offset] = cur;
//         }
//     }
//     // const int AREA = (radius + 1) * (radius + 1);
//     for(int h = 0; h < height; ++h){
//         for(int w = 0; w < width; ++w){
//         int right = std::min(width - 1, w + radius);
//         int left = std::max(0, w - radius);
//         int up = std::max(0, h - radius);
//         int bottom = std::min(height - 1, h +radius);
//         // int AREA = (bottom - up + 1) * (right - left + 1);
//         float A = 0, B = 0, C = 0, D = 0;
//         if(left > 0 && up > 0){
//             A = cachePtr[(up - 1) * width + (left - 1)];
//         }
//         if(left > 0){
//             D = cachePtr[bottom * width + (left - 1)];
//         }
//         if(up > 0){
//             B = cachePtr[(up - 1) *width + (right)];
//         }
//         C = cachePtr[bottom * width + right];
//         printf("i: %d ; A:%.2f, B:%.2f, C:%.2f, D:%.2f, left: %d, right: %d, up: %d, bottom: %d\n", h*width + w,A, B, C, D, left, right, up, bottom);
//         output[h*width + w] = (A + C - B - D);

//         }
//     }

//     for(int i = 0; i < N; ++i){
//         printf("%.1f, ", cachePtr[i]);
//     }
//     printf("\n");
//     for(int i = 0; i < N; ++i){
//         printf("%.1f, ", output[i]);
//     }

// }

TEST(netTest, fast_boxfilter_SAT)
{
    std::vector<float> input;
    std::vector<float> output;

    int size = height * width;

    input.resize(size);
    output.resize(size);

    std::random_device rd;
    std::mt19937 gen(0);
    std::uniform_real_distribution<> dis(-2.0, 2.0);

    for (int i = 0; i < size; ++i) {
        input[i] = dis(gen);
    }

    for (int i = 0; i < size; ++i) {
        output[i] = 0;
    }

    BoxFilter boxFilter;
    boxFilter.init(height, width, radius);

    float avgTime = 0;
    float tmp;
    for (int i = 0; i < loop; ++i) {
        auto startClock = std::chrono::system_clock::now();
        boxFilter.fastFilterSAT(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Fast BoxFilfer SAT Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          Fast BoxFilfer SAT Average Cost time: " << avgTime / loop << "ms" << std::endl;


    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }
}


TEST(netTest, fast_boxfilter_v2)
{
    std::vector<float> input;
    std::vector<float> output;

    int size = height * width;

    input.resize(size);
    output.resize(size);

    std::random_device rd;
    std::mt19937 gen(0);
    std::uniform_real_distribution<> dis(-2.0, 2.0);

    for (int i = 0; i < size; ++i) {
        input[i] = dis(gen);
    }

    for (int i = 0; i < size; ++i) {
        output[i] = 0;
    }

    BoxFilter boxFilter;
    boxFilter.init(height, width, radius);

    float avgTime = 0;
    float tmp;
    for (int i = 0; i < loop; ++i) {
        auto startClock = std::chrono::system_clock::now();
        boxFilter.fastFilterV2(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Fast BoxFilfer V2 Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          Fast BoxFilfer V2 Average Cost time: " << avgTime / loop << "ms" << std::endl;


    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }
}

// TEST(netTest, fast_boxfilter_v2_neon_intrinsics)
// {

//     std::vector<float> input;
//     std::vector<float> output;

//     int size = height * width;

//     input.resize(size);
//     output.resize(size);

//     std::random_device rd;
//     std::mt19937 gen(0); 
//     std::uniform_real_distribution<> dis(-2.0, 2.0);

//     for (int i = 0; i < size; ++i) {
//         input[i] = dis(gen);
//     }

//     for (int i = 0; i < size; ++i) {
//         output[i] = 0;
//     }

//     BoxFilter boxFilter;
//     boxFilter.init(height, width, radius);

//     float avgTime = 0;
//     float tmp;
//     for (int i = 0; i < loop; ++i) {
//         auto startClock = std::chrono::system_clock::now();
//         boxFilter.fastFilterV2NeonIntrinsics(&input[0], radius, height, width, &output[0]);
//         auto endClock = std::chrono::system_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
//         tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
//         avgTime += tmp;
//         std::cout << "          [" << i << "]" << " Fast BoxFilfer V2 Neon Intrinsics Cost time: " << tmp << "ms" << std::endl;

//     }
//     std::cout << "\n          Fast BoxFilfer V2 Neon Intrinsics Average Cost time: " << avgTime / loop << "ms" << std::endl;


//     if (printMat == 1) {
//         std::cout << "result: " << std::endl;
//         print(&output[0], height, width);
//     }
// }

// TEST(netTest, fast_boxfilter_v2_neon_asm)
// {

//     std::vector<float> input;
//     std::vector<float> output;

//     int size = height * width;

//     input.resize(size);
//     output.resize(size);

//     std::random_device rd;
//     std::mt19937 gen(0);
//     std::uniform_real_distribution<> dis(-2.0, 2.0);

//     for (int i = 0; i < size; ++i) {
//         input[i] = dis(gen);
//     }

//     for (int i = 0; i < size; ++i) {
//         output[i] = 0;
//     }

//     BoxFilter boxFilter;
//     boxFilter.init(height, width, radius);

//     float avgTime = 0;
//     float tmp;
//     for (int i = 0; i < loop; ++i) {
//         auto startClock = std::chrono::system_clock::now();
//         boxFilter.fastFilterV2NeonAsm(&input[0], radius, height, width, &output[0]);
//         auto endClock = std::chrono::system_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
//         tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
//         avgTime += tmp;
//         std::cout << "          [" << i << "]" << " Fast BoxFilfer V2 Neon Asm Cost time: " << tmp << "ms" << std::endl;

//     }
//     std::cout << "\n          Fast BoxFilfer V2 Neon Asm Average Cost time: " << avgTime / loop << "ms" << std::endl;


//     if (printMat == 1) {
//         std::cout << "result: " << std::endl;
//         print(&output[0], height, width);
//     }
// }

// TEST(netTest, fast_boxfilter_v2_neon_asm_v2)
// {

//     std::vector<float> input;
//     std::vector<float> output;

//     int size = height * width;

//     input.resize(size);
//     output.resize(size);

//     std::random_device rd;
//     std::mt19937 gen(0);
//     std::uniform_real_distribution<> dis(-2.0, 2.0);

//     for (int i = 0; i < size; ++i) {
//         input[i] = dis(gen);
//     }

//     for (int i = 0; i < size; ++i) {
//         output[i] = 0;
//     }

//     BoxFilter boxFilter;
//     boxFilter.init(height, width, radius);

//     float avgTime = 0;
//     float tmp;
//     for (int i = 0; i < loop; ++i) {
//         auto startClock = std::chrono::system_clock::now();
//         boxFilter.fastFilterV2NeonAsmV2(&input[0], radius, height, width, &output[0]);
//         auto endClock = std::chrono::system_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
//         tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
//         avgTime += tmp;
//         std::cout << "          [" << i << "]" << " Fast BoxFilfer V2 Neon AsmV2 Cost time: " << tmp << "ms" << std::endl;

//     }
//     std::cout << "\n          Fast BoxFilfer V2 Neon AsmV2 Average Cost time: " << avgTime / loop << "ms" << std::endl;


//     if (printMat == 1) {
//         std::cout << "result: " << std::endl;
//         print(&output[0], height, width);
//     }
// }
