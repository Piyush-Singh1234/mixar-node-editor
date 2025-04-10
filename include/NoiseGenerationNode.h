#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

// Select noise type
enum class NoiseType {
    Perlin = 0,
    Simplex,
    Worley
};

class NoiseGenerationNode : public Node {
public:
    NoiseType noiseType = NoiseType::Perlin;
    float scale = 1.0f;         // Frequency scale factor
    int octaves = 1;            // Number of octaves for fractal noise
    float persistence = 0.5f;   // Controls amplitude of octaves
    bool useAsDisplacement = false; // If true, output remains grayscale (for displacement)

    explicit NoiseGenerationNode(int id_);
    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;
};
