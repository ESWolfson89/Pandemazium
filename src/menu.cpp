// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "menu.h"

menu::menu()
{
    selection_index = 0;
}

menu::menu(point l, point a)
{
    loc = l;
    area = a;
    selection_index = 0;
    selection_change_timer = 0;
    menu_title = "";
    can_change_selection = false;
}

std::string menu::getMenuItem(int index)
{
    return menu_items[index];
}

void menu::cleanupEverything()
{
    std::vector<std::string>().swap(menu_items);
}

void menu::setMenuTitle(std::string mt)
{
    menu_title = mt;
}

std::string menu::getMenuTitle()
{
    return menu_title;
}

void menu::eraseMenuItem(int i)
{
    menu_items.erase(menu_items.begin() + i);
}

void menu::addMenuItem(std::string m)
{
    menu_items.push_back(m);
}

void menu::incSelectionIndex(int inc)
{
    selection_index += inc;
    if (selection_index > (int)menu_items.size() - 1)
        selection_index = 0;
    if (selection_index < 0)
        selection_index = (int)menu_items.size() - 1;
}

int menu::getNumMenuItems()
{
    return (int)menu_items.size();
}

void menu::setSelectionIndex(int i)
{
    if (i >= 0 && i <= (int)menu_items.size() - 1)
        selection_index = i;
}

void menu::setMenuItemString(int i, std::string s)
{
    menu_items[i] = s;
}

void menu::activateSelectionChangeTimer()
{
    if (!can_change_selection)
    {
        selection_change_timer++;
        if (selection_change_timer % 10 == 0)
            selection_change_timer = 0;
        if (selection_change_timer == 0)
            can_change_selection = true;
    }
}

void menu::setSelectionChangeFlag(bool scf)
{
    can_change_selection = scf;
}

int menu::getSelectionIndex()
{
    return selection_index;
}

bool menu::canChangeSelection()
{
    return can_change_selection;
}

point menu::getLoc()
{
    return loc;
}

point menu::getArea()
{
    return area;
}
