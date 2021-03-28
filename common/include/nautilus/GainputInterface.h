#pragma once

#include"Base.h"


#include"gainput/gainput.h"

#ifdef _DEBUG
#pragma comment(lib, "gainput-d.lib")
#else
#pragma comment(lib, "gainput.lib")
#endif


namespace nautilus {


	namespace hid {


		// Button definitions for input.
		//
		// Gainput requieres "enum" and does not allow "enum class".
		//
		// The Button enum defines possible input,
		// it is rather abstract and defines more what the user wants to do.
		//
		// In the Input Manager Map we can map these "abstract actions"
		// to specific  Keys(Keyboard), Buttons(Gamepad or Mouse).
		//
		enum Button{
			// Digital values: On or Off.
			Exit = 0,
			Confirm = 1,
			Move_Left = 2,
			Move_Right = 3,
			Move_Up = 4,
			Move_Down = 5,
			Turn_Right = 6,
			Turn_Left = 7,
			Action_Shoot = 8,
			Action_Activate_Shield = 9,
			Action_Special_Ability = 10,

			// Float values: has varying float.
			Move_Left_Float = 11,
			Move_Right_Float = 12,
			Move_Up_Float = 13,
			Move_Down_Float = 14,
			Turn_Right_Float = 15,
			Turn_Left_Float = 16,
			Action_Shoot_Float = 17,

			Max_Button_Count = Action_Shoot_Float,
		};
	

	}


}
