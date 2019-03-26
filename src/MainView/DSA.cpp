//#include "mainview.h"
//#include <cstdlib>
//#include <math.h>
//
//void MainView::map_random(size_t _xs, size_t _ys)
//{
////    qDebug() << "DSA: 0";
//    // config
//    int h0=0,h1=300;       // [m] terrain elevation range
//    size_t h_water= 0;             // [m] sea level
//    size_t h_sand=15;              // [m] sand level
//    size_t h_evergreen=1500;       // [m] evergreen level
//    size_t h_snow=2000;            // [m] snow level
//    size_t h_rock=1800;            // [m] mountine rock level
//    float a_rock=60.0;          // [deg] mountine rock slope
//    float d_pixel=15.0;         // [m] pixel size
//    bool _island=true;
//
//    // types
//    enum _cover_enum
//    {
//        _cover_none = 0,
//        _cover_water,
//        _cover_snow,
//        _covers,
//        _cover_shift = 0,
//        _cover_mask = 15,
//    };
////    unsigned int _cover[_covers] =
////            {
////                    //  RRGGBB
////                    0x00000000,     // none
////                    0x00004080,     // water
////                    0x008F8F8F,     // snow
////            };
//    enum _terrain_enum
//    {
//        _terrain_enum_beg = -1,
//        _terrain_dirt,
//        _terrain_sand,
//        _terrain_rock,
//        _terrains,
//        _terrain_shift = 4,
//        _terrain_mask = 15,
//    };
////    unsigned int _terrain[_terrains] =
////            {
////                    //  RRGGBB
////                    0x00301510,     // dirt
////                    0x00EEC49A,     // sand
////                    0x00777777,     // rock
////            };
//    enum _flora_enum
//    {
//        _flora_enum_beg = -1,
//        _flora_none,
//        _flora_grass,
//        _flora_hardwood,
//        _flora_evergreen,
//        _flora_deadwood,
//        _floras,
//        _flora_shift = 8,
//        _flora_mask = 15,
//    };
////    unsigned int _flora[_floras] =
////            {
////                    //  RRGGBB
////                    0x00000000,     // none
////                    0x007F7F3F,     // grass
////                    0x001FFF1F,     // hardwood
////                    0x00007F00,     // evergreen
////                    0x007F3F1F,     // deadwood
////            };
//
//    // variables
//    float a, b;
//    int c, t, f;
//    size_t x, y, z, xx, yy, mxs, mys, dx, dy, dx2, dy2, r, r2;
//    int **ter = NULL, **typ = NULL;
////    Randomize();
//    // align resolution to power of 2
//    for (mxs = 1; mxs + 1 < _xs; mxs <<= 1);
//    if (mxs < 3)
//        mxs = 3;
//    for (mys = 1; mys + 1 < _ys; mys <<= 1);
//    if (mys < 3)
//        mys = 3;
//    ter = new int *[mys + 1];
//    for (y = 0; y <= mys; y++)
//        ter[y] = new int[mxs + 1];
//    typ = new int *[mys + 1];
//    for (y = 0; y <= mys; y++)
//        typ[y] = new int[mxs + 1];
//
//    qDebug() << "DSA: 1";
//    // [Terrain]
//
//    // diamond & square random height map -> ter[][]
//    dx = mxs;
//    dx2 = dx >> 1;
//    r = 1 << 16;                     // init step,half step and randomness
//    dy = mys;
//    dy2 = dy >> 1;
//    r2 = r >> 1;
//    // set corners values
//    if (_island)
//    {
//        t = -r2;
//        ter[0][0] = t;
//        ter[0][mxs] = t;
//        ter[mys][0] = t;
//        ter[mys][mxs] = t;
//        ter[dy2][dx2] = r2;
//    }
//    else
//    {
//        ter[0][0] = (rand() % r + 1);
//        ter[0][mxs] = (rand() % r + 1);
//        ter[mys][0] = (rand() % r + 1);
//        ter[mys][mxs] = (rand() % r + 1);
//    }
//    qDebug() << "DSA: 2";
//    for (; dx2 | dy2; dx = dx2, dx2 >>= 1, dy = dy2, dy2 >>= 1)    // subdivide step until full image is filled
//    {
//        if (!dx)
//            dx = 1;
//        if (!dy)
//            dy = 1;
//        // diamond (skip first one for islands)
//        if ((!_island) || (dx != mxs))
//            for (y = dy2, yy = mys - dy2; y <= yy; y += dy)
//                for (x = dx2, xx = mxs - dx2; x <= xx; x += dx)
//                    ter[y][x] = ((ter[y - dy2][x - dx2] + ter[y - dy2][x + dx2] + ter[y + dy2][x - dx2] +
//                                  ter[y + dy2][x + dx2]) >> 2) + (rand() % r + 1) - r2;
//        // square
//        for (y = dy2, yy = mys - dy2; y <= yy; y += dy)
//            for (x = dx, xx = mxs - dx; x <= xx; x += dx)
//                ter[y][x] =
//                        ((ter[y][x - dx2] + ter[y][x + dx2] + ter[y - dy2][x] + ter[y + dy2][x]) >> 2) +
//                        (rand() % r + 1) - r2;
//        for (y = dy, yy = mys - dy; y <= yy; y += dy)
//            for (x = dx2, xx = mxs - dx2; x <= xx; x += dx)
//                ter[y][x] =
//                        ((ter[y][x - dx2] + ter[y][x + dx2] + ter[y - dy2][x] + ter[y + dy2][x]) >> 2) +
//                        (rand() % r + 1) - r2;
//        for (x = dx2, xx = mxs - dx2; x <= xx; x += dx)
//        {
//            y = 0;
//            ter[y][x] = ((ter[y][x - dx2] + ter[y][x + dx2] + ter[y + dy2][x]) / 3) + (rand() % r + 1) - r2;
//            y = mys;
//            ter[y][x] = ((ter[y][x - dx2] + ter[y][x + dx2] + ter[y - dy2][x]) / 3) + (rand() % r + 1) - r2;
//        }
//        for (y = dy2, yy = mys - dy2; y <= yy; y += dy)
//        {
//            x = 0;
//            ter[y][x] = ((ter[y][x + dx2] + ter[y - dy2][x] + ter[y + dy2][x]) / 3) + (rand() % r + 1) - r2;
//            x = mxs;
//            ter[y][x] = ((ter[y][x - dx2] + ter[y - dy2][x] + ter[y + dy2][x]) / 3) + (rand() % r + 1) - r2;
//        }
//
//        // adjust border
//        if (_island)
//        {
//            for (y = 0; y <= mys; y += dy2)
//            {
//                ter[y][0] = t;
//                ter[y][mxs] = t;
//            }
//            for (x = 0; x <= mxs; x += dx2)
//            {
//                ter[0][x] = t;
//                ter[mys][x] = t;
//            }
//        }
//        qDebug() << "DSA: 3";
//
//        // adjust randomness
//        // r=(r*100)>>8; if (r<2) r=2; r2=r>>1;
//        r >>= 1;
//        if (r < 2)
//            r = 2;
//        r2 = r >> 1;
//    }
//    // rescale to <h0,h1>
//    xx = ter[0][0];
//    yy = xx;
//    for (y = 0; y < mys; y++)
//        for (x = 0; x < mxs; x++)
//        {
//            z = ter[y][x];
//            if (xx > z)
//                xx = z;
//            if (yy < z)
//                yy = z;
//        }
//    qDebug() << "DSA: 4 ";
//    for (y = 0; y < mys; y++)
//        for (x = 0; x < mxs; x++)
//            ter[y][x] = h0 + (((ter[y][x] - xx) * (h1 - h0)) / (yy - xx));
//
//    qDebug() << "DSA: 5 ";
//    // [Surface]
//
//    for (y = 0; y < mys; y++)
//        for (x = 0; x < mxs; x++)
//        {
//            z = ter[y][x];
//            // max slope [deg]
//            a = atan2(ter[y][x + 1] - z, d_pixel);
//            b = atan2(ter[y + 1][x] - z, d_pixel);
//            if (a < b)
//                a = b;
//            a *= 180.0 / M_PI;
//
//            c = _cover_none;
//            if (z <= h_water)
//                c = _cover_water;
//            if (z >= h_snow)
//                c = _cover_snow;
//
//            t = _terrain_dirt;
//            if (z <= h_sand)
//                t = _terrain_sand;
//            if (z >= h_rock)
//                t = _terrain_rock;
//            if (a >= a_rock)
//                t = _terrain_rock;
//
//            f = _flora_none;
//            if (t == _terrain_dirt)
//            {
//                r = (rand() % 100 + 1);
//                if (r > 10)
//                    f = _flora_grass;
//                if (r > 50)
//                {
//                    if (z > h_evergreen)
//                        f = _flora_evergreen;
//                    else
//                    {
//                        r = (rand() % h_evergreen + 1);
//                        if (r <= z)
//                            f = _flora_evergreen;
//                        else
//                            f = _flora_hardwood;
//                    }
//                }
//                if (r < 5)
//                    f = _flora_deadwood;
//            }
//            typ[y][x] = (c << _cover_shift) | (t << _terrain_shift) | (f << _flora_shift);
//        }
//
//    qDebug() << "DSA: 6 ";
//
//    // [copy data] rewrite this part to suite your needs it just compute color based on type of terrain and height
//    // ter[][] is elevation in meters
//    // typ[][] is surface type
//
//    for (y = 0; y < _ys; y++)
//        for (x = 0; x < _xs; x++)
////        {
////            qDebug() << "x = " << x << ", y = " << y;
////            qDebug() << "pic(x,y) = ", pic[x][y];
////            qDebug() << "ter(x,y) = ", ter[x][y];
////            qDebug() << "h1 = " << h1 << ", h0 = " << h0;
////            qDebug() << "add = " << (((ter[x][y] - h0) * 255) / (h1 - h0)) * 0x00010101;
////            qDebug() << "Adding now!";
//            this->pic[x][y] = (((ter[x][y] - h0) * 255) / (h1 - h0)) * 0x00010101;
////            qDebug() << "Done";
////        }
//
//    qDebug() << "DSA: 7 ";
////    for (y = 0; y < _ys; y++)
////        for (x = 0; x < _xs; x++)
////        {
////            r = typ[y][x];
////            c = (r >> _cover_shift) & _cover_mask;
////            t = (r >> _terrain_shift) & _terrain_mask;
////            f = (r >> _flora_shift) & _flora_mask;
////            r = _terrain[t];
////            if (c)
////                r = _cover[c];
////            if (c == _cover_water)
////            {
////                xx = 256 - ((ter[y][x] << 7) / h0);
////                yy = int(r >> 16) & 255;
////                yy = (yy * xx) >> 8;
////                r = (r & 0x0000FFFF) | (yy << 16);
////                yy = int(r >> 8) & 255;
////                yy = (yy * xx) >> 8;
////                r = (r & 0x00FF00FF) | (yy << 8);
////                yy = int(r) & 255;
////                yy = (yy * xx) >> 8;
////                r = (r & 0x00FFFF00) | (yy);
////            }
////            if (f)
////            {
////                if (c)
////                    r |= _flora[f];
////                else
////                    r = _flora[f];
////            };
////
////            this->pic2[y][x + _xs] = r;
////        }
////    qDebug() << "DSA: 8 ";
//
//    // free ter[][],typ[][]
////    for (y = 0; y <= mys; y++)
////        delete[] ter[y];
////    qDebug() << "DSA: 9 ";
////    delete[] ter;
////    qDebug() << "DSA: 10 ";
////    ter = NULL;
////    qDebug() << "DSA: 11 ";
////    for (y = 0; y <= mys; y++)
////        delete[] typ[y];
////    qDebug() << "DSA: 12 ";
////    delete[] typ;
////    qDebug() << "DSA: 13 ";
////    typ = NULL;
////    qDebug() << "DSA: 14 ";
//}