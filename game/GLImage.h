//
//  GLImage.h
//  Glypha
//
//  Created by Kevin Wojniak on 7/23/12.
//  Copyright (c) 2012 Kevin Wojniak. All rights reserved.
//

#ifndef GLIMAGE_H
#define GLIMAGE_H

#include <stddef.h>
#include <OpenGL/OpenGL.h>
#include <ApplicationServices/ApplicationServices.h>
#include "GLRect.h"

class GLImage {
public:
    GLImage(void *buf, size_t bufSize);
    
    void draw(GLRect destRect, GLRect srcRect);
    void draw(GLRect destRect);
    void draw(int x, int y);
    
    int width();
    int height();

private:
	void loadTextureData_(void *texData);
    
    GLuint texture_;
    int width_, height_;
};

#endif