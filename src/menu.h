// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef MENU_H_
#define MENU_H_

#include "globals.h"
#include "point.h"

class menu
{
    public:
        menu();
        menu(point,point);
        std::string getMenuItem(int);
        std::string getMenuTitle();
        void cleanupEverything();
        void setMenuTitle(std::string);
        void addMenuItem(std::string);
        void eraseMenuItem(int);
        void incSelectionIndex(int);
        void setSelectionIndex(int);
        void setSelectionChangeFlag(bool);
        void setMenuItemString(int,std::string);
        void activateSelectionChangeTimer();
        bool canChangeSelection();
        int getSelectionIndex();
        int getNumMenuItems();
        point getLoc();
        point getArea();
    private:
        point loc;
        point area;
        int selection_index;
        int selection_change_timer;
        bool can_change_selection;
        std::string menu_title;
        std::vector<std::string> menu_items;
};

#endif
