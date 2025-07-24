
#pragma once
#include <vector>
#include <string>
#include <map>

class Char
{
private:
    
public:
    std::vector<unsigned char> data;
    int width;
    int height;
    int xadvance;
    int xoffset;
    Char(std::vector<unsigned char> data,int width,int height,int xadvance,int xoffset);
    

    Char();
};

class Font
{
private:
    std::map<int , Char> chars;
    int font_size;
    std::string font_name;

    std::string get_param(std::string buffer , std::string param);

    void parse_info(std::string line);
    
    void parse_common(std::string line);
    
    std::string parse_page(std::string line);   

    int parse_chars_count(std::string line);
    
    void parse_char(std::string line , int &id, int &x,int &y,int &width,int &height,int &xoffset,int &yoffset,int &xadvance);
    
    std::vector<unsigned char> get_char_data_from_atlas(int x , int y , int height , int width , int xoffset , int yoffset ,int xadvance,unsigned char* atlas_buffer , int chan , int atlas_width);

    Char get_char_from_atlas(int x , int y , int height , int width , int xoffset , int yoffset ,int xadvance,unsigned char* atlas_buffer , int chan , int atlas_width);
public:
    Font(std::string font_text_file);
    int get_font_size();

    Char get_char(int ascii_code);
   
    std::vector<unsigned char> from_text(std::string text , int &h , int &w);   
};
