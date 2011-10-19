/*
 *  chuck_ui.h
 *  chuck
 *
 *  Created by Spencer Salazar on 10/17/11.
 *  Copyright 2011 Spencer Salazar. All rights reserved.
 *
 */


class Chuck_UI_Manager
{
public:
    static Chuck_UI_Manager * instance();
    
    void go();
    void shutdown();
    
private:
    Chuck_UI_Manager();
};
