#include "textPrediction.h"
#include "presage.h"
#include <iostream>

WordPredictor::WordPredictor(const std::string& textString)
{
    ec = new EyewriterCallback(textString);
    presage = new Presage(ec);
}

WordPredictor::~WordPredictor() {
    delete ec;
    delete presage;
}


std::vector<std::string> WordPredictor::getPredictions() {
    return presage->predict();
}

