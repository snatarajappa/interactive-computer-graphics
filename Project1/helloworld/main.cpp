//
//  main.cpp
//  ICG
//  Created by Santhosh Natarajappa.
//
#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <cmath>

//freeglut
#include <GL/freeglut.h>

struct color
{
    float r, g, b;
};

void handleDisplay()
{
    //Clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Rendering part
    color startingColor = { 0.1, 0.4, 0.5};
    color endingColor = { 1.0, 1.0, 1.0};
    //cosine interpolation
    const int currentTime = glutGet(GLUT_ELAPSED_TIME);
    const float time = (1-(std::cos(float(currentTime)*0.001)))/2;
    color interpolatedColor;
    interpolatedColor.r = startingColor.r + (endingColor.r - startingColor.r)*time;
    interpolatedColor.g = startingColor.g + (endingColor.g - startingColor.g)*time;
    interpolatedColor.b = startingColor.b + (endingColor.b - startingColor.b)*time;
    glClearColor(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, 1.0);
    
    //Swap buffers
    glutSwapBuffers();
    
}

void handleKeyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: //ESC
            glutLeaveMainLoop();
            break;
    }
}

void handleSpecialKeyPress(int key, int x, int y)
{
    
}

void handleMouseClick(int button, int state, int x, int y){
    
}

void handleMouseMotion(int x, int y){
    
}

void handleIdle()
{
    //Tell GLUT to redraw
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    //Initialize freeglut
    glutInit(&argc, argv);
    //Create a window
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Project 1 - Hello World");
    //Register display callback function
    glutDisplayFunc(handleDisplay);
    //Register keyboard callback function
    glutKeyboardFunc(handleKeyPress);
    //Register special keyboard callback function
    glutSpecialFunc(handleSpecialKeyPress);
    //Register mouse buttons callback functions
    glutMouseFunc(handleMouseClick);
    //Register mouse movement callback function
    glutMotionFunc(handleMouseMotion);
    //Register idle callback function
    glutIdleFunc(handleIdle);
    //Call main loop
    glutMainLoop();
    return 0;
}
