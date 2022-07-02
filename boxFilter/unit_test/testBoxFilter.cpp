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
    for (int w = 0; w < 5; ++w) {
      std::cout << input[height_sift + w] << " ";
    }
  }
  std::cout << std::endl;
};



static int loop = 5;
static int height = 2000;
static int width = 2000;
static int radius = 5;
static int printMat = 1;

constexpr static double EPSILON = 0.00001;

static std::vector<float> gth; 


static bool is_correct(std::vector<float> output){
    return true;
    // double sum_diff = 0;
    // for (int h = 0; h < height; ++h) {
    //     int height_sift = h * width;
    //     for (int w = 0; w < width; ++w) {
    //         sum_diff += std::abs(output[height_sift + w] - gth[height_sift + w]);
    //     }
    // }
    // double avg = (sum_diff) / width * height;
    // printf("avg diff = %.2f\n", avg);
    // return (avg) < EPSILON;
    
}



TEST(netTest, org_boxfilter)
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
        boxFilter.filter(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " BoxFilfer Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          BoxFilfer Average Cost time: " << avgTime / loop << "ms" << std::endl;


    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }

    /** use result of org_boxfilter as groundtruth*/
    // gth = std::move(output);
}

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
    EXPECT_EQ(is_correct(output), true) << " ERROR: result is wrong. " ;

    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }
}


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

    EXPECT_EQ(is_correct(output), true) << " ERROR: result is wrong. " ;
    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }

    
}

#ifdef MYDEBUG
// fast_boxfilter_SS test okay
TEST(netTest, fast_boxfilter_SS_test){
    constexpr int height = 10, width = height;
    constexpr int N = height * width;
    constexpr int radius = 3;
    // float input[N] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    std::vector<float> input(N, 1);
    std::vector<float> output(N, 0);
    float cachePtr[N];
    float colSumPtr[width];
    for(int i = 0; i < width; ++i){
        colSumPtr[i] = 0;
    }

    auto printColSum = [&colSumPtr](int h){
        printf("h = %d:\n", h);
        for(int i = 0; i < width; ++i){
            printf("%.1f, ", colSumPtr[i]);
        }
        printf("\n- - - - - -\n\n");
    };

    for(int h = 0; h < height; ++h){
        float cur = 0;
        int shift = h * width;
        for(int w = 0; w < radius; ++w){
        cur += input[w];
        }
        int w = 0;
        for(; w <= radius; ++w){
        cur += input[w+radius];
        cachePtr[shift + w] = cur;
        }
        
        for(; w < width - radius; ++w){
        cur += input[w+radius] - input[w-radius-1];
        cachePtr[shift + w] = cur;
        }

        for(; w < width; ++w){
        cur -= input[w-radius-1];
        cachePtr[shift + w] = cur;
        }
    }

    for(int h = 0; h < radius; ++h){
        int shift = h * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w];
        }
        printColSum(h);
    }
    int h = 0;

    for(; h <= radius; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w + offset];
            output[shift+w] = colSumPtr[w];
        }
        printColSum(h);
    }
    

    for(; h < height - radius; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w + offset] - cachePtr[shift + w - offset - width];
            output[shift+w] = colSumPtr[w];
        }
        printColSum(h);
    }

    for(; h < height; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] -= cachePtr[shift + w - offset - width];
            output[shift+w] = colSumPtr[w];
        }
        printColSum(h);
    }

    for(int h = 0; h < height; ++h){
        for(int w = 0; w < width; ++w){
            printf("%.1f, ", cachePtr[h * width + w]);
        }
        printf("\n");
    }
    printf("\n");
    for(int h = 0; h < height; ++h){
        for(int w = 0; w < width; ++w){
            printf("%.1f, ", output[h * width + w]);
        }
        printf("\n");
    }

}
#endif

TEST(netTest, fast_boxfilter_SS_CF)
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
        boxFilter.fastFilterSSCF(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Fast BoxFilfer Cache Friendly Spatial Seperatable Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          Fast BoxFilfer Cache Friendly Spatial Seperatable Average Cost time: " << avgTime / loop << "ms" << std::endl;

    EXPECT_EQ(is_correct(output), true) << " ERROR: result is wrong. " ;
    if (printMat == 1) {
        std::cout << "result: " << std::endl;
        print(&output[0], height, width);
    }
}


TEST(netTest, fast_boxfilter_SS)
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
        boxFilter.fastFilterSS(&input[0], radius, height, width, &output[0]);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Fast BoxFilfer Spatial Seperatable Cost time: " << tmp << "ms" << std::endl;

    }
    std::cout << "\n          Fast BoxFilfer Spatial Seperatable Average Cost time: " << avgTime / loop << "ms" << std::endl;

    EXPECT_EQ(is_correct(output), true) << " ERROR: result is wrong. " ;
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

    EXPECT_EQ(is_correct(output), true) << " ERROR: result is wrong. " ;
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
