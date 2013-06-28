#include "offsetTraining.h"

offsetTraining::offsetTraining() : maxOffset(200), keySize(100), panelWidth(240)
{

}

offsetTraining::~offsetTraining()
{
    for(unsigned i = 0; i < drawableButtons.size(); ++i)
        delete drawableButtons[i];
}

void offsetTraining::setup()
{
    panel.setup("Options", 100, 100, panelWidth, 200);
    panel.setDraggable(false);
    panel.enableEvents();
    panel.addPanel("Options", 1, false);
    panel.setWhichPanel("Options");
    panel.addSlider("Offset", "OFFSET", 0, 0, maxOffset, true);
    panel.addToggle("Show Directions", "DIRECTIONS", false);

    oldOffset = panel.getValueI("OFFSET");

    target.setKeyID("Catch Me");
    target.setFontSize(12);
    generateNewPos();

    directions.loadImage("images/offsetTrainingInfo.png");
}

void offsetTraining::update(float mouseX, float mouseY)
{
    panel.setDimensions(panelWidth, ofGetHeight());
    panel.setPosition(ofGetWidth()-panelWidth, 0);
    panel.update();
    if(panel.getValueI("OFFSET") != oldOffset)
        generateOffset();
    oldOffset = panel.getValueI("OFFSET");

    cursorX = mouseX + offsetX;
    cursorY = mouseY + offsetY;

    target.setDimensions(mx, my, keySize, keySize, 1);

    showDirections = panel.getValueB("DIRECTIONS");
    if(showDirections)
        target.update(-1, -1);
    else
        target.update(cursorX, cursorY);
    if(target.getKeyOut()) {
        generateNewPos();
        generateOffset();
    }
}

void offsetTraining::draw()
{
    panel.setMinimized(false);
    panel.draw();

    ofEnableAlphaBlending();
    if(showDirections) {
        directions.draw(ofGetWidth()/2-directions.getHeight()/2-panel.getWidth(), ofGetHeight()/2-directions.getHeight()/2);
    } else {
        target.drawKey();
        ofSetColor(0, 255, 0, 120);
        ofFill();
        ofCircle(cursorX, cursorY, 20);
    }
    ofDisableAlphaBlending();
}

void offsetTraining::generateNewPos()
{
    mx = ofRandom(maxOffset, ofGetWidth()-keySize-maxOffset-panelWidth);
    my = ofRandom(maxOffset, ofGetHeight()-keySize-maxOffset);
}

void offsetTraining::generateOffset()
{
    float angle = ofRandom(0, 2*PI);
    offsetX = panel.getValueI("OFFSET")*cos(angle);
    offsetY = panel.getValueI("OFFSET")*sin(angle);
}

void offsetTraining::mousePressed(float x, float y, int button)
{
    panel.mousePressed(x, y, button);
}

void offsetTraining::mouseDragged(float x, float y, int button)
{
    panel.mouseDragged(x, y, button);
}

void offsetTraining::mouseReleased()
{
    panel.mouseReleased();
}
