#include <stdbool.h>
#include <stdint.h>

struct FrameImp;
typedef struct FrameImp *Frame;

Frame Frame_New (unsigned short int width, unsigned short int height,
                 uint16_t bg);

void Frame_Clear (Frame frame, uint16_t bg);

unsigned short int Frame_Fits (Frame frame);

void Frame_AddLine (Frame frame, char *name, unsigned short int line,
                    unsigned short int col, _Bool selected);

uint16_t *Frame_Draw (Frame frame);
void Frame_Delete (Frame frame);
