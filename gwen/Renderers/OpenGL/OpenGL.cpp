
#include "Gwen/Renderers/OpenGL.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <math.h>

#include "GL/glew.h"

#include <stdio.h>					// malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h>					// memset
#define FONTSTASH_IMPLEMENTATION	// Expands implementation
#include "FontStash/fontstash.h"

#define GLFONTSTASH_IMPLEMENTATION	// Expands implementation
#include "FontStash/glfontstash.h"


#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#else
#include <Windows.h>
#endif

#include <mutex>

namespace Gwen
{
	namespace Renderer
	{
		static FONScontext* fs = 0;
		static int num_renderers = 0;
		void OpenGL::Init()
		{
			num_renderers++;
			if (!fs)
			{
				fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
			}

            // find some fallback fonts
#ifdef _WIN32
            fallback_1 = fonsAddFont(fs, "malgun", "C:/Windows/Fonts/malgun.ttf");
            fallback_2 = fonsAddFont(fs, "nirmala", "C:/Windows/Fonts/nirmala.ttf");
#endif
		}
        
		OpenGL::~OpenGL()
		{
			num_renderers--;
			if (num_renderers == 0)
			{
				glfonsDelete(fs);
			}
		}

        std::string GetExecutablePath()
        {
        	char buffer[500];
        #ifdef _WIN32
        	GetModuleFileName(NULL, buffer, 500);
        #else
			int llen = readlink("/proc/self/exe", buffer, 499);
			if (llen < 0)
			{
				return "";
			}
			
			buffer[llen] = 0;
        #endif
        
        	// now remove characters after the last \ or /
        	int len = strlen(buffer);
        	for (int i = len - 1; i > 0; i--)
        	{
        	   	if (buffer[i] == '\\' || buffer[i] == '/')
        		{
        			break;
        		}
        		buffer[i] = 0;
        	}
        	return buffer;
        }
        
        
void find_font_files(const std::string& path, std::vector<std::pair<std::string, std::string>>& files)
{
#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	std::string spath = path;
	if ((hFind = FindFirstFileA((path + "/*").c_str(), &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.cFileName[0] == '.')
				continue;
			
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//recurse
				find_font_files(spath + "\\" + FindFileData.cFileName, files);
			}
			else
			{
				files.push_back({ spath + "\\" + FindFileData.cFileName, FindFileData.cFileName });
			}
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
#else
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path.c_str())))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
		//ignore . and .. directories
		if (entry->d_type == DT_DIR)
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}
			
			// now go deeper
			// todo
			
			find_font_files(path + "/" + entry->d_name, files);
		}
		else
		{
			files.push_back({ path + "/" + entry->d_name, entry->d_name});
		}
    }
    closedir(dir);
#endif
}

		struct FontFile
		{
			std::string path;
			std::string name;
			
			bool italic = false;
			bool bold = false;
			bool serif = false;// otherwise it is sans
			bool oblique = false;
			bool light = false;
			
			std::string Details() const
			{
				std::string s;
				if (bold)
				{
					s += "B";
				}
				if (italic)
				{
					s += "I";
				}
				if (oblique)
				{
					s += "O";
				}
				
				if (light)
				{
					s += " L";
				}
				if (serif)
				{
					s += " Serif";
				}
				return s;
			}
		};
			
        struct FontFamily
        {
        	std::string family;
        	
        	// for each font width, theres a set of fonts
        	std::vector<FontFile> mono;
        	std::vector<FontFile> narrow;
        	std::vector<FontFile> normal;
        };
        
        struct FontModifiers
        {
        	// regular just means not italic or bold
        	bool italic = false;
        	bool bold = false;
        	bool light = false;
        	
        	bool oblique = false;// not sure what to do about this yet
        	
        	bool serif = false;
        	
        	bool mono = false;
        	bool narrow = false;
        };
        
        // so we dont have a threading issue
        static std::mutex _font_mutex;
        static std::map<std::string, FontFamily> _fonts;
        
        bool HandleWord(const std::string& word, FontModifiers& modifiers)
        {
		    //printf("Word: %s\n", word.c_str());
		    if (word == "Bold" || word == "B" || word == "Bd")
		    {
		    	modifiers.bold = true;
		    	return true;
		    }
		    else if (word == "Italic" || word == "I" || word == "It")
		    {
		    	modifiers.italic = true;
		    	return true;
		    }
		    else if (word == "Mono" || word == "Mo")
		    {
		    	modifiers.mono = true;
		    	return true;
		    }
		    else if (word == "Narrow" || word == "C")
		    {
		    	modifiers.narrow = true;
		    	return true;
		    }
		    else if (word == "L" || word == "Light")
		    {
		    	modifiers.light = true;
		    	return true;
		    }
		    else if (word == "Serif" || word == "Se")
		    {
		    	modifiers.serif = true;
		    	return true;
		    }
		    else if (word == "Oblique")
		    {
		    	modifiers.oblique = true;
		    	return true;
		    }
		    else if (word == "Regular")// || word == "Sans")
		    {
		    	return true;
		    }
		    return false;
        }
        
        std::string SplitFont(const std::string& name, FontModifiers& modifiers)
        {        	
        	std::string family;
			
			std::string current_word;
			bool family_ended = false;
			int word = 0;
			for (int i = 0; i < name.length(); i++)
			{
				current_word += name[i];
				
				bool is_last_character = (i == name.length() - 1);
				bool is_skip_character = is_last_character ? 
				  false : 
				  (name[i+1] == '-' || name[i+1] == ' ' || name[i+1] == '_');
				  
			    // U doesnt start any keywords, this fixes things like Segoe UI
				if (!is_last_character && name[i] == 'U')
				{
					continue;
				}

				if (is_last_character || is_skip_character)// ||
				    //(name[i+1] >= 'A' && name[i+1] <= 'Z'))
				{
					// just finished a word
					if (word != 0)
					{
						family_ended = (family_ended | HandleWord(current_word, modifiers));
					}
					if (!family_ended)
					{
						//printf("\nFamily: %s\n", current_word.c_str());
						if (family.length())
						{
							family += " ";
						}
						family += current_word;
					}
					
					word++;
					current_word.clear();
					
					if (is_skip_character)
					{
						i++;
						continue;
					}
				}
			}
			
			// special case for sans serif
        	if (name.find("Sans") != -1 && name.find("Serif") != -1)
        	{
        		family = name.substr(0, name.find("Serif") + 5);
        		// todo, what do I do?
        		modifiers.serif = false;
        	}
			
			// split by capital letter, ' ' and -
			// stop when we get the first keyword
			//const char[] keywords = {"M", "I", "Bold", "Italic", "Regular", "Sarif" 
        	return family;
        }
        
        
        void LoadFonts()
        {
        	_font_mutex.lock();
        	if (_fonts.size())
        	{
        		_font_mutex.unlock();
        		return;
        	}
        	
        	// look through search paths for the font name, otherwise fall back to anything we find
        	std::vector<std::string> search_paths;
        	search_paths.push_back(GetExecutablePath());
#ifdef _WIN32
            search_paths.push_back("C:/Windows/Fonts");
#else
        	search_paths.push_back("/usr/share/fonts/truetype");
#endif
        	//search_paths.push_back("/usr/share/fonts/truetype/ttf-bitstream-vera");
        	
        	// now search through each directory recursively looking for any fonts and build a list
        	for (auto& path: search_paths)
        	{
        	    std::vector<std::pair<std::string, std::string>> files;
        		find_font_files(path, files);
        		
        		for (auto& file: files)
        		{
        			// make sure it ends with ttf
        			int l = file.second.length();
        			if (file.second.length() < 5)
        			{
        				continue;
        			}
        			
        			if (file.second[l-1] != 'f' ||
        				file.second[l-2] != 't' ||
        				file.second[l-3] != 't' ||
        				file.second[l-4] != '.')
        			{
        				continue;	
        			}

                    // Read in the font data.
                    FILE* fp = fons__fopen(file.first.c_str(), "rb");
                    if (fp == NULL) continue;
                    fseek(fp, 0, SEEK_END);
                    int dataSize = (int)ftell(fp);
                    fseek(fp, 0, SEEK_SET);
                    unsigned char* data = (unsigned char*)malloc(dataSize);
                    if (data == NULL) continue;
                    int readed = fread(data, 1, dataSize, fp);
                    fclose(fp);
                    fp = 0;
                    if (readed != dataSize) continue;

                    stbtt_fontinfo fi;
                    int stbError = stbtt_InitFont(&fi, data, 0);
                    int len = 0; int name_len = 0;
                    const char* real_name = stbtt_GetFontNameString(&fi, &name_len, 1, 0, 0, 1);
                    const char* txt = stbtt_GetFontNameString(&fi, &len, 1, 0, 0, 2);
                    uint16_t flags = stbtt_GetFontStyleFlags(&fi);
                    bool mono = stbtt_IsMonospaced(&fi);
        			
                    if (real_name == 0)
                    {
                        free(data);
                        continue;
                    }

        			FontModifiers modifiers;
                    std::string family(real_name, name_len);//  SplitFont(file.second.substr(0, l - 4), modifiers);

                    free(data);

                    if (flags & 0b1)
                        modifiers.bold = true;
                    if (flags & 0b10)
                        modifiers.italic = true;
                    if (flags & 0b100000)
                        modifiers.narrow = true;
                    if (mono)
                        modifiers.mono = true;
        			
        			if (family.length() == 0)
        			{
        				continue;
        			}
        			
        			FontFamily& ff = _fonts[family];
        			ff.family = family;
        			
        			FontFile font;
        			font.path = file.first;
        			font.name = file.second;
        			font.bold = modifiers.bold;
        			font.italic = modifiers.italic;
        			font.oblique = modifiers.oblique;
        			font.serif = modifiers.serif;
        			font.light = modifiers.light;
        			if (modifiers.mono)
        			{
        				ff.mono.push_back(font);
        			}
        			else if (modifiers.narrow)
        			{
        				ff.narrow.push_back(font);
        			}
        			else
        			{
        				ff.normal.push_back(font);
        			}
        			
        			//printf("%s: %s\n", file.first.c_str(), file.second.c_str());
        		} 
        	}
        	
        	_font_mutex.unlock();
        	
        	/*for (const auto& family : _fonts)
        	{
        		//printf("Font: %s of %s at %s\n", font.name.c_str(), font.family.c_str(), font.path.c_str());
        		//path = font.path;
        		printf("Family: %s\n", family.first.c_str());
        		
        		printf("Normal: \n");
        		for (const auto& f: family.second.normal)
        		{
        			printf("  %s %s\n", f.path.c_str(), f.Details().c_str());
        		}
        		printf("Mono: \n");
        		for (const auto& f: family.second.mono)
        		{
        			printf("  %s %s\n", f.path.c_str(), f.Details().c_str());
        		}
        		printf("Narrow: \n");
        		for (const auto& f: family.second.narrow)
        		{
        			printf("  %s %s\n", f.path.c_str(), f.Details().c_str());
        		}
        		
        		printf("\n\n");
        	}*/
        }
        
        std::string FindFont(const std::string& name, bool fallback = true)
        {
        	LoadFonts();
        	
        	FontModifiers modifiers;
        	std::string family = SplitFont(name, modifiers);
        	
        	std::string path;
        	
        	auto iter = _fonts.find(family);
        	if (iter != _fonts.end())
        	{
        		// todo handle sarif
        		if (modifiers.mono)
        		{
        		    for (const auto& fnt: iter->second.mono)
        			{
        				// stop when we find a perfect match
        				if (modifiers.bold == fnt.bold &&
        				    modifiers.italic == fnt.italic &&
        				    modifiers.oblique == fnt.oblique &&
        				    modifiers.serif == fnt.serif)
        				{
        					path = fnt.path;
        					break;
        				}
        				path = fnt.path;
        			}
        		}
        		else if (modifiers.narrow)
        		{
        		    for (const auto& fnt: iter->second.narrow)
        			{
        				// stop when we find a perfect match
        				if (modifiers.bold == fnt.bold &&
        				    modifiers.italic == fnt.italic &&
        				    modifiers.oblique == fnt.oblique &&
        				    modifiers.serif == fnt.serif)
        				{
        					path = fnt.path;
        					break;
        				}
        				path = fnt.path;
        			}
        		}
        		else
        		{
        		    for (const auto& fnt: iter->second.normal)
        			{
        				// stop when we find a perfect match
        				if (modifiers.bold == fnt.bold &&
        				    modifiers.italic == fnt.italic &&
        				    modifiers.oblique == fnt.oblique &&
        				    modifiers.serif == fnt.serif)
        				{
        					path = fnt.path;
        					break;
        				}
        				path = fnt.path;
        			}
        		}
        	}
        	
        	printf("Target font: %s (%s)\n", family.c_str(), name.c_str());
        	
        	if (path.length() == 0 && fallback)
        	{
        		printf("Falling back to closest default font...\n");
#ifdef _WIN32
                std::string fallback = "Calibri";
#else
        		std::string fallback = "Liberation Sans";
#endif
        		if (modifiers.mono)
        		{
        			fallback += " Mono";
        		}
        		if (modifiers.bold)
        		{
        			fallback += " Bold";
        		}
        		if (modifiers.italic)
        		{
        			fallback += " Italic";
        		}
        		if (modifiers.serif)
        		{
        			fallback += " Serif";
        		}
        		return FindFont(fallback, false);		
        	}
        	printf("Chose font: %s\n\n", path.c_str()); 
        	return path;
        }

		void OpenGL::RenderText( Gwen::Font* pFont, Gwen::PointF pos, const Gwen::String & text )
		{
			float fSize = pFont->size * Scale() * FontScale();
			if (pFont->pixel_size > 0)
			{
				fSize = pFont->pixel_size;
			}

			if ( !text.length() )
			{ return; }
			
			Flush();

            if (pFont->data == 0)
            {
                Gwen::String name = Gwen::Utility::UnicodeToString( pFont->facename );
                if (pFont->bold)
                {
                	name += " Bold";
                }
                int font = fonsAddFont(fs, name.c_str(), FindFont(name).c_str());
                if (font == FONS_INVALID)
                {
		            printf("Could not add font %s.\n", name.c_str());
		            return;
	            }
                if (fallback_1 >= 0)
                {
                    fonsAddFallbackFont(fs, font, fallback_1);
                }
                if (fallback_2 >= 0)
                {
                    fonsAddFallbackFont(fs, font, fallback_2);
                }
                pFont->data = (void*)font;
            }

			pos.x += m_RenderOffset.x;
			pos.y += m_RenderOffset.y;
			pos.x = (((float)pos.x) * m_fScale);
			pos.y = (((float)pos.y) * m_fScale);

			fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
			fonsSetFont(fs, (long)pFont->data);
			fonsSetColor(fs, glfonsRGBA(m_Color.r,m_Color.g,m_Color.b,m_Color.a));
			fonsSetSize(fs, fSize*1.333f);
			fonsDrawText(fs, pos.x, pos.y, text.c_str(), NULL);
		}

		Gwen::PointF OpenGL::MeasureText( Gwen::Font* pFont, const Gwen::String & text )
		{
            if (pFont->data == 0)
            {
				Gwen::String name = Gwen::Utility::UnicodeToString( pFont->facename );
                if (pFont->bold)
                {
                	name += " Bold";
                }
                int font = fonsAddFont(fs, name.c_str(), FindFont(name).c_str());
                if (font == FONS_INVALID)
                {
                    printf("Could not add font %s.\n", name.c_str());
                    return Gwen::PointF(0.0, 0.0);
                }
                if (fallback_1 >= 0)
                {
                    fonsAddFallbackFont(fs, font, fallback_1);
                }
                if (fallback_2 >= 0)
                {
                    fonsAddFallbackFont(fs, font, fallback_2);
                }
                pFont->data = (void*)font;
            }

            float fSize = pFont->size * FontScale();// dont multiply by scale for measure, we want to return unscaled units
            if (pFont->pixel_size > 0)
            {
                fSize = pFont->pixel_size;
            }

			fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
            
			fonsSetFont(fs, (long)pFont->data);
			fonsSetSize(fs, fSize*1.333f);
			
			float bounds[4];//0 is minx, 1 is miny, 2 is maxx 3 is maxy
			float advance = fonsTextBounds(fs, 0, 0, text.c_str(), NULL, bounds);
			
			Gwen::PointF p;
			p.x = advance;//std::abs(bounds[2] - bounds[0]);
			p.y = fSize*1.33;//std::abs(bounds[3] - bounds[1]);
			return p;
		}
	}
}
