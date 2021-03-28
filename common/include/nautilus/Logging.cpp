#include"Logging.h"



namespace nautilus {

	namespace core {


		void Logfile::shutDown() {

			int i = 0;
			for (auto it : m_LogfileMap) {

				m_pCurrentOpenLogfile = it.second;

				//write logfile-end and close opened file
				Textout("<br><br><hr />LOGFILE END</font></body></html>");
				i = fclose(m_pCurrentOpenLogfile);
			}


			if (g_pLogfile->m_pLogfileLoaded) g_pLogfile->m_pLogfileLoaded = false;
			g_pLogfile->Del();
		}

		FILE* Logfile::GetLogfile(std::string logName) {

			return m_LogfileMap.at(logName);
		}


		void Logfile::Textout(const char* Text, std::string logName) {

			_fTextout(Text, logName);
		}


		void Logfile::Textout(FONTCOLORS Color, const char* Text, std::string logName) {

			_fTextout(Color, Text, logName);
		}



		void Logfile::CreateLogfile(std::string logName) {

			if (!g_pLogfile->m_pLogfileLoaded) g_pLogfile->m_pLogfileLoaded = true;


			//create logfile and write header
			m_LogfileMap.insert(std::make_pair(logName, fopen(logName.c_str(), "w")));

			// set current Logfile
			m_pCurrentOpenLogfile = m_LogfileMap.at(logName);

			Textout("<html><head><title>");
			Textout(logName.c_str());
			Textout("</title></head>");
			Textout("<body><font face = 'Noto'>");
			WriteTopic("ENGINE LOGFILE", 5, g_pLogfile->Bold, g_pLogfile->Noto, g_pLogfile->Log_BLACK);


			//give out build-configuration
#ifdef _DEBUG
			Textout("BUILD: DEBUG <br>");
#else
			Textout("BUILD: RELEASE <br>");
#endif // _DEBUG

			Textout("<hr/>"); // horizontal line after opening words.

			//close and directly open the new logfile
			fclose(m_LogfileMap.at(logName));
			m_pCurrentOpenLogfile = fopen(logName.c_str(), "a");//a = read mode
		}



		void Logfile::CreateLogfile(std::string logName, std::string script) {

			if (!g_pLogfile->m_pLogfileLoaded) g_pLogfile->m_pLogfileLoaded = true;


			//create logfile and write header
			m_LogfileMap.insert(std::make_pair(logName, fopen(logName.c_str(), "w")));

			// set current Logfile
			m_pCurrentOpenLogfile = m_LogfileMap.at(logName);

			Textout("<html><head><title>");
			Textout(logName.c_str());
			Textout("</title></head>");

			// Attach specified script.
			Textout("<script src ='");
			Textout(script.c_str());
			Textout("'></script>");

			Textout("<body><font face = 'Noto'>");
			WriteTopic("ENGINE LOGFILE", 5, g_pLogfile->Bold, g_pLogfile->Noto, g_pLogfile->Log_BLACK);


			//give out build-configuration
#ifdef _DEBUG
			Textout("BUILD: DEBUG <br>");
#else
			Textout("BUILD: RELEASE <br>");
#endif // _DEBUG

			Textout("<hr/>"); // horizontal line after opening words.

			//close and directly open the new logfile
			fclose(m_LogfileMap.at(logName));
			m_pCurrentOpenLogfile = fopen(logName.c_str(), "a");//a = read mode

		}


		void Logfile::WriteTopic(const char* topic, int size,
			FONTSTYLE style, FONTFAMILY font, FONTCOLORS color, FONTCOLORS backgroundcolor, bool horizontalRule, std::string logName) {


			const char* f = 0;
			switch (font) {
			case 0:
				f = "'Arial'";
				break;
			case 1:
				f = "'Helvetica'";
				break;
			case 2:
				f = "'Times New Roman'";
				break;
			case 3:
				f = "'Arial Narrow'";
				break;
			case 4:
				f = "'Calibri'";
				break;
			case 5:
				f = "'Noto'";
				break;
			case 6:
				f = "'Futara'";
				break;
			default:
				f = "'Arial'";
				break;
			}

			const char* c = 0;
			switch (color)
			{
			case Log_BLACK:
				c = "font color = 'black';"; break;
			case Log_RED:
				c = "font color = 'red';"; break;
			case Log_GREEN:
				c = "font color = 'green';"; break;
			case Log_BLUE:
				c = "font color = 'blue';"; break;
			case Log_PURPLE:
				c = "font color = 'darkmagenta';"; break;
			case Log_AQUA:
				c = "font color = 'aqua';"; break;
			case Log_PINK:
				c = "font color = 'fuchsia';"; break;
			case Log_GOLD:
				c = "font color = 'gold';"; break;
			case Log_ORANGE:
				c = "font color = 'orange';"; break;
			case Log_LIME:
				c = "font color = 'lime';"; break;
			default:
				c = "font color = 'black';"; break;
			}

			const char* bc = 0;
			switch (backgroundcolor)
			{
			case Log_BLACK:
				bc = "bgcolor = black"; break;
			case Log_RED:
				bc = "bgcolor = red"; break;
			case Log_GREEN:
				bc = "bgcolor = green"; break;
			case Log_BLUE:
				bc = "bgcolor = blue"; break;
			case Log_PURPLE:
				bc = "bgcolor = darkmagenta"; break;
			case Log_AQUA:
				bc = "bgcolor = aqua"; break;
			case Log_PINK:
				bc = "bgcolor = fuchsia"; break;
			case Log_GOLD:
				bc = "bgcolor = gold"; break;
			case Log_ORANGE:
				bc = "bgcolor = orange"; break;
			case Log_LIME:
				bc = "bgcolor = lime"; break;
			default:
				bc = "bgcolor = white"; break;
			}

			const char* s = 0;
			switch (style)
			{
			case Logfile::Bold:
				s = "font style = 'bold';"; break;
			case Logfile::Cursive:
				s = "font style = 'cursive';"; break;
			case Logfile::Normal:
				s = "font style = 'normal';"; break;
			default:
				s = "font style = 'normal';"; break;
			}


			std::string log;
			if (strcmp("", logName.c_str()) == 0) {

				log = "";
			}
			else {

				log = logName;
			}

			//write header and flush
			Textout("<table cellSpacing='0' cellPadding ='0' width='100%%'", log);

			// font and font color
			Textout(bc, log);
			Textout(">\n<tr>\n<td>\n<font face=", log);
			Textout(f, log);
			Textout(c, log);
			Textout(s, log);

			Textout((" size = '" + std::to_string(size) + "'>\n").c_str(), log);
			Textout(topic, log);
			Textout("</font>\n</td>\n</tr>\n</table>\n<br>", log);

			if (horizontalRule) Textout("<hr/>", log);


			if (strcmp(log.c_str(), "") == 0) { // Flush to currently open Logfile

				fflush(m_pCurrentOpenLogfile);
			}
			else { // Find Logfile by name and flush to it

				fflush(m_LogfileMap.at(log));
			}
		}


		void Logfile::FunctionResult(const char* funcName, bool result, std::string logName) {

			std::string log;

			if (strcmp("", logName.c_str()) == 0) {

				log = "";
			}
			else {
				log = logName;
			}


			if (Log_OK == result || true == result)
			{
				Textout("<table width = '100%%' cellSpacing='1' cellPadding = '5'", log);
				Textout("border='0' bgcolor ='#C0C0C0'><tr><td bgcolor =", log);
				Textout(("'#FFFFFF' width ='35%%' >" + std::string(funcName) + "</TD>").c_str(), log);
				Textout("<td bgcolor='#FFFFFF' width='30%%'><font color=", log);
				Textout("'green'>OK</FONT></TD><td bgcolor='#FFFFFF'", log);
				Textout("width='35%%'>-/-</TD></tr></table>", log);
			}
			else if (Log_FAIL == result || false == result)
			{
				Textout("<table width = '100%%' cellSpacing='1' cellPadding = '5'", log);
				Textout("border='0' bgcolor ='#C0C0C0'><tr><td bgcolor =", log);
				Textout(("'#FFFFFF' width ='35%%' >" + std::string(funcName) + "</TD>").c_str(), log);
				Textout("<td bgcolor='#FFFFFF' width='30%%'><font color=", log);
				Textout("'red'>FAIL</FONT></TD><td bgcolor='#FFFFFF'", log);
				Textout("width='35%%'>-/-</TD></tr></table>", log);

			}
		}



		void Logfile::FunctionResult(const char* funcName, bool result, const char* errorDesc, std::string logName) {

			std::string log;

			if (strcmp("", logName.c_str()) == 0) {

				log = "";
			}
			else {
				log = logName;
			}


			if (Log_OK == result || true == result)
			{
				Textout("<table width = '100%%' cellSpacing='1' cellPadding = '5'", log);
				Textout("border='0' bgcolor ='#C0C0C0'><tr><td bgcolor =", log);
				Textout(("'#FFFFFF' width ='35%%' >" + std::string(funcName) + "</TD>").c_str(), log);
				Textout("<td bgcolor='#FFFFFF' width='30%%'><font color=", log);
				Textout("'green'>OK</FONT></TD><td bgcolor='#FFFFFF'", log);
				Textout("width='35%%'>-/-</TD></tr></table>", log);
			}
			else if (Log_FAIL == result || false == result)
			{
				Textout("<table width = '100%%' cellSpacing='1' cellPadding = '5'", log);
				Textout("border='0' bgcolor ='#C0C0C0'><tr><td bgcolor =", log);
				Textout(("'#FFFFFF' width ='35%%' >" + std::string(funcName) + "</TD>").c_str(), log);
				Textout("<td bgcolor='#FFFFFF' width='30%%'><font color=", log);
				Textout("'red'>FAIL</FONT></TD><td bgcolor='#FFFFFF'", log);
				Textout("width='35%%'>", log);
				Textout(errorDesc, log);  // Description of the occured error.
				Textout("</TD></tr></table>", log);

			}
		}



		std::string Logfile::GetLastErrorAsString()
		{
			//Get the error message, if any.
			DWORD errorMessageID = ::GetLastError();
			if (errorMessageID == 0)
				return std::string(); //No error message has been recorded

			LPSTR messageBuffer = nullptr;
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

			std::string message(messageBuffer, size);

			//Free the buffer.
			LocalFree(messageBuffer);

			return message;
		}


		void Logfile::_fTextout(const char* Text, std::string logName, ...) {

			if (strcmp("", logName.c_str()) == 0) { // Write to currently open Logfile

				if (m_pCurrentOpenLogfile == nullptr) return;

				//write text and flush
				fprintf(m_pCurrentOpenLogfile, Text);
				fflush(m_pCurrentOpenLogfile);
			}
			else {

				m_pCurrentOpenLogfile = m_LogfileMap.at(logName);

				//write text and flush
				fprintf(m_pCurrentOpenLogfile, Text);
				fflush(m_pCurrentOpenLogfile);
			}
		}


		void Logfile::_fTextout(FONTCOLORS Color, const char* Text, std::string logName, ...) {

			if (strcmp("", logName.c_str()) == 0) { // Write to currently open Logfile

				_fTextout(Color, false, Text);
			}
			else {

				_fTextout(Color, false, Text, logName);
			}
		}


		void Logfile::_fTextout(FONTCOLORS Color, bool List, const char* Text, std::string logName, ...) {

			if (strcmp("", logName.c_str()) == 0) { // Write to currently open Logfile


				if (List == true)
				{
					Textout("<li>");
				}

				switch (Color)
				{
				case Log_BLACK:
					_fTextout("<font color = black>"); break;
				case Log_RED:
					_fTextout("<font color = red>"); break;
				case Log_GREEN:
					_fTextout("<font color = green>"); break;
				case Log_BLUE:
					_fTextout("<font color = blue>"); break;
				case Log_PURPLE:
					_fTextout("<font color = darkmagenta>"); break;
				case Log_AQUA:
					_fTextout("<font color = aqua>"); break;
				case Log_PINK:
					_fTextout("<font color = fuchsia>"); break;
				case Log_GOLD:
					_fTextout("<font color = gold>"); break;
				case Log_ORANGE:
					_fTextout("<font color = orange>"); break;
				case Log_LIME:
					_fTextout("<font color = lime>"); break;
				default:
					_fTextout("<font color = black>"); break;
				}


				_fTextout(Text);
				_fTextout("</font>");

				if (List == false) {
					_fTextout("<br>");
				}
				else {
					_fTextout("</li>");
				}

			}
			else {


				if (List == true)
				{
					_fTextout("<li>", logName);
				}

				switch (Color)
				{
				case Log_BLACK:
					_fTextout("<font color = black>", logName); break;
				case Log_RED:
					_fTextout("<font color = red>", logName); break;
				case Log_GREEN:
					_fTextout("<font color = green>", logName); break;
				case Log_BLUE:
					_fTextout("<font color = blue>", logName); break;
				case Log_PURPLE:
					_fTextout("<font color = darkmagenta>", logName); break;
				case Log_AQUA:
					_fTextout("<font color = aqua>", logName); break;
				case Log_PINK:
					_fTextout("<font color = fuchsia>", logName); break;
				case Log_GOLD:
					_fTextout("<font color = gold>", logName); break;
				case Log_ORANGE:
					_fTextout("<font color = orange>", logName); break;
				case Log_LIME:
					_fTextout("<font color = lime>", logName); break;
				default:
					_fTextout("<font color = black>", logName); break;
				}


				_fTextout(Text, logName);
				_fTextout("</font>", logName);

				if (List == false) {
					_fTextout("<br>", logName);
				}
				else {
					_fTextout("</li>", logName);
				}


			}
		}

	}

}
