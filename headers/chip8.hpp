#include <string>

class Chip8
{
    public:
        void init();
        void emulateCycle();
        bool loadGame(std::string path);
        void setKeys();
        
        unsigned char memory[4096];
        bool gfx[2048];
        unsigned char V[16];
        unsigned short op;
        unsigned short I;
        unsigned char delayTimer;
        unsigned char soundTimer;
        unsigned short stack[16];
        unsigned short sp;
        unsigned short pc;
        bool drawFlag = true;
        unsigned char key[16];

};