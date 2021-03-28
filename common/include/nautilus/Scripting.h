#pragma once
#include"Base.h"

namespace nautilus {

	namespace core {

		// By lua callable function...
		int luaDoSomething(lua_State* L);

		// Defines functions for lua virtual machine,
		// like:
		// "doSomething", luaDoSomething, where the second is the function pointer
		typedef int(*ScriptFunction)(lua_State* L); // Returns int, only luaState as param
		struct LuaFunctionBindingMap {

			std::map<std::string, ScriptFunction> map;
		};




		struct Script {

			std::string m_Filepath;



			Script(std::string filepath) : m_Filepath(filepath){};
		};





		class LuaBinding {
		public:

			static LuaBinding* get() {

				if (!g_pLua) g_pLua = new LuaBinding();

				return g_pLua;
			}

			static void del() {
				if (g_pLua) delete g_pLua;
			}


			static bool startUp(LuaFunctionBindingMap* luaFuncs = nullptr);

			static bool execute(Script script);


		private:

			static LuaBinding* g_pLua;


			lua_State* m_LuaState = nullptr; // Should be initialized once.

		private:

			LuaBinding() = default; // Default private constructor
			LuaBinding(const LuaBinding&) = delete; // No copy
			LuaBinding& operator=(const LuaBinding&) = delete; // No assignment




			static bool _validate(lua_State* L, int r, std::string errorBuffer);

		};
	}

}
