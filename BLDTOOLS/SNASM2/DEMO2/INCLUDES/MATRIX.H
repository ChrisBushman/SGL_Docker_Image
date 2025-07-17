

extern	Uint32 frontclip;


void push_matrix(void);
void pop_matrix(void);
void initial_matrix(void);

void translate(Sint32 xpos, Sint32 ypos, Sint32 zpos);
void scale(Sint32 xpos, Sint32 ypos, Sint32 zpos);

void rotate_x(Sint32 xangle);
void rotate_y(Sint32 yangle);
void rotate_z(Sint32 zangle);

Sint32 mysin(Sint32 angle);
Sint32 mycos(Sint32 angle);
