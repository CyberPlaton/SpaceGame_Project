#include"Scripting.h"

namespace nautilus {

	namespace core {


		LuaBinding* LuaBinding::g_pLua = nullptr;



		// Lua callable functions.
		int luaDoSomething(lua_State* L) {

			float a = (float)lua_tonumber(L, 1);
			float b = (float)lua_tonumber(L, 2);

			float c = 256 * (a + b);

			lua_pushnumber(L, c); // Push result back to stack.

			return 1;
		}



		bool LuaBinding::startUp(LuaFunctionBindingMap* luaFuncs) {

			if (!g_pLua->m_LuaState) {

				g_pLua->m_LuaState = luaL_newstate();

				// Enable all standard libraries for lua.
				luaL_openlibs(g_pLua->m_LuaState);


				// Register specified functions.
				if (luaFuncs) {

					for (auto it : luaFuncs->map) {

						lua_register(g_pLua->m_LuaState, it.first.c_str(), it.second);
					}
				}



				return true;
			}
			return false;
		}

		bool LuaBinding::_validate(lua_State* L, int r, std::string errorBuffer) {

			if (r != LUA_OK) {

				errorBuffer = lua_tostring(L, -1);
				return false;
			}
		}


		bool LuaBinding::execute(Script script) {

			using namespace std;
			std::string errorBuffer;

			if (_validate(g_pLua->m_LuaState, luaL_dofile(g_pLua->m_LuaState, script.m_Filepath.c_str()), errorBuffer)) {


				return true;
			}
			else {

				cout << color(colors::RED);
				cout << "Script Error in \"" << script.m_Filepath << "\" :\n" << errorBuffer << white << endl;
				return false;
			}
		}


	}

}
