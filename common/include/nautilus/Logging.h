#pragma once
#ifndef LOGGING_H
#define LOGGING_H


#include"Base.h"
#include"Singleton.h"


namespace nautilus {

	namespace core {


#define g_pLogfile Logfile::Get()
		class Logfile : public TSingleton<Logfile> {
		public:


			enum FONTCOLORS {
				Log_BLACK = 0,
				Log_RED,
				Log_GREEN,
				Log_BLUE,
				Log_PURPLE,
				Log_AQUA,
				Log_PINK,
				Log_GOLD,
				Log_ORANGE,
				Log_LIME,

				Log_DEFAULT_COLOR // Do not touch. Internal use.
			};

			enum FONTFAMILY {

				Arial = 0,
				Helvetica = 1,
				Times_New_Roman = 2,
				Arial_Narrow = 3,
				Calibri = 4,
				Noto = 5,
				Futara = 6
			};

			enum FONTSTYLE {

				Bold = 0,
				Cursive = 1,
				Normal = 2
			};

			Logfile() {};
			~Logfile() {};


			// Closes all Logfiles and calls destructor.
			void shutDown();


			void FunctionResult(const char* funcName, bool result, std::string logName = "");
			void FunctionResult(const char* funcName, bool result, const char* errorDesc, std::string logName = "");


			// Write to specified logfile or to currently open one.
			void Textout(const char* Text, std::string logName = "");
			void Textout(FONTCOLORS Color, const char* Text, std::string logName = "");

			// Create logfile with specified name or if it already exist, just open it.
			void CreateLogfile(std::string logName);

			// Note:
			// To attach script, specify path as follows:
			// E.g. "/DirectX11App/DebugViewer/RefreshScript.js"
			void CreateLogfile(std::string logName, std::string script);

			// Write formatted header.
			void WriteTopic(const char* topic, int size,
				FONTSTYLE style, FONTFAMILY font, FONTCOLORS color,
				FONTCOLORS backgroundcolor = Log_DEFAULT_COLOR, bool horizontalRule = false, std::string logName = "");

			// Return handle to specified Logfile.
			FILE* GetLogfile(std::string logName);

			// TODO:
			// Currently these functions print numbers out to the current open logfile.
			//
			// Print any number with some formatting.
			template<typename T>
			void Log_PrintNum(T num);
			template<typename T>
			void Log_PrintNum(int Color, T num, ...);
			//template<typename T>
			//void Log_PrintNum(int Color, bool List, T num, ...);


			bool GetInitFlag() { return m_pLogfileLoaded; }

			//Returns the last Win32 error or an empty string if there was none.
			std::string GetLastErrorAsString();


			bool Log_OK = true;
			bool Log_FAIL = false;

		private:

			std::map<std::string, FILE*> m_LogfileMap;
			FILE* m_pCurrentOpenLogfile = 0;

			bool m_pLogfileLoaded = false;

		private:

			void _fTextout(const char* Text, std::string logName = "", ...);
			void _fTextout(FONTCOLORS Color, const char* Text, std::string logName = "", ...);
			void _fTextout(FONTCOLORS Color, bool List, const char* Text, std::string logName = "", ...);

			template <typename T>
			std::string _numToString(T num);
		};




		// TEMPLATE FUNCTIONS DEFINITIONS

		template <typename T>
		std::string Logfile::_numToString(T num) {

			std::stringstream Stream;
			Stream << num;
			return(Stream.str());
		}


		template <typename T>
		void Logfile::Log_PrintNum(T num) {

			this->Textout(_numToString(num).c_str());
		}


		template <typename T>
		void Logfile::Log_PrintNum(int Color, T num, ...) {

			this->Textout(Logfile::FONTCOLORS(Color), _numToString(num).c_str());
		}
	

	}


}


#endif // !LOGGINF_H