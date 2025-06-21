#include <iostream>
#include "src/font.h"



#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"



int main(int argc ,char* argv[])
{
    std::string input;

    for (size_t i = 1; i < argc; i++)
    {
        input.append(argv[i]);  
        if (i < (argc - 1))
        {
            input += " ";   
        }        
    }

    Font text_renderer("fonts/sans-serif.fnt");
    
    int height;
    int width;
    unsigned char * text_data = text_renderer.from_text(input , height , width);

    stbi_write_png("result.png", width, height , 4, text_data, width * 4);
    return 0;
}