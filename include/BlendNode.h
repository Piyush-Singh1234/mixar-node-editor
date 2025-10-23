


#pragma once
#include "Node.h"

// Define the different blend modes available
enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
    Difference
};

class BlendNode : public Node {
public:
    BlendNode(int _id);

    void render() override;
    cv::Mat process(const std::vector<cv::Mat>& inputs) override;

    // --- CRITICAL OVERRIDES FOR MULTI-INPUT NODES ---

    // 1. Tell the engine this node has TWO input attributes
    std::vector<int> getInputAttributes() const override {
        // Return a vector with the IDs for both input pins
        return { id * 10 + 1, id * 10 + 2 };
    }

    // 2. Validate that we received TWO valid images
    bool validateInputs(const std::vector<cv::Mat>& inputs) const override {
        // We need exactly two inputs, and neither can be empty
        return inputs.size() == 2 && !inputs[0].empty() && !inputs[1].empty();
    }

private:
    BlendMode mode = BlendMode::Normal;
    float opacity = 1.0f;
    float multiplyIntensity = 1.0f;
    float screenIntensity = 1.0f;
    float overlayIntensity = 1.0f;
    float differenceThreshold = 1.0f;
};