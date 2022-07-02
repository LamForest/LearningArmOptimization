#include <iostream>
#include <arm_neon.h>
#include <vector>
#include <chrono>

void rgb_deinterleave_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color) {
    /*
     * Take the elements of "rgb" and store the individual colors "r", "g", and "b".
     */
    for (int i=0; i < len_color; i++) {
        r[i] = rgb[3*i];
        g[i] = rgb[3*i+1];
        b[i] = rgb[3*i+2];
    }
}

void rgb_deinterleave_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color) {
    /*
     * Take the elements of "rgb" and store the individual colors "r", "g", and "b"
     */
    int num8x16 = len_color / 16; //一次处理16个 8bit int
    uint8x16x3_t intlv_rgb;
    for (int i=0; i < num8x16; i++) {
        intlv_rgb = vld3q_u8(rgb+3*16*i);
        vst1q_u8(r+16*i, intlv_rgb.val[0]);
        vst1q_u8(g+16*i, intlv_rgb.val[1]);
        vst1q_u8(b+16*i, intlv_rgb.val[2]);
    }
}

static constexpr int loop = 10;

float stat(decltype(rgb_deinterleave_c) func, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color){
    float avgTime = 0;
    float tmp;
    for(int i = 0; i < loop; ++i){
        auto startClock = std::chrono::system_clock::now();
        func(r, g, b, rgb, len_color);
        auto endClock = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - startClock);
        tmp = double(duration.count()) * std::chrono::microseconds::period::num / 1000;
        avgTime += tmp;
        std::cout << "          [" << i << "]" << " Cost time: " << tmp << "ms" << std::endl;
    }
    return avgTime / loop;
}

int main(){
    constexpr int len_color = 2000 * 20000;
    
    std::vector<uint8_t> img(len_color * 3, 0);

    std::vector<uint8_t> r(len_color), g(len_color), b(len_color);

    for(int i = 0; i < len_color; ++i){
        img[3*i] = 'r';
        img[3*i + 1] = 'g';
        img[3*i + 2] = 'b';
    }
    auto img_copy = img;

    float t1 = stat(rgb_deinterleave_c, &(r[0]), &(g[0]), &(b[0]), &(img[0]), len_color);
    printf("vanilia: %.3f ms\n\n", t1);

    float t2 = stat(rgb_deinterleave_neon, &(r[0]), &(g[0]), &(b[0]), &(img[0]), len_color);
    printf("neon: %.3f ms\n", t2);
    
    return 0;
}