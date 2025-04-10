#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class NoiseGenerationNode : public Node {
public:
    enum class NoiseType {
        Perlin,
        Simplex,
        Worley
    };

    enum class OutputType {
        Color,
        Displacement
    };

    explicit NoiseGenerationNode(int id);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

private:
    int selected = 0;
    int outputSelected = 0;
    float scale;
    int octaves;
    float persistence;
    int width;
    int height;
    NoiseType noiseType;
    OutputType outputType;

    void generatePerlinNoise(cv::Mat& output, const cv::Size& size);
    void generateSimplexNoise(cv::Mat& output, const cv::Size& size);
    void generateWorleyNoise(cv::Mat& output, const cv::Size& size);
};