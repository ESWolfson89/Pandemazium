// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef INPUT_H_
#define INPUT_H_

#include "globals.h"
#include "point.h"

class input
{

    public:
	input();

	void processKey();

	void setDelta(point);

	void pollEvent();

	point getDelta();

	bool pickupKeyPressed();

	bool deltaKeyPressed();

	bool jumpKeyPressed();

	bool quitKeyPressed();

	bool noKeyPressed();

	bool fireKeyPressed();

	bool onLadderKeyPressed();

	bool offLadderKeyPressed();

	bool downLadderKeyPressed();

	bool useLevelFeaturePressed();

	bool selectKeyPressed();

	bool pauseKeyPressed();

        bool shiftKeyPressed();

        bool plusKeyPressed();

        bool minusKeyPressed();

        bool toggleCarryItemKeyPressed();

     private:
	SDL_Event evt;
	point delta;
        bool delta_key_pressed;
	bool jump_key_pressed;
	bool quit_key_pressed;
	bool pickup_key_pressed;
	bool fire_key_pressed;
        bool on_ladder_key_pressed;
        bool off_ladder_key_pressed;
        bool down_ladder_key_pressed;
        bool use_level_feature_pressed;
        bool select_key_pressed;
        bool pause_key_pressed;
        bool shift_key_pressed;
        bool toggle_carry_item_pressed;
        bool plus_key_pressed;
        bool minus_key_pressed;
};

#endif
