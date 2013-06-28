/*
	This file is part of EyeWriterB
	Copyright (C) 2013  Ben Mattinson <eyewriterb@gmail.com>
	
	EyeWriterB is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EyeWriterB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EyeWriterB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cameraOverlay.h"
#include "winman.h"
const char *cameraOverlay::overlayClassName =  "OverlayClass";

cameraOverlay::cameraOverlay(IplImage *img, int overlayWidth /* = 0 */, int overlayHeight /* = 0 */)
    : image(img), resizedImage(NULL), bitmap(NULL), bitmapData(NULL),
    width(overlayWidth), height(overlayHeight), opacity(120),
    resizeNeeded(false), initSuccess(true)
{
    ///////////////////////////////////////////////////////////////////
    // Create overlay window class
    ///////////////////////////////////////////////////////////////////
    WNDCLASSEX overlayClass = {0};
    overlayClass.cbSize = sizeof(WNDCLASSEX);
	overlayClass.lpfnWndProc = cameraOverlay::wndProc;
	overlayClass.hInstance = GetModuleHandle(NULL);
	overlayClass.lpszClassName = _T(overlayClassName);

    ///////////////////////////////////////////////////////////////////
    // Attempt to register overlay window class
    ///////////////////////////////////////////////////////////////////
	if(!RegisterClassEx(&overlayClass)) {
        MessageBox(NULL, _T("Failed to register overlay window class!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
        initSuccess = false;
        return; //Initialize failed. Cannot continue without window.
	}

    ///////////////////////////////////////////////////////////////////
    // Configure width and height of window
    ///////////////////////////////////////////////////////////////////
    int windowWidth, windowHeight;

    if(width > 0)
        windowWidth = width;
    else if(image != NULL && image->width > 0)
        windowWidth = image->width;
    else
        windowWidth = 100;

    if(height > 0)
        windowHeight = height;
    else if(image != NULL && image->height > 0)
        windowHeight = image->height;
    else
        windowHeight = 100;

    ///////////////////////////////////////////////////////////////////
    // Attempt to create window
    ///////////////////////////////////////////////////////////////////
	overlayWindow = CreateWindowEx( WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,   //Ex Window Styles
                                    _T(overlayClassName),                   //Class Name
                                    NULL,                                   //Window Title
                                    WS_POPUP,                               //Window Styles
                                    0,                                      //X
                                    0,                                      //Y
                                    windowWidth,                            //Width
                                    windowHeight,                           //Height
                                    NULL,                                   //Parent
                                    NULL,                                   //Menu
                                    GetModuleHandle(NULL),                  //HINSTANCE
                                    NULL);                                  //lpParam

    if(overlayWindow == NULL) {
        MessageBox(NULL, _T("Failed to create overlay window!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
        initSuccess = false;
        return; //Initialization failed. Cannot continue without window.
    }
    ShowWindow(overlayWindow, SW_SHOW);

    ///////////////////////////////////////////////////////////////////
    // Initialize Memory DC
    ///////////////////////////////////////////////////////////////////
    HDC hDC = GetDC(overlayWindow);
    memDC = CreateCompatibleDC(hDC);
    ReleaseDC(overlayWindow, hDC);

    ///////////////////////////////////////////////////////////////////
    // Get default bitmap so it can be later restored.
    ///////////////////////////////////////////////////////////////////
    oldBitmap = static_cast<HBITMAP>(GetCurrentObject(memDC, OBJ_BITMAP));

    ///////////////////////////////////////////////////////////////////
    // Check if resize and resizedImage required
    ///////////////////////////////////////////////////////////////////
    if(image != NULL && image->width > 0 && image->height > 0) {
        checkResizeNeeded();
        allocBitmap(resizeNeeded ? resizedImage : image);
    }
}

void cameraOverlay::setImage(IplImage *img)
{
    image = img;
    if(width == 0 || height == 0) {
        setImageSize((width == 0 && img->width > 0) ? img->width : width,
                     (height == 0 && img->height > 0) ? img->height : height);
    }
}

void cameraOverlay::setImageSize(int w, int h)
{
    if(w > 0)
        width = w;
    if(h > 0)
        height = h;
}

void cameraOverlay::setOpacity(int value)
{
    if(value < 0 || value > 255)
        return;
    opacity = value;
}

cameraOverlay::~cameraOverlay()
{
    if(overlayWindow && !DestroyWindow(overlayWindow)) {
        MessageBox(NULL, _T("Could not destroy window!"), _T("Error"), MB_OK | MB_ICONERROR);
    }
    ///////////////////////////////////////////////////////////////////
    // Wait for window to process remaining messages before terminating.
    ///////////////////////////////////////////////////////////////////
    MSG msg;
    while(GetMessage(&msg, overlayWindow, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if(!UnregisterClass(overlayClassName, GetModuleHandle(NULL))) {
        MessageBox(NULL, _T("Could not unregister window class!"), _T("Error"), MB_OK | MB_ICONERROR);
    }
    ///////////////////////////////////////////////////////////////////
    // Delete allocated resources
    ///////////////////////////////////////////////////////////////////
    SelectObject(memDC, oldBitmap);
    if(bitmap)
        destroyBitmap();
    DeleteDC(memDC);

    //Delete IplImages
    if(resizedImage) {
        cvReleaseImage(&resizedImage);
        resizedImage = NULL;
    }
}


bool cameraOverlay::drawImage(int x /* = 0 */, int y /* = 0 */)
{
    ///////////////////////////////////////////////////////////////////
    // Check failure conditions. Log if function fails.
    ///////////////////////////////////////////////////////////////////
    if(!initSuccess) {
        ofLog(OF_LOG_WARNING, "drawImage(int, int) failed. initSuccess = false.\
                                (Camera overlay did not initialize correctly");
        return false;
    }
    if(image == NULL) {
        ofLog(OF_LOG_WARNING, "drawImage(int, int) failed. image = NULL");
        return false;
    }
    if(image->width == 0 || image->height == 0) {
        ofLog(OF_LOG_WARNING, "drawImage(int, int) failed. image->width or image->height = 0");
        return false;
    }
    if(width == 0 || height == 0) {
        ofLog(OF_LOG_WARNING, "drawImage(int, int) failed. width or height = 0");
        return false;
    }

    checkResizeNeeded();
    if(!bitmap) {
        allocBitmap(resizeNeeded ? resizedImage : image);
    }

    ///////////////////////////////////////////////////////////////////
    // Convert image to bitmap
    ///////////////////////////////////////////////////////////////////
    if(resizeNeeded) {
        cvResize(image, resizedImage);         //resize image if necesary
        checkImgPropsChanged(resizedImage);
        cvConvert(resizedImage, &bitmapMat);    //convert image to bitmap
    } else {
        checkImgPropsChanged(image);
        cvConvert(image, &bitmapMat);           //convert image to bitmap
    }

    ///////////////////////////////////////////////////////////////////
    // Draw bitmap on screen
    ///////////////////////////////////////////////////////////////////
    SelectObject(memDC, bitmap);
    POINT windowOrigin = {x, y};
    SIZE size = {bitmapWidth, abs(bitmapHeight)};
    POINT imageOrigin = {0, 0};
    BLENDFUNCTION blendFunction = {AC_SRC_OVER, 0, opacity, 0};
    UpdateLayeredWindow(overlayWindow, NULL, &windowOrigin, &size, memDC, &imageOrigin, 0, &blendFunction, ULW_ALPHA);

    ///////////////////////////////////////////////////////////////////
    // Check for 1 message. If exists, process it.
    ///////////////////////////////////////////////////////////////////
    MSG msg;
    if(PeekMessage(&msg, overlayWindow, 0, 0, PM_REMOVE) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

bool cameraOverlay::checkResizeNeeded()
{
    if(!image) {
        ofLog(OF_LOG_WARNING, "checkResizeNeeded() failed. Image = NULL.");
        return false;
    }

    //Check if resize needed this time
    bool resize = (image->height != height || image->width != width);
    //Compare this with last time
    if(resize == resizeNeeded) {
        //Check if resizedImage size must change, if not return
        if(resizeNeeded && resizedImage->width == width && resizedImage->height == height)
            return true;
    }

    //If resize now needed an not needed before or vise versa, allocate or deallocate image as necessary
    resizeNeeded = resize;
    if(resizeNeeded) {
        //Delete resizedImage if exists
        if(resizedImage)
            cvReleaseImage(&resizedImage);
        //Allocate image
        resizedImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, image->nChannels);
        checkImgPropsChanged(resizedImage);
    } else {
        //Delete allocated image
        if(resizedImage != NULL) {
            cvReleaseImage(&resizedImage);
            resizedImage = NULL;
        }
        checkImgPropsChanged(image);
    }
    return true;
}

bool cameraOverlay::checkImgPropsChanged(IplImage *img)
{
    if(img == NULL) {
        ofLog(OF_LOG_WARNING, "checkImgPropsChanged(IplImage*) failed. img = NULL");
        return false;
    }
    if( img->width != bitmapWidth ||
            img->height != bitmapHeight ||
            8*img->nChannels != bitmapBitCount) {
        //Properties changed
        if(bitmap != NULL)
            destroyBitmap();
        allocBitmap(img);
    }
    return true;
}

void cameraOverlay::destroyBitmap()
{
    SelectObject(memDC, oldBitmap);
    DeleteObject(bitmap);
    bitmap = NULL;
}

bool cameraOverlay::allocBitmap(IplImage *img)
{
    if(img->width == 0 || img->height == 0) {
        ofLog(OF_LOG_WARNING, "allocBitmap(IplImage*) failed. img width/height = 0.");
        return false;
    }
    ///////////////////////////////////////////////////////////////////
    // Set number of bits per pixel.
    ///////////////////////////////////////////////////////////////////
    int bpp = img->nChannels * 8;
    assert(bpp == 8 || bpp == 24 || bpp == 32);

    ///////////////////////////////////////////////////////////////////
    // Allocate bitmap headers
    ///////////////////////////////////////////////////////////////////
    unsigned char buffer[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];
    BITMAPINFO* bmi = (BITMAPINFO*)buffer;
    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

    ///////////////////////////////////////////////////////////////////
    // Set bitmap header data and variables.
    ///////////////////////////////////////////////////////////////////
    ZeroMemory(bmih, sizeof(BITMAPINFOHEADER));
    bmih->biSize = sizeof(BITMAPINFOHEADER);
    bmih->biWidth = img->width;
    bmih->biHeight = (img->origin == 1) ? abs(img->height) : -abs(img->height);
    bmih->biPlanes = 1;
    bmih->biBitCount = bpp;
    bmih->biCompression = BI_RGB;

    bitmapWidth = bmih->biWidth;
    bitmapHeight = bmih->biHeight;
    bitmapBitCount = bmih->biBitCount;

    ///////////////////////////////////////////////////////////////////
    // Initialize palette if necessary. (If not 8 bpp, palette can be NULL).
    ///////////////////////////////////////////////////////////////////
    if (bpp == 8) {
        RGBQUAD* palette = bmi->bmiColors;
        int i;
        for (i = 0; i < 256; i++) {
            palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = (BYTE)i;
            palette[i].rgbReserved = 0;
        }
    }

    ///////////////////////////////////////////////////////////////////
    // Allocate memory and create bitmap and OpenCV mat.
    ///////////////////////////////////////////////////////////////////
    bitmap = CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, &bitmapData, 0, 0);

    int type;
    switch(img->nChannels) {
        case 1:
            type = CV_8UC1;
            break;
        case 2:
            type = CV_8UC2;
            break;
        case 3:
            type = CV_8UC3;
            break;
        case 4:
            type = CV_8UC4;
            break;
        default:
            assert(0); //Other channel amounts not supported
    }
    cvInitMatHeader(&bitmapMat, img->height, img->width, type,
                    bitmapData, (img->width * img->nChannels));

    ///////////////////////////////////////////////////////////////////
    // Convert IplImage to OpenCV matrix
    ///////////////////////////////////////////////////////////////////
    cvConvert(img, &bitmapMat);
    return true;
}

LRESULT CALLBACK cameraOverlay::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
        case WM_DESTROY:
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
