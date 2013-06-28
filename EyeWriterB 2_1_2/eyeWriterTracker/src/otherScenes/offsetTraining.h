#ifndef OFFSETTRAINING_H
#define OFFSETTRAINING_H

#include "baseScene.h"
#include "ofxControlPanel.h"
#include "key.h"


class offsetTraining : public baseScene
{
    public:
        /** Default constructor */
        offsetTraining();
        /** Default destructor */
        virtual ~offsetTraining();

        virtual void setup();
        virtual void update(float mouseX, float mouseY);
        virtual void draw();

        virtual void mousePressed(float x, float y, int button);
        virtual void mouseDragged(float x, float y, int button);
        virtual void mouseReleased();

        const int maxOffset;
        const int keySize;
        const int panelWidth;

    protected:
        ofImage directions;
        float offsetX;
        float offsetY;
        void generateNewPos();
        void generateOffset();

    private:
        bool showDirections;
        int cursorX;
        int cursorY;
        ofxControlPanel panel;
        int oldOffset;
        key target;
};

#endif // OFFSETTRAINING_H
