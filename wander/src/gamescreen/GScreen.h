///////////////////////////////////////////////////////////////////
//
//  GScreen.h
//  wander
//
//  Created by Jacob Oaks on 2/16/19.
//  Copyright © 2019 Jacob Oaks. All rights reserved.
//
///////////////////////////////////////////////////////////////////

#ifndef GScreen_h
#define GScreen_h

// Includes

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "SaveManager.h"
#include "AnimGObject.h"
#include "Global.h"

///////////////////////////////////////////////////////////////////
// GScreen Class
// a generic class for a window/screen that should be kept discrete and distinct from others
// for example, this may be extended from to make the main menu screen, game screen, inventory screen, etc.
///////////////////////////////////////////////////////////////////

class GScreen {
    
public:
    
    ///////////////////////////////////////////////////////////////////
    // Constructor - takes window and debug references
    ///////////////////////////////////////////////////////////////////

    GScreen(sf::RenderWindow* w, sf::Text* d) {
        
        //set references
        this->w = w; //set window reference
        this->d = d; //set debug text reference
        
        //setup view
        this->v.setCenter(this->w->getSize().x / 2, this->w->getSize().y / 2);
        this->v.setSize(this->w->getSize().x, this->w->getSize().y);
        this->w->setView(v);
    }
    
    ///////////////////////////////////////////////////////////////////
    // the method which enters the screen and returns data once screen is finished
    ///////////////////////////////////////////////////////////////////

    virtual Node enter() {
        this->loop(); //loop

        //when loop is done, return a Node to summarize important takeaway from this screen
        //return data by default has a child node which indicates whether or not the
        //user indicated intent to return to the previous screen, or close to program entirely
        //the sending function should handle this accordingly
        rd.addC(Node("finish type", this->w->isOpen() ? "returned" : "closed"));
        return this->rd;
    }

protected:
    
    // SFML Objects
    sf::RenderWindow* w; //reference to the program's window
    sf::Text* d; //text which displays debug info
    sf::Clock c; //clock used for timekeeping purposes
    sf::View v; //view
    
    // Other Protected Data
    std::vector<AnimGObject*> gos; //vector of screen's game objects
    Node rd = Node("return data"); //the node of information which will be returned when the screen is finished
    bool intentToReturn = false; //represents whether user has shown intent to return to previous screen
    bool showDebug = false; //represents whether debug info should be shown on screen or

    ///////////////////////////////////////////////////////////////////
    // screen loop (input, compute, illustrate)
    ///////////////////////////////////////////////////////////////////

    virtual void loop() {
        while (!(this->intentToReturn) && this->w->isOpen()) { //continue looping until user closes or returns
            this->input(); //phase 1 - input
            this->compute(); //phase 2 - compute
            this->illustrate(); //phase 3 - illustrate
        }
    }
    
    ///////////////////////////////////////////////////////////////////
    // phase 1 of screen loop - gather user input
    // needs to be overriden to consider any input other than user attempting to close window
    ///////////////////////////////////////////////////////////////////

    virtual void input() {
        sf::Event e;
        while (this->w->pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed: //USER CLOSED WINDOW
                    this->w->close();
                    break;
                case sf::Event::KeyReleased: //USER PRESSED DEBUG KEY
                    if (e.key.code == gc::DEBUG_KEY)
                        this->showDebug = !this->showDebug;
                    break;
                case sf::Event::Resized: //USER RESIZED WINDOW
                    this->v.setSize(this->w->getSize().x, this->w->getSize().y);
                    this->w->setView(this->v);
                    break;
                default:
                    break;
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////
    // phase 2 of screen loop - compute
    // needs to be overriden to update anything other than the objects within gos
    ///////////////////////////////////////////////////////////////////

    virtual void compute() {
        for (AnimGObject* o : this->gos) //compute objects
            o->compute(this->c.getElapsedTime().asSeconds());
        if (this->showDebug) this->d->setString("FPS: " + std::to_string(1 / this->c.getElapsedTime().asSeconds())); //update debug text if enabled
        this->c.restart(); //restart clock
    }
    
    ///////////////////////////////////////////////////////////////////
    // phase 3 of screen loop - illustrate
    // needs to be overriden to draw anything other than the objects within gos
    ///////////////////////////////////////////////////////////////////

    virtual void illustrate() {
        this->w->clear(gc::CLEAR_COLOR); //clear screen
        for (AnimGObject* o : this->gos) //illustrate objects
            o->illustrate(this->w);
        if (this->showDebug) this->w->draw(*this->d); //draw debug if enabled
        this->w->display(); //display new screen
    }
};

#endif

///////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////
