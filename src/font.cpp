#include "font.h"
#include <sstream>
#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

Char::Char(std::vector<unsigned char> data,int width,int height,int xadvance,int xoffset)
{
    this->data = data;
    this->width = width;
    this->height = height;
    this->xadvance = xadvance;
    this->xoffset = xoffset;
}

Char::Char(){};

void Font::parse_info(std::string line)
{
    //[TODO]
    //?
}

void Font::parse_common(std::string line)
{
    //[TODO]
    //?
}

std::string Font::parse_page(std::string line)
{
    std::istringstream iss(line);
    std::vector<std::string> strs;      
    std::string str;
       
    while (iss >> str)
    {
        strs.push_back(str);
    }    

    //id=0 - no idea what this means
    int id = stoi(get_param(strs[1] , "id"));
    
    std::string font_file = get_param(strs[2] , "file");             
    return font_file;          
}

int Font::parse_chars_count(std::string line)
{
    //[TODO]
    //?
    return 0;
}

std::string Font::get_param(std::string buffer , std::string param)
{        
    int equ_pos = buffer.find("=");
        
    std::string param_name = buffer.substr(0, equ_pos);
    std::string value = buffer.substr(equ_pos + 1);

    if (param.compare(param_name) != 0)
    {
        return NULL;
    }

    if (value[0] == '"' )
    {
        return value.substr(1 , value.length() - 2);          
    }  
    return value;          
}

void Font::parse_char(std::string line , int &id, int &x,int &y,int &width,int &height,int &xoffset,int &yoffset,int &xadvance)
{
    std::istringstream iss(line);
    std::vector<std::string> tokens;      
    std::string str;
       
    while (iss >> str)
    {
        tokens.push_back(str);
    }    

    int c = 1;
    id = std::stoi(get_param(tokens[c++] , "id"));
    x = std::stoi(get_param(tokens[c++] , "x"));
    y =  std::stoi(get_param(tokens[c++] , "y"));
    width =  std::stoi(get_param(tokens[c++] , "width"));
    height =  std::stoi(get_param(tokens[c++] , "height"));
    xoffset =  std::stoi(get_param(tokens[c++] , "xoffset"));
    yoffset =  std::stoi(get_param(tokens[c++] , "yoffset"));
    xadvance =  std::stoi(get_param(tokens[c++] , "xadvance"));   
}

Font::Font(std::string font_text_file)
{
    std::string line;
    std::ifstream file(font_text_file);


    if (!file || !file.is_open()) 
    {       
        std::cout << "Couldn't open file!"<< std::endl;
        //dont know  
        throw;
    }

    getline(file , line);
    parse_info(line);

    getline(file , line);
    parse_common(line);

    getline(file , line);
    std::string font_file = parse_page(line);

    int pos = font_text_file.rfind('/');
    if(pos != std::string::npos)
    {                  
        for (size_t i = 0; i < font_file.length(); i++)
        {
            font_text_file[++pos] = font_file[i];
        }
        font_file = font_text_file;
    }
              
    unsigned char* atlas = nullptr;
    int atlas_w;
    int atlas_h;
    int atlas_ch;
    atlas = stbi_load(font_file.c_str(), &atlas_w, &atlas_h, &atlas_ch, 4);
    if (!atlas) 
    {
        //dont know what do to 
        std::cout << "Failed to load image: " << stbi_failure_reason() << std::endl;
        throw;
    }

    getline(file , line);
    parse_chars_count(line);
        
    //parse actual chars now 
    int id, x, y, width, height, xoffset, yoffset, xadvance;
    while (getline(file , line))
    {
        parse_char(line,id, x, y, width, height, xoffset, yoffset, xadvance);
            
        Char _char = get_char_from_atlas(x , y , height , width , xoffset , yoffset , xadvance,atlas , atlas_ch , atlas_w);

        chars.insert(std::make_pair(id , _char));
    }

    stbi_image_free(atlas);
    file.close();
   
}

std::vector<unsigned char> Font::get_char_data_from_atlas(int x , int y , int height , int width , int xoffset , int yoffset ,int xadvance,unsigned char* atlas_buffer , int chan , int atlas_width)
{
    int h_y_offset = height + yoffset;

    std::vector<unsigned char> data((width * h_y_offset) * chan , 0);

    unsigned char* src;
    int row = atlas_width * chan;
    for (size_t h = 0; h < height; h++)
    {
        src = atlas_buffer + (y * row) + (x * chan);
            std::copy(
            src, 
            src + width * chan ,
            data.data() + (width * chan ) * yoffset
            );
        y++;
        yoffset++;
    }

    return data;

}

Char Font::get_char_from_atlas(int x , int y , int height , int width , int xoffset , int yoffset ,int xadvance,unsigned char* atlas_buffer , int chan , int atlas_width)
{
    std::vector<unsigned char> data = get_char_data_from_atlas(x , y , height , width , xoffset , yoffset , xadvance, atlas_buffer , chan , atlas_width);
    Char _char(data, width, height + yoffset, xadvance, xoffset); 
    return _char;
}

std::vector<unsigned char> Font::from_text(std::string text , int &h , int &w)
{
    int final_w = 0;
    int final_h = 0;
    Char c;

    std::vector<Char> chars;
    std::vector<int> cursor_gaps;
    int cursor_gap;
    for (size_t i = 0; i < text.length(); i++)
    {
        c = this->chars.at((int)text[i]);
        if (final_h < c.height){final_h = c.height;}

       
        cursor_gap = c.xadvance - (c.xoffset + c.width);
        final_w += c.width + cursor_gap;

        chars.push_back(c);
        cursor_gaps.push_back(cursor_gap + c.width);      
    }

    std::vector<unsigned char> result((final_h * final_w ) * 4 , 0);

    unsigned char* src = nullptr;
    int cursor = 0;
    for (size_t i = 0; i < chars.size(); i++)
    {
        c = chars.at(i);
        for (size_t y = 0; y < c.height; y++)
        {
            src = c.data.data() + (y * c.width * 4);
            std::copy(
                src ,
                src + c.width * 4,
                result.data() + (y * final_w + cursor) * 4
            );
        }
        cursor += cursor_gaps.at(i);   
    }
    

    h = final_h;
    w = final_w;
    return result;   
}
