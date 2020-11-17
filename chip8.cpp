#include "headers/chip8.hpp"
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string>
#include <iomanip>

unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void Chip8::init()
{
    //The pc starts at 0x200
    pc      = 0x200;
    //Reset variables
    op      = 0;
    I       = 0;
    sp      = 0;
    //Clear display
    for (int i = 0; i < 64 * 32; i++)
    {
        gfx[i] = 0;
    }
    //Reset stack and registers
    for (int i = 0; i < 16; i++)
    {
        stack[i] = 0;
        V[i] = 0;
        key[i] = 0;
    }
    //Reset memory
    for (int i = 0; i < 4096; i++)
    {
        memory[i] = 0;
    }
    //Load font
    for(int i = 0; i < 80; i++)
    {
        memory[i] = chip8_fontset[i];
    }
    
    //Timers
    soundTimer = 0;
    delayTimer = 0;

    //RNG
    srand(time(NULL));
}

void Chip8::emulateCycle()
{
    //We combine 2 bytes to get the opcode
    op = memory[pc] << 8 | memory[pc + 1];

    /*std::cout << "Opcode: " << std::hex << op << "\n";*/

    //Execute op
    //At most ops we count add 2 to pc becase op has 2 bytes
    switch (op & 0xF000)
    {
        case 0x0000:
            switch (op & 0x000F)
            {
                case 0x0000:
                    for (int i = 0; i < 2048; i++)
                    {
                        gfx[i] = 0;
                    }
                    drawFlag = true;
                    pc += 2;
                break;
                //0x00EE
                case 0x000E:
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                break;
            }
        break;
        case 0x1000:
            pc = op & 0x0FFF;
        break;
        case 0x2000:
			stack[sp] = pc;
			sp++;
			pc = op & 0x0FFF;
		break;
        case 0x3000:
            if (V[(op & 0x0F00) >> 8] == (op & 0x00FF))
            {
                pc += 2;
            }
            pc += 2;
        break;
        case 0x4000:
            if (V[(op & 0x0F00) >> 8] != (op & 0x00FF))
            {
                pc += 2;
            }
            pc += 2;
        break;
        case 0x5000:
            if (V[(op & 0x0F00) >> 8] == V[(op & 0x00F0) >> 4])
            {
                pc += 2;
            }
            pc += 2;
        break;
        case 0x6000:
            V[(op & 0x0F00) >> 8] = op & 0x00FF;
            pc += 2;
        break;
        case 0x7000:
            V[(op & 0x0F00) >> 8] += op & 0x00FF;
            pc += 2;
        break;
        case 0x8000:
            switch (op & 0x000F)
            {
                case 0x0000:
                    V[(op & 0x0F00) >> 8] = V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0001:
                    V[(op & 0x0F00) >> 8] |= V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0002:
                    V[(op & 0x0F00) >> 8] &= V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0003:
                    V[(op & 0x0F00) >> 8] ^= V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0004:
                    //Carry flag
                    if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    //Count
                    V[(op & 0x0F00) >> 8] += V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0005:
                    //Carry flag
                    if(V[(op & 0x00F0) >> 4] > (V[(op & 0x0F00) >> 8]))
                    {
                        V[0xF] = 0;
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    //Count
                    V[(op & 0x0F00) >> 8] -= V[(op & 0x00F0) >> 4];
                    pc += 2;
                break;
                case 0x0006:
                    V[0xF] = V[(op & 0x0F00) >> 8] & 1;
                    V[(op & 0x0F00) >> 8] = V[(op & 0x0F00) >> 8] >> 1;
                    pc += 2;
                break;
                case 0x0007:
                    //Carry flag
                    if(V[(op & 0x0F00) >> 8] > V[(op & 0x00F0) >> 4])
                    {
                        V[0xF] = 0;
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    //Count
                    V[(op & 0x0F00) >> 8] = V[(op & 0x00F0) >> 4] - V[(op & 0x0F00) >> 8];
                    pc += 2;
                break;
                case 0x000E:
                    V[0xF] = V[(op & 0x0F00) >> 8] >> 7;
                    V[(op & 0x0F00) >> 8] = V[(op & 0x0F00) >> 8] << 1;
                    pc += 2;
                break;
            }
        break;
        case 0x9000:
            if (V[(op & 0x0F00) >> 8] != V[(op & 0x00F0) >> 4])
            {
                pc += 2;
            }
            pc += 2;
        break;
        case 0xA000:
            I = op & 0x0FFF;
            pc += 2;
        break;
        case 0xB000:
            pc = (op & 0x0FFF) + V[0];
        break;
        case 0xC000:
			V[(op & 0x0F00) >> 8] = (rand() % 0xFF) & (op & 0x00FF);
			pc += 2;
		break;
        case 0xD000:	
        { 
            unsigned char x = V[(op & 0x0F00) >> 8];
            unsigned char y = V[(op & 0x00F0) >> 4];
            unsigned char height = (op & 0x000F);
            unsigned short pixel;
            
            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            V[0xF] = 1;
                        }                                 
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            
            drawFlag = true;
            pc += 2;
        }
        break;
        case 0xE000:
            switch (op & 0x00FF)
            {
                case 0x009E:
                    if (key[V[(op & 0x0F00) >> 8]] == 1)
                    {
                        pc += 2;
                    }
                    pc += 2;
                break;
                case 0x00A1:
                    if (key[V[(op & 0x0F00) >> 8]] == 0)
                    {
                        pc += 2;
                    }
                    pc += 2;
                break;
            }
        break;
        case 0xF000:
            switch (op & 0x00FF)
            {
                case 0x0007:
                    V[(op & 0x0F00) >> 8] = delayTimer;
                    pc += 2;
                break;
                case 0x000A:
                {
                    bool cont = false;
                    for (int i = 0; i < 16; i++)
                    {
                        if (key[i] == 1)
                        {
                            V[(op & 0x0F00) >> 8] = i;
                            cont = true;
                            break;
                        }
                    }
                    if(cont)
                    {
                        pc += 2;
                    }
                    else 
                    {
                        return;
                    }
                }
                break;
                case 0x0015:
                    delayTimer = V[(op & 0x0F00) >> 8];
                    pc += 2;
                break;
                case 0x0018:
                    soundTimer = V[(op & 0x0F00) >> 8];
                    pc += 2;
                break;
                case 0x001E:
                    I += V[(op & 0x0F00) >> 8];
                    pc += 2;
                break;
                case 0x0029:
                    I = V[(op & 0x0F00) >> 8] * 0x5;
					pc += 2;
                break;
                case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
					memory[I]     = V[(op & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;					
					pc += 2;
				break;
                case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
					for (int i = 0; i <= ((op & 0x0F00) >> 8); ++i)
						memory[I + i] = V[i];	

					I += ((op & 0x0F00) >> 8) + 1;
					pc += 2;
				break;
                case 0x0065: // FX55: Stores V0 to VX in memory starting at address I					
					for (int i = 0; i <= ((op & 0x0F00) >> 8); ++i)
						V[i] = memory[I + i];	

					I += ((op & 0x0F00) >> 8) + 1;
					pc += 2;
				break;
            }
        break;
    }

    //Update timers
    if (delayTimer > 0)
    {
        delayTimer--;
    }
    if (soundTimer > 0)
    {
        soundTimer--;
        if (soundTimer == 0)
        {
            std::cout << "BEEP!\n";
        }
    }
    /*
    std::cout << "I: " << I << " ";
    for (int i = 0; i < 16; i++)
    {
        int a = V[i];
        std::cout << a << " ";
    }
    std::cout << "\n";*/
}

bool Chip8::loadGame(std::string path)
{
    //Loading starts at 0x200
    FILE* pF = fopen(path.c_str(), "rb");
    if (pF == NULL)
    {
        std::cout << "There was an error while loading the game!\n";
        std::getchar();
        return 1;
    }

    long count;
    fseek(pF , 0 , SEEK_END);
    count = ftell(pF);
    rewind(pF);

    char* buffer = new char[count];

    fread(buffer, 1, count, pF);

    //Loading into memory
    for(int i = 0; i < count; ++i)
        memory[i + 512] = buffer[i];
    //

    fclose(pF);
    delete [] buffer;
    return 0;
}