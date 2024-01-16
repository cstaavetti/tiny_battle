#ifndef _UTILH_
#define _UTILH_

#include <raylib.h>

extern Vector2 IndexToVec(short mIndex);
extern short VecToIndex(Vector2 vec);

extern short Distance(short mX, short mY);

extern short IndexUp(short mIndex);
extern short IndexDown(short mIndex);
extern short IndexLeft(short mIndex);
extern short IndexRight(short mIndex);
extern short IndexUpBy(short mIndex, short by);
extern short IndexDownBy(short mIndex, short by);
extern short IndexLeftBy(short mIndex, short by);
extern short IndexRightBy(short mIndex, short by);

extern short IndexToDirBy(short dir, short mIndex, short by);

#endif