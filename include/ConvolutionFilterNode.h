#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <array>

class ConvolutionFilterNode : public Node {
public:
    enum class KernelSize { SIZE_3x3, SIZE_5x5 };
    enum class FilterPreset { CUSTOM, SHARPEN, EMBOSS, EDGE_ENHANCE };

    explicit ConvolutionFilterNode(int id);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

private:
    KernelSize kernelSize = KernelSize::SIZE_3x3;
    FilterPreset currentPreset = FilterPreset::CUSTOM;
    std::array<std::array<float, 5>, 5> kernel; // Supports up to 5x5
    cv::Mat previewImage;
    bool previewDirty = true;

    void updatePresetKernel();
    void generatePreview();
    void createStandardKernels();
    std::map<FilterPreset, std::pair<std::vector<std::vector<float>>, KernelSize>> presetKernels;
};