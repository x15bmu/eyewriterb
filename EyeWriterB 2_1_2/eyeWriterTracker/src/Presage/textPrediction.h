#ifndef TEXTPREDICTION_H_INCLUDED
#define TEXTPREDICTION_H_INCLUDED
#include "presage.h"
#include <string>
#include <vector>


class EyewriterCallback : public PresageCallback {
public:
    EyewriterCallback(const std::string& _past_context) : past_context(_past_context) {}

    std::string get_past_stream() const { return past_context; }
    std::string get_future_stream() const { return empty; }

private:
    const std::string& past_context;
    const std::string empty;
};

class ExampleCallback : public PresageCallback
{
public:
    ExampleCallback(const std::string& _past_context) : past_context(_past_context) { }

    std::string get_past_stream() const { return past_context; }
    std::string get_future_stream() const { return empty; }

private:
    const std::string& past_context;
    const std::string empty;

};


class WordPredictor {
    public:
        WordPredictor(const std::string& textString);
        std::vector<std::string> getPredictions();
        ~WordPredictor();
    private:
        WordPredictor();
        EyewriterCallback *ec;
        Presage *presage;
};


#endif

