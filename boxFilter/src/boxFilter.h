#include <vector>

class BoxFilter {
public:
  BoxFilter() {}
  ~BoxFilter() {
    free();
  }

  void init(int height, int width, int radius) {
    free();
    cache.resize(height * width); //多此一举
    colSum.resize(width);
  }

  void filter(float *input, int radius, int height, int width, float *output);

  void fastFilter(float *input, int radius, int height, int width, float *output);

  void fastFilterSAT(float *input, int radius, int height, int width, float *output);

  //将2维卷积拆分成2次一维卷积，Spatial Seperable Conv
  //对于boxfilter，拆分是精确的
  void fastFilterSS(float *input, int radius, int height, int width, float *output); //vannila 
  void fastFilterSSCF(float *input, int radius, int height, int width, float *output); //cache friendly
  void fastFilterV2(float *input, int radius, int height, int width, float *output); //Impl by 

  void fastFilterV2NeonIntrinsics(float *input, int radius, int height, int width, float *output);

  void fastFilterV2NeonAsm(float *input, int radius, int height, int width, float *output);

  void fastFilterV2NeonAsmV2(float *input, int radius, int height, int width, float *output);

private:
  void free() {
    std::vector<float>().swap(cache);
    std::vector<float>().swap(colSum);
  }

private:
  std::vector<float> cache;
  std::vector<float> colSum;
};

