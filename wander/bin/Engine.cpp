//
//  Engine.cpp
//  wander
//
//  Created by Jacob Oaks on 2/11/19.
//  Copyright © 2019 Jacob Oaks. All rights reserved.
//

#include "Engine.h"

//ran by main.cpp. initializes engine and contains main game loop
int Engine::run() {
    
    //iniitialize engine
    this->init();
    
    gf::error("Engine.h", "This is a test error", 0);
      
    //program loop
    while (w.isOpen()) { //when window is closed, exit status is returned
        
        this->input(); //phase 1 of cycle - gather input
        this->compute(); //phase 2 of cycle - update everything
        this->illustrate(); //phase 3 of cycle - draw everything
    }
    
    //program over
    return gc::SUCCESS;
}

//initializes all values/members of engine
void Engine::init() {
    
    //window init
    int w_width = 800, w_height = 600; //window width/height
    this->loadSettings(&w_width, &w_height); //TODO: load width/height from a settings file
    this->w.create(sf::VideoMode(w_width, w_height), gc::W_TITLE, sf::Style::Default); //create window
    this->w.setFramerateLimit(gc::W_FRAMERATE_LIMIT); //limit framerate
    
    //debug text init
    this->f.loadFromFile("res//sansation.ttf"); //load font
    this->debug.setFont(this->f); //set font
    this->debug.setCharacterSize(24); //set text size
    this->debug.setString("FPS"); //set text
    this->debug.setFillColor(sf::Color::Black); //set text color
    this->showDebug = false; //hide debug by default
    
    //gobject vector init
    this->gos.push_back(new AnimGObject("res//dragon.png", 3, 4, 0.1f));
    
    //misc init
    this->c = sf::Clock(); //create clock
}

//phase 1 - handles all window input
void Engine::input() {
    
    sf::Event e; //event
    while (this->w.pollEvent(e)) { //loop through pending events
        
        if (e.type == sf::Event::Closed) //closed event
            this->w.close(); //close window
        if (e.type == sf::Event::KeyPressed) { //key pressed event
            switch (e.key.code) { //switch on type of key
                case sf::Keyboard::D:
                    this->showDebug = !this->showDebug; //toggle debug display
                    break;
                case sf::Keyboard::Left:
                    this->gos[0]->setFrameRow(2);
                    break;
                case sf::Keyboard::Right:
                    this->gos[0]->setFrameRow(3);
                    break;
                case sf::Keyboard::Down:
                    this->gos[0]->setFrameRow(1);
                    break;
                case sf::Keyboard::Up:
                    this->gos[0]->setFrameRow(4);
                    break;
                default:
                    break;
            }
        }
    }
}

//phase 2 - update everything
void Engine::compute() {
    
    //update GameObjects
    for (AnimGObject*o : this->gos)
        o->compute(this->c.getElapsedTime().asSeconds());
    
    //update additionals
    if (this->showDebug) {
        int FPS = 1 / this->c.getElapsedTime().asSeconds();
        this->debug.setString("FPS: " + std::to_string(FPS)); //show debug info
    }
    
    //restart clock
    this->c.restart();
}

//phase 3 - illustrate everything
void Engine::illustrate() {
    
    //clear screen
    this->w.clear(sf::Color::White);
    
    //draw GameObjects
    for (AnimGObject* o : this->gos)
        o->illustrate(&this->w);
    
    //draw additionals
    if (this->showDebug)
        this->w.draw(this->debug);
    
    //publish newly drawn screen
    this->w.display();
}

void Engine::loadSettings(int* w_width, int* w_height) {
    
}

void Engine::saveSettings() {
    
}
