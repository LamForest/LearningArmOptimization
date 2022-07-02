#include "boxFilter.h"
#include <chrono>
#include <random>
#include <iostream>
#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

void BoxFilter::filter(float *input, int radius, int height, int width, float *output) {
  for (int h = 0; h < height; ++h) {
    int height_sift = h * width;
    for (int w = 0; w < width; ++w) {
      int start_h = std::max(0, h - radius);
      int end_h = std::min(height - 1, h + radius);
      int start_w = std::max(0, w - radius);
      int end_w = std::min(width - 1, w + radius);

      float tmp = 0;
      for (int sh = start_h; sh <= end_h; ++sh) {
        for (int sw = start_w; sw <= end_w; ++ sw) {
          tmp += input[sh * width + sw];
        }
      }

      output[height_sift + w] = tmp;
    }
  }
}

/***
 * SS 实现1
 * */
void BoxFilter::fastFilter(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int sift = h * width;
    for (int w = 0; w < width; ++w) {
      int start_w = std::max(0, w - radius);
      int end_w = std::min(width - 1, w + radius);

      float tmp = 0;
      for (int sw = start_w; sw <= end_w; ++ sw) {
        tmp += input[sift + sw];
      }

      cachePtr[sift + w] = tmp;

      output[sift+w] = 0; //避免output输出与BoxFilter::fastFilter执行次数有关
    }
  }

  // sum vertical
  for (int h = 0; h < height; ++h) {
    int shift = h * width;
    int start_h = std::max(0, h - radius);
    int end_h = std::min(height - 1, h + radius);

    for (int sh = start_h; sh <= end_h; ++sh) {
      int out_shift = sh * width;
      for (int w = 0; w < width; ++w) {
        output[shift + w] += cachePtr[out_shift + w]; //此处为 += ，多次执行BoxFilter::fastFilter会使得output不断增大
      }
    }
  }
}
/***
 * 积分图
 * */
void BoxFilter::fastFilterSAT(float *input, int radius, int height, int width, float *output){
  float *cachePtr = &(cache[0]);
    for(int h = 0; h < height; ++h){
        for(int w = 0; w < width; ++w){
        int cur_offset = h * width + w;
        float cur = 0;
        if(h > 0){
            cur += cachePtr[cur_offset - width];
        }
        if(w > 0){
            cur += cachePtr[cur_offset - 1];
        }
        if(w > 0 && h > 0){
            cur -= cachePtr[cur_offset - width - 1];
        }
        cur += input[cur_offset];
        cachePtr[cur_offset] = cur;
        }
    }
    // const int AREA = (radius + 1) * (radius + 1);
    for(int h = 0; h < height; ++h){
        for(int w = 0; w < width; ++w){
        int right = std::min(width - 1, w + radius);
        int left = std::max(0, w - radius);
        int up = std::max(0, h - radius);
        int bottom = std::min(height - 1, h +radius);
        // int AREA = (bottom - up + 1) * (right - left + 1);
        float A = 0, B = 0, C = 0, D = 0;
        if(left > 0 && up > 0){
            A = cachePtr[(up - 1) * width + (left - 1)];
        }
        if(left > 0){
            D = cachePtr[bottom * width + (left - 1)];
        }
        if(up > 0){
            B = cachePtr[(up - 1) *width + (right)];
        }
        C = cachePtr[bottom * width + right];
        // printf("i: %d ; A:%.2f, B:%.2f, C:%.2f, D:%.2f, left: %d, right: %d, up: %d, bottom: %d\n", h*width + w,A, B, C, D, left, right, up, bottom);
        output[h*width + w] = (A + C - B - D);

        }
    }
}

/**
 * 实现2
 * **/
void BoxFilter::fastFilterSS(float *input, int radius, int height, int width, float *output) {
    float *cachePtr = &(cache[0]);
    for(int h = 0; h < height; ++h){
        float cur = 0;
        int shift = h * width;
        for(int w = 0; w < radius; ++w){
        cur += input[shift + w];
        }
        int w = 0;
        for(; w <= radius; ++w){
        cur += input[shift+w+radius];
        cachePtr[shift + w] = cur;
        }
        
        for(; w < width - radius; ++w){
        cur += input[shift+w+radius] - input[shift+w-radius-1];
        cachePtr[shift + w] = cur;
        }

        for(; w < width; ++w){
        cur -= input[shift+w-radius-1];
        cachePtr[shift + w] = cur;
        }
    }

    for(int w = 0; w < width; ++w){
        float cur = 0;
        // int shift =  * width;
        for(int h = 0; h < radius; ++h){
        cur += cachePtr[width * h + w];
        }
        int h = 0;
        for(; h <= radius; ++h){
          int shift = width * h;
          cur += cachePtr[shift+w+radius*width];
          output[shift + w] = cur;
        }
        
        for(; h < height - radius; ++h){
          int shift = width * h;
          cur += cachePtr[shift+w+radius*width] - cachePtr[shift+w-(radius+1) * width];
          output[shift + w] = cur;
        }

        for(; h < height; ++h){
          int shift = width * h;
          cur -= cachePtr[shift+w-(radius+1) * width];
          output[shift + w] = cur;
        }
    }
}

/***
 * 2次1维卷积，cache-friendly版本
 * TODO: width height < 2 radius + 1 的时候会导致段错误
 * */
void BoxFilter::fastFilterSSCF(float *input, int radius, int height, int width, float *output) {
    float *cachePtr = &(cache[0]);
    for(int h = 0; h < height; ++h){
        float cur = 0;
        int shift = h * width;
        for(int w = 0; w < radius; ++w){
        cur += input[shift + w];
        }
        int w = 0;
        for(; w <= radius; ++w){
        cur += input[shift+w+radius];
        cachePtr[shift + w] = cur;
        }
        
        for(; w < width - radius; ++w){
        cur += input[shift+w+radius] - input[shift+w-radius-1];
        cachePtr[shift + w] = cur;
        }

        for(; w < width; ++w){
        cur -= input[shift+w-radius-1];
        cachePtr[shift + w] = cur;
        }
    }
    float *colSumPtr = &(colSum[0]);
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] = 0;
    } 
    for(int h = 0; h < radius; ++h){
        int shift = h * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w];
        }
    }
    int h = 0;

    for(; h <= radius; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w + offset];
            output[shift+w] = colSumPtr[w];
        }
    }
    

    for(; h < height - radius; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] += cachePtr[shift + w + offset] - cachePtr[shift + w - offset - width];
            output[shift+w] = colSumPtr[w];
        }
    }

    for(; h < height; ++h){
        int shift = h * width;
        int offset = radius * width;
        for(int w = 0; w < width; ++w){
            colSumPtr[w] -= cachePtr[shift + w - offset - width];
            output[shift+w] = colSumPtr[w];
        }
    }   
}

//积分图
void BoxFilter::fastFilterV2(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);
  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += cachePtr[shift + w];
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += addPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift;
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] += addPtr[w];
      colSumPtr[w] -= subPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    for (int w = 0; w < width; ++w) {
      colSumPtr[w] -= subPtr[w];
      outPtr[w] = colSumPtr[w];
    }
  }
}

void BoxFilter::fastFilterV2NeonIntrinsics(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2);
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }

  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);
      _tmp = vsubq_f32(_tmp, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}

void BoxFilter::fastFilterV2NeonAsm(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  int n = width >> 2;
  int re = width - (n << 2);
  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }

  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    asm volatile(
      "0:                       \n"
      "vld1.s32 {d0-d1}, [%0]!  \n"
      "vld1.s32 {d2-d3}, [%1]!  \n"
      "vld1.s32 {d4-d5}, [%2]   \n"
      "vadd.f32 q4, q0, q2      \n"
      "vsub.f32 q3, q4, q1      \n"
      "vst1.s32 {d6-d7}, [%3]!  \n"
      "vst1.s32 {d6-d7}, [%2]!  \n"
      "subs %4, #1              \n"
      "bne  0b                  \n"
      : "=r"(tmpAddPtr), //
      "=r"(tmpSubPtr),
      "=r"(tmpColSumPtr),
      "=r"(tmpOutPtr),
      "=r"(nn)
      : "0"(tmpAddPtr),
      "1"(tmpSubPtr),
      "2"(tmpColSumPtr),
      "3"(tmpOutPtr),
      "4"(nn)
      : "cc", "memory", "q0", "q1", "q2", "q3", "q4"
    );

#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = n;
    int remain = re;
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}

void BoxFilter::fastFilterV2NeonAsmV2(float *input, int radius, int height, int width, float *output) {
  float *cachePtr = &(cache[0]);
  // sum horizonal
  for (int h = 0; h < height; ++h) {
    int shift = h * width;

    float tmp = 0;
    for (int w = 0; w < radius; ++w) {
      tmp += input[shift + w];
    }

    for (int w = 0; w <= radius; ++w) {
      tmp += input[shift + w + radius];
      cachePtr[shift + w] = tmp;
    }

    int start = radius + 1;
    int end = width - 1 - radius;
    for (int w = start; w <= end; ++w) {
      tmp += input[shift + w + radius];
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }

    start = width - radius;
    for (int w = start; w < width; ++w) {
      tmp -= input[shift + w - radius - 1];
      cachePtr[shift + w] = tmp;
    }
  }

  float *colSumPtr = &(colSum[0]);

  for (int indexW = 0; indexW < width; ++indexW) {
    colSumPtr[indexW] = 0;
  } 

  // sum vertical
  for (int h = 0; h < radius; ++h) {
    int shift = h * width;

    float *tmpColSumPtr = colSumPtr;
    float *tmpCachePtr = cachePtr + shift;
    int indexW = 0;

    int nn = width >> 2;
    int remain = width - (nn << 2);
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);
      float32x4_t _cache = vld1q_f32(tmpCachePtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _cache);

      vst1q_f32(tmpColSumPtr, _tmp); 

      tmpColSumPtr += 4;
      tmpCachePtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpCachePtr;
      tmpColSumPtr ++;
      tmpCachePtr ++;
    }
  }

  for (int h = 0; h <= radius; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;

    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;

    int nn = width >> 2;
    int remain = width - (nn << 2);
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _add = vld1q_f32(tmpAddPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vaddq_f32(_colSum, _add);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpAddPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
    }

  }

  int start = radius + 1;
  int end = height - 1 - radius;
  for (int h = start; h <= end; ++h) {
    float *addPtr = cachePtr + (h + radius) * width;
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 
    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpAddPtr = addPtr;
    float *tmpSubPtr = subPtr;

    int nn = width >> 3;
    int remain = width - (nn << 3);
#if __ARM_NEON
    asm volatile(
      "0:                       \n"
      "pld      [%0, #256]      \n"
      "vld1.s32 {d0-d3}, [%0]!  \n"
      "pld      [%2, #256]      \n"
      "vld1.s32 {d8-d11}, [%2]  \n"

      "vadd.f32 q6, q0, q4      \n"

      "pld      [%1, #256]      \n"
      "vld1.s32 {d4-d7}, [%1]!  \n"
      
      "vadd.f32 q7, q1, q5      \n"
      
      "vsub.f32 q6, q6, q2      \n"
      
      "vsub.f32 q7, q7, q3      \n"
      
      "vst1.s32 {d12-d15}, [%3]!  \n"
      
      "vst1.s32 {d12-d15}, [%2]!  \n"

      "subs %4, #1              \n"
      "bne  0b                  \n"
      : "=r"(tmpAddPtr), //
      "=r"(tmpSubPtr),
      "=r"(tmpColSumPtr),
      "=r"(tmpOutPtr),
      "=r"(nn)
      : "0"(tmpAddPtr),
      "1"(tmpSubPtr),
      "2"(tmpColSumPtr),
      "3"(tmpOutPtr),
      "4"(nn)
      : "cc", "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9"
    );

#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr += *tmpAddPtr;
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpAddPtr ++;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }

  start = height - radius;
  for (int h = start; h < height; ++h) {
    float *subPtr = cachePtr + (h - radius - 1) * width;
    int shift = h * width;
    float *outPtr = output + shift; 

    int indexW = 0;
    float *tmpOutPtr = outPtr;
    float *tmpColSumPtr = colSumPtr;
    float *tmpSubPtr = subPtr;

    int nn = width >> 2;
    int remain = width - (nn << 2);
#if __ARM_NEON
    for (; nn > 0; nn--) {
      float32x4_t _sub = vld1q_f32(tmpSubPtr);
      float32x4_t _colSum = vld1q_f32(tmpColSumPtr);

      float32x4_t _tmp = vsubq_f32(_colSum, _sub);

      vst1q_f32(tmpColSumPtr, _tmp);
      vst1q_f32(tmpOutPtr, _tmp);

      tmpSubPtr += 4;
      tmpColSumPtr += 4;
      tmpOutPtr += 4;
    }
#endif // __ARM_NEON
    for (; remain > 0; --remain) {
      *tmpColSumPtr -= *tmpSubPtr;
      *tmpOutPtr = *tmpColSumPtr;
      tmpColSumPtr ++;
      tmpOutPtr ++;
      tmpSubPtr ++;
    }
  }
}

