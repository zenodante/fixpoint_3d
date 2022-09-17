#include "game.hpp"
#include "include.h"
#include "f3d.h"
#include "building.h"
#include "buildingMap.h"
using namespace blit;

uint16_t fbtemp[120 * 120] = { 0 };




///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
    blit::set_screen_mode(ScreenMode::lores);
    SetFrameBuffAddr(fbtemp);
    ClearFBuff();
    ClearZBuff();
    InitCamera();
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
int16_t x = 0; int16_t z = -300; int16_t y = 100;
v3_Q15 angle = { 0,0,0 };
void render(uint32_t time) {
    if (pressed(Button::DPAD_RIGHT)) {
        x += 1;
    }
    if (pressed(Button::DPAD_LEFT)) {
        x -= 1;
    }
    if (pressed(Button::DPAD_DOWN)) {
        y -= 1;
    }
    if (pressed(Button::DPAD_UP)) {
        y += 1;
    }
    if (pressed(Button::A)) {
        //camera.angle.y += 200;
        z += 1;
    }
    if (pressed(Button::B)) {
        //camera.angle.y -= 200;
        z -= 1;

    }
    if (pressed(Button::X)) {
        camera.angle.x += 200;
    }
    if (pressed(Button::Y)) {
        camera.angle.x -= 200;

    }
    camera.position.x = x;
    camera.position.z = z;
    camera.position.y = y;
    v3_i16 objPosition = { 0,0,0 };
    LookAt(&(camera.position), &objPosition, &(camera.angle));
    UpdateCameraMatrix();
    
    

    
    //ClearFBuff();
    //ClearZBuff();
    NewFrame();
    RenderTextureObjNoLight(&angle, &objPosition, (i16 *)building_vec, (u16 *)building_face, (u8 *)building_uv, (u8 *)buildingMap_map, (color_t *)buildingMap_lut_GBAR4444);

    /*
    TriTextureZ(x, y, 10, 0,0,
       x+size, y, 10,7, 0,
       x, y+size, 10, 0, 7, (u8 *)uvMap, (color_t * )boxLUT);
    TriTextureZ(x+size, y, 10, 7, 0,
        x,y+ size, 10, 0, 7,
        x+size,y+ size, 10, 7, 7, (u8*)uvMap, (color_t*)boxLUT);
    
    TriTextureZ(x+20, y+10, 10, 0, 0,
        x+13, y+20, 10, 7, 0,
        x+20, y+30, 10, 0, 7, (u8*)uvMap, (color_t*)boxLUT);
    */
    uint8_t *addr = screen.data;
   /* 
    for (u32 i = 0; i < 120 * 120; i++) {
        uint16_t color = (2048-zb[i])>>2;
        //BGAR type
        addr[i * 3] = color;
        addr[i * 3 + 1] = color;
        addr[i * 3 + 2] = color;
    }


*/
    for (u32 i = 0; i <120*120; i++) {
        uint16_t color = fbtemp[i];
        //GBAR type
        uint32_t R = (color & 0x000f) << 4;
        uint32_t G = (color & 0xf000) >> 8;
        uint32_t B = (color & 0x0f00) >> 4;
        addr[i * 3] = (u8)R;
        addr[i * 3 + 1] = (u8)G;
        addr[i * 3+2] = (u8)B; //R
    }

}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
}