#include "NoiseGenerationNode.h"
#include "imgui.h"
#include "imnodes.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>

NoiseGenerationNode::NoiseGenerationNode(int id)
    : Node(id, "Noise Generation"),
      scale(0.1f), octaves(4), persistence(0.5f),
      width(512), height(512),
      noiseType(NoiseType::Perlin),
      outputType(OutputType::Color) {}

void NoiseGenerationNode::render() {
    ImNodes::BeginNode(id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("Noise Generation");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginOutputAttribute(id * 10 + 2);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();

    const char* noiseTypes[] = {"Perlin", "Simplex", "Worley"};
    ImGui::Combo("Noise Type", &selected, noiseTypes, IM_ARRAYSIZE(noiseTypes));
    noiseType = static_cast<NoiseType>(selected);

    const char* outputTypes[] = {"Color", "Displacement"};
    ImGui::Combo("Output Type", &outputSelected, outputTypes, IM_ARRAYSIZE(outputTypes));
    outputType = static_cast<OutputType>(outputSelected);

    // Parameter validation
    ImGui::SliderFloat("Scale", &scale, 0.01f, 0.5f);
    ImGui::SliderInt("Octaves", &octaves, 1, 8);
    ImGui::SliderFloat("Persistence", &persistence, 0.1f, 1.0f);
    
    ImGui::InputInt("Width", &width);
    width = std::clamp(width, 64, 4096);
    ImGui::InputInt("Height", &height);
    height = std::clamp(height, 64, 4096);

    ImNodes::EndNode();
}

cv::Mat NoiseGenerationNode::process(const std::vector<cv::Mat>& inputs) {
    cv::Size size(width, height);
    
    // Validate dimensions
    if (size.width <= 0 || size.height <= 0) {
        std::cerr << "[Noise] Invalid size, using 512x512\n";
        size = cv::Size(512, 512);
    }

    // Use input size if valid
    if (!inputs.empty() && !inputs[0].empty()) {
        cv::Size inputSize = inputs[0].size();
        if (inputSize.width > 0 && inputSize.height > 0) {
            size = inputSize;
        }
    }

    cv::Mat output;
    try {
        switch (noiseType) {
            case NoiseType::Perlin: generatePerlinNoise(output, size); break;
            case NoiseType::Simplex: generateSimplexNoise(output, size); break;
            case NoiseType::Worley: generateWorleyNoise(output, size); break;
        }
    } catch (const std::exception& e) {
        std::cerr << "[Noise] Generation failed: " << e.what() << "\n";
        output = cv::Mat::zeros(size, CV_32FC1);
    }

    // Visualization adjustments
    if (outputType == OutputType::Color) {
        // Apply color map to COLOR output
        cv::Mat display;
        cv::normalize(output, display, 0, 255, cv::NORM_MINMAX, CV_8UC1);
        cv::applyColorMap(display, output, cv::COLORMAP_VIRIDIS); // Use any color map
    } else {
        // Keep displacement as grayscale (single-channel)
        // Optional: Convert to BGR for consistency with other nodes
        cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
    }


    return output;
}

//  Perlin noise implementation
void NoiseGenerationNode::generatePerlinNoise(cv::Mat& output, const cv::Size& size) {
    if (size.width <= 0 || size.height <= 0) {
        throw std::invalid_argument("Invalid size for Perlin noise");
    }

    output.create(size, CV_32FC1);
    output.setTo(0);
    float maxAmplitude = 0.0f;

    for (int o = 0; o < octaves; ++o) {
        const float frequency = pow(2, o) * scale;
        const float amplitude = pow(persistence, o);
        
        // Calculate octave size
        const int octaveWidth = std::max(1, static_cast<int>(size.width / frequency));
        const int octaveHeight = std::max(1, static_cast<int>(size.height / frequency));

        // Generate base noise for this octave
        cv::Mat octaveNoise(octaveHeight, octaveWidth, CV_32FC1);
        cv::randu(octaveNoise, 0.0f, 1.0f);

        // Upscale to target size
        cv::Mat upscaledNoise;
        cv::resize(octaveNoise, upscaledNoise, size, 0, 0, cv::INTER_CUBIC);
        
        output += amplitude * upscaledNoise;
        maxAmplitude += amplitude;
    }

    if (maxAmplitude > 0) {
        output /= maxAmplitude;
    }
    cv::normalize(output, output, 0, 1, cv::NORM_MINMAX);
}

//  Simplex noise implementation
void NoiseGenerationNode::generateSimplexNoise(cv::Mat& output, const cv::Size& size) {
    if (size.width <= 0 || size.height <= 0) {
        throw std::invalid_argument("Invalid size for Simplex noise");
    }

    output.create(size, CV_32FC1);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            const float nx = x * scale / size.width;
            const float ny = y * scale / size.height;
            const float noise = 0.5f * (1.0f + sin(2 * CV_PI * nx + 3 * CV_PI * ny));
            output.at<float>(y, x) = noise;
        }
    }
}

//  Worley noise implementation
void NoiseGenerationNode::generateWorleyNoise(cv::Mat& output, const cv::Size& size) {
    if (size.width <= 0 || size.height <= 0) {
        throw std::invalid_argument("Invalid size for Worley noise");
    }

    output.create(size, CV_32FC1);
    const int numPoints = 20;
    std::vector<cv::Point2f> points(numPoints);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0, 1);

    // Generate random feature points
    for (auto& p : points) {
        p.x = dist(gen) * size.width;
        p.y = dist(gen) * size.height;
    }

    // Calculate distance to nearest point
    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            float minDist = std::numeric_limits<float>::max();
            for (const auto& p : points) {
                const float dx = x - p.x;
                const float dy = y - p.y;
                const float dist = sqrt(dx*dx + dy*dy);
                minDist = std::min(minDist, dist);
            }
            output.at<float>(y, x) = 1.0f - cv::saturate_cast<float>(minDist / (size.width/4.0f));
        }
    }
}
